# 2800. 包含三个字符串的最短字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/shortest-string-that-contains-three-strings/
- 题目 slug：`shortest-string-that-contains-three-strings`
- 来源专题：字符串
- 来源分类路径：一、KMP（前缀的后缀）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/shortest-string-that-contains-three-strings/solutions/2364733/mei-ju-by-endlesscheng-qc44/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举](https://leetcode.cn/problems/shortest-string-that-contains-three-strings/solutions/2364733/mei-ju-by-endlesscheng-qc44/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-by-endlesscheng-qc44`
- topic id：`2364733`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

下午两点[【b站@灵茶山艾府】](https://space.bilibili.com/206214)直播讲题，欢迎关注！

---

暴力枚举 $a,b,c$ 的全排列 $a',b',c'$，然后合并 $a',b'$ 得到最短字符串 $x$，再合并 $x,c'$ 得到最短字符串 $s$。

可能有的同学觉得，$a',b'$ 合并一个较长的会不会更优？

你可以这样理解：在没有完全包含的前提下，我们相当于在 $b'$ 的左边添加一些字母，右边添加一些字母，得到一个最短字符串 $s$。

可以先特判一下完全包含的情况，对于没有完全包含的情况，必然是合并得越短越好。

取最短且字典序最小的 $s$ 作为答案。

```py [sol-Python3]
class Solution:
    def minimumString(self, a: str, b: str, c: str) -> str:
        def merge(s: str, t: str) -> str:
            # 先特判完全包含的情况
            if t in s: return s
            if s in t: return t
            for i in range(min(len(s), len(t)), 0, -1):
                # 枚举：s 的后 i 个字母和 t 的前 i 个字母是一样的
                if s[-i:] == t[:i]:
                    return s + t[i:]
            return s + t

        ans = ""
        for a, b, c in permutations((a, b, c)):
            s = merge(merge(a, b), c)
            if ans == "" or len(s) < len(ans) or len(s) == len(ans) and s < ans:
                ans = s
        return ans
```

```go [sol-Go]
func merge(s, t string) string {
	// 先特判完全包含的情况
	if strings.Contains(s, t) {
		return s
	}
	if strings.Contains(t, s) {
		return t
	}
	for i := min(len(s), len(t)); ; i-- {
		// 枚举：s 的后 i 个字母和 t 的前 i 个字母是一样的
		if s[len(s)-i:] == t[:i] {
			return s + t[i:]
		}
	}
}

func minimumString(a, b, c string) (ans string) {
	arr := []string{a, b, c}
	// 枚举 arr 的全排列
	for _, p := range [][]int{{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}} {
		s := merge(merge(arr[p[0]], arr[p[1]]), arr[p[2]])
		if ans == "" || len(s) < len(ans) || len(s) == len(ans) && s < ans {
			ans = s
		}
	}
	return
}

func min(a, b int) int { if b < a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $a,b,c$ 的长度的最大值。
- 空间复杂度：$\mathcal{O}(n)$。

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
