# M04 索引、布局与位置算子

- 文档目的：解释 embedding、histogram、transpose、RoPE 的共同数据访问问题。
- 适用范围：`embedding/`, `histogram/`, `mat-transpose/`, `rope/`。
- 对应源码版本：`4513b31`。
- 证据状态：目录职责已确认；各算子边界需逐个实验。
- 最后更新：2026-09-10
- 前置阅读：[全局数据流](../../00-overview/global-data-flow.md)
- 后续阅读：[M05](../M05-gemv-sgemm/README.md)

## 结论摘要

M04 的共同主题是把逻辑索引映射到高效内存访问：embedding 处理离散行索引，histogram 需要竞争控制，transpose 关注 coalescing/bank conflict，RoPE 对最后维度成对旋转。它们通常是独立 Python extension，不能假设共享统一 launcher。

## 证据与边界

README 主表把这些算子列为 easy/medium kernel；源文件清单确认每个目录都有 `.cu`、Python 脚本和 README（`mat-transpose` 另有 CuTe 实现）。修改布局或索引类型时必须同步 Python 输入生成和参考实现。

## 测试建议

覆盖空/短输入、非整 tile、重复索引、越界索引、转置非方阵、RoPE 奇偶维和不同 dtype；用 PyTorch 索引/transpose/参考 RoPE 对拍，并用 racecheck 检查 histogram/写回。

## 相关文档

- [source-map.md](source-map.md)
- [interfaces.md](interfaces.md)
- [testing.md](testing.md)
- [development-guide.md](development-guide.md)

## 源码证据摘要

- `[README.md:338-387]`：embedding、transpose、reduce、softmax、RoPE 列表。
- `[kernels/mat-transpose/README.md]`：模块内文档入口（需按目标版本继续读取）。

## 未解决问题

不同算子是否接受 arbitrary stride 尚未统一确认。
