# KV Pool 生命周期与 Ownership

## 1. 端到端生命周期

```mermaid
sequenceDiagram
    participant P as Profile/Config
    participant MR as ModelRunner
    participant SCH as Scheduler
    participant ROW as ReqToTokenPool
    participant AL as Token/Page Allocator
    participant PHY as Physical KV Pool
    participant RAD as RadixCache
    participant ATT as Attention Backend

    P->>MR: max_total_tokens/page_size/dtype
    MR->>ROW: create request-row tensor
    MR->>AL: create free slot/page list
    MR->>PHY: create K/V backing tensors
    SCH->>RAD: bind row pool + allocator
    SCH->>RAD: prefix match
    SCH->>ROW: allocate/reuse request row
    SCH->>AL: allocate extend/decode slots/pages
    SCH->>PHY: write K/V at out_cache_loc
    ATT->>PHY: read K/V via indices/view
    SCH->>RAD: cache finished/unfinished prefix
    RAD->>AL: free duplicate/uninserted/evicted pages
    SCH->>ROW: free request row
```

图中：`ROW` 管 request 到 token location 的映射；`AL` 管 slot/page index；`PHY` 管真实 K/V tensor；`RAD` 只接管可复用 prefix 的索引引用。同步/异步边界取决于 scheduler loop、attention backend 和 transfer backend；本专题未执行真实 GPU 时序验证。

## 2. 关键 ownership 表

| 对象 | 实际 owner | 借用者 | 归还/释放点 | 不变量 | 破坏症状 |
|---|---|---|---|---|---|
| request row | `ReqToTokenPool` | `Req`、`ForwardBatch` | 请求结束、chunk 释放、flush | row id 不指向已释放请求 | 新请求读到旧 token location |
| KV slot/page index | allocator | Req、Radix node、backup/transfer | free、evict、retract、flush | 一个 index 同时只被合法 owner 集合引用 | double free、泄漏、UAF |
| 物理 K/V tensor | KVCache/ModelRunner | attention/backend/graph | pool teardown 或全局 flush | address/layout/dtype 与 backend 契约一致 | kernel 读错、graph replay 失败 |
| Radix node value | RadixCache | prefix match、eviction、HiCache | evict/delete/lock release | `lock_ref` 与请求/host 引用配对 | cache 永不淘汰或提前释放 |
| host/offload page | host pool/transfer layer | restore/prefetch | restore 完成、host eviction | device/host page id 显式映射 | 恢复错页、transfer race |
| CUDA Graph static buffer | graph runner | graph replay | runner teardown | capture batch/shape/LoRA/stream key 匹配 | fallback 或非法访问 |

## 3. 分配主线

### 3.1 容量预算

[已确认] SGLang 文档描述 ModelRunner 会先 profile 可用显存，再把用户 `max_total_tokens`、PP rank 最小容量和 `max_running_requests` 等约束投影到 memory pool config。来源：`../sglang/90-cross-module/pooling-and-resource-management.md`。

推荐面试表达：容量预算不是单个公式，而是以下约束的交集：

```text
available_hbm
  - model_weights
  - runtime_activations/reserved_workspace
  - cuda_graph_static_buffers
  - safety_margin
=> physical_kv_budget
=> max_total_tokens/page_count
=> admission budget per batch/request
```

### 3.2 Prefix match 与 extend

[已确认] prefix 命中的 KV indices 写入请求 row，未命中的 token 才需要 allocator 分配新位置；`prefix_indices` 与 `extend_range` 用于区分已拥有/命中和需要写入的区间。来源：`../../../sglang/01-modules/M08-kv-cache/README.md`。

### 3.3 Decode 增长

decode 每步通常为运行中请求追加新 token 的 KV。若 allocator 可用量不足，scheduler 可能执行 retraction/abort，把部分请求从 running batch 移出并释放或备份资源。

[待验证] 本仓库未运行 tiny-pool decode retraction 实验；相关行为仅按文档和源码引用分析。

## 4. 回收、淘汰与 flush

| 路径 | 触发 | 应更新的状态 | 容易漏掉的边界 |
|---|---|---|---|
| finished request cache | 请求完成 | row、Radix node、allocator duplicate/tail、lock_ref | 已插入 prefix 和未插入 tail 的分界 |
| unfinished/chunked cache | 分块 prefill 暂停 | row rematch、protected len、prefix indices、node lock | partial page 不能提前释放 |
| eviction | 可驱逐容量不足或显式清理 | Radix leaf、allocator free segment、metrics/event | lock_ref>0 不可驱逐 |
| retraction | decode 内存不足 | running batch、request state、allocator/host backup、abort | 释放前后差值与恢复失败处理 |
| flush | engine idle | Radix tree、request pool、KV allocator、可选 device cache | 非 idle 时强清会造成悬挂访问 |

## 5. Page size 与 partial page

分页减少外部碎片，但 page size 变大后，会带来 page 内部浪费和 partial page 生命周期问题：

```text
page_size = 16
sequence length = 35
full pages: [0..15], [16..31]
partial tail: [32..34]  # 可能尚不能作为完整 radix node 释放/复用
```

[推断] 如果系统把 partial tail 当成完整可共享 prefix，会造成后续请求读取未定义或错误的 K/V；如果永远不处理 partial tail，则表现为可用 token 低于预期。

## 6. 故障树

```mermaid
flowchart TD
    OOM[decode OOM / KV 不足] --> A{allocator available 是否真实下降?}
    A -->|是| B[admission/reservation 过高]
    A -->|否| C[free/release 统计不一致]
    C --> D[Radix lock/ref 泄漏]
    C --> E[deferred release 未 merge]
    C --> F[host backup/device value 状态不一致]
    OOM --> G{P99 是否同时上升?}
    G -->|是| H[offload restore 或 eviction 阻塞]
    G -->|否| I[容量碎片或特定长度分布触发]
```

排查时先记录：`free pages`、`release pages`、`evictable/protected size`、running/waiting request 数、prefix hit、retract 次数、abort 原因、GPU memory snapshot 和 transfer time。

## 7. 修改影响清单

修改 `page_size`、allocator free logic、prefix key、Radix eviction 或 offload/restore 时，必须联查：

- admission 是否仍以同一粒度计算预算；
- `Req` 的 prefix length、cache protected length 和 output length 是否同步；
- attention backend 是否接受新 layout；
- CUDA Graph capture/replay 是否仍满足静态 shape/address；
- abort/retraction/flush 是否覆盖同样的 owner 集合；

## 8. 本轮静态与动态验证结果

### 8.1 环境前置条件

本机检查结果：

- `[未验证]` `nvidia-smi` 不可用（命令不存在）；无法确认 NVIDIA GPU 和驱动。
- `[已确认]` 已安装 `torch==2.14.0+cu130`，但 `torch.cuda.is_available()` 为 `False`、设备数为 `0`；因此本轮仍无法执行 CUDA tensor、显存快照、真实 GPU KV buffer 或 CUDA Graph replay。
- `[已确认]` 已安装 `pytest==9.1.1` 以及本次导入所需的一组 SGLang 依赖，并通过 `PYTHONPATH=python` 导入源码。
- `[已确认]` `source/sglang` 中存在 memory-cache 测试集合，包括 Radix、lock/ref、eviction、SWA、Mamba、Unified pool 和 storage/HiCache 测试；“存在测试文件”不等于测试全部适用当前环境。

### 8.2 已执行的测试尝试

使用命令 `PYTHONPATH=python python3 -m pytest <test> -q` 执行了 targeted 测试：

- `[部分已验证]` `test_radix_cache_unit.py`：35 passed，1 deselected，23 subtests passed；排除的 `test_memory_allocated` 需要 CUDA。未排除时为 35 passed、1 failed，失败原因是 `RuntimeError: Found no NVIDIA driver on your system`，不是 Radix 逻辑断言失败。
- `[已验证]` `test_evict_policy.py`：23 passed。
- `[未执行]` `test_decode_radix_lock_ref.py`：收集阶段调用 `get_device()`，因没有任何 accelerator 而退出；不是用例断言失败。
- 之前的测试启动阻塞已逐层记录：最初缺少 `pytest`，随后补齐导入依赖；这些安装动作只改变本地 Python 环境，不改变源码。

因此，当前已获得 CPU-safe Radix/eviction 测试的通过记录，但不能把它外推为 GPU physical backing、decode retraction 或 CUDA Graph 已验证。

### 8.3 未完成验证及所需环境

| 内容 | 当前状态 | 需要补齐 |
|---|---|---|
| allocator/Radix correctness（CPU-safe 路径） | `[部分已验证：Radix 35 passed、eviction 23 passed]` | 继续覆盖其余 CPU-safe 测试并固定依赖版本 |
| GPU physical backing | `[待验证：当前无可用 accelerator]` | NVIDIA GPU、驱动、PyTorch/CUDA |
| CUDA Graph capture/replay | `[待验证：当前无可用 accelerator]` | 支持 Graph 的 GPU、torch、模型和可复现实例 |
| prefix hit/eviction（CPU-safe 单元路径） | `[部分已验证]` | 补充适用测试；GPU/真实 scheduler 场景仍需硬件 |
| decode lock/ref/retraction | `[待验证：测试收集即因无 accelerator 退出]` | 可用 accelerator 和对应 runtime |
| CPU offload/partial recompute | `[待验证]` | 对应 backend、模型/张量和带宽实验 |
| P/D transfer/RDMA | `[待验证]` | 多进程、多设备和网络/RDMA 环境 |
| throughput/TTFT/TPOT/P99 | `[待验证]` | 固定模型、输入分布、并发、warmup、重复次数和 baseline |

### 8.4 状态纪律

本轮没有把任何论文性能数字或动态运行行为改成“本地已验证”。新增的通过记录仅限明确列出的 CPU-safe 单元测试；GPU、CUDA Graph、真实 decode/retraction、offload、网络/RDMA 和压测结果继续标为 `[待验证]`。论文结果仍是作者报告；源码结论仍区分 `[已确认]` 与 `[推断]`。
