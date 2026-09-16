# 2080. 区间内查询数字的频率

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/range-frequency-queries/
- 题目 slug：`range-frequency-queries`
- 来源专题：二分算法
- 来源分类路径：一、二分查找 / §1.2 进阶
- 难度分：1702
- 外部题解来源：https://leetcode.cn/problems/range-frequency-queries/solutions/1113439/tong-ji-wei-zhi-er-fen-wei-zhi-by-endles-8l9u/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法：统计位置+二分查找（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/range-frequency-queries/solutions/1113439/tong-ji-wei-zhi-er-fen-wei-zhi-by-endles-8l9u/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-ji-wei-zhi-er-fen-wei-zhi-by-endles-8l9u`
- topic id：`1113439`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

对于 $\textit{arr}$ 中的每个数，计算其在 $\textit{arr}$ 中的出现位置（下标）。例如 $\textit{arr}=[1,2,1,1,2,2]$，其中数字 $2$ 的下标为 $[1,4,5]$。

知道了下标，那么对于 $\texttt{query}$ 来说，问题就变成了：

- 下标列表中，满足 $\textit{left}\le i \le \textit{right}$ 的下标 $i$ 的个数。

例如 $\texttt{query}(3,5,2)$，由于数字 $2$ 的下标列表 $[1,4,5]$ 中的下标 $4$ 和 $5$ 都在区间 $[3,5]$ 中，所以返回 $2$。

把下标列表记作数组 $a$，由于 $a$ 是**有序**数组，我们可以用**二分查找**快速求出：

- $a$ 中的第一个 $\ge \textit{left}$ 的数的下标，设其为 $p$。如果不存在，则 $p$ 等于 $a$ 的长度。
- $a$ 中的第一个 $> \textit{right}$ 的数的下标，设其为 $q$。如果不存在，则 $q$ 等于 $a$ 的长度。

$a$ 中的下标在 $[p,q-1]$ 内的数都是满足要求的，这有 $q-p$ 个。特别地，如果 $a$ 中没有满足要求的下标，那么 $q-p=0$，这仍然是正确的。

关于二分查找的原理，请看视频讲解：[二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)

```py [sol-Python3]
class RangeFreqQuery:
    def __init__(self, arr: List[int]):
        pos = defaultdict(list)
        for i, x in enumerate(arr):
            pos[x].append(i)
        self.pos = pos

    def query(self, left: int, right: int, value: int) -> int:
        a = self.pos[value]
        return bisect_right(a, right) - bisect_left(a, left)
```

```java [sol-Java]
class RangeFreqQuery {
    private final Map<Integer, List<Integer>> pos = new HashMap<>();

    public RangeFreqQuery(int[] arr) {
        for (int i = 0; i < arr.length; i++) {
            pos.computeIfAbsent(arr[i], k -> new ArrayList<>()).add(i);
        }
    }

    public int query(int left, int right, int value) {
        List<Integer> a = pos.get(value);
        if (a == null) {
            return 0;
        }
        // > right 等价于 >= right+1
        return lowerBound(a, right + 1) - lowerBound(a, left);
    }

    // 开区间写法
    // 请看 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(List<Integer> a, int target) {
        // 开区间 (left, right)
        int left = -1;
        int right = a.size();
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // a[left] < target
            // a[right] >= target
            int mid = (left + right) >>> 1;
            if (a.get(mid) < target) {
                left = mid; // 范围缩小到 (mid, right)
            } else {
                right = mid; // 范围缩小到 (left, mid)
            }
        }
        return right; // right 是最小的满足 a[right] >= target 的下标
    }
}
```

```cpp [sol-C++]
class RangeFreqQuery {
    unordered_map<int, vector<int>> pos;

public:
    RangeFreqQuery(vector<int>& arr) {
        for (int i = 0; i < arr.size(); i++) {
            pos[arr[i]].push_back(i);
        }
    }

    int query(int left, int right, int value) {
        // 不推荐写 a = pos[value]，如果 value 不在 pos 中会插入 value
        auto it = pos.find(value);
        if (it == pos.end()) {
            return 0;
        }
        auto& a = it->second;
        return ranges::upper_bound(a, right) - ranges::lower_bound(a, left);
    }
};
```

```go [sol-Go]
type RangeFreqQuery map[int][]int

func Constructor(arr []int) RangeFreqQuery {
    pos := map[int][]int{}
    for i, x := range arr {
        pos[x] = append(pos[x], i)
    }
    return pos
}

func (pos RangeFreqQuery) Query(left, right, value int) int {
    a := pos[value]
    // > right 等价于 >= right+1
    return sort.SearchInts(a, right+1) - sort.SearchInts(a, left)
}
```

```go [sol-Go 写法二]
type RangeFreqQuery map[int][]int

func Constructor(arr []int) RangeFreqQuery {
    pos := map[int][]int{}
    for i, x := range arr {
        pos[x] = append(pos[x], i)
    }
    return pos
}

func (pos RangeFreqQuery) Query(left, right, value int) int {
    a := pos[value]
    p := sort.SearchInts(a, left)
    return sort.SearchInts(a[p:], right+1) // 二分长度更短
}
```

```js [sol-JavaScript]
var RangeFreqQuery = function(arr) {
    this.pos = {};
    for (let i = 0; i < arr.length; i++) {
        if (this.pos[arr[i]] === undefined) {
            this.pos[arr[i]] = [];
        }
        this.pos[arr[i]].push(i);
    }
};

RangeFreqQuery.prototype.query = function(left, right, value) {
    const a = this.pos[value];
    if (a === undefined) {
        return 0;
    }
    // > right 等价于 >= right+1
    return lowerBound(a, right + 1) - lowerBound(a, left);
};

// 见 https://www.bilibili.com/video/BV1AP41137w7/
var lowerBound = function(a, target) {
    let left = -1, right = a.length; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        const mid = Math.floor((left + right) / 2);
        if (a[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    return right;
}
```

```rust [sol-Rust]
use std::collections::HashMap;

struct RangeFreqQuery {
    pos: HashMap<i32, Vec<usize>>,
}

impl RangeFreqQuery {
    fn new(arr: Vec<i32>) -> Self {
        let mut pos: HashMap<i32, Vec<usize>> = HashMap::new();
        for (i, &x) in arr.iter().enumerate() {
            pos.entry(x).or_default().push(i);
        }
        Self { pos }
    }

    fn query(&self, left: i32, right: i32, value: i32) -> i32 {
        if let Some(a) = self.pos.get(&value) {
            let p = a.partition_point(|&i| i < left as usize);
            let q = a.partition_point(|&i| i <= right as usize);
            (q - p) as _
        } else {
            0
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化 $\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。每次 $\texttt{query}$ 需要 $\mathcal{O}(\log n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `一、二分查找 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、二分查找 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
