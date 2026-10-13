# 构建/发布/安装系统：例子

- 文档目的：build release install：用法示例与边界/错误输入说明。
- 适用范围：M01 模块的《构建/发布/安装系统：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 最小构建

```bash
mkdir build
cd build
cmake .. -DMUSA_BUILD_DEBUG=ON        # debug 构建，对应 CMAKE_BUILD_TYPE=Debug
make
```

## 启用单测（本快照会失败：缺 unittest/ 目录）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

配置阶段会在 `add_subdirectory(unittest)` 处失败，因为 `unittest` 目录不在当前快照中。[CMakeLists.txt:167-169]

## 启用 sanitizer

```bash
cmake -S . -B build -DENABLE_ASAN_CHECK=ON     # 检测内存泄漏
cmake -S . -B build -DENABLE_TSAN_CHECK=ON     # 检测数据竞争
```

注：ASAN 与 TSAN 互斥，不应同时打开。

## 默认安装（install.sh）

```bash
./install.sh                    # 探测发行版，库装到 /usr/lib64 或 /usr/lib/x86_64-linux-gnu，工具到 /usr/local/bin
./install.sh -l /usr/lib/x86_64-linux-gnu -b /usr/local/bin
```

## MTUB 打包构建（CI 侧）

```bash
cmake -S . -B build -DMTUB_BUILD=ON -DLIBDRM_PATH=${WORKSPACE}/linux-ddk/libdrm-mt -DSHARED_INCLUDE_PATH=${WORKSPACE}/linux-ddk/shared_include
cmake --build build --target package -j $(nproc)
```

上面 `-DLIBDRM_PATH` / `-DSHARED_INCLUDE_PATH` 来自 `.mthreads-ci.yml` 的 MTUB job；其中 `SHARED_INCLUDE_PATH` 由 loader 目标消费。[.mthreads-ci.yml:48-52] [src/loader/CMakeLists.txt:14-18]

## Windows 构建

```bash
python cmake_mtmusa.py -os win32        # 生成 cmake_build，输出 mtmusa64.dll / musa_loader.dll
python cmake_mtmusa.py -os wsl2         # 生成 cmake_build/wsl，输出 libmusa.so.1.1 / libmusa_loader.so
```

## 从打包产物恢复运行环境（CI photon 链路）

```bash
bash install.sh -l /usr/lib/x86_64-linux-gnu -b /usr/local/bin
ldconfig
```

这是源码阅读模型，不是已运行的示例；其中 MTUB 与 Windows 命令来自 `.mthreads-ci.yml`，实际执行需要对应工具链。[.mthreads-ci.yml:37-65] [.mthreads-ci.yml:130-144]

## 示例对应的源码落点

上面每段命令在源码里都有一个可核对的落点；下表是“命令 → 被改到的东西”的对照，仍然只是静态阅读结果。

| 示例 | 源码落点 | 命令实际改变了什么 |
|---|---|---|
| 最小构建 / 输出目录 | [src/driver/CMakeLists.txt:1-2] `CMAKE_LIBRARY_OUTPUT_DIRECTORY` | 库与静态产物统一落到 `${BUILD_ROOT_DIR}/lib`，不由各子目录自行决定 |
| Windows / WSL2 库名 | [src/CMakeLists.txt:1-5] `DRIVER_LIB_NAME` | win32 取 `mtmusa64`，UNIX 取 `musa`，后续所有目标都引用该变量 |
| MTUB 打包安装 | [src/driver/CMakeLists.txt:54-64] `MTUB_BUILD` | 打开时装到 `${ACTUAL_INSTALL_LIBDIR}`，否则装到 `CMAKE_INSTALL_LIBDIR` |
| install.sh 装工具 | [src/tools/CMakeLists.txt:16-18] `CMAKE_INSTALL_BINDIR` | 每个工具由循环生成并安装到 bin 目录，与库目录分离 |
| loader 的 shared_include | [src/loader/CMakeLists.txt:14-23] `SHARED_INCLUDE_PATH` | 未设置时回落到仓库内 `shared_include`，头文件仍缺失则 `FATAL_ERROR` |

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/loader/CMakeLists.txt`（2 处，例：[src/loader/CMakeLists.txt:14-18] `SHARED_INCLUDE_PATH`）
- `src/driver/CMakeLists.txt`（2 处，例：[src/driver/CMakeLists.txt:54-64] `MTUB_BUILD`）
- `src/CMakeLists.txt`（1 处，例：[src/CMakeLists.txt:1-5] `DRIVER_LIB_NAME`）
- `src/tools/CMakeLists.txt`（1 处，例：[src/tools/CMakeLists.txt:16-18] `CMAKE_INSTALL_BINDIR`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
