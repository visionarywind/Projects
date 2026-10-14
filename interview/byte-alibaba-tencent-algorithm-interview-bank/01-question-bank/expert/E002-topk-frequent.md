# E002. 数据流 TopK 高频元素

- 难度：专家级
- 知识域：K05 堆、优先队列、贪心与区间
- 标签：哈希计数、堆、快速选择、计数桶、数据流、近似算法
- 公司标签：通用、字节、阿里、腾讯 `[推断]`
- 面试频率：高频 `[推断]`
- 前置题：无（本题为设计类专题，自包含）
- 关联题：E001（LRU 缓存）、E003（全排列）
- 事实状态：`[已确认]` 计数桶/堆/快速选择的复杂度与近似算法的经典结论；`[推断]` 公司训练标签；`[待验证]` 各实现在目标 OJ 未逐例运行

## 1. 题目与约束

离线版：给定整数数组 `nums` 与整数 `k`，返回出现频率最高的 `k` 个元素（顺序不限，题目保证答案唯一或允许任意顺序）。

在线版：元素逐个到达（数据流），要求维护当前出现次数最多的 `k` 个元素，支持 `add(x)` 与 `top()`；内存受限，不能保存全部历史。

约束：`1 <= nums.size() <= 10^7`；`1 <= k <= 不同元素个数`；频率范围 `1..n`。专家约束：单次 `add` 要求 O(log k)；精确解内存 O(k + d)，`d` 为不同元素数；当 `d` 远超内存时只允许近似解（误差有界）。

## 2. 面试思路

- 必须先计数：扫描一次建立 `key -> freq` 哈希表，O(n)。
- 问题转化为“在 m 个 `(element, freq)` 中取频率前 k 大”。有四个不同侧重的解：
  1. **小顶堆**：维护大小为 k 的最小堆，遍历时挤出最小堆顶。适合 k 小、可增量。
  2. **计数桶**：以频率为下标分桶，从高到低收集，O(m)。适合离线且能承受 O(n) 桶空间。
  3. **快速选择（quickselect）**：对频率数组做 `nth_element`，平均 O(m)，原地、常数小，适合离线一次精确取 top-k。
  4. **近似（Count-Min Sketch / Misra-Gries / Space-Saving）**：超大流、内存受限场景，误差有界。

面试时先讲清“k 与 m（不同元素数）的相对大小”，它决定堆、桶还是选择的取舍。

## 3. 算法与正确性

离线主解（计数桶）：

1. 哈希计数得到 `cnt`；
2. `bucket[f]` 保存出现 f 次的所有元素，f 从 1 到 n；
3. 从 f=n 递减收集，凑够 k 个即停。

正确性：出现次数最大的元素其频率也最大。从高频率桶开始收集，保证先取完所有更高频率的元素，因此收集到的前 k 个正是 top-k。

在线主解（索引小顶堆）：维护一个大小不超过 k 的最小堆（按当前频率排序），配 `freq`、`idx` 两张表。不变量：堆中恰好是当前频率前 k 大的元素（同频时用 key 的全序打破平局）。

- `add(key)`：`freq[key]++`；若 key 已在堆中，频率只增不减，故只需从它的位置 `siftDown`；否则若堆未满则插入并 `siftUp`，若已满且新频率严格大于堆顶，则替换堆顶并 `siftDown`。
- 归纳证明：每次操作后堆仍保有前 k 大集合。频率增大只会让元素排名上升，不会使堆外元素进入前 k，除非新 key 的频率超过堆顶。

## 4. C++17 实现

离线计数桶：

```cpp
#include <bits/stdc++.h>
using namespace std;

vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> cnt;
    cnt.reserve(nums.size() * 2);
    cnt.max_load_factor(0.7f);
    for (int x : nums) ++cnt[x];

    vector<vector<int>> bucket(nums.size() + 1);           // 下标即频率
    for (auto& kv : cnt) bucket[kv.second].push_back(kv.first);

    vector<int> res;
    res.reserve(k);
    for (int f = (int)nums.size(); f >= 1 && (int)res.size() < k; --f) {
        for (int key : bucket[f]) {
            res.push_back(key);
            if ((int)res.size() == k) return res;
        }
    }
    return res;
}
```

小顶堆离线版：

```cpp
vector<int> topKFrequentHeap(vector<int>& nums, int k) {
    unordered_map<int, int> cnt;
    for (int x : nums) ++cnt[x];
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;  // (freq, key)
    for (auto& kv : cnt) {
        pq.emplace(kv.second, kv.first);
        if ((int)pq.size() > k) pq.pop();
    }
    vector<int> res;
    res.reserve(k);
    while (!pq.empty()) { res.push_back(pq.top().second); pq.pop(); }
    return res;
}
```

快速选择版（离线、速度优先）：

```cpp
vector<int> topKFrequentQuickselect(vector<int>& nums, int k) {
    unordered_map<int, int> cnt;
    for (int x : nums) ++cnt[x];
    vector<pair<int,int>> items(cnt.begin(), cnt.end());   // (freq, key)
    nth_element(items.begin(), items.begin() + (k - 1), items.end(),
                [](const pair<int,int>& a, const pair<int,int>& b) {
                    return a.first > b.first;                  // 按频率降序
                });
    vector<int> res;
    res.reserve(k);
    for (int i = 0; i < k; ++i) res.push_back(items[i].second);
    return res;
}
```

在线索引堆（支持频率增长的就地调整）：

```cpp
class TopKTracker {
public:
    explicit TopKTracker(int k) : k_(k) {}

    void add(const string& key) {
        ++freq_[key];
        auto it = idx_.find(key);
        if (it != idx_.end()) { siftDown(it->second); return; }   // 频率增大只会下沉
        if ((int)heap_.size() < k_) {
            idx_[key] = (int)heap_.size();
            heap_.push_back(key);
            siftUp((int)heap_.size() - 1);
        } else if (k_ > 0 && lessKey(heap_[0], key)) {            // 新元素比堆顶更大
            idx_.erase(heap_[0]);
            heap_[0] = key;
            idx_[key] = 0;
            siftDown(0);
        }
    }

    vector<string> top() const {
        vector<string> out = heap_;
        sort(out.begin(), out.end(), [&](const string& a, const string& b) {
            if (freq_.at(a) != freq_.at(b)) return freq_.at(a) > freq_.at(b);
            return a < b;
        });
        return out;
    }

private:
    int k_;
    vector<string> heap_;                    // 小顶堆，堆顶为“最小”候选
    unordered_map<string, int> freq_;
    unordered_map<string, int> idx_;         // key -> 堆内下标

    bool lessKey(const string& a, const string& b) const {
        int fa = freq_.at(a), fb = freq_.at(b);
        if (fa != fb) return fa < fb;
        return a > b;                        // 平局时较大的 key 视为“更小”，先被淘汰
    }
    void swapAt(int i, int j) {
        swap(heap_[i], heap_[j]);
        idx_[heap_[i]] = i;
        idx_[heap_[j]] = j;
    }
    void siftUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (!lessKey(heap_[i], heap_[p])) break;
            swapAt(i, p);
            i = p;
        }
    }
    void siftDown(int i) {
        int n = (int)heap_.size();
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, m = i;
            if (l < n && lessKey(heap_[l], heap_[m])) m = l;
            if (r < n && lessKey(heap_[r], heap_[m])) m = r;
            if (m == i) break;
            swapAt(i, m);
            i = m;
        }
    }
};
```

## 5. 复杂度

| 方案 | 时间 | 空间 | 适用与说明 |
|---|---|---|---|
| 哈希 + 计数桶 | O(n + m) | O(m + n) | 离线；能承受 n 个桶 |
| 哈希 + 小顶堆 | O(n + m log k) | O(m + k) | 离线/可增量；k ≪ m 时最优 |
| 哈希 + 快速选择 | 平均 O(n + m)，最坏 O(n + m²) | O(m) | 离线一次；需随机化或中位数保证最坏 |
| 索引堆（流式） | 总计 O(n log k)，单次 add O(log k) | O(k + d) | 在线精确；d 大时内存受限 |
| Count-Min Sketch + 堆 | O(n)（常数含 1/ε · log(1/δ)） | O((1/ε) log(1/δ) + k) | 超大流；结果为近似 |

**下界论证**：

- 任何算法必须至少读一遍数据，故 Ω(n)。
- 精确 top-k 在单遍、亚线性内存下一般不可行：需要区分所有 d 个元素，最坏情况要保存 d 个计数。这是精确性与内存之间的本质冲突。
- 近似一侧有经典结论：Misra-Gries / Space-Saving 用 O(k) 量级计数器即可保证所有频率大于 n/k 的“重击者”被报告；Count-Min Sketch 用 O((1/ε)·log(1/δ)) 空间给出满足误差界的频率估计（估计取多行最小值以降低高估）`[已确认]`。

## 6. 例子推演

离线：`nums=[1,1,1,2,2,3], k=2`：

- 计数：1→3，2→2，3→1。
- 桶：`bucket[3]={1}`，`bucket[2]={2}`，`bucket[1]={3}`。
- 从 f=3 收集 1，再从 f=2 收集 2，凑满 k=2，返回 `[1,2]`。

在线：`k=2`，依次 `add`：`a, a, a, b, b, c, c, c`：

| add | freq | 堆（小顶，元素:频率） | 说明 |
|---|---|---|---|
| a | a:1 | {a:1} | 未满，插入 |
| a | a:2 | {a:2} | 已在堆中，就地下沉 |
| a | a:3 | {a:3} | 就地下沉 |
| b | b:1 | {b:1, a:3} | 未满插入，b 成新堆顶 |
| b | b:2 | {b:2, a:3} | 就地下沉 |
| c | c:1 | {b:2, a:3} | 已满，c:1 不大于堆顶 b:2，忽略 |
| c | c:2 | {b:2, a:3} | c:2 与堆顶平局，按平局规则不替换 |
| c | c:3 | {c:3, a:3} | c:3 大于堆顶 b:2，替换并下沉 |

最终 `top()`：`["a","c"]`（或按频率排序输出）。可见平局规则会影响边界结果，题面必须约定平局语义。

## 7. 测试与调试

- 边界：`k=1`；`k=m`（全部元素）；所有元素相同；所有元素互不相同。
- 平局密集输入：验证输出符合题目对平局的约定。
- 差分测试：暴力实现对全部 `(element, freq)` 排序取前 k 作 oracle；离线三种实现互相比对。
- 在线测试：随机流并随机穿插 `top()` 查询，与“每步重算离线答案”的 oracle 比对。
- 调试：打印堆数组、`idx`、`freq`，检查 `idx_[heap_[i]] == i` 与堆序不变量；检查刷新 `freq` 后堆顶语义。

## 8. 面试追问

1. **分布式 TopK（数据分散在 N 台机器）如何做？** 各节点本地先算 TopK（或 Count-Min Sketch），再做汇聚合并。若频率可加，合并本地精确计数即可得到精确结果；若用 sketch，合并后仍可保证误差界。要澄清目标是“全局精确”还是“全局近似”，以及网络与合并轮数。
2. **Count-Min Sketch 的原理与误差？** 用 d 个哈希函数映射到宽度为 w 的计数器矩阵，估计某个 key 的频率时取 d 个计数器的最小值以降低高估。误差与 w、置信度参数 δ 相关，可与堆结合做 heavy-hitter 检测。
3. **k 随流变化或需要滑动窗口的 top-k？** 纯递增计数在需要删除/时间衰减时失效。要换成时间衰减计数、滑动窗口计数，或“可增删的堆 + 懒惰删除”（堆内保留过期项，弹出时校验）。

## 9. 易错点与反例

- **反例**：`nth_element` 比较方向写反，`greater` 用成默认 `less`，结果是取到最小的 k 个而非最大的 k 个。
- **反例**：在线堆只 `push` 不清理过期项，堆里堆积同一 key 的旧频率快照，导致按旧频率淘汰错误元素。
- **易错**：桶数组大小写成 `n`，频率等于 n 时下标越界；应为 `n+1`。
- **易错**：`priority_queue<pair<int,int>, ..., greater<>>` 会按 `(freq, key)` 字典序比较，key 参与比较，确定性输出时必须意识到这一点。
- **易错**：误以为流式精确 TopK 能做到 O(k) 内存；精确性要求保存所有 d 个元素的计数。

## 10. 变体与迁移

- 结构迁移：滑动窗口内 TopK（窗口计数 + 平衡结构）、带删除的 TopK（堆 + 懒惰删除）。
- 算法迁移：快速选择迁移到“第 K 大元素”“中位数”“最近的 K 个点”等选择问题。
- 近似迁移：Heavy Hitters / Misra-Gries / Space-Saving 迁移到网络流量、数据库热点键、推荐热门项统计。
- 约束迁移：当 k 与 m 同阶时，“堆”退化为近似 O(n log n)，此时计数桶或排序更优；选择取决于 k/m 比。

**证据与来源：** `[已确认]` 哈希计数加小顶堆、计数桶、快速选择三类的复杂度和取舍属公开经典结论；`[已确认]` C++17 标准库 `priority_queue`（默认大顶堆，`greater` 反转）、`nth_element` 的平均线性与最坏二次复杂度约定；`[推断]` Misra-Gries / Space-Saving / Count-Min Sketch 的误差保证量级来自公开算法资料，具体常数与哈希设计相关；`[推断]` 公司训练标签来自公开面经归纳；各实现未在目标 OJ 逐例运行，标记 `[待验证]`。
