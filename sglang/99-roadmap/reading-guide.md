# 阅读路线

## 初学者路线

1. `00-overview/project-overview.md`、`architecture.md`、`glossary.md`。
2. `01-concepts/01-推理服务基础概念.md` 和 `02-Transformer与KV-Cache.md`。
3. M03：理解输入如何 tokenize、建立 `ReqState`、跨 IPC 和回收。
4. M04：理解 waiting/running、PrefillAdder、ScheduleBatch 和 retraction。
5. M08：理解 request row、KV slots 和 Radix prefix。
6. M05：理解 ForwardBatch、worker、forward 和 sample。
7. M09/M10：理解 graph/eager、attention metadata、grammar 和 sampling。
8. M06/M07：回看模型加载和并行拓扑如何在启动时建立上述资源。
9. D01 和 `90-cross-module/`：把模块拼成一条真实请求轨迹。

## 修改者路线

- 改输入或输出：M03 → interface-contracts → M04/M15；
- 改 admission/KV：M04 → M08 → M09 → retraction tests；
- 改 forward：M04 → M05 → M09 → model/attention backend；
- 改采样/grammar：M10 → M05 → M03 output/finish；
- 改 rank/group：M06 → M07 → M05 model layers；
- 改配置：configuration-impact-map → affected role/process → runtime path。

## 证据规则

先看函数真实调用者和被调用者，再写结论；每条关键结论给出相对路径和行号；静态推断与实际执行分开。
