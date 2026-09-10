# 技术债务

- 对应源码版本：`main` / `0983c65`。
- 最后更新：2026-09-10

## 已确认债务

1. 没有统一全仓库构建/测试编排和依赖 lockfile。
2. 没有统一 device/stream/contiguous/launch-error 检查规范。
3. 普通 extension、wheel、standalone binary 的接口和资源模型不同。
4. 深层 Tensor Core/CuTe 模板难以只靠 README 维护。
5. benchmark 历史数据缺少统一机器/版本元数据。
6. 没有 GPU CI，因此架构回归只能在目标环境发现。

## 建议顺序

P0：固定版本/环境记录，补最小 correctness 和 NMS regression。
P1：统一输入检查、错误检查和 benchmark metadata。
P2：建立按 SM 的编译 smoke matrix 和 sanitizer job。
P3：再考虑共享 launcher、统一 CMake 或测试 harness；不能先假设这些重构不会破坏教学边界。

证据：`[docs/project-understanding/00-overview/analysis-state.md:27-44]`、`[.pre-commit-config.yaml:1-41]`。
