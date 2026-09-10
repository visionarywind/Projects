# M03 - common 应用基础设施

> 状态：首版边界层；实现细节按专题文档增量展开。

## 职责

common 将 runtime 之上的应用语义集中到 `llama-common`：参数解析、日志/console、chat template、Jinja/PEG、grammar/JSON schema、sampling helper、download/cache、speculative、subprocess 等。[common/CMakeLists.txt:1-184]

## 数据边界

输入通常来自 argv、HTTP JSON 或模型缓存；输出是 typed common params、已格式化 prompt、token 序列、采样参数和 response 辅助对象。server 文档明确 JSON/template/tokenization 在 HTTP worker，inference thread 只接收 native C++ task 数据。[tools/server/README-dev.md:86-103]

## Chat/采样路径

server completion handler 从 JSON 取 prompt，调用 `process_mtmd_prompt` 或 `tokenize_input_prompts`，再调用 schema evaluator 形成 task params。[tools/server/server-context.cpp:4275-4321] 生成结果由上层 sampler/response state 处理，最后变成 JSON 或 SSE。[tools/server/server-context.cpp:4348-4414]

## 约束与风险

- llama.cpp 使用 `common/jinja`，不要引入 Minja。[AGENTS.md:83-86]
- 模板改变会改变 prompt token 数、assistant generation prompt 和测试固定结果。
- 下载/cache 和 subprocess 受平台、安全和网络影响；不能把本地未验证的缓存状态写成保证。

## 修改影响

改 parser/template/schema 需覆盖 server API、CLI 行为和相应 Python/C++ tests；改 sampler 需检查 completion、server、benchmark 和质量路径。CMake 中 common 依赖 llama、vendor、httplib 和线程。[common/CMakeLists.txt:1-184]

---

**页尾：** 本页不将应用层便利函数描述成公共 llama C API。
