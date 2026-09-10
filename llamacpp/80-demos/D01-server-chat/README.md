# D01 - Server 非流式 Chat Completion

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
