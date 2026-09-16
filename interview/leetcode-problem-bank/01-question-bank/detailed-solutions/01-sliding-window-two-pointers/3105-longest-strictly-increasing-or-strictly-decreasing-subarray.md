# 3105. 最长的严格递增或递减子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-strictly-increasing-or-strictly-decreasing-subarray/
- 题目 slug：`longest-strictly-increasing-or-strictly-decreasing-subarray`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/longest-strictly-increasing-or-strictly-decreasing-subarray/solutions/2727223/on-fen-zu-xun-huan-fu-ti-dan-pythonjavac-ep31/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 分组循环，附题单（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-strictly-increasing-or-strictly-decreasing-subarray/solutions/2727223/on-fen-zu-xun-huan-fu-ti-dan-pythonjavac-ep31/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-fen-zu-xun-huan-fu-ti-dan-pythonjavac-ep31`
- topic id：`2727223`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

读完这篇教程你就会做了：[【简单题杀手】分组循环](https://leetcode.cn/problems/longest-alternating-subarray/solution/jiao-ni-yi-ci-xing-ba-dai-ma-xie-dui-on-r57bz/)

注意本题和教程中的情况一样，第一个单调序列末尾和第二个单调序列开头，有一个元素是**重叠**的，所以下面代码在外层循环末尾要把 $i$ 减一。

请看 [视频讲解](https://www.bilibili.com/video/BV1ut421H7Wv/)，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def longestMonotonicSubarray(self, a: List[int]) -> int:
        ans = 1
        i, n = 0, len(a)
        while i < n - 1:
            if a[i + 1] == a[i]:
                i += 1  # 直接跳过
                continue
            i0 = i  # 记录这一组的开始位置
            inc = a[i + 1] > a[i]  # 定下基调：是严格递增还是严格递减
            i += 2  # i 和 i+1 已经满足要求，从 i+2 开始判断
            while i < n and a[i] != a[i - 1] and (a[i] > a[i - 1]) == inc:
                i += 1
            # 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
            ans = max(ans, i - i0)
            i -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int longestMonotonicSubarray(int[] a) {
        int ans = 1;
        int i = 0, n = a.length;
        while (i < n - 1) {
            if (a[i + 1] == a[i]) {
                i++; // 直接跳过
                continue;
            }
            int i0 = i; // 记录这一组的开始位置
            boolean inc = a[i + 1] > a[i]; // 定下基调：是严格递增还是严格递减
            i += 2; // i 和 i+1 已经满足要求，从 i+2 开始判断
            while (i < n && a[i] != a[i - 1] && (a[i] > a[i - 1]) == inc) {
                i++;
            }
            // 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
            ans = Math.max(ans, i - i0);
            i--;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestMonotonicSubarray(vector<int> &a) {
        int ans = 1;
        int i = 0, n = a.size();
        while (i < n - 1) {
            if (a[i + 1] == a[i]) {
                i++; // 直接跳过
                continue;
            }
            int i0 = i; // 记录这一组的开始位置
            bool inc = a[i + 1] > a[i]; // 定下基调：是严格递增还是严格递减
            i += 2; // i 和 i+1 已经满足要求，从 i+2 开始判断
            while (i < n && a[i] != a[i - 1] && (a[i] > a[i - 1]) == inc) {
                i++;
            }
            // 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
            ans = max(ans, i - i0);
            i--;
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestMonotonicSubarray(a []int) int {
	ans := 1
	i, n := 0, len(a)
	for i < n-1 {
		if a[i+1] == a[i] {
			i++ // 直接跳过
			continue
		}
		i0 := i              // 记录这一组的开始位置
		inc := a[i+1] > a[i] // 定下基调：是严格递增还是严格递减
		i += 2               // i 和 i+1 已经满足要求，从 i+2 开始判断
		for i < n && a[i] != a[i-1] && a[i] > a[i-1] == inc {
			i++
		}
		// 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
		ans = max(ans, i-i0)
		i--
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。时间复杂度乍一看是 $\mathcal{O}(n^2)$，但注意变量 $i$ 减少的次数是 $\mathcal{O}(n)$ 的，其它情况一直在增加，由于 $i$ 最大是 $n$，所以增加的次数是 $\mathcal{O}(n)$，所以二重循环总共循环 $\mathcal{O}(n)$ 次，时间复杂度是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 相似题目

- [1446. 连续字符](https://leetcode.cn/problems/consecutive-characters/) 1165
- [1869. 哪种连续子字符串更长](https://leetcode.cn/problems/longer-contiguous-segments-of-ones-than-zeros/) 1205
- [1957. 删除字符使字符串变好](https://leetcode.cn/problems/delete-characters-to-make-fancy-string/) 1358
- [978. 最长湍流子数组](https://leetcode.cn/problems/longest-turbulent-subarray/) 1393
- [2110. 股票平滑下跌阶段的数目](https://leetcode.cn/problems/number-of-smooth-descent-periods-of-a-stock/) 1408
- [228. 汇总区间](https://leetcode.cn/problems/summary-ranges/)
- [2760. 最长奇偶子数组](https://leetcode.cn/problems/longest-even-odd-subarray-with-threshold/) 1420
- [1887. 使数组元素相等的减少操作次数](https://leetcode.cn/problems/reduction-operations-to-make-the-array-elements-equal/) 1428
- [845. 数组中的最长山脉](https://leetcode.cn/problems/longest-mountain-in-array/) 1437
- [2038. 如果相邻两个颜色均相同则删除当前颜色](https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/) 1468
- [1759. 统计同质子字符串的数目](https://leetcode.cn/problems/count-number-of-homogenous-substrings/) 1491
- [3011. 判断一个数组是否可以变为有序](https://leetcode.cn/problems/find-if-array-can-be-sorted/) 1497
- [1578. 使绳子变成彩色的最短时间](https://leetcode.cn/problems/minimum-time-to-make-rope-colorful/) 1574
- [1839. 所有元音按顺序排布的最长子字符串](https://leetcode.cn/problems/longest-substring-of-all-vowels-in-order/) 1580
- [2765. 最长交替子序列](https://leetcode.cn/problems/longest-alternating-subarray/) 1581
- [467. 环绕字符串中唯一的子字符串](https://leetcode.cn/problems/unique-substrings-in-wraparound-string/) ~1700
- [2948. 交换得到字典序最小的数组](https://leetcode.cn/problems/make-lexicographically-smallest-array-by-swapping-elements/) 2047
- [2393. 严格递增的子数组个数](https://leetcode.cn/problems/count-strictly-increasing-subarrays/)（会员题）
- [2436. 使子数组最大公约数大于一的最小分割数](https://leetcode.cn/problems/minimum-split-into-subarrays-with-gcd-greater-than-one/)（会员题）
- [2495. 乘积为偶数的子数组数](https://leetcode.cn/problems/number-of-subarrays-having-even-product/)（会员题）
- [3063. 链表频率](https://leetcode.cn/problems/linked-list-frequency/)（会员题）

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `六、分组循环`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、分组循环`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
