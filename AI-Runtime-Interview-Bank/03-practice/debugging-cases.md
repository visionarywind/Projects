# 故障排查案例索引

以下案例作为后续题目和模拟面试材料；当前是问题框架，不代表用户本人遇到过。具体个人经历待补充。

## Case 1：显存 OOM，但监控显示还有空闲显存

- 假设根因：外部碎片、deferred block、workspace/通信 buffer、统计口径不一致、真实泄漏。
- 证据：allocated/reserved/free/largest block/deferred/外部占用/分配 trace。
- 止损：降低 admission、等待受控回收、释放可 trim cache、拒绝新任务。
- 永久修复：allocator policy、生命周期测试、资源计数 oracle 和监控。

## Case 2：多 Stream 结果偶发错误

- 假设根因：缺失 event wait、提前复用、错误 stream、跨设备同步错误。
- 对照：全局同步 vs 最小 event；固定 shape/seed；重复运行；sanitizer。
- 永久修复：访问图、owner/token、状态机和 regression。

## Case 3：训练 step 变慢但单 kernel 没变

- 假设根因：同步点、通信暴露、rank skew、数据准备、allocator 抖动、队列背压。
- 指标：step/P99、GPU timeline、通信暴露时间、NIC/PCIe、deferred、CPU。
- 不可接受的结论：只看单 kernel 或只增加并发。

## Case 4：集合通信偶发 hang

- 先保护：超时、隔离 job、保存 rank/step/顺序/连接证据。
- 分类：collective 顺序/shape、stream/event、buffer 生命周期、QP/CQ/NIC/NUMA、rank 失联。
- 恢复：重建 communicator、重试或回滚需结合一致性和幂等，不能盲目重试。

## Case 5：KV Cache 逐步耗尽

- 指标：live pages、free pages、protected/evictable、lock_ref、prefix hit、retract、transfer。
- 假设：lock 泄漏、partial page、duplicate page、reservation 过度、回收延迟。
- 验证：tiny pool、finish/abort/chunk/evict/flush 全路径资源闭合。

## 通用回答顺序

```text
现象/影响 → 保护和止损 → 根因分类 → 最小复现/对照
→ 日志/指标/trace → 排除假设 → 临时缓解
→ 永久修复 → 回归测试/监控/复盘
```
