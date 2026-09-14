# M04 开发指南

1. 新 memory API 先定义 `CUmemdesc` 的 owner/API source/location/mapping 组合。
2. 同步修改 memobj registry、UVA lookup、tools notify、context destroy 和 IPC/P2P 路径。
3. 任何异步 launch 引用必须加入 M06 tracking；free 不能只依赖 host 引用计数。
4. 分析 UVM fault/migration 时区分当前树可见的 host-side DAG 与外部 UVM/RM 行为。
5. 使用 memcpy/memmgr/memobj 测试做静态覆盖映射；本工作区未执行。
