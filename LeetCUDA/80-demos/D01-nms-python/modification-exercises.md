# D01-S06 修改练习

1. **增加 launch error check**：在两个 kernel launch 后加入一致的 CUDA 检查，记录错误归因是否更清晰；更新 M08/M10 的错误边界。
2. **检查非 contiguous 输入**：传入切片/转置 boxes，确认 binding 是否接受、`.index_select(...).contiguous()` 覆盖了哪一段；不要把结果预先当作支持承诺。
3. **固定 ties**：构造相同 score，打印 `order_t` 和返回原始 index，验证 stable sort 语义。
4. **面积语义实验**：选择小框和 threshold 边界，同时调用 GPU、torchvision 和 `nms.cc` 参考，记录差异来源。
5. **mask 容量实验**：逐步增大 N，记录分配、kernel、CPU copy 和显存；在资源不足时停止，不把 O(N²/32) 视为无限可扩展。
6. **racecheck**：在明确有 CUDA 环境后运行 sanitizer，并把实际版本/输出写入 testing 文档。

所有练习当前未执行；修改只应同步更新源码证据、测试结果和风险登记。
