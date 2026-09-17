# 构建、运行与部署总览

- 文档目的：记录 Ray 的真实构建入口、开发命令来源和未验证边界。
- 版本：HEAD `cfe4725d23`；命令为仓库文档/配置中的参考，除特别注明外未执行。
- 前置：[依赖地图](dependency-map.md)。后续：[快速上手](../99-roadmap/quick-start.md)。

## 结论摘要

Ray 使用 Bazel/C++ 构建核心和扩展，Python packaging 组装 wheel，Java/C++ 还有各自构建边界；上游开发文档明确要求依照 Ray 的 development/testing guides，不应把本知识库根目录当作可直接构建的单一项目。[已确认：`AGENTS.md:64-72`、根 BUILD/WORKSPACE、`python/setup.py`]

## 入口地图

| 目标 | 文件/命令来源 | 状态 |
|---|---|---|
| Python 安装/打包 | `python/setup.py`, `pyproject.toml`, `build-wheel.sh` | 未验证 |
| C++/扩展构建 | `BUILD.bazel`, `WORKSPACE`, `src/ray/*/BUILD.bazel` | 未验证 |
| Java | `java/BUILD.bazel`, `java/runtime`, `java/test` | 未验证 |
| 文档 | `doc/BUILD.bazel`, `.buildkite/doc.rayci.yml` | 未验证 |
| Python tests | `pytest.ini`, `python/ray/tests`, 各包 tests | 未验证 |
| CI | `.buildkite`, `ci`, GitHub workflows | 静态确认 |

## 安全与环境

上游 `AGENTS.md` 要求 Python 使用虚拟环境、提交需 DCO sign-off，并将开发/测试指南作为命令来源。[已确认：`AGENTS.md:74-92`]

## 未执行

本轮没有安装依赖、构建 wheel/Bazel target、启动 Ray、运行 pytest、连接 Kubernetes/云或运行 GPU/多节点实验。因此不能声称“构建成功”或“Demo 可运行”。

## 相关文档
[Ray 官方开发文档](../../source/ray/doc/source/ray-contribute/development.md) · [测试提示](../../source/ray/doc/source/ray-contribute/testing-tips.md)

## 源码证据摘要
`AGENTS.md:64-92`；`BUILD.bazel`；`WORKSPACE`；`python/setup.py`；`build-wheel.sh`。

## 未解决问题
需要根据实际开发目标选择 Bazel/Python/C++/Java 的最小 target，并在具备依赖的环境执行验证。

## 下一步阅读建议
先读 quick-start，再依目标模块测试页面。
