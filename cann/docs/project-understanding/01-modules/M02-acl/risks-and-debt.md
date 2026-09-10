# M02 ACL 风险与技术债务

- 证据状态：部分为风险评估，不等同于已发现缺陷

## 高风险

- ACL、Runtime、Driver 和 Toolkit 版本不匹配导致 ABI 或错误码不兼容 `[runtime/README.md:16-19]`。
- 异步任务仍引用 buffer、stream 或 callback 时提前销毁，可能造成未定义行为。
- 设备 reset 与其他线程并发使用资源的边界尚未从已读代码完整确认。

## 中风险

- 同一功能在 ACL 仓和 Runtime 仓出现，公开符号、打包目标和链接优先级需要安装包验证 `[acl/CMakeLists.txt:167-190]`、`[runtime/src/acl/aclrt.cpp:17-79]`。
- 错误映射可能丢失底层扩展错误上下文；应保留原始错误和阶段日志。

## 待补技术债务

- 补齐公共 API 的参数、线程安全、所有权和版本废弃表。
- 用 ASAN/TSAN 或 mock 覆盖异常生命周期。
- 补齐无设备 mock 与真实 NPU 的分层测试矩阵。
