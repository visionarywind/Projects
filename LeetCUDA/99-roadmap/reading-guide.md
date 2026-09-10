# 阅读路线

- 对应源码版本：`main` / `0983c65`。
- 最后更新：2026-09-10

## 按目标选择

| 目标 | 路线 |
|---|---|
| 初学 CUDA | M01 → M02 → M03 → M05 |
| 写 PyTorch extension | M08 → M02 → M10 → M06 |
| 学矩阵性能 | M05 → M06 → M11 |
| 学 attention | M03 → M06 → M07 → M09 |
| 面试复习 | M09：common → base → GEMV/GEMM → HGEMM → FlashAttention |
| 调试竞态 | M08 → M10 → error boundaries → sanitizer |

## 每个模块的固定阅读法

1. README：职责和边界。
2. source-map/interfaces：入口和契约。
3. call-chains/line-level：控制流和同步。
4. testing/risks：验证和已知债务。
5. 对照源码，不把推断当确认。

## 证据等级

- **已确认**：源码、构建脚本或可复现命令直接支持。
- **推断**：由目录/命名/调用关系归纳，需补源码或实验。
- **未知**：当前没有足够证据，不能作为 API 承诺。

## 相关入口

[README](../README.md)、[module-registry](../01-modules/module-registry.md)、[analysis-state](../00-overview/analysis-state.md)。
