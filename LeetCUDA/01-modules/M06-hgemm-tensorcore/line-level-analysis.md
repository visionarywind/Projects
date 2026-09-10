# M06 行级分析与审计

## 1. 代表符号索引

| 源码锚点 | 符号/区域 | 解释 | 状态 |
|---|---|---|---|
| `kernels/hgemm/hgemm.py:18-177` | CLI 参数与算法开关 | 决定 benchmark 要比较的 kernel family | 已确认 |
| `kernels/hgemm/tools/utils.py:18-32` | `get_build_sources()` | 将多个实现加入同一扩展源集合 | 已确认 |
| `kernels/hgemm/tools/utils.py:109-147` | build/load helper | wheel 优先，失败后源码构建 | 已确认 |
| `kernels/hgemm/pybind/hgemm.cc:7-181` | PyBind declarations/exports | ABI 边界；函数名区分变体 | 已确认 |
| `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-142` | staged kernel template + static SMEM | 128×128 tile、MMA atom、stage/padding 参数 | 已确认 |
| `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:632-686` | dsmem kernel setup | dynamic SMEM、thread/warp mapping、fragment arrays | 已确认 |
| `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-729` | async prefetch | commit/wait/sync 初始化流水 | 已确认 |
| `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:769-839` | main K loop | stage parity、ldmatrix、HMMA、next-tile prefetch | 已确认 |
| `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1850-1885` | collective store | warp shuffle 和 128-bit output store | 已确认 |
| `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1900-1959` | launcher macros | SMEM attribute、grid/block、stage dispatch | 已确认 |
| `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:536-567` | architecture/store branch | sm90 `stmatrix` 源码声明为未测试 | 已确认声明/行为未知 |
| `kernels/hgemm/hgemm.py:210-328` | benchmark | warmup/sync、handle、TFLOPS | 已确认 |

## 2. 深度审计表

| 审计项 | 当前结论 | 证据/缺口 | 状态 |
|---|---|---|---|
| 入口是否落地到具体实现 | 已从 Python → source builder → PyBind → staged kernel 追到 HMMA/store | 上表全部锚点 | 已完成 |
| 正常路径 | A/B global → cp.async staged SMEM → ldmatrix registers → HMMA RC → shuffle/store C | `hgemm_mma_stage.cu:691-839,1850-1885` | 已完成 |
| 关键分支 | stage、padding、block/warp swizzle、TN、F16/F32、architecture store | `hgemm.py:18-177`; launcher `1900-1959` | 部分完成 |
| 异常路径 | wheel fallback、SMEM 超限、arch/指令不匹配可识别；具体 exception/launch result 未全覆盖 | `tools/utils.py:109-147`; launcher `1900-1926` | 部分完成 |
| 清理路径 | Python tensor/extension 与 cublas handle 已确认；全部 standalone 资源未统一审计 | `hgemm.py:210-328` | 部分完成 |
| 数据生命周期 | PyTorch tensor 借用；A/B stage；RA/RB/RC；C store | `hgemm_mma_stage.cu:137-142,678-686` | 已完成 |
| 执行上下文 | warp/block sync、async copy、Python synchronize 已记录；current stream 未逐函数确认 | `hgemm_mma_stage.cu:691-839`; `hgemm.py:210-328` | 部分完成 |
| 行级证据 | 代表实现和 launcher 有当前提交行号 | 变体仍需逐文件 | 部分完成 |
| Demo 映射 | D02 覆盖 interview HGEMM；D01 仅覆盖扩展边界 | 见 `80-demos/` | 部分完成 |
| 总体状态 | 代表 staged MMA 已完成深度分析，kernel family 全矩阵未完成 | 变体表与未验证清单 | 部分完成 |

## 3. 必须避免的过度推断

- 有 `stage` 模板参数不等于任意 K 尺寸都安全。
- 有 `cudaFuncSetAttribute` 不等于当前 GPU 一定接受 98304 bytes。
- 有 `stmatrix` 编译分支不等于 sm90 已运行；源码明确标注未测试。
- benchmark 中的历史 TFLOPS 不能替代本机 correctness、sanitizer 或 profile。
- 同为 HGEMM 的 NN/TN、WMMA/MMA/CuTe/WGMMA 不能共用一张未经核对的 layout 图。
