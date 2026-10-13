# Driver API 导出层：测试策略

- 文档目的：driver api：测试入口、缺口与建议用例（含未验证标注）。
- 适用范围：M02 模块的《Driver API 导出层：测试策略》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 静态检查

- 入口一致性：`muapi.h` 声明的 577 个 `muapi*` 与 `src/driver/mu_*.cpp` 定义一一对应（用 `rg -c "^MUresult muapi"` 核对数量）。[src/driver/muapi.h:13-602]
- 导出表一致性：每个 `mu*` 公共符号要么在 `loader_api_list.inc` 导出、要么在 `muapiGetProcAddress` 的 `DriverEntryPointTable` 登记，两条路径都要查。[src/loader/loader_api_list.inc:342-344] [src/driver/internal.cpp:175-179]
- 失败路径：核对失败分支是否阻止半初始化对象进入 tracker/登记集合（`DestroyModule` 回滚、`delete pMemoryPool`）。[src/driver/mu_module.cpp:134-141] [src/driver/mu_mempool.cpp:112-117]
- 资源成对：`TlsCtxPush`/`TlsCtxPop`、`LockedAddCtx`/`LockedRemoveCtx`、`CreateMemory`/`DestroyMemory`、`new`/`delete` 是否配对。
- 导出表回填：`FillAccessors` 依赖源表尾部 0 哨兵与目标表 `AccessorHint` 槽位，静态检查两组表长度一致。[src/driver/mu_entry.cpp:2146-2156]
- 运行 `git diff --check` 只验证本文档仓库的格式，不验证远端项目。

## 运行验证（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

目标环境需要私有子模块、工具链、驱动和 GPU；本知识库没有声称上述命令成功。

## 模块专项用例（建议，未执行）

1. `muGetExportTable`：为每个 `Client` 取值取表并检查关键槽位非 `AccessorHint`；未知 uuid 应记录行为变化（当前静默返回 MUPTI 表）。[src/driver/mu_entry.cpp:2164-2304]
2. 错误码矩阵：NULL 参数、`TlsCtxTop()` 为空、`flags` 非法、越界设备序号，断言 `MUresult` 精确值。
3. 句柄生命周期：创建/销毁配对、primary ctx retain/release 引用计数、detached stream 销毁。
4. 同步/异步：`memBlocking`/`launchBlocking` settings 开关下，验证同步路径真正等待完成。
5. profiler 备份重入：`ProfilerSaveRestoreKernelDeviceMemory` 内部自调用 `muapiMemcpy*` 的场景。[src/driver/mu_entry.cpp:1017-1054]

模块专项用例见 `80-demos` 和远端 `tests` 文件。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（3 处，例：[src/driver/mu_entry.cpp:2146-2156]）
- `src/driver/muapi.h`（1 处，例：[src/driver/muapi.h:13-602]）
- `src/loader/loader_api_list.inc`（1 处，例：[src/loader/loader_api_list.inc:342-344]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
