# 2349. 设计数字容器系统

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/design-a-number-container-system/
- 题目 slug：`design-a-number-container-system`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.6 懒删除堆
- 难度分：1540
- 外部题解来源：https://leetcode.cn/problems/design-a-number-container-system/solutions/1693373/by-endlesscheng-wlwa/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：有序集合 / 懒删除堆（Python/Java/C++/Go）](https://leetcode.cn/problems/design-a-number-container-system/solutions/1693373/by-endlesscheng-wlwa/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-wlwa`
- topic id：`1693373`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：哈希表 + 有序集合

为了实现 $\texttt{find}$，我们需要对每个 $\textit{number}$ 创建一个有序集合，维护这个 $\textit{number}$ 对应的所有下标。用有序集合可以快速地获取最小下标。

对于 $\texttt{change}$，如果 $\textit{index}$ 处有数字，我们需要先删除旧的数字，所以还需要知道每个 $\textit{index}$ 对应的 $\textit{number}$ 是多少，这可以用一个哈希表记录。

具体来说，创建一个哈希表 $\textit{indexToNumber}$，以及一个哈希表套有序集合 $\textit{numberToIndices}$。

对于 $\texttt{change}$：

- 如果 $\textit{index}$ 处有数字 $x$，那么从 $\textit{numberToIndices}[x]$ 中删除 $\textit{index}$（删除旧的数据）。
- 然后，更新（或者插入）$\textit{indexToNumber}[\textit{index}] = \textit{number}$，往 $\textit{numberToIndices}[\textit{number}]$ 中添加 $\textit{index}$。

对于 $\texttt{find}$，获取 $\textit{numberToIndices}[\textit{number}]$ 中的最小元素即可。

```py [sol-Python3]
class NumberContainers:
    def __init__(self):
        self.index_to_number = {}
        # from sortedcontainers import SortedSet
        self.number_to_indices = defaultdict(SortedSet)

    def change(self, index: int, number: int) -> None:
        # 移除旧数据
        old_number = self.index_to_number.get(index, None)
        if old_number is not None:
            self.number_to_indices[old_number].discard(index)

        # 添加新数据
        self.index_to_number[index] = number
        self.number_to_indices[number].add(index)

    def find(self, number: int) -> int:
        indices = self.number_to_indices[number]
        return indices[0] if indices else -1
```

```java [sol-Java]
class NumberContainers {
    private final Map<Integer, Integer> indexToNumber = new HashMap<>();
    private final Map<Integer, TreeSet<Integer>> numberToIndices = new HashMap<>();

    public void change(int index, int number) {
        // 移除旧数据
        Integer oldNumber = indexToNumber.get(index);
        if (oldNumber != null) {
            numberToIndices.get(oldNumber).remove(index);
        }

        // 添加新数据
        indexToNumber.put(index, number);
        numberToIndices.computeIfAbsent(number, _ -> new TreeSet<>()).add(index);
    }

    public int find(int number) {
        TreeSet<Integer> indices = numberToIndices.get(number);
        return indices == null || indices.isEmpty() ? -1 : indices.first();
    }
}
```

```cpp [sol-C++]
class NumberContainers {
    unordered_map<int, int> index_to_number;
    unordered_map<int, set<int>> number_to_indices;

public:
    void change(int index, int number) {
        // 移除旧数据
        auto it = index_to_number.find(index);
        if (it != index_to_number.end()) {
            number_to_indices[it->second].erase(index);
        }

        // 添加新数据
        index_to_number[index] = number;
        number_to_indices[number].insert(index);
    }

    int find(int number) {
        auto it = number_to_indices.find(number);
        return it == number_to_indices.end() || it->second.empty() ? -1 : *it->second.begin();
    }
};
```

```go [sol-Go]
// import "github.com/emirpasic/gods/v2/trees/redblacktree"
type NumberContainers struct {
	indexToNumber   map[int]int
	numberToIndices map[int]*redblacktree.Tree[int, struct{}]
}

func Constructor() NumberContainers {
	return NumberContainers{map[int]int{}, map[int]*redblacktree.Tree[int, struct{}]{}}
}

func (n NumberContainers) Change(index, number int) {
	// 移除旧数据
	if oldNumber, ok := n.indexToNumber[index]; ok {
		n.numberToIndices[oldNumber].Remove(index)
	}

	// 添加新数据
	n.indexToNumber[index] = number
	if n.numberToIndices[number] == nil {
		n.numberToIndices[number] = redblacktree.New[int, struct{}]()
	}
	n.numberToIndices[number].Put(index, struct{}{})
}

func (n NumberContainers) Find(number int) int {
	indices, ok := n.numberToIndices[number]
	if !ok || indices.Empty() {
		return -1
	}
	return indices.Left().Key
}
```

#### 复杂度分析

- 时间复杂度：
   - 初始化 $\mathcal{O}(1)$。
   - $\texttt{change}$：$\mathcal{O}(\log q)$，其中 $q$ 是 $\texttt{change}$ 的调用次数。
   - $\texttt{find}$：$\mathcal{O}(\log q)$ 或者 $\mathcal{O}(1)$，取决于有序集合是否额外维护最小值。
- 空间复杂度：$\mathcal{O}(q)$。

## 方法二：哈希表 + 懒删除堆

$\textit{numberToIndices}$ 改成哈希表套最小堆。

对于 $\texttt{change}$，不删除旧数据。

对于 $\texttt{find}$，查看堆顶是否等于 $\textit{number}$，若不相同，则意味着堆顶是之前没有删除的旧数据，弹出堆顶；否则堆顶就是答案。

```py [sol-Python3]
class NumberContainers:
    def __init__(self):
        self.index_to_number = {}
        self.number_to_indices = defaultdict(list)

    def change(self, index: int, number: int) -> None:
        # 添加新数据
        self.index_to_number[index] = number
        heappush(self.number_to_indices[number], index)

    def find(self, number: int) -> int:
        indices = self.number_to_indices[number]
        while indices and self.index_to_number[indices[0]] != number:
            heappop(indices)  # 堆顶货不对板，说明是旧数据，删除
        return indices[0] if indices else -1
```

```java [sol-Java]
class NumberContainers {
    private final Map<Integer, Integer> indexToNumber = new HashMap<>();
    private final Map<Integer, PriorityQueue<Integer>> numberToIndices = new HashMap<>();

    public void change(int index, int number) {
        // 添加新数据
        indexToNumber.put(index, number);
        numberToIndices.computeIfAbsent(number, _ -> new PriorityQueue<>()).offer(index);
    }

    public int find(int number) {
        PriorityQueue<Integer> indices = numberToIndices.get(number);
        if (indices == null) {
            return -1;
        }
        while (!indices.isEmpty() && indexToNumber.get(indices.peek()) != number) {
            indices.poll(); // 堆顶货不对板，说明是旧数据，删除
        }
        return indices.isEmpty() ? -1 : indices.peek();
    }
}
```

```cpp [sol-C++]
class NumberContainers {
    unordered_map<int, int> index_to_number;
    unordered_map<int, priority_queue<int, vector<int>, greater<>>> number_to_indices;

public:
    void change(int index, int number) {
        // 添加新数据
        index_to_number[index] = number;
        number_to_indices[number].push(index);
    }

    int find(int number) {
        auto& indices = number_to_indices[number];
        while (!indices.empty() && index_to_number[indices.top()] != number) {
            indices.pop(); // 堆顶货不对板，说明是旧数据，删除
        }
        return indices.empty() ? -1 : indices.top();
    }
};
```

```go [sol-Go]
type NumberContainers struct {
	indexToNumber   map[int]int
	numberToIndices map[int]*hp
}

func Constructor() NumberContainers {
	return NumberContainers{map[int]int{}, map[int]*hp{}}
}

func (n NumberContainers) Change(index, number int) {
	// 添加新数据
	n.indexToNumber[index] = number
	if _, ok := n.numberToIndices[number]; !ok {
		n.numberToIndices[number] = &hp{}
	}
	heap.Push(n.numberToIndices[number], index)
}

func (n NumberContainers) Find(number int) int {
	indices, ok := n.numberToIndices[number]
	if !ok {
		return -1
	}
	for indices.Len() > 0 && n.indexToNumber[indices.IntSlice[0]] != number {
		heap.Pop(indices) // 堆顶货不对板，说明是旧数据，删除
	}
	if indices.Len() == 0 {
		return -1
	}
	return indices.IntSlice[0]
}

type hp struct{ sort.IntSlice }
func (h *hp) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

#### 复杂度分析

- 时间复杂度：
  - 初始化 $\mathcal{O}(1)$。
  - $\texttt{change}$：$\mathcal{O}(\log q)$，其中 $q$ 是 $\texttt{change}$ 的调用次数。
  - $\texttt{find}$：均摊 $\mathcal{O}(\log q)$。
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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
