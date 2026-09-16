# 2569. 更新数组后处理求和查询

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/handling-sum-queries-after-update/
- 题目 slug：`handling-sum-queries-after-update`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.4 Lazy 线段树（有区间更新）
- 难度分：2398
- 外部题解来源：https://leetcode.cn/problems/handling-sum-queries-after-update/solutions/2119436/xian-duan-shu-by-endlesscheng-vx80/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】Lazy 线段树（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/handling-sum-queries-after-update/solutions/2119436/xian-duan-shu-by-endlesscheng-vx80/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xian-duan-shu-by-endlesscheng-vx80`
- topic id：`2119436`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 本题视频讲解（Lazy 线段树原理）

见 [双周赛 98](https://www.bilibili.com/video/BV15D4y1G7ms/) 第四题。

小贴士：理解 [二叉树的递归](https://www.bilibili.com/video/BV1UD4y1Y769/) 对理解线段树有很大帮助。

## 思路

假设 $\textit{nums}_1$ 中总共有 $c$ 个 $1$，那么操作 2 相当于把 $\textit{nums}_2$ 的元素和增加了 $c\cdot p$。所以只需要维护 $\textit{nums}_1$ 中 $1$ 的个数。

如何实现操作 1？用 Lazy 线段树维护区间内 $1$ 的个数 $\textit{cnt}_1$，以及整个区间是否需要反转的 Lazy 标记 $\textit{flip}$。具体见上面的视频链接。

```py [sol-Python3]
class Solution:
    def handleQuery(self, nums1: List[int], nums2: List[int], queries: List[List[int]]) -> List[int]:
        n = len(nums1)
        cnt1 = [0] * (2 << n.bit_length())
        flip = [False] * (2 << n.bit_length())

        # 维护区间 1 的个数
        def maintain(o: int) -> None:
            cnt1[o] = cnt1[o * 2] + cnt1[o * 2 + 1]

        # 执行区间反转
        def do(o: int, l: int, r: int) -> None:
            cnt1[o] = r - l + 1 - cnt1[o]
            flip[o] = not flip[o]

        # 初始化线段树   o,l,r=1,0,n-1
        def build(o: int, l: int, r: int) -> None:
            if l == r:
                cnt1[o] = nums1[l]
                return
            m = (l + r) // 2
            build(o * 2, l, m)
            build(o * 2 + 1, m + 1, r)
            maintain(o)

        # 反转区间 [L,R]   o,l,r=1,0,n-1
        def update(o: int, l: int, r: int, L: int, R: int) -> None:
            if L <= l and r <= R:
                do(o, l, r)
                return
            m = (l + r) // 2
            if flip[o]:
                do(o * 2, l, m)
                do(o * 2 + 1, m + 1, r)
                flip[o] = False
            if m >= L:
                update(o * 2, l, m, L, R)
            if m < R:
                update(o * 2 + 1, m + 1, r, L, R)
            maintain(o)

        build(1, 0, n - 1)
        ans = []
        s = sum(nums2)
        for op, l, r in queries:
            if op == 1:
                update(1, 0, n - 1, l, r)
            elif op == 2:
                s += l * cnt1[1]
            else:
                ans.append(s)
        return ans
```

```java [sol-Java]
class Solution {
    private int[] cnt1;
    private boolean[] flip;

    public long[] handleQuery(int[] nums1, int[] nums2, int[][] queries) {
        int n = nums1.length;
        cnt1 = new int[2 << (32 - Integer.numberOfLeadingZeros(n))];
        flip = new boolean[cnt1.length];
        build(nums1, 1, 0, n - 1);

        long sum = 0;
        for (int x : nums2) {
            sum += x;
        }

        int m = 0;
        for (int[] q : queries) {
            if (q[0] == 3) {
                m++;
            }
        }

        long[] ans = new long[m];
        int i = 0;
        for (int[] q : queries) {
            if (q[0] == 1) {
                update(1, 0, n - 1, q[1], q[2]);
            } else if (q[0] == 2) {
                sum += (long) q[1] * cnt1[1];
            } else {
                ans[i++] = sum;
            }
        }
        return ans;
    }

    // 维护区间 1 的个数
    private void maintain(int o) {
        cnt1[o] = cnt1[o * 2] + cnt1[o * 2 + 1];
    }

    // 执行区间反转
    private void do_(int o, int l, int r) {
        cnt1[o] = r - l + 1 - cnt1[o];
        flip[o] = !flip[o];
    }

    // 初始化线段树   o,l,r=1,0,n-1
    private void build(int[] a, int o, int l, int r) {
        if (l == r) {
            cnt1[o] = a[l];
            return;
        }
        int m = (l + r) / 2;
        build(a, o * 2, l, m);
        build(a, o * 2 + 1, m + 1, r);
        maintain(o);
    }

    // 反转区间 [L,R]   o,l,r=1,0,n-1
    private void update(int o, int l, int r, int L, int R) {
        if (L <= l && r <= R) {
            do_(o, l, r);
            return;
        }
        int m = (l + r) / 2;
        if (flip[o]) {
            do_(o * 2, l, m);
            do_(o * 2 + 1, m + 1, r);
            flip[o] = false;
        }
        if (m >= L) {
            update(o * 2, l, m, L, R);
        }
        if (m < R) {
            update(o * 2 + 1, m + 1, r, L, R);
        }
        maintain(o);
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<int> cnt1, flip;

    // 维护区间 1 的个数
    void maintain(int o) {
        cnt1[o] = cnt1[o * 2] + cnt1[o * 2 + 1];
    }

    // 执行区间反转
    void do_(int o, int l, int r) {
        cnt1[o] = r - l + 1 - cnt1[o];
        flip[o] = !flip[o];
    }

    // 初始化线段树   o,l,r=1,0,n-1
    void build(vector<int>& a, int o, int l, int r) {
        if (l == r) {
            cnt1[o] = a[l];
            return;
        }
        int m = (l + r) / 2;
        build(a, o * 2, l, m);
        build(a, o * 2 + 1, m + 1, r);
        maintain(o);
    }

    // 反转区间 [L,R]   o,l,r=1,0,n-1
    void update(int o, int l, int r, int L, int R) {
        if (L <= l && r <= R) {
            do_(o, l, r);
            return;
        }
        int m = (l + r) / 2;
        if (flip[o]) {
            do_(o * 2, l, m);
            do_(o * 2 + 1, m + 1, r);
            flip[o] = false;
        }
        if (m >= L) {
            update(o * 2, l, m, L, R);
        }
        if (m < R) {
            update(o * 2 + 1, m + 1, r, L, R);
        }
        maintain(o);
    }

public:
    vector<long long> handleQuery(vector<int>& nums1, vector<int>& nums2, vector<vector<int>>& queries) {
        int n = nums1.size();
        int m = 2 << (32 - __builtin_clz(n));
        cnt1.resize(m);
        flip.resize(m);
        build(nums1, 1, 0, n - 1);

        vector<long long> ans;
        long long sum = reduce(nums2.begin(), nums2.end(), 0LL);
        for (auto& q : queries) {
            if (q[0] == 1) {
                update(1, 0, n - 1, q[1], q[2]);
            } else if (q[0] == 2) {
                sum += (long long) q[1] * cnt1[1];
            } else {
                ans.push_back(sum);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
type seg []struct {
    l, r, cnt1 int
    flip       bool
}

// 维护区间 1 的个数
func (t seg) maintain(o int) {
    t[o].cnt1 = t[o<<1].cnt1 + t[o<<1|1].cnt1
}

// 初始化线段树   o,l,r=1,0,n-1
func (t seg) build(a []int, o, l, r int) {
    t[o].l, t[o].r = l, r
    if l == r {
        t[o].cnt1 = a[l]
        return
    }
    m := (l + r) >> 1
    t.build(a, o<<1, l, m)
    t.build(a, o<<1|1, m+1, r)
    t.maintain(o)
}

// 执行区间反转
func (t seg) do(O int) {
    o := &t[O]
    o.cnt1 = o.r - o.l + 1 - o.cnt1
    o.flip = !o.flip
}

// 反转区间 [l,r]   o=1
func (t seg) update(o, l, r int) {
    if l <= t[o].l && t[o].r <= r {
        t.do(o)
        return
    }
    if t[o].flip {
        t.do(o << 1)
        t.do(o<<1 | 1)
        t[o].flip = false
    }
    m := (t[o].l + t[o].r) >> 1
    if l <= m {
        t.update(o<<1, l, r)
    }
    if m < r {
        t.update(o<<1|1, l, r)
    }
    t.maintain(o)
}

func handleQuery(nums1, nums2 []int, queries [][]int) (ans []int64) {
    t := make(seg, 2<<bits.Len(uint(len(nums1)-1)))
    t.build(nums1, 1, 0, len(nums1)-1)

    sum := 0
    for _, x := range nums2 {
        sum += x
    }
    for _, q := range queries {
        if q[0] == 1 {
            t.update(1, q[1], q[2])
        } else if q[0] == 2 {
            sum += q[1] * t[1].cnt1
        } else {
            ans = append(ans, int64(sum))
        }
    }
    return
}
```

```js [sol-JavaScript]
var handleQuery = function(nums1, nums2, queries) {
    const n = nums1.length;
    const m = 2 << (32 - Math.clz32(n));
    const cnt1 = Array(m).fill(0);
    const flip = Array(m).fill(false);

    // 维护区间 1 的个数
    function maintain(o) {
        cnt1[o] = cnt1[o * 2] + cnt1[o * 2 + 1];
    }

    // 执行区间反转
    function do_(o, l, r) {
        cnt1[o] = r - l + 1 - cnt1[o];
        flip[o] = !flip[o];
    }

    // 初始化线段树   o,l,r=1,0,n-1
    function build(a, o, l, r) {
        if (l === r) {
            cnt1[o] = a[l];
            return;
        }
        const m = Math.floor((l + r) / 2);
        build(a, o * 2, l, m);
        build(a, o * 2 + 1, m + 1, r);
        maintain(o);
    }

    // 反转区间 [L,R]   o,l,r=1,0,n-1
    function update(o, l, r, L, R) {
        if (L <= l && r <= R) {
            do_(o, l, r);
            return;
        }
        const m = Math.floor((l + r) / 2);
        if (flip[o]) {
            do_(o * 2, l, m);
            do_(o * 2 + 1, m + 1, r);
            flip[o] = false;
        }
        if (m >= L) {
            update(o * 2, l, m, L, R);
        }
        if (m < R) {
            update(o * 2 + 1, m + 1, r, L, R);
        }
        maintain(o);
    }

    build(nums1, 1, 0, n - 1);
    const ans = [];
    let sum = _.sum(nums2);
    for (const [op, l, r] of queries) {
        if (op === 1) {
            update(1, 0, n - 1, l, r);
        } else if (op === 2) {
            sum += l * cnt1[1];
        } else {
            ans.push(sum);
        }
    }
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+q\log n)$，其中 $n$ 为 $\textit{nums}_1$ 的长度，$q$ 为 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `八、树状数组和线段树 / §8.4 Lazy 线段树（有区间更新）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、树状数组和线段树 / §8.4 Lazy 线段树（有区间更新）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
