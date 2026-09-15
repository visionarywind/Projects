# D02 - CLI 文本推理

- 文档目的：解释 80-demos/D02-cli-inference/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D02-cli-inference/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 文档元数据（规范补充）

- 文档目的：说明 `80-demos/D02-cli-inference/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `80-demos/D02-cli-inference/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
