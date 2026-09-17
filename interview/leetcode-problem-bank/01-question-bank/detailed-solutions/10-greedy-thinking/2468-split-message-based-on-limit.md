# 2468. 根据限制分割消息

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/split-message-based-on-limit/
- 题目 slug：`split-message-based-on-limit`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：2382
- 外部题解来源：https://leetcode.cn/problems/split-message-based-on-limit/solutions/1964970/mei-ju-by-endlesscheng-gt7c/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举+视频讲解（Python/Java/C++/Go）](https://leetcode.cn/problems/split-message-based-on-limit/solutions/1964970/mei-ju-by-endlesscheng-gt7c/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-by-endlesscheng-gt7c`
- topic id：`1964970`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

[视频讲解](https://www.bilibili.com/video/BV1gd4y1b7qj) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

设分割个数为 $i$，它能「容纳」多长的 $\textit{message}$？

核心思路：设 $\textit{message}$ 的长度为 $n$。枚举分割个数 $i$，不断增大容量 $\textit{cap}$，直到 $\textit{cap} \ge n$ 为止，就可以分割了。

```py [sol1-Python3]
class Solution:
    def splitMessage(self, message: str, limit: int) -> List[str]:
        i = cap = 0
        while True:
            i += 1
            if i < 10:
                tail_len = 5  # 结尾的长度
            elif i < 100:
                if i == 10: cap -= 9  # 前面的结尾的长度都 +1，那么容量就要减小
                tail_len = 7
            elif i < 1000:
                if i == 100: cap -= 99
                tail_len = 9
            else:
                if i == 1000: cap -= 999
                tail_len = 11
            if tail_len >= limit: return []  # cap 无法增大，寄
            cap += limit - tail_len
            if cap < len(message): continue  # 容量没有达到，继续枚举

            ans, k = [], 0
            for j in range(1, i + 1):
                tail = f"<{j}/{i}>"
                if j == i:
                    ans.append(message[k:] + tail)
                else:
                    m = limit - len(tail)
                    ans.append(message[k: k + m] + tail)
                    k += m
            return ans
```

```java [sol1-Java]
class Solution {
    public String[] splitMessage(String message, int limit) {
        var n = message.length();
        for (int i = 1, cap = 0, tail_len; ; ++i) {
            if (i < 10) tail_len = 5; // 结尾的长度
            else if (i < 100) {
                if (i == 10) cap -= 9; // 前面的结尾的长度都 +1，那么容量就要减小
                tail_len = 7;
            } else if (i < 1000) {
                if (i == 100) cap -= 99;
                tail_len = 9;
            } else {
                if (i == 1000) cap -= 999;
                tail_len = 11;
            }
            if (tail_len >= limit) return new String[]{}; // cap 无法增大，寄
            cap += limit - tail_len;
            if (cap < n) continue; // 容量没有达到，继续枚举

            var ans = new String[i];
            for (int j = 0, k = 0; j < i; ++j) {
                var tail = "<" + (j + 1) + "/" + i + ">";
                if (j == i - 1) ans[j] = message.substring(k) + tail;
                else {
                    var m = limit - tail.length();
                    ans[j] = message.substring(k, k + m) + tail;
                    k += m;
                }
            }
            return ans;
        }
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<string> splitMessage(string message, int limit) {
        int n = message.length();
        for (int i = 1, cap = 0, tail_len;; ++i) {
            if (i < 10) tail_len = 5; // 结尾的长度
            else if (i < 100) {
                if (i == 10) cap -= 9; // 前面的结尾的长度都 +1，那么容量就要减小
                tail_len = 7;
            } else if (i < 1000) {
                if (i == 100) cap -= 99;
                tail_len = 9;
            } else {
                if (i == 1000) cap -= 999;
                tail_len = 11;
            }
            if (tail_len >= limit) return {}; // cap 无法增大，寄
            cap += limit - tail_len;
            if (cap < n) continue; // 容量没有达到，继续枚举

            vector<string> ans(i);
            for (int j = 0, k = 0; j < i; ++j) {
                string tail = "<" + to_string(j + 1) + "/" + to_string(i) + ">";
                if (j == i - 1) ans[j] = message.substr(k) + tail;
                else {
                    int m = limit - tail.length();
                    ans[j] = message.substr(k, m) + tail;
                    k += m;
                }
            }
            return ans;
        }
    }
};
```

```go [sol1-Go]
func splitMessage(message string, limit int) []string {
	for i, cap, tailLen := 1, 0, 0; ; i++ {
		if i < 10 {
			tailLen = 5 // 结尾的长度
		} else if i < 100 {
			if i == 10 { cap -= 9 } // 前面的结尾的长度都 +1，那么容量就要减小
			tailLen = 7
		} else if i < 1000 {
			if i == 100 { cap -= 99 }
			tailLen = 9
		} else {
			if i == 1000 { cap -= 999 }
			tailLen = 11
		}
		if tailLen >= limit { return nil } // cap 无法增大，寄
		cap += limit - tailLen
		if cap < len(message) { continue } // 容量没有达到，继续枚举

		ans := make([]string, i)
		for j := range ans {
			tail := fmt.Sprintf("<%d/%d>", j+1, i)
			if j == i-1 {
				ans[j] = message + tail
			} else {
				m := limit - len(tail)
				ans[j] = message[:m] + tail
				message = message[m:]
			}
		}
		return ans
	}
}
```

#### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{message}$ 的长度。构造答案需要 $O(n\log n)$ 的时间。
- 空间复杂度：$O(1)$，返回值的空间不计入。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
