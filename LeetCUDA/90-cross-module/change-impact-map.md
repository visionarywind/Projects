# 变更影响地图

- 对应源码版本：`main` / `0983c65`。
- 证据状态：按源码依赖和入口归纳；模板实例化影响需编译确认。
- 最后更新：2026-09-10

| 修改类型 | 直接影响 | 必查文件/模块 | 必做验证 |
|---|---|---|---|
| 改 Python 参数 | benchmark/shape/编译分支 | 对应 `.py`、README | 默认值、边界、reference |
| 改 binding 签名 | Python ABI、导出符号 | `.cu/.cc`、调用脚本 | dtype/device/shape、import |
| 改 tile/layout | shared/register、结果布局 | kernel、pybind、测试 | 非倍数尺寸、sanitizer、对拍 |
| 改 dtype/accumulator | 数值误差和 Tensor Core 路径 | kernel、reference、阈值 | 多 dtype 和极值输入 |
| 改 NMS sort/mask | 保留顺序和原始索引 | `nms.cu`, `nms.py` | ties、N×seed×threshold |
| 改架构宏 | 可编译性和指令集 | build.sh/setup.py/flags | 每个目标 SM 编译/最小运行 |
| 改 benchmark 计时 | 所有历史比较 | Python/C++ benchmark | warmup/sync/重复次数 |
| 改公共 interview header | 多个 phase | `common.cuh`, `base.cuh`及全部 include | `--arch` smoke + tests |
| 改 third-party submodule | headers/API/生成代码 | `.gitmodules`、构建入口 | 固定 commit、全相关模块构建 |

## 影响半径规则

- 只改单个 kernel 的数学公式，仍要检查 launcher 的 shape/pack 分支。
- 只改文档性能数字，不应改变源码契约；必须标明硬件、版本和是否历史数据。
- 修复异步/同步问题时，检查 tensor 生命周期、current stream、后续 CPU copy 和 benchmark synchronize。

## 相关文档

- [shared-data-and-types.md](shared-data-and-types.md)
- [error-boundaries.md](error-boundaries.md)
- [../00-overview/evidence-index.md](../00-overview/evidence-index.md)
