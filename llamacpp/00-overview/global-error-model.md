# 全局错误模型

## C API 边界

公共 wrapper 多数以 `nullptr`、负数或特定返回码表达失败：模型加载检查输入来源和 backend，失败返回空指针；context 检查参数后失败返回空指针；decode 对非 0/1 返回值记录错误。[src/llama.cpp:380-444] [src/llama-context.cpp:3660-3765] [src/llama-context.cpp:4247-4255]

## 模型加载

- metadata/path/file 必须且只能提供一个，否则返回空。[src/llama.cpp:380-403]
- 没有加载 backend 且不是 vocab-only，直接失败并提示先加载 backend。[src/llama.cpp:404-409]
- 未知架构抛异常，被加载函数捕获并转为失败；hparams/vocab 追加阶段信息。[src/llama.cpp:344-377]
- progress callback 返回取消会成为状态 `-2`，上层记录 cancelled。[src/llama.cpp:315-317] [src/llama.cpp:428-440]

## Context/compute

参数不合法、tensor split/flash attention 不兼容、cache block size 不整除时返回空 context；异步 scheduler 非成功状态记录错误。[src/llama-context.cpp:3663-3733] [src/llama-context.cpp:2511-2518]

## Server

HTTP handler 用 `ex_wrapper` 把 `invalid_argument` 映射 400，其他 exception 映射 500，并尽量构造 JSON error。[tools/server/server.cpp:52-85] decode 的 context exceeded、invalid batch、compute error 会向 processing slots 发 error、释放 slot、清理 prompt，再抛出终止本轮异常。[tools/server/server-context.cpp:3688-3725]

## 清理原则

错误路径必须释放 model/context/slot；stream session 用 cancel/finalize 唤醒读者；server cleanup 先停 stream GC，再释放 backend（生命周期结论来自 README-dev，具体 cleanup 全量行级审计未完成）。[tools/server/README-dev.md:127-149]

---

**页尾：** 错误文案是实现事实，不应在文档中承诺为稳定 API，除非公共 API 明确保证。
