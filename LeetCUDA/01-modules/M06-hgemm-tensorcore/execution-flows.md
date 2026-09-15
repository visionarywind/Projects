# M06 执行流：HGEMM 的控制、数据、资源与错误

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/execution-flows.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/execution-flows.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 主控制流

```text
D01/CLI: hgemm.py
  1. 解析 M/N/K、算法开关、warmup/iters
  2. 选择或构建扩展
  3. 创建输入/输出 tensor
  4. 按 tag 调用 PyBind 导出
  5. warmup + synchronize
  6. measured iterations + synchronize
  7. correctness / TFLOPS / optional plot
```

参数入口在 `[kernels/hgemm/hgemm.py:18-177]`，计时和 handle 生命周期在 `[kernels/hgemm/hgemm.py:210-328]`。上述 1-7 是 **已确认**；某个 tag 最终实例化哪个模板，要继续读对应 PyBind/launcher。

## 2. 代表 MMA stage 的数据流

| 阶段 | 输入/状态 | 设备位置 | 主要操作 | 证据 |
|---|---|---|---|---|
| A | A/B half tensor | global memory | 由线程按 tile 计算 global 地址 | `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:663-674]` |
| B | K-stage 空间 | shared memory | `cp.async` 搬运 A/B 并 commit/wait | `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-729]` |
| C | 当前 stage fragment | registers | `ldmatrix` 取 A/B fragment | `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:769-839]` |
| D | RC accumulator | registers | `mma.sync.m16n8k16` 累加 | `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:769-839]` |
| E | fragment regroup | registers/warp | `__shfl_sync` 交换值 | `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1850-1885]` |
| F | output tile | global memory | lane-0-of-four 128-bit store | `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1850-1885]` |

## 3. Stage pipeline 时序

```text
预取初始化:
  cp.async stage 0 ... stage K_STAGE-2
  commit_group
  wait_group(K_STAGE-2)
  __syncthreads()

每个 k:
  ┌─选择下一 shared stage (k+1)%K_STAGE
  ├─cp.async 下一 A/B + commit
  ├─ldmatrix 当前 stage k%K_STAGE
  ├─HMMA 当前 fragments
  └─下一轮 wait/sync 后复用 stage
```

源码中的 `smem_sel`、`smem_sel_next`、register index flip 和 `CP_ASYNC_WAIT_GROUP` 共同构成软件流水。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-839] **已确认**。该图表达控制依赖，不表示硬件一定实现固定时延。

## 4. 正常路径的同步点

1. `cp.async` group wait：保证异步搬运达到所需完成度；
2. `__syncthreads()`：让 block 内线程共同读取/复用 shared stage；
3. warp-level `ldmatrix/HMMA`：依赖符合 PTX participation/layout；
4. `__shfl_sync`：依赖指定 mask 的 lane 同时参与；
5. Python 侧 `torch.cuda.synchronize()`：将 benchmark 计时边界从异步 kernel 变成可观察完成点。[kernels/hgemm/hgemm.py:210-328]

**未知**：代表 PyBind kernel 是否在每次 launch 后显式调用 `cudaGetLastError`；应按具体导出函数继续核对。`torch` 后续操作可能触发同步，但不等于清晰的 launch-error 契约。

## 5. 资源流和生命周期

```text
PyTorch input tensor ──借用 data_ptr──> kernel
PyTorch output tensor ──由 allocator 持有──> kernel writes C
扩展 module ──cache/进程生命周期──> subsequent calls
cuBLAS handle ──benchmark init──> calls ──destroy──> end
```

源码明确记录了 cublas tag 初始化/销毁和 measured sync；普通 tensor 的释放由 PyTorch 生命周期管理。[kernels/hgemm/hgemm.py:210-328] **已确认**。standalone 路径的 buffer/event 生命周期不能从 Python 代码推断，需看对应 makefile/C++ 文件。

## 6. 失败路径审计

| 失败点 | 源码可确认的现象 | 状态 |
|---|---|---|
| 未安装 wheel/动态编译失败 | `utils.py` 有 wheel 优先、源码 fallback 逻辑 | 已确认 |
| 不匹配的 arch/指令 | setup/source flags 与 WMMA/MMA/WGMMA 变体相关 | 已确认风险 |
| dynamic SMEM 超限 | launcher 请求 attribute 和动态字节数 | 已确认风险；运行结果未知 |
| 非 tile 尾部 | 模板存在边界路径但代表配置未由本文证明 | 未知 |
| async launch failure | benchmark 有同步，但每个 kernel 的显式 error check 不统一 | 未知 |
| 数值误差 | benchmark 有 reference/TFLOPS；阈值和变体差异需看实现 | 部分完成 |

## 7. 清理路径

- Python 输入/输出和 extension 临时对象：由 PyTorch 引用计数/allocator 管理；**已确认总体模型**。
- cuBLAS handle：脚本 benchmark 显式销毁；**已确认**。
- standalone device/host/event：必须依赖具体 C++ benchmark 的 `cudaFree/free/event destroy`；M06 总体入口没有统一 RAII 契约；**部分完成**。

## 8. Demo 映射

- D01（NMS）不直接调用 M06，但展示同一 PyTorch extension 边界和异步 kernel 的观察方法。
- D02（Interview）展示显式 host/device 生命周期和 HGEMM benchmark；对应 `[kernels/interview/notes-v2.cu:2420-2579]`。
- M06 自身实际入口由 `hgemm.py` 驱动；当前未执行命令全部标为“未验证”。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
