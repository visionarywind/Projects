# 项目风险登记

- 文档目的：汇总跨模块风险、证据和验证优先级。
- 适用范围：LevelDB 全仓库。
- 源码版本：`main` / `7ee830d`。
- 证据状态：触发机制已由源码确认；严重度和频率多数未实验。
- 最后更新：2026-09-10
- 前置阅读：[错误边界](../90-cross-module/error-boundaries.md)
- 后续阅读：[技术债务](technical-debt.md)

| 优先级 | 风险 | 主要模块 | 证据/验证 |
|---|---|---|---|
| P0 | MANIFEST/CURRENT 更新失败造成恢复状态不一致 | M02/M04/M06 | `version_set.cc:777-859`；故障注入/重开 |
| P0 | WAL、表或内部 key 格式解析错误 | M03/M05 | log/format reader；损坏、fuzz、ASan |
| P0 | Version/iterator/cache 引用错误 | M02/M04/M05/M06 | Ref/cleanup 代码；ASan/TSan |
| P0 | compaction/drop 误删或旧版本不可见 | M02/M04 | `DoCompactionWork`；snapshot 多版本 |
| P1 | 后台关闭、Schedule、CondVar 竞态 | M02/M06 | shutdown stress/TSan |
| P1 | pending output/live file 集合不完整 | M02/M04 | 并发 compaction、文件清理测试 |
| P1 | comparator/边界 separator 不一致 | M01/M03/M04/M05 | reverse comparator/boundary |
| P1 | fd/mmap/cache 资源泄漏 | M05/M06 | limit/eviction/长压 |
| P1 | Sync 语义与实际文件系统持久性差异 | M02/M03/M06 | fault injection/平台实验 |
| P2 | 可选压缩/CRC 组合缺少覆盖 | M05/M06/M07 | 多 CMake 配置 |
| P2 | 旧 benchmark 数字误导性能判断 | M07 | 固定硬件和当前 benchmark |
| P2 | 平台实现与公共 Env 契约漂移 | M06/M07 | CI 矩阵 |

## 处理原则

先保护数据正确性和恢复，再处理性能与可维护性；修改前建立最小复现，修改后记录未执行的验证。风险表不把“代码存在检查”误写成“风险已消除”。
