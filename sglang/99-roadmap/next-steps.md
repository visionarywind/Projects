# 后续步骤

## 下一批优先级

1. 复核 43 篇 Markdown 的链接、fence、标题和源码行号；
2. 完成 M09/M10 与 M03-M08 的双向链接和真实测试地图；
3. 深化 M06 loader、M07 group 消费、M08 allocator/eviction、M09 capture 和 M10 sampler kernel；
4. 补 M01/M02、M11-M18 中最影响普通请求主线的模块；
5. 扩展 D01 的实际运行记录，或登记第二个真实测试 Demo；
6. 补 HTTP/OpenAI、multimodal、disaggregation 和 Rust 变体的端到端追踪；
7. 完成实现深度、Demo 深度和跨模块审计后，再把状态从“部分/进行中”改成“已完成”。

## 完成判据

不能只看文件数量。每个模块必须有入口落地、正常路径、关键分支、异常、清理、数据生命周期、执行上下文、行级证据、Demo 映射和修改影响；至少一个 Demo 必须有从入口到输出和 cleanup 的完整轨迹，并明确哪些步骤实际运行过。
