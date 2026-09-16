# 2176. 统计数组中相等且可以被整除的数对

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-equal-and-divisible-pairs-in-an-array/
- 题目 slug：`count-equal-and-divisible-pairs-in-an-array`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.5 因子
- 难度分：2200
- 外部题解来源：https://leetcode.cn/problems/count-equal-and-divisible-pairs-in-an-array/solutions/1277713/mo-ni-by-endlesscheng-wegn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(nlogn) 数学做法，枚举右维护左（Python/Java/C++/Go）](https://leetcode.cn/problems/count-equal-and-divisible-pairs-in-an-array/solutions/1277713/mo-ni-by-endlesscheng-wegn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ni-by-endlesscheng-wegn`
- topic id：`1277713`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

如果数组长度 $n=10^5$，$\mathcal{O}(n^2)$ 的暴力枚举就超时了，怎么优化？

从特殊到一般，先考虑所有 $\textit{nums}[i]$ 都相同的情况。此时问题简化成：

- 统计 $0 \le i < j < n$ 且 $ij$ 能被 $k$ 整除的下标对 $(i, j)$ 的数目。

> 由于 $0$ 不是因子，单独统计（$i=0$ 时，$ij=0$，一定能被 $k$ 整除），所以下面讨论 $1 \le i < j < n$ 的情况。

如果 $k=12$，$j=9$，哪些 $i$ 是满足要求的？

$i$ 如果是 $12$ 的倍数，肯定可以满足要求，因为 $12$ 的倍数一定能被 $12$ 整除。还有其他的 $i$ 吗？

注意到 $j=9$ 是 $3$ 的倍数，而 $12=4\times 3$，那么 $i$ 只需要是 $4$ 的倍数，便可以满足 $ij$ 是 $k$ 的倍数，因为这种情况下 $ij=(4i')(3j')=12i'j'$ 一定是 $12$ 的倍数。

换句话说，由于 $j$ 和 $k$ 的最大公因子（GCD）是 $3$，所以 $i$ 只需要是 $\dfrac{12}{3}=4$ 的倍数就可以满足要求，即 $i=4,8,12,\cdots$ 都是满足要求的。

一般地，枚举 $j$，那么 $i$ 必须是 $k'=\dfrac{k}{\text{GCD}(k,j)}$ 的倍数。

回到本题，多了一个元素值必须相同的约束。

遍历数组，设当前元素 $x=\textit{nums}[j]$，我们需要知道左边值为 $x$ 且下标是 $k'$ 的倍数的数的个数。怎么维护？例如 $\textit{nums}[4]=\textit{nums}[6]=50$，对于 $i=4$，把 $50$ 以及 $4$ 的所有因子 $1,2,4$，组成三个二元组 $(50,1),(50,2),(50,4)$ 加到哈希表中（统计二元组的个数）；对于 $i=6$，把 $50$ 以及 $6$ 的所有因子 $1,2,3,6$，组成四个二元组 $(50,1),(50,2),(50,3),(50,6)$ 加到哈希表中。这样如果后面遍历到一个值为 $50$ 的数，且 $k'=2$，通过查询哈希表中的 $(50,2)$ 的个数，就能知道，左边有两个数，值为 $50$ 且下标是 $2$ 的倍数。

```py [sol-Python3]
# 预处理每个数的因子
MX = 101
divisors = [[] for _ in range(MX)]
for i in range(1, MX):
    for j in range(i, MX, i):
        divisors[j].append(i)

class Solution:
    def countPairs(self, nums: list[int], k: int) -> int:
        ans = 0
        cnt = defaultdict(int)
        for j, x in enumerate(nums):  # 枚举 j，计算左边有多少个符合要求的 i
            if j and x == nums[0]:
                ans += 1  # 单独统计 i=0 的情况
            k2 = k // gcd(k, j)  # i 必须是 k2 的倍数
            ans += cnt[(x, k2)]
            for d in divisors[j]:  # j 是 d 的倍数
                cnt[(x, d)] += 1
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MX = 101;
    private static final List<Integer>[] divisors = new ArrayList[MX];

    static {
        Arrays.setAll(divisors, i -> new ArrayList<>());
        // 预处理每个数的因子
        for (int i = 1; i < MX; i++) {
            for (int j = i; j < MX; j += i) {
                divisors[j].add(i);
            }
        }
    }

    public int countPairs(int[] nums, int k) {
        int ans = 0;
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int j = 0; j < nums.length; j++) { // 枚举 j，计算左边有多少个符合要求的 i
            int x = nums[j];
            if (j > 0 && x == nums[0]) {
                ans++; // 单独统计 i=0 的情况
            }
            int k2 = k / gcd(k, j); // i 必须是 k2 的倍数
            // 用位运算把二元组 (x, k2) 合并成一个整数
            ans += cnt.getOrDefault(k2 << 10 | x, 0);
            for (int d : divisors[j]) { // j 是 d 的倍数
                cnt.merge(d << 10 | x, 1, Integer::sum); // cnt[d<<10|x]++
            }
        }
        return ans;
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = b % a;
            b = a;
            a = tmp;
        }
        return b;
    }
}
```

```cpp [sol-C++]
const int MX = 101;
vector<int> divisors[MX];

auto init = [] {
    for (int i = 1; i < MX; i++) {
        for (int j = i; j < MX; j += i) {
            divisors[j].push_back(i);
        }
    }
    return 0;
}();

class Solution {
public:
    int countPairs(vector<int>& nums, int k) {
        int ans = 0;
        unordered_map<int, int> cnt;
        for (int j = 0; j < nums.size(); j++) { // 枚举 j，计算左边有多少个符合要求的 i
            int x = nums[j];
            if (j && x == nums[0]) {
                ans++; // 单独统计 i=0 的情况
            }
            int k2 = k / gcd(k, j); // i 必须是 k2 的倍数
            ans += cnt[k2 << 10 | x]; // 用位运算把二元组 (x, k2) 合并成一个整数
            for (int d : divisors[j]) { // j 是 d 的倍数
                cnt[d << 10 | x]++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
const mx = 101
var divisors [mx][]int

func init() {
    // 预处理每个数的因子
    for i := 1; i < mx; i++ {
        for j := i; j < mx; j += i {
            divisors[j] = append(divisors[j], i)
        }
    }
}

func countPairs(nums []int, k int) (ans int) {
    type pair struct{ v, d int }
    cnt := map[pair]int{}
    for j, x := range nums { // 枚举 j，计算左边有多少个符合要求的 i
        if j > 0 && x == nums[0] {
            ans++ // 单独统计 i=0 的情况
        }
        k2 := k / gcd(k, j)
        ans += cnt[pair{x, k2}] // 统计左边有多少个数，值为 x 且下标是 k2 的倍数
        for _, d := range divisors[j] { // j 是 d 的倍数
            cnt[pair{x, d}]++
        }
    }
    return
}

func gcd(a, b int) int { for a != 0 { a, b = b%a, a }; return b }
```

#### 复杂度分析

由**调和级数**可得，预处理的时间是 $\mathcal{O}(M\log M)$，其中 $M=100$。

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。每次计算 GCD 需要 $\mathcal{O}(\log \min(k,n))$ 的时间。遍历 $1$ 到 $n-1$ 的所有因子跟预处理是一样的，由调和级数可得，需要 $\mathcal{O}(n\log n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n\log n)$。

## 相似题目

[2183. 统计可以被 K 整除的下标对数目](https://leetcode.cn/problems/count-array-pairs-divisible-by-k/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.5 因子`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.5 因子`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
