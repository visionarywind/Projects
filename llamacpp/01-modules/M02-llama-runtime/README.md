# M02 - llama Runtime 与模型

- 文档目的：解释 01-modules/M02-llama-runtime/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-llama-runtime/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M02-llama-runtime/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M02-llama-runtime/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
