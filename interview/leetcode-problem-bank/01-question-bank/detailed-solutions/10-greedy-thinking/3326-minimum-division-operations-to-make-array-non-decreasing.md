# 3326. 使数组非递减的最少除法操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-division-operations-to-make-array-non-decreasing/
- 题目 slug：`minimum-division-operations-to-make-array-non-decreasing`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1864
- 外部题解来源：https://leetcode.cn/problems/minimum-division-operations-to-make-array-non-decreasing/solutions/2957768/yu-chu-li-lpf-cong-you-dao-zuo-tan-xin-p-k3gt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理 LPF + 从右到左贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-division-operations-to-make-array-non-decreasing/solutions/2957768/yu-chu-li-lpf-cong-you-dao-zuo-tan-xin-p-k3gt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-lpf-cong-you-dao-zuo-tan-xin-p-k3gt`
- topic id：`2957768`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

定义 $\text{LPF}(x)$ 为 $x$ 的**最小质因子**。规定 $\text{LPF}(1)=1$。

- 如果 $\text{LPF}(x)=x$，说明 $x$ 是 $1$ 或者质数，无法变小。
- 如果 $\text{LPF}(x)<x$，说明 $x$ 是合数，可以变小。由于题目规定只能除以最大真因数，我们可以把 $x$ 除以 $\dfrac{x}{\text{LPF}(x)}$，得到 $\text{LPF}(x)$。

贪心，最后一个数肯定无需减少，所以我们从 $i=n-2$ 开始倒着遍历 $\textit{nums}$：

- 如果 $\textit{nums}[i] > \textit{nums}[i+1]$，那么把 $\textit{nums}[i]$ 更新为 $\text{LPF}(\textit{nums}[i])$，操作次数加一。注意更新后 $\textit{nums}[i]$ 一定是质数或 $1$，无法再变小。
- 更新后，如果 $\textit{nums}[i] > \textit{nums}[i+1]$ 仍然成立，说明无法把 $\textit{nums}$ 变成非降的，返回 $-1$。

代码实现时，由于 $1$ 不比其他数大，所以无需初始化 $\text{LPF}(1)=1$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1UcyYY4EnQ/) 第三题，欢迎点赞关注~

```py [sol-Python3]
MX = 1_000_001
LPF = [0] * MX
for i in range(2, MX):
    if LPF[i] == 0:
        for j in range(i, MX, i):
            if LPF[j] == 0:
                LPF[j] = i

class Solution:
    def minOperations(self, nums: List[int]) -> int:
        ans = 0
        for i in range(len(nums) - 2, -1, -1):
            if nums[i] > nums[i + 1]:
                nums[i] = LPF[nums[i]]
                if nums[i] > nums[i + 1]:
                    return -1
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MX = 1_000_001;
    private static final int[] lpf = new int[MX];

    static {
        for (int i = 2; i < MX; i++) {
            if (lpf[i] == 0) {
                for (int j = i; j < MX; j += i) {
                    if (lpf[j] == 0) {
                        lpf[j] = i;
                    }
                }
            }
        }
    }

    public int minOperations(int[] nums) {
        int ans = 0;
        for (int i = nums.length - 2; i >= 0; i--) {
            if (nums[i] > nums[i + 1]) {
                nums[i] = lpf[nums[i]];
                if (nums[i] > nums[i + 1]) {
                    return -1;
                }
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
const int MX = 1'000'001;
int LPF[MX];

auto init = [] {
    for (int i = 2; i < MX; i++) {
        if (LPF[i] == 0) {
            for (int j = i; j < MX; j += i) {
                if (LPF[j] == 0) {
                    LPF[j] = i;
                }
            }
        }
    }
    return 0;
}();

class Solution {
public:
    int minOperations(vector<int>& nums) {
        int ans = 0;
        for (int i = nums.size() - 2; i >= 0; i--) {
            if (nums[i] > nums[i + 1]) {
                nums[i] = LPF[nums[i]];
                if (nums[i] > nums[i + 1]) {
                    return -1;
                }
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
const mx = 1_000_001
var lpf = [mx]int{}

func init() {
	for i := 2; i < mx; i++ {
		if lpf[i] == 0 {
			for j := i; j < mx; j += i {
				if lpf[j] == 0 {
					lpf[j] = i
				}
			}
		}
	}
}

func minOperations(nums []int) (ans int) {
	for i := len(nums) - 2; i >= 0; i-- {
		if nums[i] > nums[i+1] {
			nums[i] = lpf[nums[i]]
			if nums[i] > nums[i+1] {
				return -1
			}
			ans++
		}
	}
	return
}
```

#### 复杂度分析

预处理的时间复杂度为 $\mathcal{O}(U\log\log U)$，其中 $U=10^6$。注：用欧拉筛（线性筛）可以做到 $\mathcal{O}(U)$。

对于 $\texttt{minOperations}$：

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

更多相似题目，见下面贪心题单中的「**§1.4 从最左/最右开始贪心**」，以及数学题单中的「**§1.3 质因数分解**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.4 从最左/最右开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.4 从最左/最右开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
