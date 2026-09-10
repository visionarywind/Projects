# M02 风险与债务

- 对应源码版本：`main` / `0983c65`。
- 证据状态：源码缺口已确认；运行时后果部分未验证。
- 最后更新：2026-09-10

## 已确认风险

1. binding 只显式检查 dtype，没有统一检查 shape/device/contiguous/stride；错误输入可能在 kernel 中产生错误结果或非法访问。[kernels/elementwise/elementwise.cu:134-180]
2. `float4`、`half2` 和 128-bit alias 假设连续、适当对齐的地址；源码没有运行时对齐检查。[kernels/elementwise/elementwise.cu:32-49,75-127]
3. vectorized kernel 依赖完整 pack 条件，tail 由 scalar loop 处理；新增 pack 宽度时若只改 launcher 或只改 idx 步长，会造成覆盖/遗漏。[kernels/elementwise/elementwise.cu:34-48,62-72,77-127]
4. launch 后未见统一 `cudaGetLastError`；错误暴露时机是异步且需后续实验确认。[kernels/elementwise/elementwise.cu:154-180]
5. Python benchmark 的 `.contiguous()` 会隐藏调用者的 layout 前提，不能代替 binding 的契约检查。[kernels/elementwise/elementwise.py:69-95]

## 跨变体风险

各激活目录可能拥有不同的 dtype、shape、tail 和导出方式，不能从 add 的 wrapper 自动推广到所有 elementwise/activation 算子。逐目录 source map 尚未完成。

## 缓解建议

- 在 wrapper 显式检查 CUDA device、shape 相等和连续布局，或明确实现 stride-aware kernel。
- 对 packed path 明确记录对齐要求；不能满足时回退 scalar 或 contiguous copy。
- 增加 `torch.add` 对拍、pack tail、非连续输入和 sanitizer 回归。
- 保留 scalar 版本作为 correctness 基线。

以上措施尚未实施；GPU 测试与 sanitizer 当前未执行。

## 相关文档

- [interfaces.md](interfaces.md)
- [testing.md](testing.md)
- [development-guide.md](development-guide.md)
