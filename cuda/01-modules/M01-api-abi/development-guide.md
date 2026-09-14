# M01 开发指南

1. 新 API 先在 `inc/cuda.h` 明确 ABI/版本宏，再加入 API wrapper、内部接口、生成输入和导出表。
2. wrapper 必须初始化输出、检查 flags/句柄、保持错误码语义，并在局部失败时回滚。
3. 涉及异步资源时，把 tracking、context destroy 和 stream synchronization 一起检查。
4. 同时验证 v1/v2、PTDS/PTSZ、Linux/Windows 导出差异。
5. 构建依赖 nvmake 和外部 compiler；本工作区未执行构建或测试。
