# 资料与证据

## 资料基线

本批以本仓库 `cann/docs/project-understanding` 的 CANN 项目理解文档为主要证据入口，当前未指定单独的 upstream 提交、硬件型号或运行环境。文档记录的源码版本以各文件头部为准；例如跨模块内存生命周期页记录 GE `8ee1b040a`、ACL/Runtime `50be4c922`、Driver `977336913`。如果后续题目直接引用源码，需再次核对目标 checkout 的提交和行号。

## 主要证据入口

| 主题 | 来源 | 可支持的结论 | 状态边界 |
|---|---|---|---|
| 分层架构 | `cann/docs/project-understanding/00-overview/architecture.md:10-34` | GE、Runtime、Driver 的职责和调用层次 | 跨仓细节部分为推断 |
| Context/Stream 生命周期 | `cann/docs/project-understanding/90-cross-module/memory-and-resource-lifecycle.md:38-55` | Setup、ACTIVE、TearDown、引用和清理顺序 | 真实设备完成语义未验证 |
| 异步资源边界 | `.../memory-and-resource-lifecycle.md:88-100` | 提交、完成、同步释放不是同一时刻 | 设备侧实际时序待验证 |
| 错误分层 | `cann/docs/project-understanding/00-overview/global-error-model.md:13-42` | Driver/HAL/Runtime/ACL/GE 的错误边界和诊断字段 | 完整数值映射属于后续证据增强项，当前不作为已验证结论 |
| 失败窗口 | `.../90-cross-module/error-boundaries.md:25-48` | 普通内存、SOMA、异步路径的清理与残留风险 | 硬件状态待验证 |
| 性能路径 | `.../90-cross-module/performance-critical-paths.md:13-43` | 编译、执行、内存和同步的潜在瓶颈 | 无设备性能数据 |
| 调试顺序 | `.../99-roadmap/debugging-guide.md:13-30` | 固定版本、记录上下文、找第一个失败边界 | 属于流程建议 |

## 证据标签规范

- `[已确认]`：引用资料直接陈述；
- `[推断]`：说明推导链和仍需确认的假设；
- `[待验证]`：硬件、网络、多进程、压测或特定版本实验；
- `[建议]`：设计方案或治理建议；
- `[存在争议]`：不同实现/版本有差异，列出适用条件。

文档中出现的命令只表示验证方案，不表示本工作区已经成功执行。当前没有执行构建、设备操作、模型运行、网络或 RDMA 实验。
