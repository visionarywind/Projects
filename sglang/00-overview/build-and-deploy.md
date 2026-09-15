# 构建与部署

- 文档目的：解释 00-overview/build-and-deploy.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/build-and-deploy.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. Python 包

源码安装入口是 `python/pyproject.toml` 和 `python/setup.py`。pyproject 声明 Python 依赖、可选 extras 和 setuptools build backend；setup hook 从 `../rust` Cargo metadata 自动发现声明了 `[package.metadata.sglang]` 的 PyO3 扩展。[`python/setup.py:1-24`]

推荐的开发安装命令记录在仓库 `CLAUDE.md`：

```bash
python3 -m pip install --upgrade pip
python3 -m pip install -e "python"
```

没有匹配 CUDA/torch/本地编译环境时，可以设置 `SGLANG_BUILD_RUST_EXTS=none` 关闭 Rust 扩展发现；这只改变构建，不代表运行时所有功能可用。

## 2. Rust 组件

`rust/Cargo.toml` 定义 `sglang-grpc`、`sglang-mm` 和 `sglang-server` workspace，并通过 metadata 纳入单独 manifest 的 radix-tree crate。[`rust/Cargo.toml:1-16`]

独立的 `experimental/sgl-router` 与 `sgl-model-gateway` 不是 SRT scheduler 的同一个进程；部署时必须按各自 README 和 Cargo manifest 区分。

## 3. 运行入口

```bash
sglang serve --model-path <model>
python3 -m sglang.launch_server --model-path <model>
```

前者是推荐 CLI，后者是兼容入口。`launch_server.py` 说明了 encoder-only、SMG gRPC、Ray 和默认 HTTP 分支。[`python/sglang/launch_server.py:16-58`]

## 4. 测试和 lint

- 单个 Python 测试：`python3 test/registered/unit/...py` 或针对文件使用 `pytest`。
- CI suite：`python3 test/run_suite.py --hw <backend> --suite <suite>`。
- Python/Rust/YAML 等统一检查：`pre-commit run`。
- 文档检查：视环境运行 lychee 或 `cd docs && mint validate`。

这些命令来自仓库指导文件；本阶段没有执行完整测试、GPU 服务、pre-commit 或 Mintlify，因此均不能标记为已验证成功。

## 5. 部署分支

HTTP 默认部署、Ray、Rust gRPC、encoder disaggregation、PD disaggregation 和 diffusion runtime 的进程图不同。部署文档应同时给出硬件、依赖、模型文件、端口、进程角色和清理方式，不能只给一条启动命令。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
