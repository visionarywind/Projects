# 模拟面试路线

## Route 1：AI Runtime/设备软件主线（60 分钟）

### 开场（5 分钟）

- 1 分钟自我介绍：13 年系统软件经验，最近聚焦 MindSpore/UMD、Memory/Stream；
- 2 分钟展开最近项目：模块、本人职责、边界、一个难点；
- 2 分钟确认：目标平台、C++ 深度、是否涉及 GPU/NPU 和性能。

### 技术深挖（25 分钟）

1. B001：Framework 到设备调用链；
2. B002：allocated/reserved/碎片；
3. I001：多 Stream 生命周期；
4. A001：设计异步 allocator；
5. 追问：如何证明 buffer 可复用、OOM 在哪里定位。

### 源码/实现（15 分钟）

- 画 block 状态机；
- 写 `allocate/release/reclaim` 伪代码；
- 说明 event、owner、错误和清理；
- 明确哪些是通用设计，哪些是个人实际做过。

### 复盘（15 分钟）

- 记录是否混淆 API 返回和设备完成；
- 是否说清一个真实模块和代码边界；
- 是否给出指标和验证 oracle；
- 是否在不确定处主动说 `[待验证]`。

## Route 2：AI 通信/高性能网络（60 分钟）

1. 5 分钟介绍 Host 组网/网络项目；
2. 10 分钟解释 AllReduce、Ring/Tree 和拓扑；
3. 15 分钟做 A002：分块、双 buffer、通信 stream、event；
4. 15 分钟排查 collective hang：rank 顺序、shape/dtype、连接、NUMA、buffer；
5. 10 分钟估算 effective bandwidth 和 step time；
6. 5 分钟说明 TCP 经验与 RDMA/NCCL 经验的边界。

## Route 3：LLM 推理/Serving 转型（60 分钟）

1. 解释 prefill、decode、TTFT、TPOT；
2. 从模型参数计算每 token KV bytes；
3. 设计 page allocator、request row、prefix cache、eviction；
4. 讨论 page size、内部碎片、retraction 和 P99；
5. 结合检索引擎说明 cache/index/低延迟经验如何迁移；
6. 诚实说明尚未生产实践的部分和验证计划。

## 三个必须准备的个人案例

### Case A：MindSpore/框架案例

```text
背景：
目标：
我负责的模块/代码边界：
调用链：
一个关键设计决策：
遇到的问题：
定位证据：
解决方案和替代方案：
验证指标：
结果（不能披露则给范围/相对变化，并说明口径）：
复盘：
```

### Case B：UMD/设备软件案例

```text
设备/运行时边界：
命令或数据如何流转：
资源和错误如何管理：
我亲自修改/定位的代码：
并发/时序问题：
测试和故障注入：
```

### Case C：网络/检索/Java 迁移案例

```text
原系统目标和规模：
延迟/吞吐/SLO：
数据结构或网络路径：
瓶颈：
优化取舍：
故障恢复：
如何迁移到 AI Infra：
```

## 面试表达红线

- 不说“我精通 NCCL/RDMA”，除非有真实实现/排障证据；
- 不说“我实现了 PyTorch allocator”，除非确实改过对应代码；
- 不把 LeetCUDA 练习、论文数字或通用设计写成生产结果；
- 不用“加缓存/加机器/加强监控”替代机制、证据和验证；
- 不确定时说：“这一点我需要按具体版本确认；我的判断依据是……验证方式是……”。
