# M07 图示

- 文档目的：展示构建目标、测试和 CI 的关系。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：图示根据 CMake/CI 整理。
- 最后更新：2026-09-10
- 前置阅读：[M07 call-chains](call-chains.md)
- 后续阅读：[M07 testing](testing.md)

## 目标图

```mermaid
flowchart TD
  C[CMake configure] --> L[leveldb library]
  C --> T[leveldb_tests + c_test + env_test]
  C --> B[db_bench optional]
  C --> I[install/export optional]
  T --> CT[CTest]
  L --> T
  L --> B
```

## CI 时序

```mermaid
sequenceDiagram
  participant CI as GitHub Actions
  participant C as CMake
  participant B as Build
  participant T as CTest
  participant P as db_bench/install
  CI->>C: configure matrix
  C->>B: generate targets
  CI->>B: cmake --build
  CI->>T: ctest --verbose
  CI->>P: benchmark and install
```

## 失败定位

```mermaid
flowchart LR
  F[失败] --> G{阶段}
  G -->|configure| D[编译器/依赖/选项]
  G -->|build| S[源码/ABI/链接]
  G -->|CTest| R[行为回归/平台]
  G -->|benchmark| Q[性能或资源]
  G -->|install| I[导出/安装布局]
```
