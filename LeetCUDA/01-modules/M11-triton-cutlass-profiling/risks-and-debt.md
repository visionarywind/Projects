# M11 风险与债务

- submodule checkout commit 未固定到知识库支持矩阵。
- DSL 示例和 CUDA/PyTorch 版本耦合。
- profiling 输出可能依赖真实 GPU、权限和工具版本。
- 将第三方行为误读为项目实现会扩大错误影响范围。

缓解：只记录明确 include/build 边界，并把运行结论标为未验证。
