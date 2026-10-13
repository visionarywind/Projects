# 平台与设备管理：测试策略

- 文档目的：platform device：测试入口、缺口与建议用例（含未验证标注）。
- 适用范围：M03 模块的《平台与设备管理：测试策略》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：部分推断（引用较少，证据密度不足）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 静态检查

- 核对初始化状态：`m_InitStatus` 只在 `Init` 末尾写入，失败路径不得暴露半初始化平台。[src/musa/core/platform.cpp:133-138]
- 核对失败路径是否阻止半初始化对象进入 tracker：`CreateContext` 成功才 `LockedAddContext` + `Retain`，失败路径直接释放。[src/musa/core/device.cpp:635-675]
- 核对设备可见性筛选边界：空列表、越界、重复 index。[src/musa/core/platform.cpp:61-83]
- 核对资源创建和销毁是否成对：`LockedAddCtx`/`LockedRemoveCtx`、`LockedAddLibrary`/`LockedRemoveLibrary`、`LockedAddMemoryPool`/`LockedRemoveMemoryPool`。
- 核对析构顺序：`LockedWait` -> `ReleaseUnfreedMemories` -> 默认 pool 删除 -> `Hal::DestroyPlatform`。[src/musa/core/platform.cpp:610-656]
- 运行 `git diff --check` 只验证本文档仓库的格式，不验证远端项目。

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

- `src/musa/core/platform.cpp`（3 处，例：[src/musa/core/platform.cpp:133-138]）
- `src/musa/core/device.cpp`（1 处，例：[src/musa/core/device.cpp:635-675]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
