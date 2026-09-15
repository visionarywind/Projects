# 配置影响图

- 文档目的：说明配置解析结果如何影响进程、并行拓扑、模型加载、KV 和调度。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10

## 配置传播

```text
ServerArgs raw input
  -> resolve_once
  -> publish / role-specific config
  -> PortArgs + process launch
  -> ModelConfig / LoadConfig
  -> TP/PP/other groups
  -> KV pools and attention backend
  -> scheduler admission and batch mode
```

**已确认**：原始 CLI 参数不是所有 runtime 代码直接消费的最终配置；解析后的值被投影到不同角色和进程。启动顺序在 Engine、scheduler 和并行初始化源码中可见。[`python/sglang/srt/entrypoints/engine.py:846-962`][`python/sglang/srt/managers/scheduler.py:1094-1119`][`python/sglang/srt/distributed/bootstrap.py:70-172`]

## 影响矩阵

| 配置类别 | 直接影响 | 需要联查 |
|---|---|---|
| model/tokenizer | ModelConfig、tokenizer、输入合法性 | M03/M05/M06 |
| `tp_size`/`pp_size` | rank、groups、worker 拓扑 | M06/M07 |
| max sequence/KV memory | pool 容量、admission、retraction | M04/M08/M09 |
| prefill/chunk budget | `PrefillAdder` 和 batch 边界 | M04/M08 |
| overlap/graph | batch 生命周期、stream/event 和 capture | M04/M05/M09 |
| sampling/grammar | `ScheduleBatch` flags 和 result processor | M04/M10 |

## 变更规则

配置值若被 resolution 派生，必须在解析阶段固定并发布，不能在热路径重复推导。改变一个字段前应追踪它进入哪个 process、哪个 device tensor，以及失败时由谁报告。

**未验证**：本文未执行不同配置组合的启动矩阵，也未执行多卡通信。

## 相关文档

- [系统 wiring](system-wiring.md)
- [接口契约](interface-contracts.md)
- [修改影响图](change-impact-map.md)
