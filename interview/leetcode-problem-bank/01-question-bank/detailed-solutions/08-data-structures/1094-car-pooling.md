# 1094. 拼车

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/car-pooling/
- 题目 slug：`car-pooling`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.1 基础
- 难度分：1441
- 外部题解来源：https://leetcode.cn/problems/car-pooling/solutions/2550264/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【算法小课堂】差分数组（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/car-pooling/solutions/2550264/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra`
- topic id：`2550264`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![图解差分数组](https://pic.leetcode.cn/1747885435-uDDcwT-lc1094-c.png)

## 举例

考虑数组 $a=[1,3,3,5,8]$，对其中的相邻元素两两作差（右边减左边），得到数组 $[2,0,2,3]$。然后在开头补上 $a[0]$，得到差分数组

$$
d=[1,2,0,2,3]
$$

这有什么用呢？如果从左到右累加 $d$ 中的元素，我们就「还原」回了 $a$ 数组 $[1,3,3,5,8]$。这类似**求导与积分**。

这又有什么用呢？现在把连续子数组 $a[1],a[2],a[3]$ 都加上 $10$，得到 $a'=[1,13,13,15,8]$。再次两两作差，并在开头补上 $a'[0]$，得到差分数组

$$
d'=[1,12,0,2,-7]
$$

对比 $d$ 和 $d'$，可以发现只有 $d[1]$ 和 $d[4]$ 变化了，这意味着对 $a$ 中连续子数组的操作，可以转变成对差分数组 $d$ 中**两个数**的操作。

## 定义和性质

对于数组 $a$，定义其**差分数组**（difference array）为

$$
d[i] =
\begin{cases}
a[0],&i=0\\
a[i]-a[i-1],&i\ge 1
\end{cases}
$$

**性质 1**：从左到右累加 $d$ 中的元素，可以得到数组 $a$。

**性质 2**：如下两个操作是等价的。

- 把 $a$ 的子数组 $a[i],a[i+1],\ldots,a[j]$ 都加上 $x$。
- 把 $d[i]$ 增加 $x$，把 $d[j+1]$ 减少 $x$。

利用性质 2，我们只需要 $\mathcal{O}(1)$ 的时间就可以完成对 $a$ 的子数组的操作。最后利用性质 1 从差分数组复原出数组 $a$。

**注**：也可以这样理解，$d[i]$ 表示把下标 $\ge i$ 的数都加上 $d[i]$。

## 本题思路

对于本题，设 $a[i]$ 表示车行驶到位置 $i$ 时车上的人数。我们需要判断是否所有 $a[i]$ 都不超过 $\textit{capacity}$。

$\textit{trips}[i]$ 相当于把 $a$ 中下标从 $\textit{from}_i$ 到 $\textit{to}_i-1$ 的数都增加 $\textit{numPassengers}_i$。这正好可以用上面讲的差分数组解决。

例如示例 1 对应的 $d$ 数组，$d[1]=2,\ d[5]=-2,\ d[3]=3,\ d[7]=-3$，即

$$
d = [0, 2, 0, 3, 0, -2, 0, -3,\ldots]
$$

从左到右累加，得到

$$
a = [0, 2, 2, 5, 5, 3, 3, 0,\ldots]
$$

$\textit{capacity}=4$，由于 $\max(a)=5>4$，所以返回 `false`。

## 实现方法

有两种写法：

1. 第一种写法是，创建一个长为 $1001$ 的差分数组，这可以保证 $d$ 数组不会下标越界。
2. 第二种写法是，用平衡树（C++ 中的 `map`，Java 中的 `TreeMap`）代替差分数组，因为我们只需要考虑在 $\textit{from}_i$ 和 $\textit{to}_i$ 这些位置上的乘客数，其余位置的乘客是保持不变的，无需考虑。平衡树可以保证我们是从小到大遍历这些位置的。当然，如果你不想用平衡树的话，也可以用哈希表，把哈希表的 key 取出来排序，就可以从小到大遍历这些位置了。

代码实现时，其实无需创建数组 $a$，只需要用一个变量 $s$ 累加差分值，如果在累加过程中发现 $s>\textit{capacity}$ 就返回 `false`。如果没有出现这种情况，就返回 `true`。

#### 第一种写法

```py [sol-Python3]
class Solution:
    def carPooling(self, trips: List[List[int]], capacity: int) -> bool:
        d = [0] * 1001
        for num, from_, to in trips:
            d[from_] += num
            d[to] -= num
        return all(s <= capacity for s in accumulate(d))
```

```java [sol-Java]
class Solution {
    public boolean carPooling(int[][] trips, int capacity) {
        int[] d = new int[1001];
        for (int[] t : trips) {
            int num = t[0], from = t[1], to = t[2];
            d[from] += num;
            d[to] -= num;
        }

        int s = 0;
        for (int v : d) {
            s += v;
            if (s > capacity) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool carPooling(vector<vector<int>> &trips, int capacity) {
        int d[1001]{};
        for (auto &t : trips) {
            int num = t[0], from = t[1], to = t[2];
            d[from] += num;
            d[to] -= num;
        }

        int s = 0;
        for (int v : d) {
            s += v;
            if (s > capacity) {
                return false;
            }
        }
        return true;
    }
};
```

```go [sol-Go]
func carPooling(trips [][]int, capacity int) bool {
    d := [1001]int{}
    for _, t := range trips {
        d[t[1]] += t[0]
        d[t[2]] -= t[0]
    }

    s := 0
    for _, v := range d {
        s += v
        if s > capacity {
            return false
        }
    }
    return true
}
```

```js [sol-JavaScript]
var carPooling = function(trips, capacity) {
    const d = Array(1001).fill(0);
    for (const [num, from, to] of trips) {
        d[from] += num;
        d[to] -= num;
    }

    let s = 0;
    for (const v of d) {
        s += v;
        if (s > capacity) {
            return false;
        }
    }
    return true;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn car_pooling(trips: Vec<Vec<i32>>, capacity: i32) -> bool {
        let mut d = vec![0; 1001];
        for t in trips {
            let num = t[0];
            let from = t[1] as usize;
            let to = t[2] as usize;
            d[from] += num;
            d[to] -= num;
        }

        let mut s = 0;
        for v in d {
            s += v;
            if s > capacity {
                return false;
            }
        }
        true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+U)$，其中 $n$ 为 $\textit{trips}$ 的长度，$U=\max(\textit{to}_i)$。
- 空间复杂度：$\mathcal{O}(U)$。

#### 第二种写法

```py [sol-Python3]
class Solution:
    def carPooling(self, trips: List[List[int]], capacity: int) -> bool:
        d = defaultdict(int)
        for num, from_, to in trips:
            d[from_] += num
            d[to] -= num

        s = 0
        for k in sorted(d):
            s += d[k]
            if s > capacity:
                return False
        return True
```

```java [sol-Java]
class Solution {
    public boolean carPooling(int[][] trips, int capacity) {
        TreeMap<Integer, Integer> d = new TreeMap<>();
        for (int[] t : trips) {
            int num = t[0], from = t[1], to = t[2];
            d.merge(from, num, Integer::sum);
            d.merge(to, -num, Integer::sum);
        }

        int s = 0;
        for (int v : d.values()) {
            s += v;
            if (s > capacity) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool carPooling(vector<vector<int>>& trips, int capacity) {
        map<int, int> d;
        for (auto& t : trips) {
            int num = t[0], from = t[1], to = t[2];
            d[from] += num;
            d[to] -= num;
        }

        int s = 0;
        for (auto& [_, v] : d) {
            s += v;
            if (s > capacity) {
                return false;
            }
        }
        return true;
    }
};
```

```go [sol-Go]
func carPooling(trips [][]int, capacity int) bool {
    d := map[int]int{}
    for _, t := range trips {
        d[t[1]] += t[0]
        d[t[2]] -= t[0]
    }

    s := 0
    for _, x := range slices.Sorted(maps.Keys(d)) {
        s += d[x]
        if s > capacity {
            return false
        }
    }
    return true
}
```

```js [sol-JavaScript]
var carPooling = function(trips, capacity) {
    const d = new Map();
    for (const [num, from, to] of trips) {
        d.set(from, (d.get(from) ?? 0) + num);
        d.set(to, (d.get(to) ?? 0) - num);
    }

    let s = 0;
    for (const k of [...d.keys()].sort((a, b) => a - b)) {
        s += d.get(k);
        if (s > capacity) {
            return false;
        }
    }
    return true;
};
```

```rust [sol-Rust]
use std::collections::BTreeMap;

impl Solution {
    pub fn car_pooling(trips: Vec<Vec<i32>>, capacity: i32) -> bool {
        let mut d = BTreeMap::new();
        for t in trips {
            let num = t[0];
            let from = t[1];
            let to = t[2];
            *d.entry(from).or_insert(0) += num;
            *d.entry(to).or_insert(0) -= num;
        }

        let mut s = 0;
        for (_, v) in d {
            s += v;
            if s > capacity {
                return false;
            }
        }
        true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{trips}$ 的长度。
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

本题来自 `二、差分 / §2.1 一维差分 / §2.1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、差分 / §2.1 一维差分 / §2.1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
