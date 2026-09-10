# Demos 注册表

| ID | Demo | 目标 | 覆盖模块 | 实际状态 |
|---|---|---|---|---|
| D01 | Server 非流式 Chat Completion | 从 HTTP JSON 到生成结果 | M03、M04、M02、M01（并可涉及 M06） | 静态解剖，未运行 |
| D02 | CLI 文本推理 | 最短用户侧推理路径 | M05、M03、M02、M01 | 静态解剖，未运行 |

D01 是主 Demo，因为它的测试既覆盖 prompt/template、usage、response，又覆盖 server queue/decode；D02 用于在进入并发 server 前理解最小 consumer。这是文档组织推断，不是性能优先级。

---

**页尾：** Demo 状态只有执行命令并记录输出后才可改为“已运行”。
