# Module 与 Kernel 执行：设计分析

- 文档目的：module kernel：设计意图、职责边界与关键取舍。
- 适用范围：M07 模块的《Module 与 Kernel 执行：设计分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 设计问题

该模块把 `module load -> function -> launch params -> DispatchCommand` 拆成稳定边界：driver 入口负责契约与校验，`Module`/`Function` 负责符号与状态，`GraphKernelNode` 负责 launch 参数规范化，`DispatchCommand` 负责 HAL cmd buffer 构建，M3D 负责最终调度。这样减少了公共 ABI 对硬件实现的直接依赖，但也形成多层错误传播和生命周期耦合。

## 关键机制分析

- **惰性加载**：`LoadImage` 用 `std::call_once` 保证单次解析，非 eager 模式在 `FindSymbolByName`/`GetFunction` 时按需触发；失败会 `DestroyModule`。[src/musa/core/module.cpp:187-233] [src/musa/core/module.cpp:235-256]
- **函数与 HAL kernel 分离**：`Function` 是符号表中一个 `SymbolBase_t`，`Init()` 才创建 `Hal::IKernel`（含 pipeline），未使用前不产生 HAL 成本。[src/musa/core/symbol.cpp:434-452]
- **launch 参数三层组装**：driver 组装 `MUSA_KERNEL_NODE_PARAMS` -> `UpdateParams` 校验并生成 `KernelState` -> `Build/UpdateImplicitResources` 把隐式资源写入 `KernelState`，最后 `CmdDispatch` 侧用 `GetResourceBuffer` 生成 const buffer。[src/musa/core/node/graphKernelNode.cpp:57-131] [src/musa/core/command/dispatchCommand.cpp:773-1053] [src/hal/m3d/cmdBuffer.cpp:242-345]
- **graph 与 stream 共用一条 launch 路径**：capture 激活时 `GraphKernelNode` 入 capture 图，否则转 `CmdLaunchKernel`；两种模式共用同一节点/命令结构。[src/musa/core/context.cpp:666-673]

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| 分层而非 API 直达硬件 | 可替换 HAL、集中管理对象 | 调用链长、排查成本高 | 已确认 |
| 惰性加载/懒创建 kernel | 降低启动开销、按需创建 | 首次调用承担失败和锁开销 | 已确认于 LoadImage/Function::Init |
| kernelParams/extra 双参数入口 | 兼容 CUDA 风格两种传参 | extra 要求完整 buffer 布局，校验薄弱 | 已确认于 CreateState |
| 隐式资源集中到 KernelState | 编译器/HAL 可扩展新槽位 | 类型枚举膨胀，unknown 类型仅打日志 | 已确认于 UpdateImplicitResources |
| 对象池化 DispatchCommand | 减少分配开销 | 生命周期交给 shared_ptr deleter，易错 | 已确认于 CreatePooledDispatchCommand |
| 加载模式（eager/lazy）可配置 | 适配不同启动场景 | 行为随平台设置变化 | 已确认于 platformSettings |

## 不变量

1. 公共句柄在使用前必须能映射到有效对象：`muapiModuleLoadData`/`muapiModuleGetFunction` 先校验再转换。[src/driver/mu_module.cpp:126-133] [src/driver/mu_module.cpp:161-167]
2. Core 资源登记成功前，底层初始化失败不得暴露半初始化对象：`CreateModule` 先 `RecordModule`，`LoadFatBinary` 失败 `DestroyModule` 回滚。[src/musa/core/context.cpp:1280-1286] [src/driver/mu_module.cpp:134-141]
3. 异步操作的资源必须活到对应 Command 完成：`m_KernelResource` 以 `shared_ptr` 挂在 Command 上直到 `ReleaseResources`。[src/musa/core/command/dispatchCommand.cpp:1277-1292]
4. 销毁路径必须先处理未完成工作，再释放 HAL 对象：`DispatchCommand::~DispatchCommand` 先 `Wait()`；`Module` 析构先清符号再 `Destroy` HAL library。[src/musa/core/command/dispatchCommand.cpp:57-64] [src/musa/core/module.cpp:174-185]

以上第 1、2、4 项由多处校验/析构路径支持；第 3 项是异步资源管理的实现约束，完整证明仍需覆盖所有 Command。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/module.cpp`（3 处，例：[src/musa/core/module.cpp:187-233]）
- `src/musa/core/command/dispatchCommand.cpp`（3 处，例：[src/musa/core/command/dispatchCommand.cpp:773-1053]）
- `src/driver/mu_module.cpp`（3 处，例：[src/driver/mu_module.cpp:126-133]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
