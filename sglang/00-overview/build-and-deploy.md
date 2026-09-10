# 构建与部署

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
