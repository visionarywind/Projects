# Module 与 Kernel 执行：测试策略

- 文档目的：module kernel：测试入口、缺口与建议用例（含未验证标注）。
- 适用范围：M07 模块的《Module 与 Kernel 执行：测试策略》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 静态检查

- 核对入口是否出现在导出/accessor 表：`muapiModuleLoad/LoadData/GetFunction/LaunchKernel/ModuleUnload` 与函数属性/全局变量入口。[src/driver/mu_module.cpp:87-146] [src/driver/mu_module.cpp:157-178] [src/driver/mu_module.cpp:231-268]
- 核对失败路径是否阻止半初始化对象进入 tracker：`LoadFatBinary` 失败 `DestroyModule`；`CreateState` 失败 `Hal::KernelState::Destroy`。[src/driver/mu_module.cpp:134-141] [src/musa/core/symbol.cpp:261-264]
- 核对资源创建和销毁是否成对：`CreateModule`/`DestroyModule`、`CreateLibrary`/`Destroy`、`KernelState::Create`/`Destroy`。[src/musa/core/module.cpp:39-42] [src/musa/core/module.cpp:182-185]
- 核对 launch 参数校验覆盖：grid/block 边界、cluster 整除性、动态 shared 上限、TCE warp 倍数。[src/musa/core/node/graphKernelNode.cpp:66-111] [src/musa/core/symbol.cpp:225-259]
- 运行 `git diff --check` 只验证本文档仓库的格式，不验证远端项目。

## 建议用例（按源码分支）

| 场景 | 关注点 | 覆盖位置 |
|---|---|---|
| eager vs lazy 加载 | `LoadImage` 是否单次、惰性失败回滚 | [src/musa/core/module.cpp:187-233] [src/musa/core/module.cpp:235-256] |
| kernelParams 传参 | 每个非 implicit mapping 逐项写入 | [src/musa/core/symbol.cpp:169-192] |
| extra 传参 | BUFFER_POINTER/SIZE/END 解析与缺失错误 | [src/musa/core/symbol.cpp:193-214] |
| 动态 shared 超限 | 上限校验与 PINGHU 补偿 | [src/musa/core/symbol.cpp:225-259] |
| grid/block/cluster 越界 | `MUSA_ERROR_INVALID_VALUE`/`MUSA_ERROR_LAUNCH_OUT_OF_RESOURCES`/`MUSA_ERROR_INVALID_CLUSTER_SIZE` | [src/musa/core/node/graphKernelNode.cpp:66-111] |
| DP-CE kernel | graph 内拒绝、engine-sync 前置检查 | [src/musa/core/node/graphKernelNode.cpp:133-139] [src/musa/core/command/dispatchCommand.cpp:1062-1072] |
| spill 内存 | per-stream vs m3d 分配、单核降级 | [src/musa/core/command/dispatchCommand.cpp:543-593] [src/musa/core/command/dispatchCommand.cpp:196-203] |
| capture 中 launch | 进入 capture 图而非执行 | [src/musa/core/context.cpp:666-673] |
| module unload | 同步后销毁、texture/surface 引用清理 | [src/driver/mu_module.cpp:341-361] [src/musa/core/context.cpp:1288-1313] |

## 运行验证（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

目标环境需要私有子模块、工具链、驱动和 GPU；本知识库没有声称上述命令成功。模块专项用例见 `80-demos` 和远端 `tests` 文件。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_module.cpp`（5 处，例：[src/driver/mu_module.cpp:87-146]）
- `src/musa/core/symbol.cpp`（5 处，例：[src/musa/core/symbol.cpp:261-264]）
- `src/musa/core/module.cpp`（4 处，例：[src/musa/core/module.cpp:39-42]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
