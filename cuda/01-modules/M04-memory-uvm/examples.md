# M04 代表性例子

- interior pointer 传给 `cuMemFree`：能找到对象但不等于 base pointer，因此返回 invalid value，保护 backing block。
- `cuMemFree(0)`：按 wrapper 语义成功返回，不进入对象释放。
- 固定 device VA 或 external sharing：禁止普通 suballocation，避免地址/所有权冲突。

### Best-fit 示例（静态推演）

假设同一 descriptor-compatible tree 有 64 KiB、256 KiB 两个 free region，请求经 alignment 取整后为 48 KiB。`radixTreeFindGEQ` 选择 64 KiB 区，切出 48 KiB 给 memobj，把剩余 16 KiB 插回 tree；释放该 memobj 时，若 16 KiB 与它相邻，两段会合并回 64 KiB。实际 block size、alignment 和物理地址由目标 HAL/DMAL 决定，示例不代表运行结果（机制证据：[src/cui/suballocator.c:163-220,343-405]）。
