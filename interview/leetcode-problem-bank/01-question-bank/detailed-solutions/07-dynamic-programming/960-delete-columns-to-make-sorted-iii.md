# 960. 删列造序 III

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/delete-columns-to-make-sorted-iii/
- 题目 slug：`delete-columns-to-make-sorted-iii`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶
- 难度分：2247
- 外部题解来源：https://leetcode.cn/problems/delete-columns-to-make-sorted-iii/solutions/3854506/zui-chang-di-zeng-zi-xu-lie-pythonjavacc-itpk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最长递增子序列（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/delete-columns-to-make-sorted-iii/solutions/3854506/zui-chang-di-zeng-zi-xu-lie-pythonjavacc-itpk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-chang-di-zeng-zi-xu-lie-pythonjavacc-itpk`
- topic id：`3854506`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

考虑最多保留多少列。

如果 $n=1$，那么本题是允许相邻元素相等的 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)。视频讲解：[最长递增子序列【基础算法精讲 20】](https://www.bilibili.com/video/BV1ub411Q7sB/)。

如果 $n>1$ 呢？能不能用同样的套路（枚举选哪个）解决？学习一下 300 题，动手试一试吧！

定义 $f[i]$ 表示每个子序列都以 $i$ 列结尾时，最多保留的列数。

枚举子序列的倒数第二列是 $j$。如果对于每一行，$j$ 列的字母都不超过 $i$ 列的字母，那么和 300 题一样，用 $f[j] + 1$ 更新 $f[i]$ 的最大值。（代码先用 $f[j]$ 更新 $f[i]$ 的最大值，循环结束后再加一。）

注意 $i$ 列也可以单独形成一个长为 $1$ 的子序列。

答案为 $m - \max(f)$。其中 $m$ 是 $\textit{strs}[i]$ 的长度。

```py [sol-Python3]
class Solution:
    def minDeletionSize(self, strs: List[str]) -> int:
        m = len(strs[0])
        f = [0] * m
        for i in range(m):
            for j in range(i):
                # 如果 f[j] <= f[i]，就不用跑 O(n) 的 all 了
                if f[j] > f[i] and all(s[j] <= s[i] for s in strs):
                    f[i] = f[j]
            f[i] += 1
        return m - max(f)
```

```java [sol-Java]
class Solution {
    public int minDeletionSize(String[] strs) {
        int m = strs[0].length();
        int[] f = new int[m];
        int maxF = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < i; j++) {
                // 如果 f[j] <= f[i]，就不用跑 O(n) 的 lessEq 了
                if (f[j] > f[i] && lessEq(strs, j, i)) {
                    f[i] = f[j];
                }
            }
            f[i]++;
            maxF = Math.max(maxF, f[i]);
        }
        return m - maxF;
    }

    // 对于每一行，j 列的字母都 <= i 列的字母？
    private boolean lessEq(String[] strs, int j, int i) {
        for (String s : strs) {
            if (s.charAt(j) > s.charAt(i)) {
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
    int minDeletionSize(vector<string>& strs) { 
        // 对于每一行，j 列的字母都 <= i 列的字母？
        auto less_eq = [&](int j, int i) -> bool {
            for (auto& s : strs) {
                if (s[j] > s[i]) {
                    return false;
                }
            }
            return true;
        };

        int m = strs[0].size();
        vector<int> f(m);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < i; j++) {
                // 如果 f[j] <= f[i]，就不用跑 O(n) 的 less_eq 了
                if (f[j] > f[i] && less_eq(j, i)) {
                    f[i] = f[j];
                }
            }
            f[i]++;
        }
        return m - ranges::max(f);
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int minDeletionSize(char** strs, int strsSize) { 
    // 对于每一行，j 列的字母都 <= i 列的字母？
    bool less_eq(int j, int i) {
        for (int k = 0; k < strsSize; k++) {
            if (strs[k][j] > strs[k][i]) {
                return false;
            }
        }
        return true;
    }

    int m = strlen(strs[0]);
    int* f = calloc(m, sizeof(int));
    int max_f = 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < i; j++) {
            // 如果 f[j] <= f[i]，就不用跑 O(n) 的 less_eq 了
            if (f[j] > f[i] && less_eq(j, i)) {
                f[i] = f[j];
            }
        }
        f[i]++;
        max_f = MAX(max_f, f[i]);
    }

    free(f);
    return m - max_f;
}
```

```go [sol-Go]
func minDeletionSize(strs []string) int {
	// 对于每一行，j 列的字母都 <= i 列的字母？
	lessEq := func(j, i int) bool {
		for _, s := range strs {
			if s[j] > s[i] {
				return false
			}
		}
		return true
	}

	m := len(strs[0])
	f := make([]int, m)
	for i := range m {
		for j := range i {
			// 如果 f[j] <= f[i]，就不用跑 O(n) 的 lessEq 了
			if f[j] > f[i] && lessEq(j, i) {
				f[i] = f[j]
			}
		}
		f[i]++
	}
	return m - slices.Max(f)
}
```

```js [sol-JavaScript]
var minDeletionSize = function(strs) {
    // 对于每一行，j 列的字母都 <= i 列的字母？
    function lessEq(j, i) {
        for (const s of strs) {
            if (s[j] > s[i]) {
                return false;
            }
        }
        return true;
    }

    const m = strs[0].length;
    const f = Array(m).fill(0);
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < i; j++) {
            // 如果 f[j] <= f[i]，就不用跑 O(n) 的 lessEq 了
            if (f[j] > f[i] && lessEq(j, i)) {
                f[i] = f[j];
            }
        }
        f[i]++;
    }
    return m - Math.max(...f);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_deletion_size(strs: Vec<String>) -> i32 {
        let m = strs[0].len();
        let mut f = vec![0; m];
        for i in 0..m {
            for j in 0..i {
                // 如果 f[j] <= f[i]，就不用跑 O(n) 的 all 了
                if f[j] > f[i] && strs.iter().all(|s| s.as_bytes()[j] <= s.as_bytes()[i]) {
                    f[i] = f[j];
                }
            }
            f[i] += 1;
        }
        m as i32 - *f.iter().max().unwrap()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm^2)$，其中 $n$ 是 $\textit{strs}$ 的长度，$m$ 是 $\textit{strs}[i]$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$。

## 专题训练

见下面动态规划题单的「**§4.2 最长递增子序列（LIS）**」。

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

本题来自 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
