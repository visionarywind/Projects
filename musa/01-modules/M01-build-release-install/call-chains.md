# 构建/发布/安装系统：调用链

- 文档目的：build release install：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M01 模块的《构建/发布/安装系统：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 链 1：CMake 配置期（入口 -> 子目录）

```
cmake -S . -B build -D<option>
  -> 根 CMakeLists.txt：option / 版本 / 平台分支        [CMakeLists.txt:21-107]
  -> MTUB_BUILD 时走 mt-toolchain + CPack 打包           [CMakeLists.txt:40-61]
  -> 否则 add_version_query(develop) 生成版本查询工具     [CMakeLists.txt:66-69]
  -> add_subdirectory(src)                              [CMakeLists.txt:165]
    -> 设置 DRIVER_LIB_NAME（musa / mtmusa64）           [src/CMakeLists.txt:1-5]
    -> 顺序 util -> hal -> musa -> driver -> loader -> gdb -> tools  [src/CMakeLists.txt:7-15]
```

## 链 2：编译/链接期（目标依赖）

```
工具可执行文件（src/tools/*.cpp）  [src/tools/CMakeLists.txt:12-18]
  -> musa_dynamic（SHARED 驱动库）  [src/driver/CMakeLists.txt:9-11]
  -> musaCore（STATIC 核心库）      [src/driver/CMakeLists.txt:17]
  -> halM3d（STATIC HAL 库）        [src/musa/core/CMakeLists.txt:11]
  -> m3d / scpc / util（子模块库）   [src/hal/m3d/CMakeLists.txt:36-40]
```

## 链 3：导出符号（清单 -> 链接器）

```
loader_api_list.inc + windows_api_list.inc   [cmake/export_symbols.cmake:11-14]
  -> 解析并去重 DEFAULT_MUSA_EXPORTED_SYMBOLS  [cmake/export_symbols.cmake:22-59]
  -> generate_export_files：.def / .ver        [cmake/export_symbols.cmake:64-109]
  -> setup_unified_exports（驱动库 / loader）   [cmake/export_symbols.cmake:135-138]
```

## 链 4：安装期（install.sh -> 系统路径）

```
build/lib/libmusa*            -> LIB_INSTALL_DIR   [install.sh:100]
build/bin/musa_driver_version -> BIN_INSTALL_DIR   [install.sh:103]
build/bin/muInfo              -> BIN_INSTALL_DIR   [install.sh:104]
目录缺一即报错退出或回退默认路径                     [install.sh:27-30] [install.sh:51-81]
```

## 链 5：CI 流水线（构建产物 -> 测试门禁）

```
ddk_build.sh 各模式打包          [.ciConfig.yaml:8-97]
  -> ddk2.0_musa_master.tar.gz    [.ciConfig.yaml:25-34]
  -> musa_cts / benchmark 消费     [.ciConfig.yaml:100-119]
  -> commit_status 门禁后进入 photon 链路  [.mthreads-ci.yml:146-168]
```

## 相关证据

[CMakeLists.txt:21-31] [src/driver/CMakeLists.txt:9-17] [install.sh:27-105]

## 链路缺口

若链路进入 `src/hal/m3d/m3d` 子模块，当前只确认构建和接口边界（其 `CMakeLists.txt` 不在本快照内逐行核对），未把私有子模块内部函数写成已确认事实。`ddk_build.sh` 属于 linux-ddk 仓库，`.ciConfig.yaml` 中的构建命令无法在 musa 快照内直接复现。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/CMakeLists.txt`（3 处，例：[src/driver/CMakeLists.txt:9-11]）
- `src/CMakeLists.txt`（2 处，例：[src/CMakeLists.txt:1-5]）
- `src/tools/CMakeLists.txt`（1 处，例：[src/tools/CMakeLists.txt:12-18]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
