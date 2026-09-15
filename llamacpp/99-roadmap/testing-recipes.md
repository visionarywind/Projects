# 测试配方

- 文档目的：解释 99-roadmap/testing-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/testing-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## C++

```bash
ctest --test-dir build -N
ctest --test-dir build -L main --output-on-failure
ctest --test-dir build -R '^test-jinja$' --output-on-failure
```

CTest label 默认是 `main`，模型/fixture 测试可能需要下载或先生成 dummy models。[tests/CMakeLists.txt:19-46] [tests/CMakeLists.txt:201-257]

## Backend

```bash
cmake --build build --target test-backend-ops -j$(nproc)
./build/bin/test-backend-ops -b CPU
```

命令来源已确认，未运行。

## Server

```bash
python3 -m pip install -r tools/server/tests/requirements.txt
cd tools/server/tests
./tests.sh unit/test_chat_completion.py::test_invalid_chat_completion_req
```

`tests.sh` 没参数时默认排除 slow marker，有参数时将参数直接交给 pytest。[tools/server/tests/tests.sh:7-20]

## Python/UI

```bash
python3 -m unittest discover ./gguf-py -v
cd tools/ui && npm run check && npm run lint && npm run test
```

以上命令都只完成静态来源确认；本批没有测试通过结果。

---

**页尾：** “注册”“可构建”“运行通过”是三种不同状态。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
