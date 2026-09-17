# 2430. 对字母串可执行的最大删除数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-deletions-on-a-string/
- 题目 slug：`maximum-deletions-on-a-string`
- 来源专题：字符串
- 来源分类路径：二、Z 函数（后缀的前缀）
- 难度分：2102
- 外部题解来源：https://leetcode.cn/problems/maximum-deletions-on-a-string/solutions/1864049/xian-xing-dppythonjavacgo-by-endlesschen-gpx9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-deletions-on-a-string/solutions/1864049/xian-xing-dppythonjavacgo-by-endlesschen-gpx9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xian-xing-dppythonjavacgo-by-endlesschen-gpx9`
- topic id：`1864049`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

定义 $f[i]$ 表示删除后缀 $s[i:]$ 所需的最大操作数。

根据题意，我们可以枚举删除字母的长度 $j$，如果 $s[i:i+j] = s[i+j:i+2j]$，那么可以删除，此时有转移 $f[i] = f[i+j] + 1$。如果不存在两个子串相等的情况，则 $f[i] = 1$。$f[i]$ 取所有情况的最大值。

倒着计算 $f[i]$，答案为 $f[0]$。

最后，我们需要快速判断两个子串是否相同。这可以用 $\mathcal{O}(n^2)$ 的 DP 预处理出来，具体见代码。

[视频讲解](https://www.bilibili.com/video/BV1kd4y1q7fC)

```py [sol-Python3]
class Solution:
    def deleteString(self, s: str) -> int:
        n = len(s)
        if len(set(s)) == 1:  # 特判全部相同的情况
            return n

        lcp = [[0] * (n + 1) for _ in range(n + 1)]  # lcp[i][j] 表示 s[i:] 和 s[j:] 的最长公共前缀
        for i in range(n - 1, -1, -1):
            for j in range(n - 1, i, -1):
                if s[i] == s[j]:
                    lcp[i][j] = lcp[i + 1][j + 1] + 1

        f = [0] * n
        for i in range(n - 1, -1, -1):
            for j in range(1, (n - i) // 2 + 1):
                if lcp[i][i + j] >= j:  # 说明 s[i:i+j] == s[i+j:i+2*j]
                    f[i] = max(f[i], f[i + j])  # 这里改成手动 if 更新会更快
            f[i] += 1
        return f[0]
```

```java [sol-Java]
class Solution {
    public int deleteString(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        if (allEqual(s)) { // 特判全部相同的情况
            return n;
        }

        int[][] lcp = new int[n + 1][n + 1]; // lcp[i][j] 表示 s[i:] 和 s[j:] 的最长公共前缀
        for (int i = n - 1; i >= 0; i--) {
            for (int j = n - 1; j > i; j--) {
                if (s[i] == s[j]) {
                    lcp[i][j] = lcp[i + 1][j + 1] + 1;
                }
            }
        }

        int[] f = new int[n];
        for (int i = n - 1; i >= 0; i--) {
            for (int j = 1; i + j * 2 <= n; j++) {
                if (lcp[i][i + j] >= j) { // 说明 s[i:i+j] == s[i+j:i+j*2]
                    f[i] = Math.max(f[i], f[i + j]);
                }
            }
            f[i]++;
        }
        return f[0];
    }

    private boolean allEqual(char[] s) {
        for (int i = 1; i < s.length; i++) {
            if (s[i] != s[0]) {
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
    int deleteString(string s) {
        int n = s.size();
        if (equal(s.begin() + 1, s.end(), s.begin())) { // 特判全部相同的情况
            return n;
        }

        vector lcp(n + 1, vector<int>(n + 1)); // lcp[i][j] 表示 s[i:] 和 s[j:] 的最长公共前缀
        for (int i = n - 1; i >= 0; i--) {
            for (int j = n - 1; j > i; j--) {
                if (s[i] == s[j]) {
                    lcp[i][j] = lcp[i + 1][j + 1] + 1;
                }
            }
        }

        vector<int> f(n);
        for (int i = n - 1; i >= 0; i--) {
            for (int j = 1; i + j * 2 <= n; j++) {
                if (lcp[i][i + j] >= j) { // 说明 s[i:i+j] == s[i+j:i+j*2]
                    f[i] = max(f[i], f[i + j]);
                }
            }
            f[i]++;
        }
        return f[0];
    }
};
```

```go [sol-Go]
func deleteString(s string) int {
	n := len(s)
	if allEqual(s) { // 特判全部相同的情况
		return n
	}

	lcp := make([][]int, n+1) // lcp[i][j] 表示 s[i:] 和 s[j:] 的最长公共前缀
	lcp[n] = make([]int, n+1)
	for i := n - 1; i >= 0; i-- {
		lcp[i] = make([]int, n+1)
		for j := n - 1; j > i; j-- {
			if s[i] == s[j] {
				lcp[i][j] = lcp[i+1][j+1] + 1
			}
		}
	}

	f := make([]int, n)
	for i := n - 1; i >= 0; i-- {
		for j := 1; i+j*2 <= n; j++ {
			if lcp[i][i+j] >= j { // 说明 s[i:i+j] == s[i+j:i+j*2]
				f[i] = max(f[i], f[i+j])
			}
		}
		f[i]++
	}
	return f[0]
}

func allEqual(s string) bool {
	for i := 1; i < len(s); i++ {
		if s[i] != s[0] {
			return false
		}
	}
	return true
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

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

本题来自 `二、Z 函数（后缀的前缀）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、Z 函数（后缀的前缀）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
