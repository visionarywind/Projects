# 3076. 数组中的最短非公共子字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/shortest-uncommon-substring-in-an-array/
- 题目 slug：`shortest-uncommon-substring-in-an-array`
- 来源专题：字符串
- 来源分类路径：八、后缀数组/后缀自动机
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/shortest-uncommon-substring-in-an-array/solutions/2678694/bao-li-jian-ji-xie-fa-pythonjavacgo-by-e-tjlm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[暴力，简洁写法（Python/Java/C++/Go）](https://leetcode.cn/problems/shortest-uncommon-substring-in-an-array/solutions/2678694/bao-li-jian-ji-xie-fa-pythonjavacgo-by-e-tjlm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bao-li-jian-ji-xie-fa-pythonjavacgo-by-e-tjlm`
- topic id：`2678694`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 13:14:52 +0800

分别计算每个 $\textit{answer}[i]$。

对于 $\textit{arr}[i]$，从小到大枚举长度 $\textit{size}$，然后枚举 $\textit{arr}[i]$ 的长为 $\textit{size}$ 的所有子串 $t$，判断 $t$ 是否在其它字符串中，如果不在，就更新 $\textit{answer}[i]$ 的最小值。

```py [sol-Python3]
class Solution:
    def shortestSubstrings(self, arr: List[str]) -> List[str]:
        def check(i: int, sub: str) -> bool:
            for j, s in enumerate(arr):
                if j != i and sub in s:
                    return False
            return True

        ans = []
        for i, s in enumerate(arr):
            m = len(s)
            res = ""
            for size in range(1, m + 1):
                for j in range(size, m + 1):
                    t = s[j - size: j]
                    if (not res or t < res) and check(i, t):
                        res = t
                if res: break
            ans.append(res)
        return ans
```

```java [sol-Java]
class Solution {
    public String[] shortestSubstrings(String[] arr) {
        int n = arr.length;
        String[] ans = new String[n];
        for (int i = 0; i < n; i++) {
            int m = arr[i].length();
            String res = "";
            for (int size = 1; size <= m && res.isEmpty(); size++) {
                for (int j = size; j <= m; j++) {
                    String t = arr[i].substring(j - size, j);
                    if ((res.isEmpty() || t.compareTo(res) < 0) && check(arr, i, t)) {
                        res = t;
                    }
                }
            }
            ans[i] = res;
        }
        return ans;
    }

    private boolean check(String[] arr, int i, String sub) {
        for (int j = 0; j < arr.length; j++) {
            if (j != i && arr[j].contains(sub)) {
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
    vector<string> shortestSubstrings(vector<string> &arr) {
        int n = arr.size();
        auto check = [&](int i, string &sub) {
            for (int j = 0; j < n; j++) {
                if (j != i && arr[j].find(sub) != string::npos) {
                    return false;
                }
            }
            return true;
        };

        vector<string> ans(n);
        for (int i = 0; i < n; i++) {
            int m = arr[i].size();
            string res;
            for (int size = 1; size <= m && res.empty(); size++) {
                for (int j = size; j <= m; j++) {
                    string t = arr[i].substr(j - size, size);
                    if ((res.empty() || t < res) && check(i, t)) {
                        res = t;
                    }
                }
            }
            ans[i] = res;
        }
        return ans;
    }
};
```

```go [sol-Go]
func shortestSubstrings(arr []string) []string {
	ans := make([]string, len(arr))
	for i, s := range arr {
		m := len(s)
		res := ""
		for size := 1; size <= m && res == ""; size++ {
		next:
			for k := size; k <= m; k++ {
				sub := s[k-size : k]
				if res != "" && sub >= res {
					continue
				}
				for j, t := range arr {
					if j != i && strings.Contains(t, sub) {
						continue next
					}
				}
				res = sub
			}
		}
		ans[i] = res
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2m^4)$，其中 $n$ 为 $\textit{arr}$ 的长度，$m$ 为 $\textit{arr}[i]$ 的长度，不超过 $20$。
- 空间复杂度：$\mathcal{O}(m)$ 或 $\mathcal{O}(1)$。忽略返回值的空间。

注：线性做法可以学习「后缀数组」和「后缀自动机」。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

## 本地原创解析

### 1. 题意重述

本题来自 `八、后缀数组/后缀自动机`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、后缀数组/后缀自动机`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
