# 3036. 匹配模式数组的子数组数目 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-subarrays-that-match-a-pattern-ii/
- 题目 slug：`number-of-subarrays-that-match-a-pattern-ii`
- 来源专题：字符串
- 来源分类路径：一、KMP（前缀的后缀）
- 难度分：1895
- 外部题解来源：https://leetcode.cn/problems/number-of-subarrays-that-match-a-pattern-ii/solutions/2637713/liang-chong-fang-fa-kmp-zhan-shu-pythonj-zil4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：KMP / Z函数（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-subarrays-that-match-a-pattern-ii/solutions/2637713/liang-chong-fang-fa-kmp-zhan-shu-pythonj-zil4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-kmp-zhan-shu-pythonj-zil4`
- topic id：`2637713`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

把 $\textit{nums}$ 的相邻元素，根据题目规定的大小关系，转换成 $1,0,-1$，得到一个长为 $n-1$ 的数组 $b$。

问题相当于问 $b$ 中有多少个连续子数组等于 $\textit{pattern}$。

这是一个标准的字符串匹配问题（本题匹配的是数字不是字符），可以用 KMP 或者 Z 函数解决。

## 方法一：KMP

关于 KMP 的原理，请看我在知乎上的 [这篇讲解](https://www.zhihu.com/question/21923021/answer/37475572)。

```py [sol-Python3]
class Solution:
    def countMatchingSubarrays(self, nums: List[int], pattern: List[int]) -> int:
        m = len(pattern)
        pi = [0] * m
        cnt = 0
        for i in range(1, m):
            v = pattern[i]
            while cnt and pattern[cnt] != v:
                cnt = pi[cnt - 1]
            if pattern[cnt] == v:
                cnt += 1
            pi[i] = cnt

        ans = cnt = 0
        for x, y in pairwise(nums):
            v = (y > x) - (y < x)
            while cnt and pattern[cnt] != v:
                cnt = pi[cnt - 1]
            if pattern[cnt] == v:
                cnt += 1
            if cnt == m:
                ans += 1
                cnt = pi[cnt - 1]
        return ans
```

```java [sol-Java]
class Solution {
    public int countMatchingSubarrays(int[] nums, int[] pattern) {
        int m = pattern.length;
        int[] pi = new int[m];
        int cnt = 0;
        for (int i = 1; i < m; i++) {
            int v = pattern[i];
            while (cnt > 0 && pattern[cnt] != v) {
                cnt = pi[cnt - 1];
            }
            if (pattern[cnt] == v) {
                cnt++;
            }
            pi[i] = cnt;
        }

        int ans = 0;
        cnt = 0;
        for (int i = 1; i < nums.length; i++) {
            int v = Integer.compare(nums[i], nums[i - 1]);
            while (cnt > 0 && pattern[cnt] != v) {
                cnt = pi[cnt - 1];
            }
            if (pattern[cnt] == v) {
                cnt++;
            }
            if (cnt == m) {
                ans++;
                cnt = pi[cnt - 1];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countMatchingSubarrays(vector<int> &nums, vector<int> &pattern) {
        int m = pattern.size();
        vector<int> pi(m);
        int cnt = 0;
        for (int i = 1; i < m; i++) {
            int v = pattern[i];
            while (cnt && pattern[cnt] != v) {
                cnt = pi[cnt - 1];
            }
            if (pattern[cnt] == v) {
                cnt++;
            }
            pi[i] = cnt;
        }

        int ans = 0;
        cnt = 0;
        for (int i = 1; i < nums.size(); i++) {
            int x = nums[i - 1], y = nums[i];
            int v = (y > x) - (y < x);
            while (cnt && pattern[cnt] != v) {
                cnt = pi[cnt - 1];
            }
            if (pattern[cnt] == v) {
                cnt++;
            }
            if (cnt == m) {
                ans++;
                cnt = pi[cnt - 1];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countMatchingSubarrays(nums, pattern []int) (ans int) {
	m := len(pattern)
	pi := make([]int, m)
	cnt := 0
	for i := 1; i < m; i++ {
		v := pattern[i]
		for cnt > 0 && pattern[cnt] != v {
			cnt = pi[cnt-1]
		}
		if pattern[cnt] == v {
			cnt++
		}
		pi[i] = cnt
	}

	cnt = 0
	for i := 1; i < len(nums); i++ {
		v := cmp.Compare(nums[i], nums[i-1])
		for cnt > 0 && pattern[cnt] != v {
			cnt = pi[cnt-1]
		}
		if pattern[cnt] == v {
			cnt++
		}
		if cnt == m {
			ans++
			cnt = pi[cnt-1]
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$，其中 $m$ 为 $\textit{pattern}$ 的长度。

## 方法二：Z 函数（扩展 KMP）

上场周赛讲了 [Z 函数的原理](https://www.bilibili.com/video/BV1it421W7D8/)。

我们可以把 $\textit{pattern}$ 拼在 $b$ 的前面（为了防止匹配越界，中间插入一个不在数组中的数字，比如 $2$），根据 Z 函数的定义，只要 $z[i] = m$，我们就找到了一个匹配。

```py [sol-Python3]
class Solution:
    def countMatchingSubarrays(self, nums: List[int], pattern: List[int]) -> int:
        m = len(pattern)
        pattern.append(2)
        pattern.extend((y > x) - (y < x) for x, y in pairwise(nums))

        n = len(pattern)
        z = [0] * n
        l = r = 0
        for i in range(1, n):
            if i <= r:
                z[i] = min(z[i - l], r - i + 1)
            while i + z[i] < n and pattern[z[i]] == pattern[i + z[i]]:
                l, r = i, i + z[i]
                z[i] += 1

        return sum(lcp == m for lcp in z[m + 1:])
```

```java [sol-Java]
class Solution {
    public int countMatchingSubarrays(int[] nums, int[] pattern) {
        int m = pattern.length;
        int[] s = Arrays.copyOf(pattern, m + nums.length);
        s[m] = 2;
        for (int i = 1; i < nums.length; i++) {
            s[m + i] = Integer.compare(nums[i], nums[i - 1]);
        }

        int n = s.length;
        int[] z = new int[n];
        int l = 0, r = 0;
        for (int i = 1; i < n; i++) {
            if (i <= r) {
                z[i] = Math.min(z[i - l], r - i + 1);
            }
            while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
                l = i;
                r = i + z[i];
                z[i]++;
            }
        }

        int ans = 0;
        for (int i = m + 1; i < n; i++) {
            if (z[i] == m) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countMatchingSubarrays(vector<int> &nums, vector<int> &pattern) {
        int m = pattern.size();
        pattern.push_back(2);
        for (int i = 1; i < nums.size(); i++) {
            int x = nums[i - 1], y = nums[i];
            pattern.push_back((y > x) - (y < x));
        }

        int n = pattern.size();
        vector<int> z(n);
        int l = 0, r = 0;
        for (int i = 1; i < n; i++) {
            if (i <= r) {
                z[i] = min(z[i - l], r - i + 1);
            }
            while (i + z[i] < n && pattern[z[i]] == pattern[i + z[i]]) {
                l = i;
                r = i + z[i];
                z[i]++;
            }
        }

        int ans = 0;
        for (int i = m + 1; i < n; i++) {
            ans += z[i] == m;
        }
        return ans;
    }
};
```

```go [sol-Go]
func countMatchingSubarrays(nums, pattern []int) (ans int) {
	m := len(pattern)
	pattern = append(pattern, 2)
	for i := 1; i < len(nums); i++ {
		pattern = append(pattern, cmp.Compare(nums[i], nums[i-1]))
	}

	n := len(pattern)
	z := make([]int, n)
	l, r := 0, 0
	for i := 1; i < n; i++ {
		if i <= r {
			z[i] = min(z[i-l], r-i+1)
		}
		for i+z[i] < n && pattern[z[i]] == pattern[i+z[i]] {
			l, r = i, i+z[i]
			z[i]++
		}
	}

	for _, lcp := range z[m+1:] {
		if lcp == m {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

## 本地原创解析

### 1. 题意重述

本题来自 `一、KMP（前缀的后缀）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、KMP（前缀的后缀）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
