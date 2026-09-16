# 3815. 设计拍卖系统

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/design-auction-system/
- 题目 slug：`design-auction-system`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.6 懒删除堆
- 难度分：1854
- 外部题解来源：https://leetcode.cn/problems/design-auction-system/solutions/3883292/lan-shan-chu-dui-pythonjavacgo-by-endles-hwsw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[懒删除堆（Python/Java/C++/Go）](https://leetcode.cn/problems/design-auction-system/solutions/3883292/lan-shan-chu-dui-pythonjavacgo-by-endles-hwsw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`lan-shan-chu-dui-pythonjavacgo-by-endles-hwsw`
- topic id：`3883292`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

根据题意：

- 我们需要知道每个商品对应的所有 $(出价,用户编号)$ 中，最大出价对应的最大用户编号。这可以用有序集合维护，也可以用懒删除堆维护，后者常数小。
- 还需要知道每个 $(用户编号,商品编号)$ 对应的实际出价。这可以用一个哈希表维护。

对于 $\texttt{getHighestBidder}$，不断查看堆顶，如果堆顶出价与实际出价不符，那么弹出堆顶。 

[本题视频讲解](https://www.bilibili.com/video/BV1PskxBnEP7/?t=24m53s)，欢迎点赞关注~

```py [sol-Python3]
class AuctionSystem:
    def __init__(self):
        self.amount = {}  # (userId, itemId) -> bidAmount
        self.item_h = defaultdict(list)  # itemId -> [(bidAmount, userId)]

    def addBid(self, userId: int, itemId: int, bidAmount: int) -> None:
        self.amount[(userId, itemId)] = bidAmount
        heappush_max(self.item_h[itemId], (bidAmount, userId))

    def updateBid(self, userId: int, itemId: int, newAmount: int) -> None:
        self.addBid(userId, itemId, newAmount)
        # 堆中重复的元素在 getHighestBidder 中删除（懒更新）

    def removeBid(self, userId: int, itemId: int) -> None:
        # 题目保证 (userId, itemId) 在 self.amount 中，如果不保证的话，用下面这行代码
        # self.amount.pop((userId, itemId), None)
        del self.amount[(userId, itemId)]
        # 堆中元素在 getHighestBidder 中删除（懒删除）

    def getHighestBidder(self, itemId: int) -> int:
        h = self.item_h.get(itemId, None)
        while h:
            bidAmount, userId = h[0]
            if bidAmount == self.amount.get((userId, itemId), None):
                return userId
            # 货不对板，堆顶出价与实际出价不符
            heappop_max(h)
        return -1
```

```java [sol-Java]
class AuctionSystem {
    private final Map<Integer, Integer> amount = new HashMap<>(); // (userId, itemId) -> bidAmount
    private final Map<Integer, PriorityQueue<int[]>> itemPQ = new HashMap<>(); // itemId -> [(bidAmount, userId)]

    public void addBid(int userId, int itemId, int bidAmount) {
        // 把 (userId, itemId) 压缩到一个 32 位 int 中
        amount.put(userId << 16 | itemId, bidAmount);
        itemPQ.computeIfAbsent(itemId, k -> new PriorityQueue<>((a, b) -> a[0] != b[0] ? b[0] - a[0] : b[1] - a[1]))
                .offer(new int[]{bidAmount, userId});
    }

    public void updateBid(int userId, int itemId, int newAmount) {
        addBid(userId, itemId, newAmount);
        // 堆中重复的元素在 getHighestBidder 中删除（懒更新）
    }

    public void removeBid(int userId, int itemId) {
        amount.remove(userId << 16 | itemId);
        // 堆中元素在 getHighestBidder 中删除（懒删除）
    }

    public int getHighestBidder(int itemId) {
        PriorityQueue<int[]> pq = itemPQ.get(itemId);
        if (pq == null) {
            return -1;
        }

        while (!pq.isEmpty()) {
            int[] top = pq.peek();
            int bidAmount = top[0];
            int userId = top[1];
            if (bidAmount == amount.getOrDefault(userId << 16 | itemId, -1)) {
                return userId;
            }
            // 货不对板，堆顶出价与实际出价不符
            pq.poll();
        }
        return -1;
    }
}
```

```cpp [sol-C++]
class AuctionSystem {
    unordered_map<uint32_t, int> amount; // (userId, itemId) -> bidAmount
    unordered_map<int, priority_queue<pair<int, int>>> item_pq; // itemId -> [(bidAmount, userId)]

public:
    void addBid(int userId, int itemId, int bidAmount) {
        // 把 (userId, itemId) 压缩到一个 32 位 int 中
        amount[(uint32_t) userId << 16 | itemId] = bidAmount;
        item_pq[itemId].emplace(bidAmount, userId);
    }

    void updateBid(int userId, int itemId, int newAmount) {
        addBid(userId, itemId, newAmount);
        // 堆中重复的元素在 getHighestBidder 中删除（懒更新）
    }

    void removeBid(int userId, int itemId) {
        amount.erase((uint32_t) userId << 16 | itemId);
        // 堆中元素在 getHighestBidder 中删除（懒删除）
    }

    int getHighestBidder(int itemId) {
        auto it = item_pq.find(itemId);
        if (it == item_pq.end()) {
            return -1;
        }

        auto& pq = it->second;
        while (!pq.empty()) {
            auto [bidAmount, userId] = pq.top();
            auto it2 = amount.find((uint32_t) userId << 16 | itemId);
            if (it2 != amount.end() && it2->second == bidAmount) {
                return userId;
            }
            // 货不对板，堆顶出价与实际出价不符
            pq.pop();
        }
        return -1;
    }
};
```

```go [sol-Go]
type pair struct{ userId, itemId int }
type AuctionSystem struct {
	amount map[pair]int // (userId, itemId) -> bidAmount
	itemH  map[int]*hp  // itemId -> [(bidAmount, userId)]
}

func Constructor() AuctionSystem {
	return AuctionSystem{map[pair]int{}, map[int]*hp{}}
}

func (a AuctionSystem) AddBid(userId, itemId, bidAmount int) {
	a.amount[pair{userId, itemId}] = bidAmount

	if a.itemH[itemId] == nil {
		a.itemH[itemId] = &hp{}
	}
	heap.Push(a.itemH[itemId], hpPair{bidAmount, userId})
}

func (a AuctionSystem) UpdateBid(userId, itemId, newAmount int) {
	a.AddBid(userId, itemId, newAmount)
	// 堆中重复的元素在 GetHighestBidder 中删除（懒更新）
}

func (a AuctionSystem) RemoveBid(userId, itemId int) {
	delete(a.amount, pair{userId, itemId})
	// 堆中元素在 GetHighestBidder 中删除（懒删除）
}

func (a AuctionSystem) GetHighestBidder(itemId int) (ans int) {
	h := a.itemH[itemId]
	if h == nil {
		return -1
	}
	for h.Len() > 0 {
		if (*h)[0].bidAmount == a.amount[pair{(*h)[0].userId, itemId}] {
			return (*h)[0].userId
		}
		// 货不对板，堆顶出价与实际出价不符
		heap.Pop(h)
	}
	return -1
}

type hpPair struct{ bidAmount, userId int }
type hp []hpPair
func (h hp) Len() int { return len(h) }
func (h hp) Less(i, j int) bool {
	a, b := h[i], h[j]
	return a.bidAmount > b.bidAmount || a.bidAmount == b.bidAmount && a.userId > b.userId
}
func (h hp) Swap(i, j int) { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)   { *h = append(*h, v.(hpPair)) }
func (h *hp) Pop() any     { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

#### 复杂度分析

- 时间复杂度：
  - 初始化：$\mathcal{O}(1)$。
  - $\texttt{addBid}$ 和 $\texttt{updateBid}$：$\mathcal{O}(\log q)$，其中 $q$ 是 $\texttt{addBid}$ 和 $\texttt{updateBid}$ 的执行次数。
  - $\texttt{removeBid}$：$\mathcal{O}(1)$。
  - $\texttt{getHighestBidder}$：均摊 $\mathcal{O}(\log q)$。
- 空间复杂度：$\mathcal{O}(q)$。

## 专题训练

见下面数据结构题单的「**§5.6 懒删除堆**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `五、堆（优先队列） / §5.6 懒删除堆`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `五、堆（优先队列） / §5.6 懒删除堆`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
