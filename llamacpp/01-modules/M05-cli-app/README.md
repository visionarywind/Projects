# M05 - CLI 与统一应用

> 状态：首版入口层分析。

## 统一入口

`app/llama.cpp` 用静态 command table 将 `serve`、`cli`、`download` 以及隐藏的 bench/quantize/perplexity 等名称映射到实现函数；匹配后设置 `LLAMA_APP_CMD` 并调用函数。[app/llama.cpp:64-78] [app/llama.cpp:129-148]

## CLI 边界

`tools/cli` 产出可复用 `llama-cli-impl` 和 executable；它与 common、server impl 共享基础设施，CLI 的具体 prompt loop 和采样处理在 `tools/cli/cli*.cpp`，需在专题变更时继续追踪。[tools/cli/CMakeLists.txt:1-27]

## 最小运行路径

`llama-cli -m MODEL -p PROMPT -n N` 是产品入口；它最终调用 M02 的 model/context/decode，读取 logits 并通过 common sampler 输出 token。命令来源已确认于根开发指导，但本批未执行。

## 错误与影响

统一入口未知 command 返回 1；子命令错误由子实现返回。修改 command table 会影响 aliases、router child re-invocation 和安装二进制名称；修改 CLI loop 会影响 tokenizer/template/sampler 行为。

---

**页尾：** CLI 的全量交互状态机尚未逐行展开。
