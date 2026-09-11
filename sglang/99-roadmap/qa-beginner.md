# 入门级面试题：SGLang
- 题数：100
- 适用对象：第一次接触项目、Transformer、GPU 训练/推理系统的读者
- 证据锚点：当前知识库记录的 checkout `f1a512c51c73ab660cf41e1af3110c7c11e3b600`。
- 作答要求：每题包含参考答案、小白解释、技术分析和拓展分析；涉及 GPU/多卡/性能的结论必须区分静态源码理解与实际运行验证。
- 项目一句话：SGLang Runtime 面向大模型推理服务，把请求规范化、tokenization、IPC、scheduler、KV cache、ModelRunner、采样和输出组织成连续批处理系统。
- 主要证据：README、00-overview、01/02 请求流程、M03-M10、90-cross-module 和现有 QA/实践文档。

## A. LLM 推理基础（1-10）

### 1. 什么是「大语言模型推理」，它在 SGLang 中解决什么问题？

**参考答案：** 自回归推理每一步用已有 token 预测下一个 token；服务还必须管理排队、显存、批处理、停止条件和输出协议。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。自回归推理每一步用已有 token 预测下一个 token；服务还必须管理排队、显存、批处理、停止条件和输出协议。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 自回归推理每一步用已有 token 预测下一个 token；服务还必须管理排队、显存、批处理、停止条件和输出协议。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 2. SGLang 里为什么需要理解「token」？

**参考答案：** token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 3. 请用小白能懂的话解释「tokenizer」的作用。

**参考答案：** TokenizerManager 在输入侧做 tokenization/校验，在输出侧做 detokenization；流式输出必须记住已发送文本边界。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。TokenizerManager 在输入侧做 tokenization/校验，在输出侧做 detokenization；流式输出必须记住已发送文本边界。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** TokenizerManager 在输入侧做 tokenization/校验，在输出侧做 detokenization；流式输出必须记住已发送文本边界。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 4. 如果「prompt」出错，初学者最容易观察到什么现象？

**参考答案：** prompt 是请求提供的上下文 token 序列；系统指令、历史对话和多模态占位都会影响长度与 KV 预算。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。prompt 是请求提供的上下文 token 序列；系统指令、历史对话和多模态占位都会影响长度与 KV 预算。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** prompt 是请求提供的上下文 token 序列；系统指令、历史对话和多模态占位都会影响长度与 KV 预算。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 5. 「prefill」和相邻模块之间通常传递什么数据？

**参考答案：** prefill 一次处理输入上下文并把每层 K/V 写入 cache；它通常计算密集，直接影响 TTFT。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。prefill 一次处理输入上下文并把每层 K/V 写入 cache；它通常计算密集，直接影响 TTFT。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** prefill 一次处理输入上下文并把每层 K/V 写入 cache；它通常计算密集，直接影响 TTFT。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 6. 什么是「decode」，它在 SGLang 中解决什么问题？

**参考答案：** decode 在已有 cache 上逐步生成新 token；单步 token 少，但要读取整个历史 KV，常受内存带宽和调度影响。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。decode 在已有 cache 上逐步生成新 token；单步 token 少，但要读取整个历史 KV，常受内存带宽和调度影响。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** decode 在已有 cache 上逐步生成新 token；单步 token 少，但要读取整个历史 KV，常受内存带宽和调度影响。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 7. SGLang 里为什么需要理解「logits」？

**参考答案：** logits 是词表每个 token 的未归一化分数；penalty、grammar 和 temperature 的顺序决定最终采样语义。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。logits 是词表每个 token 的未归一化分数；penalty、grammar 和 temperature 的顺序决定最终采样语义。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** logits 是词表每个 token 的未归一化分数；penalty、grammar 和 temperature 的顺序决定最终采样语义。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 8. 请用小白能懂的话解释「softmax」的作用。

**参考答案：** softmax 把分数变成概率；实现需要数值稳定，并处理 mask 后全为无穷小的异常情况。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。softmax 把分数变成概率；实现需要数值稳定，并处理 mask 后全为无穷小的异常情况。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** softmax 把分数变成概率；实现需要数值稳定，并处理 mask 后全为无穷小的异常情况。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 9. 如果「greedy decoding」出错，初学者最容易观察到什么现象？

**参考答案：** greedy 每步选最大 logit，确定性强但缺少随机探索；temperature 为零常会进入类似路径。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。greedy 每步选最大 logit，确定性强但缺少随机探索；temperature 为零常会进入类似路径。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** greedy 每步选最大 logit，确定性强但缺少随机探索；temperature 为零常会进入类似路径。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 10. 「temperature」和相邻模块之间通常传递什么数据？

**参考答案：** temperature 改变分数尺度；低温度使分布更尖，高温度带来更多随机性，极端值必须有明确语义。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。temperature 改变分数尺度；低温度使分布更尖，高温度带来更多随机性，极端值必须有明确语义。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** temperature 改变分数尺度；低温度使分布更尖，高温度带来更多随机性，极端值必须有明确语义。 本题属于“LLM 推理基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## B. 采样与停止（11-20）

### 11. 什么是「top-k」，它在 SGLang 中解决什么问题？

**参考答案：** top-k 保留固定数量最高分候选；k 的边界、排序稳定性和 grammar mask 后的候选数需要检查。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。top-k 保留固定数量最高分候选；k 的边界、排序稳定性和 grammar mask 后的候选数需要检查。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** top-k 保留固定数量最高分候选；k 的边界、排序稳定性和 grammar mask 后的候选数需要检查。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 12. SGLang 里为什么需要理解「top-p」？

**参考答案：** top-p 按概率累计保留动态候选；舍入、至少保留一个 token 和 mask 后空集合是常见边界。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。top-p 按概率累计保留动态候选；舍入、至少保留一个 token 和 mask 后空集合是常见边界。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** top-p 按概率累计保留动态候选；舍入、至少保留一个 token 和 mask 后空集合是常见边界。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 13. 请用小白能懂的话解释「EOS」的作用。

**参考答案：** EOS 是模型定义的结束 token；scheduler 判断 EOS 后仍需把 finish 结果传回并释放请求资源。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。EOS 是模型定义的结束 token；scheduler 判断 EOS 后仍需把 finish 结果传回并释放请求资源。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** EOS 是模型定义的结束 token；scheduler 判断 EOS 后仍需把 finish 结果传回并释放请求资源。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 14. 如果「stop string」出错，初学者最容易观察到什么现象？

**参考答案：** stop string 可能跨多个 token；TokenizerManager 需要增量拼接，避免将 stop 文本错误发送给客户端。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。stop string 可能跨多个 token；TokenizerManager 需要增量拼接，避免将 stop 文本错误发送给客户端。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** stop string 可能跨多个 token；TokenizerManager 需要增量拼接，避免将 stop 文本错误发送给客户端。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 15. 「最大新 token」和相邻模块之间通常传递什么数据？

**参考答案：** 它限制 decode 轮数和 KV 增长，是防止无限请求的调度边界。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。它限制 decode 轮数和 KV 增长，是防止无限请求的调度边界。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 它限制 decode 轮数和 KV 增长，是防止无限请求的调度边界。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 16. 什么是「上下文长度」，它在 SGLang 中解决什么问题？

**参考答案：** 上下文上限同时限制模型位置、KV 容量和 admission；输入与输出的总长度不能只看 prompt。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。上下文上限同时限制模型位置、KV 容量和 admission；输入与输出的总长度不能只看 prompt。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 上下文上限同时限制模型位置、KV 容量和 admission；输入与输出的总长度不能只看 prompt。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 17. SGLang 里为什么需要理解「流式输出」？

**参考答案：** streaming 每生成一段就返回；它降低感知延迟，却要求维护 offset、断连、重复消息和最终终态。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。streaming 每生成一段就返回；它降低感知延迟，却要求维护 offset、断连、重复消息和最终终态。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** streaming 每生成一段就返回；它降低感知延迟，却要求维护 offset、断连、重复消息和最终终态。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 18. 请用小白能懂的话解释「TTFT」的作用。

**参考答案：** TTFT 从请求进入到首 token，包含排队、tokenize、prefill 和首个输出；平均值会掩盖长尾。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。TTFT 从请求进入到首 token，包含排队、tokenize、prefill 和首个输出；平均值会掩盖长尾。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** TTFT 从请求进入到首 token，包含排队、tokenize、prefill 和首个输出；平均值会掩盖长尾。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 19. 如果「TPOT」出错，初学者最容易观察到什么现象？

**参考答案：** TPOT 反映后续 token 的间隔，decode 调度、KV 读取、采样和通信都会影响它。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。TPOT 反映后续 token 的间隔，decode 调度、KV 读取、采样和通信都会影响它。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** TPOT 反映后续 token 的间隔，decode 调度、KV 读取、采样和通信都会影响它。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 20. 「吞吐」和相邻模块之间通常传递什么数据？

**参考答案：** 吞吐是单位时间完成的 token/请求；只提升吞吐而牺牲 TTFT/TPOT 未必符合服务目标。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。吞吐是单位时间完成的 token/请求；只提升吞吐而牺牲 TTFT/TPOT 未必符合服务目标。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 吞吐是单位时间完成的 token/请求；只提升吞吐而牺牲 TTFT/TPOT 未必符合服务目标。 本题属于“采样与停止”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## C. SGLang 总体（21-30）

### 21. 什么是「SRT」，它在 SGLang 中解决什么问题？

**参考答案：** SRT 是 SGLang Runtime，串起请求管理、scheduler、模型执行、KV cache、sampling 和输出。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。SRT 是 SGLang Runtime，串起请求管理、scheduler、模型执行、KV cache、sampling 和输出。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** SRT 是 SGLang Runtime，串起请求管理、scheduler、模型执行、KV cache、sampling 和输出。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 22. SGLang 里为什么需要理解「Engine」？

**参考答案：** Engine 是离线/Python 调用入口；它与 HTTP server 的入口不同，但可汇入相似的 runtime 请求链。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。Engine 是离线/Python 调用入口；它与 HTTP server 的入口不同，但可汇入相似的 runtime 请求链。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** Engine 是离线/Python 调用入口；它与 HTTP server 的入口不同，但可汇入相似的 runtime 请求链。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 23. 请用小白能懂的话解释「HTTP server」的作用。

**参考答案：** HTTP server 把网络协议转换为内部请求，负责鉴权/参数/流式响应等边界，不应直接拥有 GPU KV 细节。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。HTTP server 把网络协议转换为内部请求，负责鉴权/参数/流式响应等边界，不应直接拥有 GPU KV 细节。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** HTTP server 把网络协议转换为内部请求，负责鉴权/参数/流式响应等边界，不应直接拥有 GPU KV 细节。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 24. 如果「ServerArgs」出错，初学者最容易观察到什么现象？

**参考答案：** ServerArgs 承载启动输入和派生配置；模型、设备、并行规模决定的值应在启动期解析而非每 token 重算。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。ServerArgs 承载启动输入和派生配置；模型、设备、并行规模决定的值应在启动期解析而非每 token 重算。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** ServerArgs 承载启动输入和派生配置；模型、设备、并行规模决定的值应在启动期解析而非每 token 重算。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 25. 「配置解析」和相邻模块之间通常传递什么数据？

**参考答案：** 配置解析将 raw CLI/环境输入投影成角色所需 runtime config；不同进程看到的字段集合可以不同。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。配置解析将 raw CLI/环境输入投影成角色所需 runtime config；不同进程看到的字段集合可以不同。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 配置解析将 raw CLI/环境输入投影成角色所需 runtime config；不同进程看到的字段集合可以不同。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 26. 什么是「控制面」，它在 SGLang 中解决什么问题？

**参考答案：** 控制面决定请求、配置、队列和状态；它不等于 GPU 上的实际 kernel 完成状态。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。控制面决定请求、配置、队列和状态；它不等于 GPU 上的实际 kernel 完成状态。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 控制面决定请求、配置、队列和状态；它不等于 GPU 上的实际 kernel 完成状态。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 27. SGLang 里为什么需要理解「执行面」？

**参考答案：** 执行面负责 tensor、attention、模型层、通信和采样；它通过 batch/结果契约接受控制面计划。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。执行面负责 tensor、attention、模型层、通信和采样；它通过 batch/结果契约接受控制面计划。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 执行面负责 tensor、attention、模型层、通信和采样；它通过 batch/结果契约接受控制面计划。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 28. 请用小白能懂的话解释「ready 信号」的作用。

**参考答案：** ready 应表示模型、通信、KV pool 和 backend 初始化完成；进程存活只表示 OS 进程没有退出。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。ready 应表示模型、通信、KV pool 和 backend 初始化完成；进程存活只表示 OS 进程没有退出。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** ready 应表示模型、通信、KV pool 和 backend 初始化完成；进程存活只表示 OS 进程没有退出。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 29. 如果「进程边界」出错，初学者最容易观察到什么现象？

**参考答案：** Tokenizer、Scheduler、Detokenizer 和 worker 之间的进程边界决定序列化、错误传播和资源清理方式。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。Tokenizer、Scheduler、Detokenizer 和 worker 之间的进程边界决定序列化、错误传播和资源清理方式。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** Tokenizer、Scheduler、Detokenizer 和 worker 之间的进程边界决定序列化、错误传播和资源清理方式。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 30. 「IPC」和相邻模块之间通常传递什么数据？

**参考答案：** IPC 传输 tokenized request、结果、abort、shutdown 和 ready；字段改动必须同时检查发送、接收和兼容性。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。IPC 传输 tokenized request、结果、abort、shutdown 和 ready；字段改动必须同时检查发送、接收和兼容性。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** IPC 传输 tokenized request、结果、abort、shutdown 和 ready；字段改动必须同时检查发送、接收和兼容性。 本题属于“SGLang 总体”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## D. 请求对象（31-40）

### 31. 什么是「GenerateReqInput」，它在 SGLang 中解决什么问题？

**参考答案：** 它是 API 侧原始请求对象，面向用户参数；dispatch 前需要规范化为 scheduler 能消费的 tokenized 形态。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。它是 API 侧原始请求对象，面向用户参数；dispatch 前需要规范化为 scheduler 能消费的 tokenized 形态。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 它是 API 侧原始请求对象，面向用户参数；dispatch 前需要规范化为 scheduler 能消费的 tokenized 形态。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 32. SGLang 里为什么需要理解「TokenizedGenerateReqInput」？

**参考答案：** 它携带已处理 token ids 和采样/会话元数据，适合跨进程传递，不应依赖 HTTP 对象生命周期。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。它携带已处理 token ids 和采样/会话元数据，适合跨进程传递，不应依赖 HTTP 对象生命周期。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 它携带已处理 token ids 和采样/会话元数据，适合跨进程传递，不应依赖 HTTP 对象生命周期。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 33. 请用小白能懂的话解释「ReqState」的作用。

**参考答案：** ReqState 位于 tokenizer/API 侧，保存 event、输出 chunks、完成状态和 rid 映射；它不是 GPU scheduler 的请求对象。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。ReqState 位于 tokenizer/API 侧，保存 event、输出 chunks、完成状态和 rid 映射；它不是 GPU scheduler 的请求对象。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** ReqState 位于 tokenizer/API 侧，保存 event、输出 chunks、完成状态和 rid 映射；它不是 GPU scheduler 的请求对象。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 34. 如果「scheduler-side Req」出错，初学者最容易观察到什么现象？

**参考答案：** Req 是 scheduler 侧的运行时状态，维护 token、队列、KV、finish reason 和采样相关字段。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。Req 是 scheduler 侧的运行时状态，维护 token、队列、KV、finish reason 和采样相关字段。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** Req 是 scheduler 侧的运行时状态，维护 token、队列、KV、finish reason 和采样相关字段。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 35. 「rid」和相邻模块之间通常传递什么数据？

**参考答案：** rid 是跨请求对象、batch 输出和本地 state 的关联键；batch 位置改变时仍靠 rid 找回正确调用者。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。rid 是跨请求对象、batch 输出和本地 state 的关联键；batch 位置改变时仍靠 rid 找回正确调用者。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** rid 是跨请求对象、batch 输出和本地 state 的关联键；batch 位置改变时仍靠 rid 找回正确调用者。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 36. 什么是「BatchOutput」，它在 SGLang 中解决什么问题？

**参考答案：** BatchOutput 按 batch 行携带 token/文本/状态；接收方必须用 rids 重新映射，而不能盲信列表位置。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。BatchOutput 按 batch 行携带 token/文本/状态；接收方必须用 rids 重新映射，而不能盲信列表位置。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** BatchOutput 按 batch 行携带 token/文本/状态；接收方必须用 rids 重新映射，而不能盲信列表位置。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 37. SGLang 里为什么需要理解「stream」？

**参考答案：** 「stream」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「stream」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「stream」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 38. 请用小白能懂的话解释「abort」的作用。

**参考答案：** abort 是取消/失败控制消息；dispatch 后的 abort 需要让 scheduler 停止推进并释放 row、slot 和 cache 引用。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。abort 是取消/失败控制消息；dispatch 后的 abort 需要让 scheduler 停止推进并释放 row、slot 和 cache 引用。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** abort 是取消/失败控制消息；dispatch 后的 abort 需要让 scheduler 停止推进并释放 row、slot 和 cache 引用。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 39. 如果「finish reason」出错，初学者最容易观察到什么现象？

**参考答案：** 「finish reason」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「finish reason」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「finish reason」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 40. 「detokenization」和相邻模块之间通常传递什么数据？

**参考答案：** detokenization 把 token 增量转成文本；UTF-8 和子词边界使“每 token 一段字符串”的假设不可靠。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。detokenization 把 token 增量转成文本；UTF-8 和子词边界使“每 token 一段字符串”的假设不可靠。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** detokenization 把 token 增量转成文本；UTF-8 和子词边界使“每 token 一段字符串”的假设不可靠。 本题属于“请求对象”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## E. 调度基础（41-50）

### 41. 什么是「Scheduler」，它在 SGLang 中解决什么问题？

**参考答案：** Req 是 scheduler 侧的运行时状态，维护 token、队列、KV、finish reason 和采样相关字段。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。Req 是 scheduler 侧的运行时状态，维护 token、队列、KV、finish reason 和采样相关字段。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** Req 是 scheduler 侧的运行时状态，维护 token、队列、KV、finish reason 和采样相关字段。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 42. SGLang 里为什么需要理解「waiting queue」？

**参考答案：** waiting queue 保存尚未进入当前执行计划的请求；等待时间长可能是预算/优先级问题而非模型慢。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。waiting queue 保存尚未进入当前执行计划的请求；等待时间长可能是预算/优先级问题而非模型慢。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** waiting queue 保存尚未进入当前执行计划的请求；等待时间长可能是预算/优先级问题而非模型慢。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 43. 请用小白能懂的话解释「running batch」的作用。

**参考答案：** running batch 表示 scheduler 已接纳的请求集合；它还可能对应尚未完成的异步设备工作。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。running batch 表示 scheduler 已接纳的请求集合；它还可能对应尚未完成的异步设备工作。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** running batch 表示 scheduler 已接纳的请求集合；它还可能对应尚未完成的异步设备工作。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 44. 如果「admission」出错，初学者最容易观察到什么现象？

**参考答案：** admission 同时检查 token、KV page、请求数、prefix hit 和 backend shape 约束。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。admission 同时检查 token、KV page、请求数、prefix hit 和 backend shape 约束。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** admission 同时检查 token、KV page、请求数、prefix hit 和 backend shape 约束。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 45. 「continuous batching」和相邻模块之间通常传递什么数据？

**参考答案：** continuous batching 每轮允许请求加入/完成/离开，使 GPU 少等待，但 admission 和状态清理更复杂。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。continuous batching 每轮允许请求加入/完成/离开，使 GPU 少等待，但 admission 和状态清理更复杂。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** continuous batching 每轮允许请求加入/完成/离开，使 GPU 少等待，但 admission 和状态清理更复杂。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 46. 什么是「ScheduleBatch」，它在 SGLang 中解决什么问题？

**参考答案：** ScheduleBatch 是 scheduler 的计划和资源对象，可能跨轮保存并被 overlap 逻辑引用。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。ScheduleBatch 是 scheduler 的计划和资源对象，可能跨轮保存并被 overlap 逻辑引用。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** ScheduleBatch 是 scheduler 的计划和资源对象，可能跨轮保存并被 overlap 逻辑引用。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 47. SGLang 里为什么需要理解「ForwardBatch」？

**参考答案：** ForwardBatch 是一次设备执行的快照，包含 input ids、seq lens、KV locations 和 attention metadata。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。ForwardBatch 是一次设备执行的快照，包含 input ids、seq lens、KV locations 和 attention metadata。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** ForwardBatch 是一次设备执行的快照，包含 input ids、seq lens、KV locations 和 attention metadata。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 48. 请用小白能懂的话解释「PrefillAdder」的作用。

**参考答案：** PrefillAdder 将等待请求按预算逐个放入 prefill，并处理 prefix、chunking 和资源不足。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。PrefillAdder 将等待请求按预算逐个放入 prefill，并处理 prefix、chunking 和资源不足。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** PrefillAdder 将等待请求按预算逐个放入 prefill，并处理 prefix、chunking 和资源不足。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 49. 如果「chunked prefill」出错，初学者最容易观察到什么现象？

**参考答案：** chunked prefill 把长 prompt 分多轮处理；必须保留 request row、KV 映射和推进位置。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。chunked prefill 把长 prompt 分多轮处理；必须保留 request row、KV 映射和推进位置。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** chunked prefill 把长 prompt 分多轮处理；必须保留 request row、KV 映射和推进位置。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 50. 「batch row」和相邻模块之间通常传递什么数据？

**参考答案：** batch 是一次 forward 同时处理的请求/token 集合；不同长度需要 seq lens、索引和 mask metadata。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。batch 是一次 forward 同时处理的请求/token 集合；不同长度需要 seq lens、索引和 mask metadata。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** batch 是一次 forward 同时处理的请求/token 集合；不同长度需要 seq lens、索引和 mask metadata。 本题属于“调度基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## F. KV 基础（51-60）

### 51. 什么是「KV cache」，它在 SGLang 中解决什么问题？

**参考答案：** KV cache 保存 attention 历史的 key/value，避免每个 decode step 重新计算全部历史，是推理服务主要显存账本。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。KV cache 保存 attention 历史的 key/value，避免每个 decode step 重新计算全部历史，是推理服务主要显存账本。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** KV cache 保存 attention 历史的 key/value，避免每个 decode step 重新计算全部历史，是推理服务主要显存账本。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 52. SGLang 里为什么需要理解「KV slot」？

**参考答案：** KV slot 是物理 cache 存储位置；它和表示请求的 row 不是同一个概念。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。KV slot 是物理 cache 存储位置；它和表示请求的 row 不是同一个概念。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** KV slot 是物理 cache 存储位置；它和表示请求的 row 不是同一个概念。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 53. 请用小白能懂的话解释「request row」的作用。

**参考答案：** request row 是请求到 token/KV 映射表中的行；释放 row 前要确保设备和延迟结果不再使用。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。request row 是请求到 token/KV 映射表中的行；释放 row 前要确保设备和延迟结果不再使用。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** request row 是请求到 token/KV 映射表中的行；释放 row 前要确保设备和延迟结果不再使用。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 54. 如果「ReqToTokenPool」出错，初学者最容易观察到什么现象？

**参考答案：** 它维护逻辑 token 位置到物理 KV slot 的映射，让 attention 找到正确的历史。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。它维护逻辑 token 位置到物理 KV slot 的映射，让 attention 找到正确的历史。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 它维护逻辑 token 位置到物理 KV slot 的映射，让 attention 找到正确的历史。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 55. 「token-to-KV allocator」和相邻模块之间通常传递什么数据？

**参考答案：** token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 56. 什么是「Radix Cache」，它在 SGLang 中解决什么问题？

**参考答案：** Radix Cache 按 token 前缀组织可复用 KV；命中减少 prefill，但增加 lock/ref、eviction 和 ownership 约束。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。Radix Cache 按 token 前缀组织可复用 KV；命中减少 prefill，但增加 lock/ref、eviction 和 ownership 约束。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** Radix Cache 按 token 前缀组织可复用 KV；命中减少 prefill，但增加 lock/ref、eviction 和 ownership 约束。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 57. SGLang 里为什么需要理解「prefix hit」？

**参考答案：** prefix hit 返回已有 KV indices；本轮只计算未命中区间，同时仍需按完整序列建立 metadata。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。prefix hit 返回已有 KV indices；本轮只计算未命中区间，同时仍需按完整序列建立 metadata。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** prefix hit 返回已有 KV indices；本轮只计算未命中区间，同时仍需按完整序列建立 metadata。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 58. 请用小白能懂的话解释「page」的作用。

**参考答案：** page 是 cache 管理和 attention 索引的粒度；page size 会影响碎片、命中率和 metadata 开销。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。page 是 cache 管理和 attention 索引的粒度；page size 会影响碎片、命中率和 metadata 开销。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** page 是 cache 管理和 attention 索引的粒度；page size 会影响碎片、命中率和 metadata 开销。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 59. 如果「cache lock」出错，初学者最容易观察到什么现象？

**参考答案：** lock/ref 防止正在使用的 cache node 被 eviction；漏加会过早释放，漏减会泄漏。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。lock/ref 防止正在使用的 cache node 被 eviction；漏加会过早释放，漏减会泄漏。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** lock/ref 防止正在使用的 cache node 被 eviction；漏加会过早释放，漏减会泄漏。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 60. 「cache cleanup」和相邻模块之间通常传递什么数据？

**参考答案：** cleanup 必须同时处理 row、slot、radix 引用、sampling 状态和本地 rid state。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。cleanup 必须同时处理 row、slot、radix 引用、sampling 状态和本地 rid state。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** cleanup 必须同时处理 row、slot、radix 引用、sampling 状态和本地 rid state。 本题属于“KV 基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## G. 模型执行（61-70）

### 61. 什么是「TpModelWorker」，它在 SGLang 中解决什么问题？

**参考答案：** 它把 scheduler 的计划转换成设备 forward，并处理 PP rank、prefill/decode 和结果契约。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。它把 scheduler 的计划转换成设备 forward，并处理 PP rank、prefill/decode 和结果契约。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 它把 scheduler 的计划转换成设备 forward，并处理 PP rank、prefill/decode 和结果契约。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 62. SGLang 里为什么需要理解「ModelRunner」？

**参考答案：** ModelRunner 持有模型和执行 backend，负责 attention metadata、eager/graph 选择、forward 和 sampling。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。ModelRunner 持有模型和执行 backend，负责 attention metadata、eager/graph 选择、forward 和 sampling。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** ModelRunner 持有模型和执行 backend，负责 attention metadata、eager/graph 选择、forward 和 sampling。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 63. 请用小白能懂的话解释「ForwardBatch.init_new」的作用。

**参考答案：** ForwardBatch 是一次设备执行的快照，包含 input ids、seq lens、KV locations 和 attention metadata。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。ForwardBatch 是一次设备执行的快照，包含 input ids、seq lens、KV locations 和 attention metadata。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** ForwardBatch 是一次设备执行的快照，包含 input ids、seq lens、KV locations 和 attention metadata。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 64. 如果「attention backend」出错，初学者最容易观察到什么现象？

**参考答案：** 「attention backend」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「attention backend」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「attention backend」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 65. 「CUDA Graph」和相邻模块之间通常传递什么数据？

**参考答案：** CUDA Graph 复用已捕获的 launch 图，要求 shape、地址和 metadata 满足条件；不能把“想用”当成“一定可用”。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。CUDA Graph 复用已捕获的 launch 图，要求 shape、地址和 metadata 满足条件；不能把“想用”当成“一定可用”。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** CUDA Graph 复用已捕获的 launch 图，要求 shape、地址和 metadata 满足条件；不能把“想用”当成“一定可用”。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 66. 什么是「eager mode」，它在 SGLang 中解决什么问题？

**参考答案：** eager 允许动态 shape 和 metadata，但 launch 开销可能更高；它是 graph 不适用时的正确性兜底。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。eager 允许动态 shape 和 metadata，但 launch 开销可能更高；它是 graph 不适用时的正确性兜底。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** eager 允许动态 shape 和 metadata，但 launch 开销可能更高；它是 graph 不适用时的正确性兜底。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 67. SGLang 里为什么需要理解「sampling batch」？

**参考答案：** batch 是一次 forward 同时处理的请求/token 集合；不同长度需要 seq lens、索引和 mask metadata。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。batch 是一次 forward 同时处理的请求/token 集合；不同长度需要 seq lens、索引和 mask metadata。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** batch 是一次 forward 同时处理的请求/token 集合；不同长度需要 seq lens、索引和 mask metadata。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 68. 请用小白能懂的话解释「GenerationBatchResult」的作用。

**参考答案：** 它承载 logits、next token、PP proxy 或辅助结果；字段是否有效取决于 forward mode 和 rank。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。它承载 logits、next token、PP proxy 或辅助结果；字段是否有效取决于 forward mode 和 rank。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 它承载 logits、next token、PP proxy 或辅助结果；字段是否有效取决于 forward mode 和 rank。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 69. 如果「PP rank」出错，初学者最容易观察到什么现象？

**参考答案：** PP 切分层间 stage；P2P hidden 传递和 stage 顺序构成执行契约。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。PP 切分层间 stage；P2P hidden 传递和 stage 顺序构成执行契约。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** PP 切分层间 stage；P2P hidden 传递和 stage 顺序构成执行契约。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 70. 「logprob」和相邻模块之间通常传递什么数据？

**参考答案：** logprob 必须对应实际采样所用的 logits 语义和 token 历史；prefill/decode、filter/reorder 会改变对齐风险。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。logprob 必须对应实际采样所用的 logits 语义和 token 历史；prefill/decode、filter/reorder 会改变对齐风险。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** logprob 必须对应实际采样所用的 logits 语义和 token 历史；prefill/decode、filter/reorder 会改变对齐风险。 本题属于“模型执行”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## H. 并行基础（71-80）

### 71. 什么是「TP」，它在 SGLang 中解决什么问题？

**参考答案：** TP 切分层内计算；group、权重 shard、activation shape 和 collective 都必须一致。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。TP 切分层内计算；group、权重 shard、activation shape 和 collective 都必须一致。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** TP 切分层内计算；group、权重 shard、activation shape 和 collective 都必须一致。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 72. SGLang 里为什么需要理解「PP」？

**参考答案：** PP 切分层间 stage；P2P hidden 传递和 stage 顺序构成执行契约。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。PP 切分层间 stage；P2P hidden 传递和 stage 顺序构成执行契约。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** PP 切分层间 stage；P2P hidden 传递和 stage 顺序构成执行契约。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 73. 请用小白能懂的话解释「DP」的作用。

**参考答案：** DP 复制执行并按请求/数据分工；不同 rank 的 batch/padding 仍可能必须参加同步。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。DP 复制执行并按请求/数据分工；不同 rank 的 batch/padding 仍可能必须参加同步。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** DP 复制执行并按请求/数据分工；不同 rank 的 batch/padding 仍可能必须参加同步。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 74. 如果「EP」出错，初学者最容易观察到什么现象？

**参考答案：** EP 将 experts 分布到 rank，routing 后需要 dispatch/combine；动态负载造成通信和尾部延迟。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。EP 将 experts 分布到 rank，routing 后需要 dispatch/combine；动态负载造成通信和尾部延迟。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** EP 将 experts 分布到 rank，routing 后需要 dispatch/combine；动态负载造成通信和尾部延迟。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 75. 「global rank」和相邻模块之间通常传递什么数据？

**参考答案：** 「global rank」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「global rank」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「global rank」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 76. 什么是「local GPU」，它在 SGLang 中解决什么问题？

**参考答案：** 「local GPU」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「local GPU」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「local GPU」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 77. SGLang 里为什么需要理解「process group」？

**参考答案：** 「process group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「process group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「process group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 78. 请用小白能懂的话解释「collective」的作用。

**参考答案：** 「collective」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「collective」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「collective」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 79. 如果「placement」出错，初学者最容易观察到什么现象？

**参考答案：** 「placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 80. 「multi-node」和相邻模块之间通常传递什么数据？

**参考答案：** 「multi-node」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「multi-node」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「multi-node」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“并行基础”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## I. 加载与扩展（81-90）

### 81. 什么是「model loader」，它在 SGLang 中解决什么问题？

**参考答案：** loader 读取 checkpoint 并将参数交给模型对象；加载成功不代表 KV/backend/graph 已 ready。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。loader 读取 checkpoint 并将参数交给模型对象；加载成功不代表 KV/backend/graph 已 ready。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** loader 读取 checkpoint 并将参数交给模型对象；加载成功不代表 KV/backend/graph 已 ready。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 82. SGLang 里为什么需要理解「checkpoint shard」？

**参考答案：** 文件 shard 是存储布局，运行时 TP/PP shard 是计算布局；loader 负责二者映射。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。文件 shard 是存储布局，运行时 TP/PP shard 是计算布局；loader 负责二者映射。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 文件 shard 是存储布局，运行时 TP/PP shard 是计算布局；loader 负责二者映射。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 83. 请用小白能懂的话解释「weight_loader」的作用。

**参考答案：** weight_loader 处理切片、fused 参数、transpose、quant scale 和 expert 归属，不能用简单字典赋值替代。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。weight_loader 处理切片、fused 参数、transpose、quant scale 和 expert 归属，不能用简单字典赋值替代。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** weight_loader 处理切片、fused 参数、transpose、quant scale 和 expert 归属，不能用简单字典赋值替代。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 84. 如果「quantization」出错，初学者最容易观察到什么现象？

**参考答案：** 量化不只是换 dtype，还包括 packed layout、scale、group size 和 kernel 解释方式。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。量化不只是换 dtype，还包括 packed layout、scale、group size 和 kernel 解释方式。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 量化不只是换 dtype，还包括 packed layout、scale、group size 和 kernel 解释方式。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 85. 「MoE」和相邻模块之间通常传递什么数据？

**参考答案：** MoE 用 router 将 token 发给少数 experts；容量、负载均衡和 dispatch 通信决定性能。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。MoE 用 router 将 token 发给少数 experts；容量、负载均衡和 dispatch 通信决定性能。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** MoE 用 router 将 token 发给少数 experts；容量、负载均衡和 dispatch 通信决定性能。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 86. 什么是「LoRA」，它在 SGLang 中解决什么问题？

**参考答案：** LoRA 为请求或模型增加低秩 adapter；不同 adapter 可能阻止 batch 合并或 CUDA Graph 复用。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。LoRA 为请求或模型增加低秩 adapter；不同 adapter 可能阻止 batch 合并或 CUDA Graph 复用。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** LoRA 为请求或模型增加低秩 adapter；不同 adapter 可能阻止 batch 合并或 CUDA Graph 复用。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 87. SGLang 里为什么需要理解「multimodal」？

**参考答案：** 多模态输入可能把媒体转成 embedding/占位 token，token 数与实际输入布局不再是简单文本假设。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。多模态输入可能把媒体转成 embedding/占位 token，token 数与实际输入布局不再是简单文本假设。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 多模态输入可能把媒体转成 embedding/占位 token，token 数与实际输入布局不再是简单文本假设。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 88. 请用小白能懂的话解释「speculative decoding」的作用。

**参考答案：** speculative 先由 draft 提议多个 token，再由 target 验证；accepted token 才能提交到序列和 KV。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。speculative 先由 draft 提议多个 token，再由 target 验证；accepted token 才能提交到序列和 KV。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** speculative 先由 draft 提议多个 token，再由 target 验证；accepted token 才能提交到序列和 KV。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 89. 如果「disaggregation」出错，初学者最容易观察到什么现象？

**参考答案：** disaggregation 把 prefill/decode 或服务角色拆开；KV 传输需要 layout、版本、完成事件和失败回收协议。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。disaggregation 把 prefill/decode 或服务角色拆开；KV 传输需要 layout、版本、完成事件和失败回收协议。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** disaggregation 把 prefill/decode 或服务角色拆开；KV 传输需要 layout、版本、完成事件和失败回收协议。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 90. 「HiCache」和相邻模块之间通常传递什么数据？

**参考答案：** 分层 cache 在 GPU、host 或远端之间迁移 KV；每层必须有版本和 ownership，否则可能读到旧数据。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。分层 cache 在 GPU、host 或远端之间迁移 KV；每层必须有版本和 ownership，否则可能读到旧数据。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 分层 cache 在 GPU、host 或远端之间迁移 KV；每层必须有版本和 ownership，否则可能读到旧数据。 本题属于“加载与扩展”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
## J. 排错入门（91-100）

### 91. 什么是「请求超时」，它在 SGLang 中解决什么问题？

**参考答案：** 「请求超时」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「请求超时」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「请求超时」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 92. SGLang 里为什么需要理解「进程启动失败」？

**参考答案：** 「进程启动失败」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「进程启动失败」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「进程启动失败」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 93. 请用小白能懂的话解释「显存不足」的作用。

**参考答案：** 显存不足可能来自真实容量、碎片、protected cache、graph buffer 或 allocator 泄漏，不能只看 nvidia-smi。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。显存不足可能来自真实容量、碎片、protected cache、graph buffer 或 allocator 泄漏，不能只看 nvidia-smi。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 显存不足可能来自真实容量、碎片、protected cache、graph buffer 或 allocator 泄漏，不能只看 nvidia-smi。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 94. 如果「输出错位」出错，初学者最容易观察到什么现象？

**参考答案：** 「输出错位」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「输出错位」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「输出错位」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 95. 「重复 token」和相邻模块之间通常传递什么数据？

**参考答案：** token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** token 是 tokenizer 产生的整数 ID；一个汉字、词片段或空格都可能是不同 token，不能假定 token 与字符一一对应。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 96. 什么是「graph fallback」，它在 SGLang 中解决什么问题？

**参考答案：** 「graph fallback」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「graph fallback」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「graph fallback」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 97. SGLang 里为什么需要理解「tokenizer 瓶颈」？

**参考答案：** TokenizerManager 在输入侧做 tokenization/校验，在输出侧做 detokenization；流式输出必须记住已发送文本边界。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。TokenizerManager 在输入侧做 tokenization/校验，在输出侧做 detokenization；流式输出必须记住已发送文本边界。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** TokenizerManager 在输入侧做 tokenization/校验，在输出侧做 detokenization；流式输出必须记住已发送文本边界。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 98. 请用小白能懂的话解释「scheduler 瓶颈」的作用。

**参考答案：** 「scheduler 瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「scheduler 瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「scheduler 瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 99. 如果「GPU 瓶颈」出错，初学者最容易观察到什么现象？

**参考答案：** 「GPU 瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「GPU 瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「GPU 瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。
### 100. 「未验证性能」和相邻模块之间通常传递什么数据？

**参考答案：** 「未验证性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 这道题的核心结论是：必须同时追踪请求 ID、batch 行、request row、KV slot 和终态；推理服务的逻辑状态与 GPU 异步执行完成点不是同一件事。

**小白解释：** 可以把 SGLang 想成一个同时接待很多人的厨房。「未验证性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 就像点单、排队、备料、烹饪或出餐中的一个步骤；如果只看步骤名字而不看订单号，就可能把一个人的结果交给另一个人。

**技术分析：** 「未验证性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“排错入门”主题。在 SGLang 的推理主线中，相关对象通常沿请求状态、scheduler、batch、设备执行、KV cache 或输出 IPC 边界传递。 重点检查三条链：请求是否通过 TokenizerManager/ReqState 和 IPC 正确交给 Scheduler/Req；batch 构造是否保持 rid、行、seq lens、KV metadata 和 sampling 参数的一致置换；设备 stream/event 完成后，row、slot、radix 引用和输出 state 才能安全回收。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 知识库以 `srt/entrypoints`、managers、model_executor、mem_cache、distributed 和现有请求流程文章为证据；真实 GPU、模型和多卡行为仍需运行验证。

**拓展分析：** 可继续追问：如何构造三个带不同 token、seed、grammar 和 KV sentinel 的请求，随机执行 filter、prefix hit、chunked prefill、retraction、overlap 和 abort，证明不会串行或泄漏？性能上记录 TTFT/TPOT 的分位数、队列时间、cache 命中、retraction、graph fallback、IPC 和 GPU forward 时间；单 kernel 变快不等于端到端变快。


## 使用建议

先遮住答案自测，再对照四段内容复盘。回答技术题时，明确对象、输入输出、状态变化、资源所有权、失败语义和验证边界；不要把未执行的 GPU、NPU、模型、多卡或性能命令写成运行事实。
