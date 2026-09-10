# M04 开发指南

先画逻辑索引到物理地址的映射，再决定 tile 和向量化。任何索引类型、layout 或边界改变都要更新 Python 输入生成、reference、README 和 regression case。涉及共享写入时同时检查 atomic/同步和 sanitizer。

根贡献流程见 `[CONTRIBUTE.md:21-31]`。
