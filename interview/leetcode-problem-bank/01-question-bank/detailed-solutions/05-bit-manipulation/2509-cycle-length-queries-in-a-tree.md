# 2509. 查询树中环的长度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/cycle-length-queries-in-a-tree/
- 题目 slug：`cycle-length-queries-in-a-tree`
- 来源专题：位运算
- 来源分类路径：八、思维题
- 难度分：1948
- 外部题解来源：https://leetcode.cn/problems/cycle-length-queries-in-a-tree/solutions/2024527/zui-jin-gong-gong-zu-xian-pythonjavacgo-v8ata/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最近公共祖先 + 位运算优化时间复杂度（Python/Java/C++/Go）](https://leetcode.cn/problems/cycle-length-queries-in-a-tree/solutions/2024527/zui-jin-gong-gong-zu-xian-pythonjavacgo-v8ata/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-jin-gong-gong-zu-xian-pythonjavacgo-v8ata`
- topic id：`2024527`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

[视频讲解](https://www.bilibili.com/video/BV1LW4y1T7if/) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

# 方法一：暴力上跳到最近公共祖先

挨个回答每个询问。

环可以看成是从 $a$ 出发往上走，在某个位置「拐弯」，往下走到 $b$。

这个拐弯的地方就是 $a$ 和 $b$ 的**最近公共祖先**。

设 $\textit{LCA}$ 为 $a$ 和 $b$ 的最近公共祖先，那么环长等于 $\textit{LCA}$ 到 $a$ 的距离加 $\textit{LCA}$ 到 $b$ 的距离加一。

如何找 $\textit{LCA}$？

注意到在完全二叉树中，深度越深的点，其编号必定大于上一层的节点编号，根据这个性质，我们可以不断循环，每次循环比较 $a$ 和 $b$ 的大小：

- 如果 $a>b$，则 $a$ 的深度大于等于 $b$ 的深度，那么把 $a$ 移动到其父节点，即 $a=a/2$；
- 如果 $a<b$，则 $a$ 的深度小于等于 $b$ 的深度，那么把 $b$ 移动到其父节点，即 $b=b/2$；
- 如果 $a=b$，则找到了 $\textit{LCA}$，退出循环。

循环次数加一即为环长。

```py [sol1-Python3]
class Solution:
    def cycleLengthQueries(self, n: int, queries: List[List[int]]) -> List[int]:
        for i, (a, b) in enumerate(queries):
            res = 1
            while a != b:
                if a > b: a //= 2
                else: b //= 2
                res += 1
            queries[i] = res
        return queries
```

```java [sol1-Java]
class Solution {
    public int[] cycleLengthQueries(int n, int[][] queries) {
        var m = queries.length;
        var ans = new int[m];
        for (var i = 0; i < m; ++i) {
            int res = 1, a = queries[i][0], b = queries[i][1];
            while (a != b) {
                if (a > b) a /= 2;
                else b /= 2;
                ++res;
            }
            ans[i] = res;
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<int> cycleLengthQueries(int n, vector<vector<int>> &queries) {
        int m = queries.size();
        vector<int> ans(m);
        for (int i = 0; i < m; ++i) {
            int res = 1, a = queries[i][0], b = queries[i][1];
            while (a != b) {
                a > b ? a /= 2 : b /= 2;
                ++res;
            }
            ans[i] = res;
        }
        return ans;
    }
};
```

```go [sol1-Go]
func cycleLengthQueries(_ int, queries [][]int) []int {
	ans := make([]int, len(queries))
	for i, q := range queries {
		res := 1
		for a, b := q[0], q[1]; a != b; res++ {
			if a > b {
				a /= 2
			} else {
				b /= 2
			}
		}
		ans[i] = res
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$O(nm)$，其中 $m$ 为 $\textit{queries}$ 的长度。回答一个询问的时间复杂度为 $O(n)$。
- 空间复杂度：$O(1)$，仅用到若干额外变量。

# 方法二：位运算优化

进一步挖掘完全二叉树的性质：节点编号的二进制的长度恰好等于节点深度。

以二进制下的 $a=110,\ b=11101$ 为例：

1. 算出两个节点的深度，分别为 $3$ 和 $5$，深度之差 $d=5-3=2$，那么把 $b$ 右移 $d$ 位（相当于上跳 $d$ 步），得到 $111$，这样 $b$ 就和 $a$ 在同一层了。
2. 如果此时 $a=b$，说明 $a$ 就是 $\textit{LCA}$，答案为 $d+1$。
3. 如果此时 $a\ne b$，计算 $a$ 异或 $b$ 的结果 $1$，它的二进制长度为 $L=1$，那么 $a$ 和 $b$ 需要各上跳 $L$ 步才能到达 $\textit{LCA}$，答案为 $d+2L+1$。

```py [sol2-Python3]
class Solution:
    def cycleLengthQueries(self, n: int, queries: List[List[int]]) -> List[int]:
        for i, (a, b) in enumerate(queries):
            if a > b: a, b = b, a  # 保证 a <= b
            d = b.bit_length() - a.bit_length()
            queries[i] = d + (a ^ (b >> d)).bit_length() * 2 + 1
        return queries
```

```java [sol2-Java]
class Solution {
    public int[] cycleLengthQueries(int n, int[][] queries) {
        var m = queries.length;
        var ans = new int[m];
        for (var i = 0; i < m; ++i) {
            int a = queries[i][0], b = queries[i][1];
            if (a > b) {
                var tmp = a;
                a = b;
                b = tmp; // 交换，保证 a <= b
            }
            var d = Integer.numberOfLeadingZeros(a) - Integer.numberOfLeadingZeros(b);
            ans[i] = d + (32 - Integer.numberOfLeadingZeros(a ^ (b >> d))) * 2 + 1;
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    vector<int> cycleLengthQueries(int n, vector<vector<int>> &queries) {
        int m = queries.size();
        vector<int> ans(m);
        for (int i = 0; i < m; ++i) {
            int a = queries[i][0], b = queries[i][1];
            if (a > b) swap(a, b); // 保证 a <= b
            int d = __builtin_clz(a) - __builtin_clz(b);
            b >>= d; // 上跳，和 a 在同一层
            ans[i] = a == b ? d + 1 : d + (32 - __builtin_clz(a ^ b)) * 2 + 1;
        }
        return ans;
    }
};
```

```go [sol2-Go]
func cycleLengthQueries(_ int, queries [][]int) []int {
	ans := make([]int, len(queries))
	for i, q := range queries {
		a, b := uint(q[0]), uint(q[1])
		if a > b {
			a, b = b, a // 保证 a <= b
		}
		d := bits.Len(b) - bits.Len(a)
		ans[i] = d + bits.Len(b>>d^a)*2 + 1
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$O(m)$，其中 $m$ 为 $\textit{queries}$ 的长度。回答一个询问的时间复杂度为 $O(1)$。
- 空间复杂度：$O(1)$，仅用到若干额外变量。

## 本地原创解析

### 1. 题意重述

本题来自 `八、思维题`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、思维题`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
