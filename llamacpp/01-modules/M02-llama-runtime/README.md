# M02 - llama Runtime 与模型

> 状态：首版实现层；模型加载和 decode 边界已确认。

## 职责

M02 对外提供 `include/llama.h` 的稳定 C API，对内管理 GGUF loader、词表、架构模型、context、batch、KV/memory、graph 和 decode。构建目标 `llama` 位于 `src/`，链接 ggml。[src/CMakeLists.txt:1-67]

## 模型加载调用链

1. `llama_model_load_from_file` 将路径封装为 split 列表并进入 impl。[src/llama.cpp:465-470]
2. impl 要求 metadata/path/file 三者恰好一个，检查 backend registry，然后调用 `llama_model_load`。[src/llama.cpp:380-428]
3. `llama_model_load` 构造 loader，设置 lazy mode，按 hparams、vocab、stats、tensors 顺序加载。[src/llama.cpp:316-377]
4. loader 的 arch 交给 `llama_model_create(ml, params)`；未知 arch 抛异常，已知 arch 在 switch 中创建具体 `llama_model_*`。[src/llama-model.cpp:343-369]

## Context 与 decode

`llama_init_from_model` 先检查空 model、batch/context、tensor split、flash attention、cache type 和 head block size，再 `new llama_context(*model, params)`。[src/llama-context.cpp:3660-3765]

`llama_decode(ctx,batch)` 是 API wrapper，调用 context decode；graph compute 设置 CPU/backend 线程并交给 scheduler 异步执行。[src/llama-context.cpp:4247-4255] [src/llama-context.cpp:2492-2518]

## 关键分支

- `vocab_only` 返回 model，不读取 tensors。[src/llama.cpp:364-370]
- progress callback 可取消加载，状态 -2；普通失败为 -1。[src/llama.cpp:315-317] [src/llama.cpp:428-440]
- tensor split 自动启用 flash attention；不满足约束返回 null。[src/llama-context.cpp:3683-3695]
- quantized V cache 必须启用 flash attention；K/V head size 必须被 block size 整除。[src/llama-context.cpp:3702-3733]

## 数据与资源

model 持有权重和设备映射；context 依附 model 并持有执行调度和 memory；batch 是 decode 输入及 output 选择。失败路径以 null/返回码和日志表达，调用者负责不继续使用空对象。

## 修改影响

新增模型要协调架构枚举、`src/models/`、转换代码和测试；公共 header 变化影响所有 tools、examples、bindings。具体模型 graph 尚未全部覆盖。

---

**页尾：** 引用只对当前 HEAD 有效。
