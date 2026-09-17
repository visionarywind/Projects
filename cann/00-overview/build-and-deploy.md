# 构建、运行与部署总览

- 文档目的：汇总四仓真实构建/测试入口和环境限制
- 适用范围：GE、ACL、Runtime、Driver
- 对应源码版本：见 `analysis-state.md`
- 证据状态：命令来自仓库 README/AGENTS，未在本环境执行
- 最后更新：2026-09-10
- 前置阅读：project-overview.md
- 后续阅读：`99-roadmap/quick-start.md`

## 结论摘要

四个仓库独立构建，通常先准备 CANN toolkit、第三方依赖、编译器/CMake，再按版本和 SoC 构建。构建成功不等于设备运行成功；样例和多数集成测试还要求匹配 Driver/Firmware/NPU。

## 命令矩阵

| 仓库 | 构建 | 测试 | 备注 |
|---|---|---|---|
| GE | `cd ge && bash build.sh`；可用 `--ge_compiler`、`--ge_executor`、`--dflow`、`--asan`、`--cov` | `bash tests/run_test.sh`（具体选项见 `-h`） | `ge/CMakeLists.txt:30-47` 支持分组件包；编译依赖 CANN 环境 |
| ACL | `cd acl && bash build_third_party.sh --output_path=<path>`；`bash build.sh --ascend_install_path=<path> --ascend_3rd_lib_path=<path>` | `bash tests/run_test.sh --ut --ascend_install_path=<path> --ascend_3rd_lib_path=<path>` | README 明确 UT 不支持 root；第三方库可在线下载 |
| Runtime | `cd runtime && bash build.sh`；离线依赖先 `python download_3rd_party.py`，再 `bash build.sh --cann_3rd_lib_path=third_party` | `bash tests/build_ut.sh --ut=acl --target=ascendcl_utest -c --cann_3rd_lib_path=<path>` | `runtime/CMakeLists.txt:21-32,52-70` 控制 UT/COV |
| Driver | `cd driver && bash build.sh --pkg --soc=ascend910b` | `bash test/run_test.sh`；测试模式由 `driver/CMakeLists.txt:12-14` 进入 | 可能需要内核头、设备/固件和产品工具链 |

## 环境与部署

- ACL 要求 CANN toolkit、GCC ≥7.3、CMake ≥3.14、ccache、Bash ≥5.1.16 `[acl/README.md:44-87]`。
- Runtime 要求 Python、GCC 7.3–13、CMake ≥3.16、构建工具 `[runtime/README.md:61-83]`；运行样例需驱动/固件 `[runtime/README.md:85-120]`。
- Driver 的 QUICKSTART 是设备/产品参数的权威入口；`--soc` 需按实际芯片替换。
- GE 构建通过 `ASCEND_INSTALL_PATH`、`CANN_3RD_LIB_PATH` 和 CMake module/prefix path 查找依赖；缺失时直接失败 `[ge/CMakeLists.txt:49-68]`。

## 未验证事项

没有执行下载依赖、构建、安装、测试或硬件样例；不应把上述命令标成成功。部署和回滚命令须在目标机器按具体包版本执行。

## 相关文档

- [quick-start.md](../99-roadmap/quick-start.md)
- [testing-recipes.md](../99-roadmap/testing-recipes.md)

## 源码证据摘要

见命令矩阵和引用。

## 未解决问题

需要目标 SoC、CPU 架构、CANN toolkit 安装路径和是否可联网，才能给出已验证命令。

## 下一步阅读建议

先只运行 `bash <repo>/build.sh -h` 获取本机版本支持的选项，再选最窄测试目标。
