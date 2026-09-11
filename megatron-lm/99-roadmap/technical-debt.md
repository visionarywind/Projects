# 技术债务

1. M01 还需逐 backend 分析 local/TE/inference attention 与 MLP。
2. M02 还需逐分支覆盖交错 pipeline 和 overlap P2P。
3. M03 还需补全 fault tolerance、async checkpoint、full CUDA graph。
4. M04 还需对真实 indexed dataset 和 SFT/packing 分支做深入分析。
5. M05 还需 round-trip 实测和异步保存清理流程。
6. M06 还需选择代表 Demo 并运行 offline/server smoke。
7. 跨模块文档需要随源码行号精化。

这些债务不阻塞当前知识库导航，但会影响把文档标记为“已完成”。