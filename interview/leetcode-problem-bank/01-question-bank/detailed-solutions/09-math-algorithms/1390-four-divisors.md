# 1390. 四因数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/four-divisors/
- 题目 slug：`four-divisors`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.5 因子
- 难度分：1478
- 外部题解来源：https://leetcode.cn/problems/four-divisors/solutions/3867962/mo-ban-yu-chu-li-mei-ge-shu-de-yin-zi-ge-e0v6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】预处理每个数的因子个数、因子和（Python/Java/C++/Go）](https://leetcode.cn/problems/four-divisors/solutions/3867962/mo-ban-yu-chu-li-mei-ge-shu-de-yin-zi-ge-e0v6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-yu-chu-li-mei-ge-shu-de-yin-zi-ge-e0v6`
- topic id：`3867962`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

对于一个数，它有哪些因子？这不能一眼看出来。

但反过来，对于一个数 $i$，$i$ 的倍数都有因子 $i$。

枚举 $i=1,2,3,\ldots,10^5$，以及 $i$ 的倍数 $j$，把 $j$ 的因子个数加一，因子和加上 $i$。

对于 $\textit{nums}$ 中的因子个数为 $4$ 的元素，累加这些元素的因子和，即为答案。

```py [sol-Python3]
MX = 100_001
divisor_num = [0] * MX
divisor_sum = [0] * MX
for i in range(1, MX):
    for j in range(i, MX, i):  # 枚举 i 的倍数 j
        divisor_num[j] += 1  # i 是 j 的因子
        divisor_sum[j] += i

class Solution:
    def sumFourDivisors(self, nums: List[int]) -> int:
        ans = 0
        for x in nums:
            if divisor_num[x] == 4:
                ans += divisor_sum[x]
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MX = 100_001;
    private static final int[] divisorNum = new int[MX];
    private static final int[] divisorSum = new int[MX];
    private static boolean initialized = false;

    // 这样写比 static block 快
    public Solution() {
        if (initialized) {
            return;
        }
        initialized = true;

        for (int i = 1; i < MX; i++) {
            for (int j = i; j < MX; j += i) { // 枚举 i 的倍数 j
                divisorNum[j]++; // i 是 j 的因子
                divisorSum[j] += i;
            }
        }
    }

    public int sumFourDivisors(int[] nums) {
        int ans = 0;
        for (int x : nums) {
            if (divisorNum[x] == 4) {
                ans += divisorSum[x];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
constexpr int MX = 100'001;
int divisor_num[MX];
int divisor_sum[MX];

int init = [] {
    for (int i = 1; i < MX; i++) {
        for (int j = i; j < MX; j += i) { // 枚举 i 的倍数 j
            divisor_num[j]++; // i 是 j 的因子
            divisor_sum[j] += i;
        }
    }
    return 0;
}();

class Solution {
public:
    int sumFourDivisors(vector<int>& nums) {
        int ans = 0;
        for (int x : nums) {
            if (divisor_num[x] == 4) {
                ans += divisor_sum[x];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
const mx = 100_001

var divisorNum, divisorSum [mx]int

func init() {
	for i := 1; i < mx; i++ {
		for j := i; j < mx; j += i { // 枚举 i 的倍数 j
			divisorNum[j]++ // i 是 j 的因子
			divisorSum[j] += i
		}
	}
}

func sumFourDivisors(nums []int) (ans int) {
	for _, x := range nums {
		if divisorNum[x] == 4 {
			ans += divisorSum[x]
		}
	}
	return
}
```

#### 复杂度分析

> 设 $U=\max(\textit{nums})$。预处理的时间为 $\mathcal{O}(U\log U)$（调和级数和），空间为 $\mathcal{O}(U)$，不计入。

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面数学题单的「**§1.5 因子**」。

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
