# D01-S04 调试 walkthrough

## 推荐观察顺序

1. 在 `nms.py` 的 `lib = load(...)` 处确认扩展构建和加载；
2. 在 `nms.cu:nms` 的输入检查处确认 dtype/device/shape；
3. 观察 `order_t` 和 `boxes_sorted`，确认 stable descending 与 contiguous；
4. 在两个 kernel launch 后加入临时 `C10_CUDA_KERNEL_LAUNCH_CHECK()` 或使用 blocking/sanitizer（仅作为本地调试修改，不是当前仓库事实）；
5. 检查 `mask` 的 `mask_words` 和 N²/32 分配；
6. 在 resolve 的每个 i 观察 `suppressed` 与 `keep`；
7. 在 CPU mapping 处打印 `order_t`、排序位置和原始 index；
8. 检查返回 tensor 的 dtype/device。

## 调试命令（未验证）

```bash
CUDA_LAUNCH_BLOCKING=1 python3 nms.py
compute-sanitizer --tool memcheck python3 nms.py
compute-sanitizer --tool racecheck python3 nms.py
```

这些工具可能改变同步和性能；当前没有执行结果。

## 三个最小诊断输入

- 两个完全重叠框：检查只保留高分框；
- 两个不重叠框：检查两者都保留；
- 相同 score 的框：检查 stable sort 和原始 index 映射。

不要在没有记录 threshold、坐标语义和 reference 的情况下解释差异。

## 典型定位结论

- 输入检查报错：先看 `nms.cu:126-150`，不是 CUDA race；
- output 顺序错误：先看 `order_t` 映射 `nms.cu:166-192`；
- 随机 race/未定义结果：看 mask 初始化、resolve barrier 和 launch error；
- CPU 对拍边界差异：先比较 `nms.cc` 的 `+1` 面积与 GPU 连续面积。
