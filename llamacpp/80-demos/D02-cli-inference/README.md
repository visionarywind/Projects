# D02 - CLI 文本推理

> 状态：`llama-cli` CPU 构建和 `--help` smoke test 已通过，未运行模型推理。

## 命令

```bash
cmake --build build --target llama-cli -j$(nproc)
./build/bin/llama-cli -m PATH_TO_MODEL -p "Hello" -n 32
```

也可使用统一程序：

```bash
./build/bin/llama cli -m PATH_TO_MODEL -p "Hello" -n 32
```

命令来源已确认于根开发指导；模型路径、生成结果和性能均未验证。

## 轨迹

统一程序 main 从 command table 匹配 `cli`，设置 `LLAMA_APP_CMD` 后调用 `llama_cli`。[app/llama.cpp:64-78] [app/llama.cpp:129-148] CLI 实现再使用 common 参数和 runtime model/context/decode。模型加载、架构派发、context 约束和 scheduler 轨迹见 [M02](../../01-modules/M02-llama-runtime/README.md)。

## 与 D01 的差异

D02 没有 HTTP worker、server queue 或 slot batching；它适合观察单 consumer 如何提交 batch。D01 的复杂性来自请求并发、task/result、SSE/JSON 和 server lifecycle，而不是另一套底层模型执行。

## 观察点

`./build/bin/llama --help` 查看 command table；`--list-devices` 检查设备；`--device none` 禁用 accelerator（命令来源已确认，未验证）。

---

**页尾：** 本页不假定 CLI 的默认 sampler 或 prompt 行为在未来版本不变。
