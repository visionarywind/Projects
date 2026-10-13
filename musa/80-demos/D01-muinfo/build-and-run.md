# D01：构建与运行

- 文档目的：构建与运行步骤（运行结果未验证）。
- 适用范围：80-demos《D01：构建与运行》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`../demo-registry.md`](../demo-registry.md)、[`../../00-overview/project-overview.md`](../../00-overview/project-overview.md)
- 后续阅读：[`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)、[`../../99-roadmap/risk-register.md`](../../99-roadmap/risk-register.md)


## 构建（未执行）

```bash
cmake -S . -B build
cmake --build build --target muInfo
LD_LIBRARY_PATH="$PWD/build/lib:$LD_LIBRARY_PATH" ./build/bin/muInfo
```

实际 target/output 目录应以目标源码生成结果为准；`src/tools/CMakeLists.txt` 明确用 `add_executable` 建立可执行文件并用 `target_link_libraries` 链接 `${DRIVER_LIB_NAME}_dynamic`。[src/tools/CMakeLists.txt:10-18]

## 构建目标的静态核对（未执行）

- 根 `CMakeLists.txt` 把运行产物目录设为 `${BUILD_ROOT_DIR}/bin`（`CMAKE_RUNTIME_OUTPUT_DIRECTORY`），再 `add_subdirectory(src)`；上面 `./build/bin/muInfo` 的 `bin` 层级对应这两行。[CMakeLists.txt:157-158] [CMakeLists.txt:165]
- `src/CMakeLists.txt` 末行 `add_subdirectory` 把 `tools` 子目录纳入构建，`muInfo` 因此才有目标可建。[src/CMakeLists.txt:15]
- 工具目录的头文件搜索路径包含共享头 `musa_shared_include` 与 armory include。[src/tools/CMakeLists.txt:5-8]
- 每个 `.cpp` 由 `add_executable` 生成同名可执行文件，再 `target_link_libraries` 链到驱动动态库。[src/tools/CMakeLists.txt:12-15]
- 构建完成后由 `install` 规则把可执行文件放进 `${CMAKE_INSTALL_BINDIR}`（根 `CMakeLists.txt` 中定义为 `bin`）。[src/tools/CMakeLists.txt:16-19]
- 单元测试分支由 `MUSA_BUILD_UT` 控制并指向 `unittest` 子目录；本快照既未开启该选项，快照中也没有该目录，因此本文不列任何测试构建步骤。[CMakeLists.txt:167-169]

## 运行入口的静态核对（未执行）

- 可执行文件的 `main` 顺序是 `printCompilerInfo` → `muInit(0)` → `muDeviceGetCount`，设备数决定后面的循环次数。[src/tools/muInfo.cpp:372-378]
- 循环体内先 `muDevicePrimaryCtxRetain`、`muCtxSetCurrent`，然后才对该设备调 `printDeviceProp`。[src/tools/muInfo.cpp:380-385]
- 任一 Driver API 返回非成功值都会经 `__checkMusaErrors` 向 stderr 打印并 `exit(EXIT_FAILURE)`；所以“跑通”只能说明第一个错误之前的调用成功，不能说明所有查询都被执行过。[src/tools/muInfo.cpp:9-20]

## 安装后运行（未执行）

```bash
./install.sh
muInfo
```

安装会写系统库和 bin 目录（`BIN_INSTALL_DIR` 默认 `/usr/local/bin`），需明确授权。[install.sh:83-105]

`muInfo` 本体由 `BIN_BUILD_DIR` 下拷贝到 `BIN_INSTALL_DIR`（未传 `-b` 时默认 `/usr/local/bin`），与 `musa_driver_version` 在同一段里处理。[install.sh:102-104]

## 相关文档

- [`../../00-overview/analysis-state.md`](../../00-overview/analysis-state.md)
- [`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)

## 源码证据摘要

- `src/tools/CMakeLists.txt`（4 处，例：[src/tools/CMakeLists.txt:10-18] `target_link_libraries`）
- `src/tools/muInfo.cpp`（3 处，例：[src/tools/muInfo.cpp:372-378] `muInit`）
- `CMakeLists.txt`（3 处，例：[CMakeLists.txt:157-158] `CMAKE_RUNTIME_OUTPUT_DIRECTORY`）
- `install.sh`（2 处，例：[install.sh:102-104] `BIN_BUILD_DIR`）
- `src/CMakeLists.txt`（1 处，例：[src/CMakeLists.txt:15] `add_subdirectory`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 风险与未决项见知识库风险登记。

## 下一步阅读建议

阅读 [`../../00-overview/analysis-state.md`](../../00-overview/analysis-state.md)；阅读 [`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)。
