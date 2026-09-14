# 风险登记

| 风险 | 影响 | 状态 |
|---|---|---|
| 源码无 Git 元数据 | 无法确认 commit/branch | 未解决 |
| 外部树缺失 | 无法闭合 RM/compiler/firmware | 未解决 |
| 架构条件编译 | 静态文件不代表 binary 包含 | 未解决 |
| 异步资源生命周期 | 过早释放可能破坏 launch | 源码已有 tracking，运行未验证 |
| 工具改变控制流 | skip/blocking/profiler 影响主路径 | 静态确认，未运行 |
| 测试聚合误读 | waived/no-result 与 pass 不同 | 已记录规则 |
