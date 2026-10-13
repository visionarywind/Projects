# 构建/发布/安装系统：图示

- 文档目的：build release install：核心关系与流程图示，及节点到源码的映射。
- 适用范围：M01 模块的《构建/发布/安装系统：图示》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：部分推断（引用较少，证据密度不足）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 分层图

```mermaid
flowchart LR
    U[CI / 开发者] --> I[根 CMakeLists.txt]
    I --> O[src 各子目录 CMakeLists.txt]
    O --> H[静态库 util / halM3d / musaCore]
    H --> D[动态库 musa_dynamic / musadebugger / musa_loader]
    D --> R[工具可执行文件 + 安装产物]
```

## 目标依赖图

```mermaid
flowchart LR
    T[tools/*] --> DY[musa_dynamic]
    DY --> C[musaCore]
    C --> HAL[halM3d]
    HAL --> M["m3d / scpc / util（子模块）"]
    LO[musa_loader] -.仅 Windows/WSL.-> DY
    GDB[musadebugger] -.独立头文件.-> INC[musa_shared_include]
```

## 构建阶段图

```mermaid
flowchart TD
    P1[cmake 配置：option + 版本 + 子目录] --> P2[编译静态库]
    P2 --> P3[编译/链接动态库 + 导出符号]
    P3 --> P4[strip 与符号瘦身]
    P4 --> P5[ctest / install.sh / CPack]
```

## CI 流水线图（Linux）

```mermaid
flowchart LR
    B1["ddk_build.sh -a 0 -m 1"] --> P1["ddk2.0_musa_master.tar.gz"]
    B2["ddk_build.sh -a 0 -m 3"] --> P2["ddk2.0_musa_trace_on_master.tar.gz"]
    B3["ddk_build.sh -a 0 -m 2,a"] --> P3["ddk2.0_musa_master_debug_asan.tar.gz"]
    P1 --> T1[musa_benchmark / musa_cts]
    P3 --> T2[musa_cts asan]
```

## 安装流图

```mermaid
flowchart TD
    L1["build/lib/libmusa*"] --> A1["LIB_INSTALL_DIR"]
    B1["build/bin/musa_driver_version、muInfo"] --> A2["BIN_INSTALL_DIR"]
```

## 生命周期图

```mermaid
stateDiagram-v2
    [*] --> Built
    Built --> Installed: install.sh
    Built --> Packaged: CPack（deb/rpm）
    Installed --> [*]
    Packaged --> [*]
```

## 证据

[src/CMakeLists.txt:7-15] [src/musa/core/CMakeLists.txt:7] [src/driver/CMakeLists.txt:9-11] [.ciConfig.yaml:8-23]

状态名是跨模块抽象，不能替代源码中具体枚举；实际枚举和异常状态请查 `implementation.md`。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/CMakeLists.txt`（1 处，例：[src/CMakeLists.txt:7-15]）
- `src/musa/core/CMakeLists.txt`（1 处，例：[src/musa/core/CMakeLists.txt:7]）
- `src/driver/CMakeLists.txt`（1 处，例：[src/driver/CMakeLists.txt:9-11]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
