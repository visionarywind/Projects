# 设计原则、取舍与稳定契约

- 文档目的：区分项目明确的设计原则、可观察实现与建议。
- 适用范围：全仓库教学/benchmark 代码。
- 对应源码版本：`4513b31`。
- 证据状态：原则由 README/贡献指南确认，部分动机为推断。
- 最后更新：2026-09-10
- 前置阅读：[project-overview.md](project-overview.md)
- 后续阅读：[模块注册表](../01-modules/module-registry.md)

## 结论摘要

项目的第一原则是“使用先于极致优化”：先让 kernel、PyTorch binding 和测试跑通，再逐级引入向量化、tiling、shared memory、Tensor Core、pipeline、swizzle 和异步传输。第二原则是保留对照实现和教学注释，使每次优化能被验证和测量。第三原则是以 CUDA/PyTorch 真实环境为边界，不伪装成跨平台生产运行时。

## 已确认原则

| 原则 | 证据 | 具体影响 |
|---|---|---|
| 学习优先而非生产最优 | `CONTRIBUTE.md:2-3` | 可接受多种渐进实现；生产使用官方库 |
| 逐级优化 | `README.md:265-279`、`interview/notes-v2.cu:5-20` | 保留 naive、tile、vectorize、Tensor Core 版本 |
| 正确性与性能并列 | 各 Python benchmark 先 warmup/sync，再比较输出和时间 | 修改优化时不能只看 TFLOPS |
| 硬件特化 | `interview/build.sh:45-81`、HGEMM makefile | SM 架构、CUDA 版本和库版本影响可用性 |
| 公开 PR/可重复贡献 | `CONTRIBUTE.md:5-31` | 新 kernel 应有实现、绑定/测试和格式化 |

## 关键取舍

1. **脚本即时编译 vs 统一构建系统**：即时编译适合小实验和快速迭代；牺牲统一依赖锁定与增量构建可见性。
2. **显式多版本 vs 自动调优**：保留命名清晰的 `f16x2`、`wmma`、`mma`、`stages` 版本，便于教学；没有统一运行时调优器。
3. **裸指针/手工布局 vs 高级抽象**：基础模块直接展示线程和地址计算；CuTe/CUTLASS 模块展示可组合布局，但模板错误和版本耦合更强。
4. **历史 benchmark vs CI 门禁**：仓库 README 提供实测表格，但没有全仓库自动 GPU CI；数字必须带硬件和版本上下文。

## 修改时的稳定契约

- 不改变公开 PyBind 函数名和参数语义，除非同步更新 Python 调用方和 README。
- 明确 dtype、device、维度、layout、contiguous 和边界尺寸；不能依靠函数名让调用者猜测。
- 任何异步 kernel 的计时必须 warmup 后同步；错误检查必须在能定位 launch 的位置完成。
- 共享内存、寄存器和动态 smem 的变化需要重新审查 occupancy、边界和同步。
- 新 kernel 至少提供参考路径或对拍，并记录支持的 SM/依赖。

## 事实、推断、建议

- **已确认**：HGEMM README 将 padding/swizzle 用于降低 bank conflict，并区分 NN/TN 布局。[kernels/hgemm/README.md:195-204]
- **推断**：教学代码中相似 kernel 的重复实现是为了展示优化变量，而非代码复用失败。
- **建议**：做生产化修改时优先抽取公共 launcher/校验；做教学修改时保持对照版本和注释梯度。

## 相关文档

- [architecture.md](architecture.md)
- [global-error-model.md](global-error-model.md)
- [../99-roadmap/feature-development-recipes.md](../99-roadmap/feature-development-recipes.md)

## 源码证据摘要

- `[CONTRIBUTE.md:2-3]`、`[CONTRIBUTE.md:21-31]`。
- `[kernels/interview/notes-v2.cu:5-20]`。
- `[kernels/hgemm/README.md:195-204]`。

## 未解决问题

- 尚无统一 API 版本和弃用策略。
- 没有明确规定所有模块的误差容忍度。

## 下一步阅读建议

对具体改动，先查模块的接口和测试文档，再使用跨模块影响图。
