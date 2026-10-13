# Module 与 Kernel 执行：数据结构与所有权

- 文档目的：module kernel：核心数据结构、字段含义与持有关系。
- 适用范围：M07 模块的《Module 与 Kernel 执行：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 结构关系

```text
MUmodule 句柄 -> Module(IModule)
  └-> m_SymbolMap: unordered_map<string, unique_ptr<SymbolBase_t>>
      ├-> Function(SYMBOL_KIND_KERNEL)  : 持 shared_ptr<IMemory> 程序内存 + Hal::IKernel*
      └-> VariableSymbol(SYMBOL_KIND_VARIABLE) : 持 shared_ptr<IMemory> + texture/surface ref

MUfunction 句柄 -> Function
MUkernel 句柄 -> Kernel(IKernel)  : 持 per-device ELF/User attribute 数组，隶属 Library
Library  : m_ContextToModuleMap + m_KernelMap + m_ArchToCodeMap
```

## Module 内部字段

`m_ParentCtx`(Context*)、`m_pHalLibrary`(Hal::ILibrary*)、`m_FatBinary`、`m_ModuleId`、`m_IsModuleLoaded`(once_flag)、`m_SymbolLock`、`m_LoadResult`、`m_SymbolMap`。[src/musa/core/module.h:63-71]

- `m_IsModuleLoaded` + `std::call_once` 保证 `LoadImage` 只解析一次。[src/musa/core/module.cpp:187-233]
- `m_SymbolMap` 预留容量为 4 倍符号数、load_factor 0.75，符号名->唯一对象。[src/musa/core/module.cpp:193-199]

## Library / Kernel 字段

- `Kernel`：`m_Name`、`m_ParentLibrary`、`m_AttributesLock`、按设备索引的 `m_ElfAttributes`/`m_UserAttributes`（`std::vector<std::array<Entry, MU_FUNC_ATTRIBUTE_MAX>>`）。[src/musa/core/library.h:39-52]
- `Library`：`m_ModulesLock`+`m_ContextToModuleMap`、`m_KernelsLock`+`m_KernelMap`、`m_DeviceCodeLock`+`m_ArchToCodeMap`。[src/musa/core/library.h:90-97]

## DispatchCommand 字段

`m_NodeParams`(MUSA_KERNEL_NODE_PARAMS)、`m_BlockClusterSize`(dim3)、`m_KernelResource`(shared_ptr<KernelResource>)、`m_BufferGpuAddrs/CpuAddrs`、`m_UsePerStreamSpillMemory`、`m_HasTrap`、`m_GpuCopyJobResources`。[src/musa/core/command/dispatchCommand.h:84-96]

- 构造时从 `GraphKernelNode` 拷贝 nodeParams、clusterDim、KernelResource，并按硬件属性决定是否使用 per-stream spill 内存。[src/musa/core/command/dispatchCommand.cpp:10-55]
- `GetKernel/GetGridSize/GetBlockSize` 由 `m_NodeParams` 直接映射。[src/musa/core/command/dispatchCommand.h:37-44]

## KernelResourceMapping（HAL 侧参数布局）

`type/pName/offset/size/align` 加按类型的 union（`deviceVa.isLocal`、`implicit.type` 等）；类型枚举含 `Constant/Struct/Array/DeviceVa/.../Implicit`。[src/hal/halKernel.h:109-139] [src/hal/halKernel.h:42-68]

Implicit 类型枚举定义每个编译器注入的资源槽位（workgroup size/count、spill、print、cluster、program addr、const data 等），DpCe 指针位由 m3d 在 PDS 阶段注入。[src/hal/halKernel.h:70-104]

## 所有权要点

- `Module` 析构先遍历 `m_SymbolMap`：对 VARIABLE 先 `UntrackMemory` 再 reset，最后 `m_pHalLibrary->Destroy()`。[src/musa/core/module.cpp:174-185]
- `Library` 通过 `m_ContextToModuleMap` 持有每个 context 的 Module；`UnloadFromAllContexts` 逐个 `pCtx->DestroyModule(pMod)` 后清空。[src/musa/core/library.cpp:477-489]
- `Context::DestroyModule` 先移除 texture/surface ref、从 critical data 移除、最后 `delete module`。[src/musa/core/context.cpp:1288-1313]
- `DispatchCommand` 经对象池创建（placement new + shared_ptr 自定义 deleter），`m_KernelResource` 在 `ReleaseResources` 时释放。[src/musa/core/command/dispatchCommand.cpp:1347-1360] [src/musa/core/command/dispatchCommand.cpp:1277-1292]

具体模块的字段布局应以对应头文件为准；本文不臆测未读字段。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/module.cpp`（3 处，例：[src/musa/core/module.cpp:187-233]）
- `src/musa/core/command/dispatchCommand.cpp`（3 处，例：[src/musa/core/command/dispatchCommand.cpp:10-55]）
- `src/hal/halKernel.h`（3 处，例：[src/hal/halKernel.h:109-139]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
