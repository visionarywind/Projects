# 功能开发配方

## 新模型架构

1. 先读 `docs/development/HOWTO-add-model.md`。
2. 增加架构 metadata/枚举与 `src/models/` 实现。
3. 在 `llama_model_mapping` 接入具体类并保留未知架构失败路径。[src/llama-model.cpp:343-369]
4. 同步 `conversion/` 和必要 GGUF metadata。
5. 使用 dummy model/架构测试和实际模型质量测试；不要声称未运行的结果。

## Server endpoint

1. 明确 feature 属于 server scope；外部文件能力默认关闭。[tools/server/README-dev.md:7-32]
2. 在 HTTP 层解析 JSON/template/tokenize；转换成 native task，不把 raw JSON 传入 slot。[tools/server/README-dev.md:86-103]
3. 通过 queue/result 与 context 通信，避免在 inference thread 做重 post-processing。
4. 覆盖非流式、流式、断开和错误清理分支。

## Backend/operator

保持 ggml backend 抽象；同时检查 runtime graph 使用者和 `test-backend-ops`。具体 kernel 修改需要目标硬件验证。

---

**页尾：** 这是现有边界的开发配方，不是替代 maintainer 设计审查的审批流程。
