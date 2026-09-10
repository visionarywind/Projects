# 下一步计划

- 对应源码版本：`main` / `0983c65`。
- 最后更新：2026-09-10

## 源码深挖

1. 为 M06/M07 补深层 `.cu/.cuh` 的实际符号、layout、shared/register 生命周期和架构条件。
2. 为 M02-M05 逐目录核对空输入、尾部、stride、dtype、stream 和 launch error。
3. 重新核对 `.gitmodules` 和 submodule commit。

## 自动验证

在具备 GPU 后按顺序执行：

1. 记录 Python/PyTorch/CUDA/driver/GPU。
2. elementwise 最小 correctness。
3. NMS 固定回归 + 随机 sweep。
4. interview 单架构 build/smoke。
5. sanitizer，再做 benchmark/profile。

所有命令当前均未验证。

## 知识库维护

每次源码变更同步更新模块 README/source-map/testing、跨模块影响图、证据索引和 analysis-state；用脚本检查相对链接和引用行号漂移。

## 完成标准

文档覆盖、源码证据、链接、图节点真实性、未验证标记、测试结果和未解决问题都通过最终验收。
