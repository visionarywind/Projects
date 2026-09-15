# 风险登记

- 对应源码版本：`main` / `4513b31`。
- 最后更新：2026-09-10

| ID | 风险 | 影响 | 缓解/状态 |
|---|---|---|---|
| R01 | 架构/工具链矩阵未统一 | 编译失败或 no kernel image | 构建前固定 GPU、CUDA、SM；未验证 |
| R02 | 输入检查不一致 | 非法访问或错误结果 | 每个 binding 声明契约；逐模块补测 |
| R03 | 异步错误延迟 | 错误归因错误 | 同步/检查 launch；interview 已有 `check()` |
| R04 | layout/swizzle 不匹配 | Tensor Core 错误/性能退化 | 对照实例化和 reference |
| R05 | NMS mask/resolve 边界 | 错误保留集或显存压力 | ties、N=0、大 N、sanitizer |
| R06 | 第三方 submodule 漂移 | API/header 变化 | 记录 commit，构建前初始化固定版本 |
| R07 | 历史 benchmark 被误读 | 错误性能结论 | 标记硬件/版本/未验证 |
| R08 | 无统一 CI/harness | 回归不易发现 | 建议后续添加分层 smoke/correctness |

证据入口：[global-error-model](../00-overview/global-error-model.md)、[error-boundaries](../90-cross-module/error-boundaries.md)。
