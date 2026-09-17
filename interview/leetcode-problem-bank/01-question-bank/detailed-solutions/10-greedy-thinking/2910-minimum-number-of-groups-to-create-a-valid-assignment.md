# 2910. 合法分组的最少组数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-groups-to-create-a-valid-assignment/
- 题目 slug：`minimum-number-of-groups-to-create-a-valid-assignment`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.6 先枚举，再贪心
- 难度分：2132
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-groups-to-create-a-valid-assignment/solutions/2493313/ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-t174/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[本题最简单写法（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-number-of-groups-to-create-a-valid-assignment/solutions/2493313/ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-t174/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-t174`
- topic id：`2493313`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

统计每个数字的出现次数，记在哈希表 $\textit{cnt}$ 中。

假设可以分成大小为 $k$ 和 $k+1$ 的组，现在需要算出每个 $\textit{cnt}[x]$ 最少可以分成多少组。

举例说明，假设 $\textit{\textit{cnt}}[x]=32$，$k=10$，那么 $32=10+10+10+2$，多出的 $2$ 可以分成两个 $1$，加到两个 $10$ 中，从而得到 $11,11,10$ 这三组。

但如果 $\textit{\textit{cnt}}[x]=34$，那么 $34=10+10+10+4$，多出的 $4$ 无法加到另外三个 $10$ 中。

设 $q=\left\lfloor\dfrac{\textit{cnt}[x]}{k}\right\rfloor$，$r = \textit{cnt}[x] \bmod k$。

如果 $q < r$ 则无法分成 $k$ 和 $k+1$ 组，否则一定可以分组。

**在可以分组的前提下**，分出的 $k+1$ 越多，组数就越少，所以最少可以分出

$$
\left\lceil\dfrac{\textit{cnt}[x]}{k+1}\right\rceil
$$

组。累加组数即为分组个数。

例如 $\textit{cnt}[x] = 9$，如果先分出尽量多的 $k=2$，再分出 $k+1=3$，那么分组方案就是 $9=2+2+2+3$，但是先按照 $k+1=3$ 分，则有 $9=3+3+3$，可以分出更少的组。

从 $\min(\textit{cnt}[x])$ 开始倒着枚举 $k$，只要可以分，就立刻返回答案。

```py [sol-Python3]
class Solution:
    def minGroupsForValidAssignment(self, nums: List[int]) -> int:
        cnt = Counter(nums)
        for k in range(min(cnt.values()), 0, -1):
            ans = 0
            for c in cnt.values():
                q, r = divmod(c, k)
                if q < r:
                    break
                ans += (c + k) // (k + 1)
            else:
                return ans
```

```java [sol-Java]
class Solution {
    public int minGroupsForValidAssignment(int[] nums) {
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            cnt.merge(x, 1, Integer::sum);
        }
        int k = nums.length;
        for (int c : cnt.values()) {
            k = Math.min(k, c);
        }
        for (; ; k--) {
            int ans = 0;
            for (int c : cnt.values()) {
                if (c / k < c % k) {
                    ans = 0;
                    break;
                }
                ans += (c + k) / (k + 1);
            }
            if (ans > 0) {
                return ans;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minGroupsForValidAssignment(vector<int>& nums) {
        unordered_map<int, int> cnt;
        for (int x : nums) {
            cnt[x]++;
        }
        int k = min_element(cnt.begin(), cnt.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        })->second;
        for (; ; k--) {
            int ans = 0;
            for (auto &[_, c] : cnt) {
                if (c / k < c % k) {
                    ans = 0;
                    break;
                }
                ans += (c + k) / (k + 1);
            }
            if (ans) {
                return ans;
            }
        }
    }
};
```

```go [sol-Go]
func minGroupsForValidAssignment(nums []int) int {
	cnt := map[int]int{}
	for _, x := range nums {
		cnt[x]++
	}
	k := len(nums)
	for _, c := range cnt {
		k = min(k, c)
	}
	for ; ; k-- {
		ans := 0
		for _, c := range cnt {
			if c/k < c%k {
				ans = 0
				break
			}
			ans += (c + k) / (k + 1)
		}
		if ans > 0 {
			return ans
		}
	}
}

func min(a, b int) int { if b < a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。设哈希表的大小为 $m$，哈希表中最小的 value 为 $k$，由于所有 value 之和为 $n$，所以 $km\le n$。而循环次数又至多为 $km$，所以时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.6 先枚举，再贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.6 先枚举，再贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
