# D01 - Server 非流式 Chat Completion

- 文档目的：解释 80-demos/D01-server-chat/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-server-chat/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


> 主 Demo；状态：`llama-server` CPU 构建已通过，模型启动和 pytest 未执行。

## 目标

复现测试中 `POST /chat/completions` 的非流式路径，说明 HTTP 请求如何变成 task、slot batch、`llama_decode` 和 JSON response。测试 fixture 默认使用 tinyllama2 preset，并断言 token usage、completion 内容和 finish reason。[tools/server/tests/unit/test_chat_completion.py:5-12] [tools/server/tests/unit/test_chat_completion.py:28-51]

## 验证记录

- `cmake -B build -DLLAMA_BUILD_UI=OFF && cmake --build build --target llama-server llama-cli -j$(nproc)`：通过（CPU/x86_64，OpenSSL 未找到，因此 HTTPS disabled；UI assets 从 HF bucket provisioned）。
- `./build/bin/llama-server --help`：通过。
- `./tests.sh unit/test_chat_completion.py::test_chat_completion -v`：未执行；缺少模型/pytest 环境验证。

## 执行阶段（命令来源已确认，部分已验证）

```bash
python3 -m pip install -r tools/server/tests/requirements.txt
cmake --build build --target llama-server -j$(nproc)
cd tools/server/tests
./tests.sh unit/test_chat_completion.py::test_chat_completion -v
```

实际运行还依赖模型下载/缓存、Python 包、端口和硬件；本批没有执行，不能给出输出或通过结论。`tests.sh` 会切到脚本目录并用 pytest-xdist 运行目标。[tools/server/tests/tests.sh:1-20]

## 源码轨迹

1. `server.cpp::llama_server` 设置 locale、common、stream manager，解析参数并初始化 backend。[tools/server/server.cpp:88-112]
2. HTTP handler 进入 `server_routes::handle_completions_impl`，创建 `server_res_generator`；默认会等待 server 不处于 sleep。[tools/server/server-context.cpp:4227-4247]
3. handler 从 JSON 取 prompt，tokenize，构造 task params 和 task，并 `rd.post_tasks`。[tools/server/server-context.cpp:4275-4344]
4. server context 处理 slot；prompt 完成时只选择最后 token 输出 logits，然后 decode 批次。[tools/server/server-context.cpp:3591-3604] [tools/server/server-context.cpp:3645-3686]
5. runtime wrapper 进入 context decode，graph compute 交给 ggml scheduler。[src/llama-context.cpp:4247-4255] [src/llama-context.cpp:2492-2518]
6. 非流式 reader 等待所有结果，结果转 JSON，单结果直接返回。[tools/server/server-context.cpp:4348-4380]

## 正常/错误/清理

- 正常：HTTP 200，响应包含 choices/usage；测试检查 prompt/completion token 数和 regex 内容。[tools/server/tests/unit/test_chat_completion.py:41-51]
- 无效请求：handler 捕获异常并返回 400；未知运行异常由外层返回 500。[tools/server/server.cpp:52-85]
- context exceeded/compute error：processing slots 收到 error，释放并清空 prompt。[tools/server/server-context.cpp:3688-3725]
- 连接终止：reader 返回 terminated，HTTP 不继续构造结果；stream manager 的生命周期由 server cleanup 管理。[tools/server/server-context.cpp:4350-4358] [tools/server/README-dev.md:127-149]

## 模块映射

| 阶段 | 模块 | 文档 |
|---|---|---|
| argv/common/backend/model/context | M02/M03 | [M02](../../01-modules/M02-llama-runtime/README.md)、[M03](../../01-modules/M03-common-app-infra/README.md) |
| route/task/slot/response | M04 | [M04](../../01-modules/M04-server/README.md) |
| graph/backend compute | M01/M02 | [M01](../../01-modules/M01-ggml-backend/README.md) |
| pytest fixture/assertions | M08 | [M08](../../01-modules/M08-build-tests-ci/README.md) |

## 调试观察点

开启 `DEBUG=1` 和 `-s -v -x` 查看 server 日志；用 `LLAMA_CACHE` 避免重复下载；若怀疑 KV 空间，观察 decode 返回和缩小 batch 分支。命令来自测试 README，未验证。[tools/server/tests/README.md:34-46]

## 未解决

模型实际 prompt token 数和 generated content 不应复制成固定事实，除非目标模型、seed、版本和测试真实执行一致。stream、router、MTMD 分支不属于本 Demo 的主路径。

---

**页尾：** D01 的“完整”指静态主路径完整，不代表覆盖全部 server feature。

## 文档元数据（规范补充）

- 文档目的：说明 `80-demos/D01-server-chat/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `80-demos/D01-server-chat/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
