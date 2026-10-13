# 测试配方

- 文档目的：说明本代码库"该测什么、在哪一层测、当前快照能跑到哪一步"，并登记测试设施缺口。
- 适用范围：`source/musa` 快照（5.3.0 develop）的 Driver / Core / HAL / Util / Loader 五层与 Demo 冒烟。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：**测试设施现状为已确认（静态）；所有测试命令均未执行**。
- 最后更新：2026-10-07
- 前置阅读：[`../00-overview/build-and-deploy.md`](../00-overview/build-and-deploy.md)
- 后续阅读：[`feature-development-recipes.md`](feature-development-recipes.md)、[`technical-debt.md`](technical-debt.md)

## 结论摘要

本快照**没有可运行的测试代码**：根目录不存在 `unittest/`，也没有 `tests/`，仓库内没有 `*_test.cpp`。构建系统却保留了测试开关——`MUSA_BUILD_UT` 与两处 `enable_testing`，其中一处会 `add_subdirectory(unittest)` 指向不存在的目录 [CMakeLists.txt:167-169]。因此当前能做的只有：文档仓库自检、静态核对、以及拿到环境后的 Demo 冒烟。任何"测试通过"的说法在本快照都**不成立**。

---

## 1. 当前快照的检查能力

### 1.1 可执行（不依赖 GPU / 不依赖构建）

```bash
# 文档仓库自检：空白错误、冲突标记
git diff --check

# 引用与内容校验（知识库维护脚本，位于临时目录，不属于仓库产物）
python3 verify_kb.py
python3 verify_content.py
```

### 1.2 存在但不可用

| 设施 | 代码证据 | 现状 |
|---|---|---|
| 单元测试开关 | `option(MUSA_BUILD_UT ...)` [CMakeLists.txt:29-29] | 打开后会失败，见下行 |
| googletest 拉取 | `FetchContent_Declare(googletest ...)` 指向内网 URL [CMakeLists.txt:146-154] | 未执行，网络可用性未知 |
| 单测子目录 | `add_subdirectory(unittest)` [CMakeLists.txt:167-169] | **目录不存在**，配置即失败 |
| CTest 注册 | 顶层 `enable_testing()` [CMakeLists.txt:125-125] | 无测试可注册 |
| 私有子模块自带测试 | `add_test(NAME ${TEST_NAME} ...)` [src/hal/m3d/m3d/src/util/imported/mt-tools-extension/tests/CMakeLists.txt:30-45] | 属于 `mt-tools-extension`，与驱动主工程测试无关 |

结论：本快照的"测试入口"是**文档自检 + 静态引用校验**，不是 CTest。

---

## 2. 分层测试配方（目标状态）

下表是恢复测试能力后应覆盖的层次。"落点"列给出该层最值得打桩/断言的源码位置；当前状态一律为**未验证**。

| 层 | 该测什么 | 落点（源码入口） | 当前状态 |
|---|---|---|---|
| Driver API | 空指针参数、越界尺寸、无 context、`_v1`/`_v2`/`_ptsz` 三种签名 | `muapiMemAlloc_v2` 的三条错误分支 [src/driver/mu_memory.cpp:271-304] | 无测试文件 |
| 错误语义 | 错误码是否与同类 API 一致、sticky error 是否保留 | `muapiMemFree_v2` 的类型/offset 校验 [src/driver/mu_memory.cpp:722-788] | 无测试文件 |
| Core 对象 | 句柄生命周期、重复创建/销毁、TLS context 缺失 | `Context::CreateMemory` [src/musa/core/context.cpp:1044-1099] | 无测试文件 |
| HAL 内存 | 池命中、字段比对兜底、`Free` 键一致性、并发分配 | `MemMgr::Allocate` [src/hal/m3d/memMgr.cpp:81-147] | 无测试文件 |
| Util 数据结构 | 树的插入/查找/删除、析构是否释放、分支方向 | `SplayTree::Search` [src/util/utilSplayTree.h:216-226] | **最高优先级**：该函数有已确认缺陷，却无用例可拦 |
| Loader | 清单与实际导出的一致性、错误码可区分性 | 清单三处展开 [src/loader/loader.cpp:119-123] | 无测试文件 |
| Demo 冒烟 | 无设备 / 单设备 / 多设备下工具不崩溃 | `muInfo` 主循环 [src/tools/muInfo.cpp:372-387] | 需目标机 |

---

## 3. 具体用例清单（写给未来的 `unittest/`）

### 3.1 Driver API 边界

- `muapiMemAlloc_v2(dptr=nullptr, ...)` → `MUSA_ERROR_INVALID_VALUE`；
- `muapiMemAlloc_v2(&p, 0)` → `MUSA_ERROR_INVALID_VALUE` 且 `*dptr == 0` [src/driver/mu_memory.cpp:271-304]；
- 未 `muCtxSetCurrent` 时调用 → `MUSA_ERROR_INVALID_CONTEXT`；
- `muapiMemPoolSetAttribute(pool, attr, nullptr)` → `MUSA_ERROR_INVALID_VALUE`，导入池同返回 [src/driver/mu_mempool.cpp:139-158]；
- `muapiGraphAddNode` 传未知 `type` → `MUSA_ERROR_NOT_SUPPORTED`（对应 `default` 分支）[src/driver/mu_graph.cpp:1012-1069]。

### 3.2 生命周期与并发

- 创建后立刻 `Destroy`、重复 `Destroy`、context reset 后再用句柄；
- 多线程同时 `Allocate`/`Free`，断言不串池（直击 `MemMgr` 那把 `m_Lock`）[src/hal/m3d/memMgr.cpp:81-147]；
- `QueueCommand` 达到容量上限时调用线程不忙等死锁 [src/musa/core/stream.cpp:1056-1071]。

### 3.3 异步与顺序

- `query` / `synchronize` 后数据可见性；
- 跨 stream、跨 event 的依赖是否被 `ResolveDependencyAndQueueCommand` 正确记录 [src/musa/core/context.cpp:2012-2076]；
- 错误延迟暴露：提交失败后下一次同步才返回错误，而不是当场丢错。

### 3.4 回归必须包含的三类

无设备、单设备、多设备（含 peer）。硬件相关用例在无环境时只能准备方案，**不得标注为通过**。

---

## 4. 恢复测试设施的最小步骤（方案，未验证）

1. 新建 `unittest/` 目录与 `unittest/CMakeLists.txt`（当前不存在，属于新增）；
2. 打开 `MUSA_BUILD_UT`，先确认 googletest 的 `FetchContent` URL 在本环境可达 [CMakeLists.txt:146-154]；
3. 从第 3 节的 3.4 用例里挑 `SplayTree` 与 `MemMgr` 各 5 条做首批用例——这两处有已确认缺陷，能立刻验证测试是否真的在跑；
4. `ctest --test-dir build --output-on-failure`（**未执行**）。

若确认测试代码在其他仓库，则把本文件的第 3 节当作需求清单移交，并在 [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md) 登记为快照缺口，而不是在本仓库伪造测试文件。

---

## 5. Demo 层冒烟

| Demo | 入口 | 冒烟目标 | 状态 |
|---|---|---|---|
| D01 | `muInfo` [src/tools/muInfo.cpp:372-387] | 逐设备打印属性、peer 矩阵、memInfo 不崩溃 | 需目标机，**未执行** |
| D02 | 见 [`../80-demos/D02-memory-copy/README.md`](../80-demos/D02-memory-copy/README.md) | 分配 → 异步拷贝 → 同步 → 校验数据 | 候选入口 `tests/*.cu` 不在快照 |
| D03 | 见 [`../80-demos/D03-graph/README.md`](../80-demos/D03-graph/README.md) | 建图 → 实例化 → launch → 结果校验 | 同上 |

---

## 6. 结果标记规范

- 每条测试结果必须写明：**执行环境、构建类型、开关状态（`MUSA_BUILD_UT` / `MUSA_LOG`）、是否真跑**。
- 依赖 GPU、私有子模块或内网 URL 的用例，环境不可用时只能标"未验证"，不能标"跳过即通过"。
- 文档仓库自检通过 ≠ 源码正确：校验脚本只保证引用不错，不保证有引用。

## 相关文档

- [`../00-overview/build-and-deploy.md`](../00-overview/build-and-deploy.md)
- [`technical-debt.md`](technical-debt.md)
- [`debugging-guide.md`](debugging-guide.md)
- [`../80-demos/demo-registry.md`](../80-demos/demo-registry.md)

## 源码证据摘要

- 测试开关与缺失目录：[CMakeLists.txt:29-29]、[CMakeLists.txt:125-125]、[CMakeLists.txt:146-154]、[CMakeLists.txt:167-169]
- 私有子模块测试：[src/hal/m3d/m3d/src/util/imported/mt-tools-extension/tests/CMakeLists.txt:30-45]
- 各层落点：[src/driver/mu_memory.cpp:271-304]、[src/driver/mu_memory.cpp:722-788]、[src/driver/mu_mempool.cpp:139-158]、[src/driver/mu_graph.cpp:1012-1069]、[src/musa/core/context.cpp:1044-1099]、[src/musa/core/context.cpp:2012-2076]、[src/hal/m3d/memMgr.cpp:81-147]、[src/util/utilSplayTree.h:216-226]、[src/loader/loader.cpp:119-123]、[src/tools/muInfo.cpp:372-387]

## 未解决问题

1. `unittest/`、`tests/` 是否存在于其他仓库未知。
2. googletest 内网 URL 是否可达未知。
3. 第 3 节所有用例均未编写、未执行。

## 下一步阅读建议

先读 [`technical-debt.md`](technical-debt.md) 的 D-3，再决定是补测试目录还是把清单移交外部仓库。
