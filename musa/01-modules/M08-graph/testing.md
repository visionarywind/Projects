# Graph 执行：测试策略

- 文档目的：说明本模块当前能做和不能做的验证，并把已确认的风险逐条映射到用例
- 适用范围：M08 模块的《Graph 执行：测试策略》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态检查项可复现；运行验证**未执行**，本知识库不声称任何构建或测试通过
- 最后更新：2026-10-07
- 前置阅读：`risks-and-debt.md`、`development-guide.md`
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 1. 现状与边界

| 项目 | 现状 | 证据 |
|---|---|---|
| 单元测试目录 | 源码快照中不存在 `unittest/`、`tests/` | [CMakeLists.txt:167-169] |
| 测试框架 | 打开 `MUSA_BUILD_UT` 后由 FetchContent 拉取 googletest 1.17.0 私有 URL | [CMakeLists.txt:146-153] |
| 覆盖率开关 | 由 `GCOV_TEST` 环境变量加编译参数 | [CMakeLists.txt:120-123] |
| 知识库登记的示例 | `80-demos/D03-graph` 只列候选，标注未读取、未运行 | [CMakeLists.txt:167-169] |

结论：**本环境没有可执行的 graph 测试基线**，因此文档里的所有「通过/失败」判定都只能来自静态检查。

## 2. 静态检查清单

这些检查只读源码，不需构建，可在任何机器上复现。

| 检查项 | 方法 | 当前判定 | 证据 |
|---|---|---|---|
| 四处登记齐全 | 比对 `muGraphCreate` 在类型表、导出表成员、注册表、loader 桩中是否都出现 | 四处齐全 | [src/musa_shared_include/export_table.h:1280] |
| 统一入口覆盖度 | 数 `muapiGraphAddNode` 的 case 数，与 `MUgraphNodeType` 枚举值个数比对 | 13 个 case，枚举共 18 个值 | [src/driver/mu_graph.cpp:1012-1066] |
| 恒失败接口 | 搜函数体只含 `MUSA_ERROR_NOT_SUPPORTED` 的入口 | 至少 `muapiGraphUpload` 一处 | [src/driver/mu_graph.cpp:2403-2405] |
| 跨 context 死分支 | 检查内层循环是否用 `*it` 参与查找 | 当前传的是 `curCtx` | [src/musa/core/context.cpp:1679-1683] |
| 节点释放完整性 | 检查 `Graph::DeleteNode` 是否有 `delete` | 没有 `delete`，仅摘除引用 | [src/musa/core/graph.cpp:187-241] |
| 边与度数一致性 | 检查删边是否只改 `m_Edges` 而不动 `m_NodeInDegree`/`m_NodeOutDegree` | 未同步 | [src/musa/core/graph.cpp:311-328] |
| 引用行号漂移 | 对本知识库跑行级校验脚本 | 本文档所属文件为 0 错 | [src/driver/internal.cpp:497] |

## 3. 运行验证（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

- `MUSA_BUILD_UT` 默认 `OFF`，需要显式打开才会编译单测。[CMakeLists.txt:29]
- 打开后会 `enable_testing()`，并用 FetchContent 从私有地址拉 googletest。[CMakeLists.txt:146-153]
- 覆盖率需要额外设置 `GCOV_TEST` 环境变量。[CMakeLists.txt:120-123]
- `enable_testing()` 无论是否开单测都会无条件调用一次。[CMakeLists.txt:125]
- README 说明 `ENABLE_ASAN_CHECK` 用于查内存泄漏、`ENABLE_TSAN_CHECK` 用于查线程安全，两者互斥。[README.md:27-31]
- README 同时给出 `MUSA_BUILD_UT` 的用途说明。[README.md:36-37]

**前置条件（全部不满足）**：私有 googletest 下载地址可访问、匹配的编译工具链、目标 GPU 与驱动、以及存在的 `unittest/` 源目录。本知识库不引用任何构建产物。

## 4. 用例设计（待实现）

按 `risks-and-debt.md` 的高风险逐条给出最小用例；期望结果来自源码阅读，**均未运行**。

| 用例 | 输入 | 期望（静态推断） | 对应风险与证据 |
|---|---|---|---|
| TC-01 跨 context 销毁执行图 | 在 context A 实例化，切到 context B 后调用销毁 | 当前死分支走不到 `RemoveGraphExec`，修复后应成功解除登记 | [src/musa/core/context.cpp:1717-1725] |
| TC-02 删除节点的内存回收 | 建 1 节点图，调用 `muGraphDestroyNode` 触发 `Graph::DeleteNode`，再销毁图 | 修复后不泄漏；当前实现节点对象无人释放 | [src/musa/core/graph.cpp:187-241] |
| TC-03 删边后的根节点 | 加 A->B，删该边，再调 `GetRootNodes` | 应返回 A、B 两个根；当前度数未同步会漏判 | [src/musa/core/graph.cpp:98-106] |
| TC-04 野节点句柄 | 传一个不属于任何图的节点句柄销毁 | 源码注释声明会段错误；修复后应先判 `GetParentGraph` 是否为空 | [src/driver/mu_graph.cpp:1244-1246] |
| TC-05 含 mem 节点的图编辑 | 图中加一个 mem_alloc 节点后删除普通节点 | 当前返回 `MUSA_ERROR_NOT_SUPPORTED`，修复后仅校验目标节点 | [src/musa/core/graph.cpp:190-196] |
| TC-06 `_v2` 边属性 | 用 `muGraphAddDependencies_v2` 传非默认 `edgeData` | 当前被忽略，仍建成普通依赖 | [src/driver/mu_graph.cpp:296-303] |
| TC-07 写参数不校验执行图 | 传一个不属于自己的执行图与节点 | 当前可能直接写入，修复后应返回 `MUSA_ERROR_INVALID_VALUE` | [src/driver/mu_graph.cpp:1071-1154] |
| TC-08 DOT 导出到不可写路径 | `muGraphDebugDotPrint` 传只读目录 | 修复后应返回错误；当前返回 `MUSA_SUCCESS` | [src/musa/core/graph.cpp:354-357] |
| TC-09 `GetNodes` 失败语义 | 先传 `NULL` 探测数量，再用失效句柄调用 | 调用方 `numNodes` 不被改写；需文档化 | [src/driver/mu_graph.cpp:1867-1871] |
| TC-10 统一入口 5 类节点 | 用 `muGraphAddNode` 加 batch_mem_op 等 5 类 | 返回 `MUSA_ERROR_NOT_SUPPORTED` | [src/driver/mu_graph.cpp:1063-1065] |

补充：capture 状态机的用例（`Query` 不置失效、`Synchronize` 置失效、`EndCapture` 在失效态报错）见 `execution-flows.md`，本文件不重复列。[src/musa/core/stream.cpp:223-227]

## 5. 判定标准

- 静态检查项：以「与源码一致」为通过，允许在源码变更后变红。
- 运行用例：只有实际执行并留下日志/产物才算通过；未执行一律记为「未验证」。
- 任何引用行号发生变化时，先跑校验脚本再改文档，不手工猜行号。[src/driver/internal.cpp:497]

## 源码证据摘要

- 测试入口：`MUSA_BUILD_UT`（`CMakeLists.txt:29`）、FetchContent googletest（`CMakeLists.txt:146-153`）
- 覆盖率：`GCOV_TEST`（`CMakeLists.txt:120-123`）
- 用例对应的缺陷位置见 `risks-and-debt.md` 第 1、2 节

## 未解决问题

- 是否存在仓库外的测试工程，当前快照无法确认。
- 私有 googletest 地址在本环境是否可达，未测试。
- 10 条用例的期望值全部待运行验证。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
