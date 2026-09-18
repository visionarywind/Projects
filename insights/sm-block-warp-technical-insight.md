# GPU 中 SM、Block 与 Warp 的对应关系：技术洞察

## 1. 摘要

CUDA 编程模型中的 `thread`、`warp`、`block`、`grid`，以及 GPU 硬件中的 `SM`（Streaming Multiprocessor）属于不同层次的概念。理解它们之间的关系，是理解 GPU 并发、同步、调度以及 GPU driver 中 stream/channel 设计的基础。

最重要的层次关系可以概括为：

```text
GPU
└── 多个 SM（Streaming Multiprocessor）
    └── 多个驻留 Block（Resident Block）
        └── 多个 Warp
            └── 多个 CUDA Thread
```

从 CUDA 程序的角度看：

```text
一个 Grid 包含多个 Block
一个 Block 包含多个 Thread
硬件通常把连续的 32 个 Thread 组织成一个 Warp
一个 Block 通常整体驻留在一个 SM 上
一个 SM 可以同时驻留多个 Block
```

其中需要特别注意：

- `thread` 是编程模型中的逻辑线程，不等于一个独立的 CPU 核心。
- `warp` 通常是 GPU 硬件调度的基本单位。
- `block` 是资源分配、shared memory 以及 block 内协作同步的基本单位。
- `SM` 是实际承载 block、调度 warp 并执行指令的硬件单元。
- block 到 SM 的具体映射通常由硬件调度器动态决定，普通 CUDA 程序不应依赖它。

本文中的“wrap”统一按 GPU 术语 `warp` 讨论。

---

## 2. 从 Grid 到 Thread 的编程模型

CUDA kernel 的启动形式通常是：

```cpp
kernel<<<gridDim, blockDim>>>(args);
```

这会创建一个逻辑上的 grid。grid 由多个 block 组成，每个 block 又由多个 thread 组成。

```text
Grid
├── Block 0
│   ├── Thread 0
│   ├── Thread 1
│   └── ...
├── Block 1
│   ├── Thread 0
│   ├── Thread 1
│   └── ...
└── Block 2
```

一个 thread 通常通过以下内置变量确定自己处理的数据：

```cpp
int global_id = blockIdx.x * blockDim.x + threadIdx.x;
```

在这一层，CUDA 主要向开发者暴露的是逻辑并行模型：开发者描述每个 thread 的工作，而不是手工指定某个 thread 应该运行在哪个 SM 上。

### 2.1 Grid

Grid 是一次 kernel launch 的全部执行实例。它可以是一维、二维或三维：

```cpp
kernel<<<dim3(grid_x, grid_y, grid_z),
         dim3(block_x, block_y, block_z)>>>(...);
```

Grid 中所有 block 通常执行同一份 kernel 代码，但处理不同的数据区间。

### 2.2 Block

Block 是一组可以协作的 thread。block 内 thread 可以使用：

- shared memory；
- `__syncthreads()` 等 block 级同步；
- block 内的 warp 协作机制；
- block 内共享的执行上下文和资源。

普通 CUDA 编程模型通常保证：一个 block 的 thread 能够在同一个 SM 上协作。因此一个 block 通常不会被拆到多个 SM 上执行。

### 2.3 Thread

Thread 是 CUDA 编程模型的最小逻辑执行实体。每个 thread 具有自己的：

- `threadIdx`；
- 局部变量；
- 寄存器状态；
- 逻辑程序执行位置；
- active/inactive 状态。

但硬件通常不会为每个 CUDA thread 提供一个独立的 CPU 风格取指和调度单元。GPU 会把 thread 组织成 warp 后进行调度。

---

## 3. Thread 如何组成 Warp

在绝大多数 NVIDIA CUDA 架构中，一个 warp 包含 32 个 thread。对于一个 block，warp 数量通常为：

```text
warp_count = ceil(threads_per_block / 32)
```

例如：

| Block 中的 thread 数 | Warp 数 |
|---:|---:|
| 32 | 1 |
| 64 | 2 |
| 100 | 4 |
| 128 | 4 |
| 256 | 8 |
| 1024 | 32 |

当 block 的 thread 数不是 32 的整数倍时，最后一个 warp 仍然以 32-lane 的形式存在，但未使用的 lane 会被屏蔽。

例如：

```cpp
kernel<<<1, 100>>>(...);
```

逻辑上有 100 个 thread，硬件通常组织成：

```text
Warp 0: Thread   0 ~ 31
Warp 1: Thread  32 ~ 63
Warp 2: Thread  64 ~ 95
Warp 3: Thread  96 ~ 99 + 28 个 inactive lane
```

### 3.1 Warp 是主要调度单位

从硬件角度看，GPU 通常以 warp 为单位选择下一组可执行指令：

```text
Warp 0 -> 执行指令 I0
Warp 1 -> 执行指令 I1
Warp 2 -> 等待 global memory
Warp 3 -> 执行指令 I2
```

一个 warp 内的多个 thread 通常共享一条指令流，但每个 lane 使用自己的：

- 寄存器值；
- 地址；
- predicate/active 状态；
- 计算结果。

这就是 SIMT（Single Instruction, Multiple Threads）模型。

### 3.2 Warp divergence

如果同一个 warp 内的 thread 走不同分支，例如：

```cpp
if (threadIdx.x % 2 == 0) {
    a = a + 1;
} else {
    b = b + 1;
}
```

硬件通常不能让同一个 warp 的两组 lane 完全独立地同时执行两条不同的控制流，而是类似于：

```text
1. 执行偶数 thread 的分支，屏蔽奇数 thread
2. 执行奇数 thread 的分支，屏蔽偶数 thread
```

这称为 warp divergence。两条路径都会被执行时，分支可能降低有效吞吐量。

因此，GPU 中很多性能问题不是“thread 数不够”，而是：

- warp 内分支不一致；
- 内存访问不合并；
- warp 数量不足以隐藏延迟；
- block 资源使用过高导致驻留 warp 数下降。

---

## 4. SM 是什么

SM（Streaming Multiprocessor）是 GPU 中承载 kernel 执行的主要硬件单元。一个 SM 通常包含：

```text
SM
├── Warp scheduler
├── Instruction dispatch 单元
├── FP32/FP64/INT 执行单元
├── Load/Store 单元
├── Special Function Unit
├── Tensor Core（取决于架构）
├── Register file
├── Shared memory / L1 cache
└── Block、warp 和线程的驻留状态
```

“GPU 有很多核心”是一个较粗略的说法。更准确地说：

- GPU 由多个 SM 组成；
- SM 内部包含多个不同类型的执行单元；
- CUDA core 只是 SM 内部的一类执行资源；
- warp scheduler 负责从驻留 warp 中选择可执行 warp；
- SM 管理 block 的资源分配和 warp 的执行。

因此，SM 不是一个单独的 CUDA thread，也不等同于一个 CPU core。

---

## 5. Block 如何映射到 SM

当 kernel 启动后，硬件调度器会把 grid 中的 block 分配到可用 SM 上：

```text
Grid blocks:
    Block 0, Block 1, Block 2, Block 3, ...

GPU:
    SM 0 <- Block 0
    SM 1 <- Block 1
    SM 2 <- Block 2
    SM 3 <- Block 3
```

当某个 block 完成并释放资源后，调度器会把尚未执行的 block 放到该 SM：

```text
初始：
    SM 0 <- Block 0
    SM 1 <- Block 1
    SM 2 <- Block 2
    SM 3 <- Block 3

资源释放后：
    SM 0 <- Block 4
    SM 1 <- Block 5
    SM 2 <- Block 6
    SM 3 <- Block 7
```

### 5.1 一个 Block 通常只驻留在一个 SM

一个普通 CUDA block 通常会整体驻留在一个 SM 上，而不会拆成：

```text
Block 0 的一部分 -> SM 0
Block 0 的另一部分 -> SM 1
```

原因是 block 内线程需要协作访问：

- 同一个 shared memory 空间；
- 同一个 block barrier；
- 同一个 block 级执行状态。

如果一个 block 可以任意跨多个 SM，`__syncthreads()` 和 shared memory 的实现就会变成跨 SM 的全局同步问题，代价和复杂度都会显著增加。

因此 block 是一个很重要的边界：

> block 是可协作执行的线程组，也是 GPU 进行资源分配的基本单位。

### 5.2 一个 SM 可以同时驻留多个 Block

一个 SM 并不一定只运行一个 block。例如：

```text
SM 0
├── Block 0
│   ├── Warp 0
│   ├── Warp 1
│   └── Warp 2
└── Block 4
    ├── Warp 0
    ├── Warp 1
    └── Warp 2
```

多个 block 能否同时驻留，取决于它们对 SM 资源的消耗，包括：

- 每个 block 的 thread 数；
- 每个 thread 的寄存器使用量；
- 每个 block 的 shared memory 使用量；
- SM 的最大 resident warp 数；
- SM 的最大 resident block 数；
- 当前 GPU 架构的限制；
- kernel 的编译属性和 launch 配置。

例如，即使架构允许一个 SM 同时驻留很多 block，如果一个 block 使用了大量 shared memory，也可能导致每个 SM 只能驻留一个 block。

---

## 6. Occupancy：为什么同样的线程数会有不同并发度

SM 上能够同时驻留的 warp/block 数量称为 occupancy 相关能力。它不等于实际利用率，但会影响 GPU 隐藏延迟的能力。

可以用一个简化模型表示：

```text
实际 resident blocks
    = min(
        thread 资源限制，
        register 资源限制，
        shared memory 资源限制，
        warp 数限制，
        block 数限制
      )
```

### 6.1 寄存器限制

如果 kernel 中每个 thread 使用很多寄存器，那么一个 SM 能同时保存的 thread 状态就会减少：

```text
高寄存器使用量
    -> 每个 SM 能驻留的 block 减少
    -> 可用 warp 数减少
    -> 隐藏内存延迟的能力下降
```

### 6.2 Shared memory 限制

如果每个 block 使用大量 shared memory：

```text
SM shared memory 总量有限
    -> 同时驻留的 block 数下降
```

### 6.3 Block size 限制

block 太大时，一个 SM 可能只能驻留很少的 block；block 太小时，又可能产生额外调度和边界开销。最佳 block size 取决于：

- kernel 的寄存器使用；
- shared memory 使用；
- 内存访问模式；
- 计算/访存比例；
- 目标 GPU 架构。

因此“启动更多 thread”不一定会带来更高并发，关键是 SM 是否有足够多的 ready warp，以及资源是否均衡。

---

## 7. SM 内部如何执行多个 Warp

假设一个 256-thread block 被分成 8 个 warp，并且驻留在 SM 0：

```text
SM 0 / Block 0
├── Warp 0
├── Warp 1
├── Warp 2
├── Warp 3
├── Warp 4
├── Warp 5
├── Warp 6
└── Warp 7
```

SM 的 warp scheduler 会从这些 warp 中选择 ready warp：

```text
Cycle 0: 发射 Warp 0 的指令
Cycle 1: 发射 Warp 3 的指令
Cycle 2: Warp 0 等待 global memory，发射 Warp 5
Cycle 3: 发射 Warp 1
Cycle 4: Warp 3 继续执行
```

这种机制的核心目标是 **latency hiding**：

```text
Warp A 等待内存
        ↓
SM 执行 Warp B
        ↓
SM 执行 Warp C
        ↓
Warp A 的数据返回，继续执行
```

GPU 的高吞吐能力主要依赖：

- 同时驻留大量 warp；
- 在 ready warp 之间快速切换；
- 用其他 warp 的计算隐藏内存访问延迟。

这与 CPU 主要依赖大型乱序执行窗口的方式不同。GPU 当然存在硬件依赖跟踪和指令调度，但更典型的并发手段是大量 warp 的交错执行。

---

## 8. GPU 中的“乱序执行”应如何理解

“GPU 会不会乱序执行”不能只回答“会”或“不会”，需要按照层次区分。

### 8.1 同一个 Warp 内

同一个 warp 的必要数据依赖必须得到满足。例如：

```cpp
a = load(x);
b = a + 1;
store(y, b);
```

不能违反 `load -> add -> store` 的可观察依赖。

编译器和硬件可以安排没有依赖的指令，但不能破坏程序要求的依赖关系。GPU 也会跟踪寄存器和执行资源是否 ready。

### 8.2 不同 Warp 之间

不同 warp 的执行顺序通常没有保证：

```text
Warp 0 可能先执行
Warp 1 可能先执行
也可能两个 warp 交错执行
```

当一个 warp 等待内存时，scheduler 可以切换到另一个 warp。程序不能把 warp ID 当作执行先后关系。

### 8.3 不同 Block 之间

不同 block 也没有默认的执行或完成顺序：

```text
Block 1 可能先于 Block 0 完成
Block 7 可能在 Block 2 之前被调度
```

即使 block ID 较小，也不代表它一定先运行。block 可能因为 SM 资源、其他 kernel 或调度策略而延迟。

### 8.4 不同 Stream 之间

同一个 stream 内通常保持提交顺序；不同 stream 之间如果没有显式依赖，通常允许并发：

```text
stream 1: H2D copy
stream 2: kernel
stream 3: D2H copy
```

是否真正并发，还要看：

- 是否使用不同的硬件 engine；
- compute/copy engine 是否可并发；
- channel 和上下文资源；
- memory bandwidth；
- stream/event 依赖；
- 默认 stream 语义。

---

## 9. GPU 并发的四个层次

### 9.1 Warp-level 并发

一个 SM 同时驻留多个 warp，并在它们之间交错发射指令：

```text
Warp 0 等待内存 -> 执行 Warp 1
Warp 1 等待依赖 -> 执行 Warp 2
```

### 9.2 Block-level 并发

一个 SM 同时驻留多个 block：

```text
SM 0:
    Block 0 + Block 4
```

这些 block 的 warp 可以交错执行，资源允许时也可以利用不同执行管线。

### 9.3 SM-level 并发

多个 SM 同时执行不同 block：

```text
SM 0 -> Block 0
SM 1 -> Block 1
SM 2 -> Block 2
SM 3 -> Block 3
```

这是 GPU 大规模数据并行的主要来源。

### 9.4 Engine/Stream-level 并发

不同 CUDA stream 的任务可以被提交到不同 engine 或 channel：

```text
Copy engine:
    H2D copy

Compute engine:
    kernel

另一个 Copy engine:
    D2H copy
```

如果任务之间存在依赖，driver 会插入底层控制命令，例如 semaphore release/acquire、wait-value、memory barrier 或 event completion。

---

## 10. 为什么普通 Block 之间没有全局隐式同步

假设一个 kernel 有 1000 个 block，但 GPU 只有 80 个 SM。某个 block 写数据后，另一个 block 读取数据：

```text
Block A: 写 global memory
Block B: 读 global memory
```

不能只依赖 block ID 推断顺序。更危险的是，如果 Block B 先驻留并进入等待，而 Block A 尚未被调度，可能导致资源占用和死锁问题：

```text
Block B 已经占用资源
Block B 等待 Block A
Block A 因资源不足无法启动
```

因此普通 CUDA kernel 通常只提供：

- warp 内的相应协作机制；
- block 内的 `__syncthreads()` 等同步；
- 通过 global memory 和 atomic 组织更高层协议，但需要开发者正确设计；
- kernel launch 边界或显式 stream/event 依赖实现全局任务级顺序。

常见的全局阶段拆分方式是：

```text
Kernel A:
    所有 block 生成中间结果

Kernel A 完成

Kernel B:
    所有 block 消费中间结果
```

或者在同一个 stream 中提交：

```text
Kernel A <<<..., stream>>>(...);
Kernel B <<<..., stream>>>(...);
```

同一个 stream 的顺序由 driver 和 GPU command queue 保证。

---

## 11. GPU 内部同步与 Driver 控制同步的边界

GPU 内部已经具备多种同步/排序原语，但它们解决的问题不同。

### 11.1 Warp-level 数据交换

例如：

```cpp
__shfl_sync(...);
```

用于 warp 内 lane 之间交换寄存器数据。

### 11.2 Block-level execution barrier

例如：

```cpp
__syncthreads();
```

用于同一个 block 内的 thread 到达共同执行点后再继续。

### 11.3 Memory fence

例如：

```cpp
__threadfence_block();
__threadfence();
__threadfence_system();
```

主要用于保证写入顺序和可见性。它本身不等于让其他线程停在某个条件上等待。

### 11.4 Atomic

例如：

```cpp
atomicAdd(&counter, 1);
```

用于原子更新共享状态，但完整的生产者-消费者协议通常还需要：

- 对数据写入的可见性保证；
- 对 flag/counter 的正确发布顺序；
- 消费方的条件等待；
- 必要的 memory scope。

### 11.5 Driver/command-level semaphore

driver 需要协调的对象通常是：

```text
copy engine 完成
compute engine 才能启动
stream A 的 event 完成
stream B 才能继续
staging buffer 被释放
另一个 context 才能使用资源
```

这类依赖不是一个 kernel 内的 thread barrier，而是多个 command queue 或 engine 之间的任务依赖。driver 会把它翻译成硬件可执行的 wait/release/fence 等 command。

因此应区分：

```text
kernel 内同步：GPU ISA / 编译器 / SM 处理
kernel 之间同步：driver / command processor / semaphore 处理
```

---

## 12. 与 CUDA Stream、Channel 和 Control Path 的关系

可以把整个执行链条串起来：

```text
CUDA API
  └── kernel launch / memcpy / event
       └── CUDA Stream（逻辑时间线）
            └── Channel（硬件 command queue 的软件抽象）
                 └── GPU Engine（compute/copy 等）
                      └── SM
                           └── Block
                                └── Warp
                                     └── Thread
```

例如：

```cpp
cudaMemcpyAsync(d, h, n, cudaMemcpyHostToDevice, stream1);
cudaEventRecord(event, stream1);
cudaStreamWaitEvent(stream2, event, 0);
kernel<<<grid, block, 0, stream2>>>(d);
```

driver 可能把它组织为：

```text
Copy channel / stream1:
    DMA h -> d
    semaphore release

Compute channel / stream2:
    semaphore acquire
    kernel launch
```

kernel launch 被 GPU 接受后，才会进入 block-to-SM 的调度阶段：

```text
Kernel grid
├── Block 0 -> 某个可用 SM
├── Block 1 -> 某个可用 SM
├── Block 2 -> 某个可用 SM
└── ...
```

这里有三个不同的问题：

1. **stream/channel 层：** kernel 什么时候允许提交和开始？
2. **SM 层：** block 什么时候能获得资源并驻留？
3. **warp 层：** SM 当前选择哪个 ready warp 发射指令？

Control stream 或 control command 主要解决第一个问题，并不直接指定 block 到哪个 SM。

---

## 13. 一个完整的示例

假设：

```cpp
kernel<<<10, 256>>>(...);
```

则：

```text
Grid:
    10 Blocks

每个 Block:
    256 Threads
    = 8 Warps

整个 Kernel:
    10 × 256 = 2560 Threads
    10 × 8   = 80 Warps
```

如果 GPU 有 4 个 SM，并且由于寄存器、shared memory 等限制，每个 SM 最多同时驻留 2 个该 block：

```text
第一批：
SM 0 -> Block 0, Block 4
SM 1 -> Block 1, Block 5
SM 2 -> Block 2, Block 6
SM 3 -> Block 3, Block 7
```

剩余：

```text
Block 8, Block 9
```

某些 block 完成后，资源释放：

```text
SM 0 -> Block 8
SM 1 -> Block 9
```

实际映射可能不同；普通程序不应假设上面的具体编号关系。可以依赖的只是更抽象的规则：

- block 的所有 thread 通常共同驻留在一个 SM；
- 一个 SM 可以同时承载多个 block；
- block 完成后，后续 block 才能获得释放的资源；
- 不同 block 的执行/完成顺序通常不保证。

---

## 14. 对 GPU driver 设计的启示

### 14.1 Driver 管的是任务级顺序，不是每个 thread 的调度

driver 通常不负责决定：

```text
Block 0 的 Warp 3 在哪个周期执行
```

这属于 GPU 硬件的 SM/warp scheduler。

driver 主要负责：

```text
哪个 kernel/copy 被提交
提交到哪个 stream/channel
不同任务之间是否有依赖
何时 flush command queue
哪个 context 可以运行
如何处理 event、semaphore、memory residency
```

### 14.2 Stream 不等于 SM

stream 是逻辑任务队列；一个 stream 中的 kernel 可能使用多个 SM。一个 SM 也可能同时执行来自不同 stream 的 block。

```text
Stream A -> Kernel A -> 多个 SM
Stream B -> Kernel B -> 多个 SM
```

如果 A 和 B 没有依赖，它们的 block 可能在同一时间共享 GPU 资源；如果资源或调度策略不允许，也可能被串行化。

### 14.3 Channel 不等于 SM

channel 是命令提交和排序的硬件队列抽象，SM 是执行 kernel block 的硬件单元。一个 compute channel 可以向整个 GPU 提交 kernel；kernel 再由硬件分发到多个 SM。

```text
Compute channel
    -> Kernel launch command
        -> GPU scheduler
            -> 多个 SM
```

### 14.4 同步命令不是 payload

`semaphore acquire/release`、event signal、memory barrier 等 control command 不执行用户 kernel 的业务计算，但它们决定 payload 是否可以安全开始、结束或复用资源。

可以把它们理解成：

```text
payload command：做实际工作
control command：规定实际工作什么时候可以做
```

---

## 15. 常见误区

### 误区一：一个 CUDA thread 对应一个 GPU core

不准确。CUDA thread 是逻辑执行实体，通常以 warp 为单位被 SM 调度。

### 误区二：一个 block 对应一个固定 SM

不准确。block 通常在一个 SM 上驻留，但具体是哪一个 SM 由硬件动态决定，且不同时间可能不同。

### 误区三：一个 SM 同一时间只能执行一个 block

不准确。只要寄存器、shared memory、thread 和其他资源允许，一个 SM 可以同时驻留多个 block。

### 误区四：block ID 越小就越先执行

不应依赖。block 的启动和完成顺序通常不向普通 kernel 提供保证。

### 误区五：`__threadfence()` 会等待所有 thread

不准确。fence 主要是内存顺序/可见性机制，不等于全局执行 barrier。

### 误区六：`__syncthreads()` 可以同步整个 grid

不准确。普通情况下它只同步同一个 block 内的 thread。

### 误区七：不同 stream 一定并发

不准确。不同 stream 允许并发，但最终还受硬件 engine、SM 资源、依赖关系、带宽和调度策略影响。

### 误区八：GPU 有同步指令，所以 driver 不需要 control path

不准确。GPU 有底层同步原语，但 event、stream、跨 engine、跨 context、资源生命周期等高级语义仍需要 driver 编排和翻译。

---

## 16. 结论

### 16.1 层次关系

```text
Grid
  -> Block
      -> Warp
          -> Thread

GPU
  -> SM
      -> Resident Block
          -> Warp
              -> Thread
```

### 16.2 执行关系

```text
一个 Block 通常整体驻留在一个 SM
一个 SM 可以同时驻留多个 Block
一个 Block 包含多个 Warp
一个 Warp 通常包含 32 个 Thread
SM 以 Warp 为主要调度单位
```

### 16.3 并发关系

```text
warp 之间可以交错执行
block 之间可以在多个 SM 上并行
多个 block 可以共享一个 SM 的资源
多个 stream 可以在不同 engine 或 SM 资源上并行
没有显式依赖时，不应假设不同 warp/block/stream 的完成顺序
```

### 16.4 Driver 关系

```text
Stream：定义用户可见的逻辑顺序
Channel：承载 command queue
Control command：表达任务间 wait/release/fence
Engine：执行 compute/copy 等任务
SM：承载 kernel block
Block：组织可协作 thread
Warp：硬件调度 thread group
Thread：CUDA 编程模型中的逻辑执行实体
```

最核心的理解是：

> **SM 是执行资源，Block 是资源分配和协作边界，Warp 是硬件调度单位，Thread 是编程模型中的逻辑线程。**
>
> **Stream/channel 解决任务级提交与依赖；SM/Block/Warp 解决 kernel 被提交之后如何在 GPU 上执行。**

这两个层次相互连接，但不能混为一谈。
