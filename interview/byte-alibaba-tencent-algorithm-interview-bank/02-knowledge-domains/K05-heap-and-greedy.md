# K05 堆、优先队列、贪心与区间

## 核心问题

局部最优、交换论证、区间调度、TopK、双堆。学习目标是能识别题目中的结构、选择合适算法、证明复杂度并写出边界正确的 C++17 实现。

本域有两条主线：**堆/优先队列**（用 O(log n) 维护动态集合的最值，TopK 与双堆求中位数是典型应用）和**贪心**（每步取局部最优，正确性靠交换论证或"按端点排序后一次扫描"的结构保证）。区间类问题通常是贪心与排序、扫描线的组合。

## 训练顺序

1. 先完成对应入门题，建立术语和模板。
2. 再完成中等级变体，训练多约束组合。
3. 高级和专家题用于训练证明、在线约束、工程边界和追问。

## 核心模式

### 模式 1：堆求 TopK（大小为 k 的小顶堆）

求"最大的 k 个"用**小顶堆**：堆内始终保留当前最大的 k 个元素，堆顶是这 k 个里最小的；新元素大于堆顶就替换堆顶。这样堆中任意时刻即答案候选集，空间 O(k)。

```cpp
#include <bits/stdc++.h>
using namespace std;

// 返回最大的 k 个元素
vector<int> topK(vector<int>& a, int k) {
    if (k <= 0) return {};
    priority_queue<int, vector<int>, greater<int>> minHeap;  // 小顶堆
    for (int x : a) {
        if ((int)minHeap.size() < k) {
            minHeap.push(x);
        } else if (x > minHeap.top()) {
            minHeap.pop();
            minHeap.push(x);
        }
    }
    vector<int> res;
    while (!minHeap.empty()) { res.push_back(minHeap.top()); minHeap.pop(); }
    return res;                            // 升序的 TopK
}
```

**不变量**：处理完前 `i` 个元素后，堆恰好是这 `i` 个元素中最大的 `min(k, i)` 个。时间 O(n log k)，空间 O(k)。求"最小的 k 个"对称地改用大顶堆。

### 模式 2：优先队列（自定义比较器与双堆）

`priority_queue` 默认大顶堆，小顶堆用 `greater<T>`。自定义结构需提供比较器；若用 lambda，需在构造函数传入。

```cpp
struct Task { int priority, id; };
struct Cmp { bool operator()(const Task& a, const Task& b) const {
    return a.priority > b.priority;       // 返回 true 表示 a 优先级更低
}};
priority_queue<Task, vector<Task>, Cmp> pq;   // 小顶（按 priority）

// 双堆求中位数：小顶堆存较大一半，大顶堆存较小一半
class MedianFinder {
    priority_queue<int> low;                                  // 大顶堆
    priority_queue<int, vector<int>, greater<int>> high;      // 小顶堆
public:
    void addNum(int x) {
        low.push(x);
        high.push(low.top()); low.pop();   // 经 low 交换，保证 low 全部 <= high
        if (high.size() > low.size()) { low.push(high.top()); high.pop(); }
    }
    double findMedian() const {
        if (low.size() > high.size()) return low.top();
        return (low.top() + high.top()) / 2.0;
    }
};
```

**不变量**：`low` 中所有元素 `<= high` 中所有元素，且 `low.size()` 为 `high.size()` 或 `high.size()+1`。每次插入 O(log n)。

### 模式 3：区间贪心（按端点排序）

区间问题的核心是**排序键的选择**。不同目标对应不同排序键：

- **合并重叠区间**：按左端点排序，一次扫描，维护当前合并区间的右端。
- **最多不重叠区间（区间调度）**：按**右端点**升序排序，贪心选结束最早的。
- **用最少箭引爆气球 / 最少会议室**：按左端点排序 + 小顶堆维护当前活跃区间的右端。

```cpp
// 合并区间
vector<vector<int>> mergeIntervals(vector<vector<int>>& iv) {
    if (iv.empty()) return {};
    sort(iv.begin(), iv.end(),
         [](const vector<int>& a, const vector<int>& b) { return a[0] < b[0]; });
    vector<vector<int>> res{iv[0]};
    for (int i = 1; i < (int)iv.size(); ++i) {
        if (iv[i][0] <= res.back()[1])                 // 有重叠（端点相接也算）
            res.back()[1] = max(res.back()[1], iv[i][1]);
        else
            res.push_back(iv[i]);
    }
    return res;
}

// 最多不重叠区间：按右端点升序，选结束最早的
int maxNonOverlap(vector<pair<int,int>> iv) {
    sort(iv.begin(), iv.end(),
         [](const pair<int,int>& a, const pair<int,int>& b) { return a.second < b.second; });
    int cnt = 0, lastEnd = INT_MIN;
    for (auto& [l, r] : iv)
        if (l >= lastEnd) { ++cnt; lastEnd = r; }      // 与已选区间不重叠
    return cnt;
}
```

**合并区间不变量**：`res.back()` 是当前处理到的、右端点已扩展到最大的合并块；新区间左端 `<= res.back()[1]` 则并入。时间 O(n log n)（排序主导），空间 O(n)。

**调度贪心正确性（交换论证）**：设最优解第一个区间结束于 `e*`，贪心选的结束于 `e <= e*`；把最优解第一个换成贪心所选不会减少可行区间数，故贪心不劣于最优。

### 模式 4：扫描线

把所有事件的端点抽成事件点（开始 +1、结束 -1 或 +val），排序后一次扫描，维护"当前活跃数"与峰值。适合"同时最大重叠数""区间覆盖总长度"。

```cpp
// 最大并发（同时进行的区间数）
int maxConcurrent(vector<pair<int,int>>& iv) {
    vector<pair<int,int>> ev;              // (位置, 变化量)
    for (auto& [l, r] : iv) { ev.push_back({l, +1}); ev.push_back({r + 1, -1}); }
    sort(ev.begin(), ev.end());            // 同位置先处理 -1 再 +1 可用 (pos,delta) 次序控制
    int cur = 0, best = 0;
    for (auto& [pos, d] : ev) { cur += d; best = max(best, cur); }
    return best;
}
```

时间 O(n log n)，空间 O(n)。若端点相接视为不重叠，结束事件放在 `r`（区间 `[l, r)`）或用 `(pos, delta)` 排序控制同点先后。

## 常见陷阱

- **TopK 用错堆类型**：求最大 k 个必须用小顶堆（堆顶是最小候选），用大顶堆会挤出正确元素。
- **`priority_queue` 比较器方向**：`greater<T>` 得到小顶堆；自定义比较器返回 `true` 表示"优先级更低"（下沉），方向写反得到相反堆。
- **区间排序键选错**：合并区间和"最多不重叠"排序键不同；把按左端点的逻辑套到调度题会得到错误答案。
- **端点点相接的语义**：`[1,2]` 与 `[2,3]` 是否重叠取决于题目，决定用 `<` 还是 `<=`。
- **扫描线同位置事件顺序**：同一坐标既有 +1 又有 -1 时，先处理哪个决定峰值是否虚高。
- **双堆平衡被破坏**：`addNum` 若先平衡再交换，会违反 `low <= high` 不变量。
- **堆用实时修改**：STL 堆不支持删除任意元素，需"懒惰删除"（标记 + 取堆顶时清理）或改用 `multiset`。
- **贪心未证**：局部最优不等于全局最优，未做交换论证的贪心可能被反例推翻。

## 复杂度速查

| 模式 | 时间 | 空间 | 关键前提 |
|---|---|---|---|
| 堆求 TopK | O(n log k) | O(k) | 小顶堆保留 k 个候选 |
| 建堆（heapify） | O(n) | O(1) 额外 | 一次性建堆 |
| 双堆中位数 | 插入 O(log n)，查询 O(1) | O(n) | 两堆平衡不变量 |
| 合并区间 | O(n log n) | O(n) | 按左端点排序 |
| 最多不重叠区间 | O(n log n) | O(1) 额外 | 按右端点排序 |
| 扫描线 | O(n log n) | O(n) | 端点事件排序 |

## 题目索引

- `../01-question-bank/intermediate/I003-merge-intervals.md`（合并区间：按左端点排序 + 单次扫描）
- `../01-question-bank/expert/E002-topk-frequent.md`（数据流 TopK 高频元素：哈希计数 + 堆/快速选择/计数桶）

跨域相关题：`K06`（快速选择求第 k 大，是 TopK 的线性期望替代）、`A003`（可行性贪心校验配合二分答案）。

**证据与来源：** `[已确认]` 堆求 TopK、双堆中位数、区间调度交换论证、扫描线均为公开经典结论；`[推断]` 训练顺序按难度归纳。样例未在目标 OJ 逐例运行，标记 `[待验证]`。
