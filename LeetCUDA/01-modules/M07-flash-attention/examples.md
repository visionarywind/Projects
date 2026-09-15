# M07 示例与修改练习

- 文档目的：解释 01-modules/M07-flash-attention/examples.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-flash-attention/examples.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 示例 A：代表 tiling-QKV（未验证）

```bash
python3 kernels/flash-attn/flash_attn_mma.py --D 64 --B 1 --H 1 --N 128 --check
```

源码约束要求 sequence length 对代表 Br/Bc 整 tile；实际 `N=128` 是否匹配所选 dispatch、依赖和 GPU，需运行确认。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-945]

## 示例 B：边界实验

比较 `N` 为整 tile和非整 tile、`D` 为 64/128/256/其他值，记录 assert、dispatch、误差和 launch error。不能将异常结果解释为算法本身错误，需先判断是否违反源码契约。

## 修改练习

1. 跟踪 V alias Q shared region，增加同步前后注释并用 sanitizer 验证。
2. 记录 F16 与 F32 accumulator 的最大误差。
3. 改变 stage 1/2，比较 shared memory、时间和 occupancy。
4. 为代表 binding 增加显式 rank/device/contiguous 检查，再更新接口文档。

所有命令和练习当前未执行。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
