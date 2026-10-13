# Loader 薄加载层：调用链

- 文档目的：loader：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M11 模块的《Loader 薄加载层：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


## 链 1：首次 API 调用的完整生命周期

这是 loader 最重要的一条链，任何一个 `mu*` 调用首次进入都会走完。

```
extern "C" muStreamCreate        src/loader/loader.cpp:212   （宏生成）
  -> MiniLoader::Forward         src/loader/loader.cpp:180
    -> MiniLoader::EnsureInitialized  src/loader/loader.cpp:97
      -> InitOnceExecuteOnce / std::call_once  src/loader/loader.cpp:101 / :107
        -> InitializeDriverCore  src/loader/loader.cpp:58
          -> ResolveDriverLocation           src/loader/driver_resolver.cpp:301
            -> GetDriverPaths                src/loader/driver_resolver.cpp:272
            -> PlatformThunks::Load          src/loader/driver_resolver.cpp:71
              -> Util::Library::ReloadPath  src/util/utilLibrary.h:158
                -> Util::Library::Unload    src/util/utilLibrary.h:70
                -> Util::Library::LoadPath  src/util/utilLibrary.h:57
                  -> PlatformThunks::Resolve src/loader/driver_resolver.cpp:89
                    -> Util::Library::GetSymbol  src/util/utilLibrary.h:172
                      -> Util::Library::GetSymbol  src/util/utilLibrary.h:85
                        -> GetProcAddress / dlsym  src/util/utilLibrary.h:87 / :89
            -> AdapterCollection::Reset      src/loader/driver_resolver.cpp:106
            -> EnumerateAdapters             src/loader/driver_resolver.cpp:125
              -> D3DKMTEnumAdapters2         src/loader/driver_resolver.cpp:131, :149
            -> QueryPackageId                src/loader/driver_resolver.cpp:205
              -> CheckSum                    src/loader/driver_resolver.cpp:228
              -> ParsePackageId              src/loader/driver_resolver.cpp:173
            -> EqualPackageIds               src/loader/driver_resolver.cpp:258
            -> BuildDriverPath               src/loader/driver_resolver.cpp:289
            -> fs::is_regular_file          src/loader/driver_resolver.cpp:356
            -> fs::canonical                 src/loader/driver_resolver.cpp:361
          -> LoadDriverModule               src/loader/loader.cpp:47
            -> Util::Library::LoadPath      src/util/utilLibrary.h:57
              -> LoadLibraryExW              src/util/utilLibrary.h:60   （Win）
              -> dlopen                      src/util/utilLibrary.h:66   （WSL）
          -> ResolveDriverExports           src/loader/loader.cpp:146
            -> ResolveFunction              src/loader/loader.cpp:138
              -> Util::Library::GetSymbol   src/util/utilLibrary.h:85
            -> ResolveData                  src/loader/loader.cpp:142
              -> SynchronizeDebuggerData    src/loader/loader.cpp:165（仅 muDebugInit）
    -> driverFunctions.<member>              src/loader/loader.cpp:195
```

## 链 2：probe 诊断路径

```
main / wmain                  src/loader/probe.cpp:26 / :7
  -> ResolveDriverLocation    src/loader/driver_resolver.cpp:301
```

`probe.cpp` 只调用 `ResolveDriverLocation` 并打印三个字段 [src/loader/probe.cpp:14-17]、[src/loader/probe.cpp:31-33]，退出码即原始 `MUresult` [src/loader/probe.cpp:10]、[src/loader/probe.cpp:27]。它**不加载后端、不解析符号、不提交任何内核**，`README.md:92-95` 明确声明了这一点。

## 链 3：适配器枚举（D3DKMT 两段式调用）

```
EnumerateAdapters                 src/loader/driver_resolver.cpp:125
  -> AdapterCollection::Reset      src/loader/driver_resolver.cpp:106
  -> thunks.enumAdapters2 (计数)   src/loader/driver_resolver.cpp:131   （pAdapters == nullptr）
  -> collection.adapters.resize    src/loader/driver_resolver.cpp:146
  -> thunks.enumAdapters2 (填充)   src/loader/driver_resolver.cpp:149
  -> ~AdapterCollection            src/loader/driver_resolver.cpp:104
    -> closeAdapter_               src/loader/driver_resolver.cpp:111
```

超过 `kMaximumAdapterCount = 256` 返回 `MUSA_ERROR_SYSTEM_DRIVER_MISMATCH` [src/loader/driver_resolver.cpp:141]。

## 链 4：驱动自校验（逐适配器）

```
QueryPackageId                    src/loader/driver_resolver.cpp:205
  -> thunks.queryAdapterInfo      src/loader/driver_resolver.cpp:217   （KMTQAITYPE_UMDRIVERPRIVATE）
  -> CheckSum                      src/loader/driver_resolver.cpp:228
  -> thunks.queryAdapterInfo      src/loader/driver_resolver.cpp:242   （KMTQAITYPE_ADAPTERTYPE）
  -> ParsePackageId                src/loader/driver_resolver.cpp:173
    -> IsPackageCharacter         src/loader/driver_resolver.cpp:168
```

## 链 5：包选择与路径构造

```
ResolveDriverLocation 主循环       src/loader/driver_resolver.cpp:327
  -> QueryPackageId               src/loader/driver_resolver.cpp:205
  -> EqualPackageIds              src/loader/driver_resolver.cpp:258
```

所有 MUSA 适配器必须上报同一个包，否则 [src/loader/driver_resolver.cpp:342] 返回 `MUSA_ERROR_SYSTEM_DRIVER_MISMATCH`。

## 链 6：转发热路径（初始化后）

```
extern "C" muStreamCreate        src/loader/loader.cpp:212
  -> MiniLoader::Forward         src/loader/loader.cpp:180
    -> MiniLoader::EnsureInitialized  src/loader/loader.cpp:97
      -> once 机制直接返回       src/loader/loader.cpp:101 / :107
    -> driverStatus != SUCCESS ? 返回错误 : 继续   src/loader/loader.cpp:189-197
    -> driverFunctions.<member>  src/loader/loader.cpp:195
```

每次调用仍会经过一次 `once` 机制检查和一次 `driverStatus` 读取，不存在缓存跳转表。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/loader/probe.cpp`（4 处，例：[src/loader/probe.cpp:14-17]）
- `src/loader/driver_resolver.cpp`（2 处，例：[src/loader/driver_resolver.cpp:141]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
