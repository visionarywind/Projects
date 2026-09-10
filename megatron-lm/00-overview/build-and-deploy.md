# 构建、运行与部署总览

- 文档目的：提供来自仓库文档/技能/CI 的真实命令，并标明验证状态。
- 适用范围：开发、测试和文档构建。
- 对应源码版本：`8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：命令已由仓库文件确认，未在本机执行 GPU/容器命令
- 最后更新：2026-09-10
- 前置阅读：[依赖地图](dependency-map.md)
- 后续阅读：[快速上手](../99-roadmap/quick-start.md)

## 结论摘要

项目推荐在 CI dev 容器中用 uv 管理环境；源码安装会编译可选 C++ dataset helper。GPU unit tests 必须经 `torch.distributed.run`，正式训练和推理必须依照 examples/docs 提供数据、tokenizer、GPU 和分布式配置。

## 环境与安装

依据 `skills/mcore-build-and-dependency/SKILL.md`：

```bash
uv sync --locked --group dev --group test
uv sync --locked --only-group linting
uv pip install --no-build-isolation -e "[training,dev]"
```

这些命令必须在容器中运行；本知识库未验证本机是否有 uv、CUDA 或 GPU。

## 容器

```bash
docker build --target main --build-arg FROM_IMAGE_NAME=$(cat docker/.ngc_version.dev) \
  --build-arg IMAGE_TYPE=dev -f docker/Dockerfile.ci.dev -t megatron-lm:local .
docker run --rm --gpus all -v "$(pwd):/workspace" -w /workspace \
  megatron-lm:local bash -c '<command>'
```

`--target main` 是因为 dev Dockerfile 的 jet stage 需要内部 secret。[skill:104-125]

## 检查与测试

```bash
BASE_REF=main CHECK_ONLY=true SKIP_DOCS=false bash tools/autoformat.sh
uv run python -m torch.distributed.run --nproc-per-node 8 -m pytest -q tests/unit_tests
```

单测试与 functional recipe 见 [测试配方](../99-roadmap/testing-recipes.md)。

## 文档构建

```bash
cd docs
uv run --group docs sphinx-build . _build/html
uv run --group docs sphinx-build --builder linkcheck . _build/linkcheck
```

来源：[docs/documentation.md:21-47]。

## 部署边界

CI 使用 DockerExecutor/集群环境；SLURM/Pyxis 命令见 build skill 和 `mcore-run-on-slurm`，不是本机默认部署。生产 job 的分布式配置、数据路径和 checkpoint 路径必须由部署者提供。

## 相关文档

- [快速上手](../99-roadmap/quick-start.md)
- [调试](../99-roadmap/debugging-guide.md)
- [仓库技能](../../../skills/mcore-build-and-dependency/SKILL.md)

## 源码证据摘要

`pyproject.toml:145-203`; `setup.py:6-13`; `docs/documentation.md:21-60`; `skills/mcore-build-and-dependency/SKILL.md:104-142`。

## 未解决问题

尚未验证 Debug/Release 的独立 build profile（项目以 Python/容器分发为主），也未在 GPU 上运行部署命令。

## 下一步阅读建议

运行环境可用后先执行 D01，再执行单个 unit test。
