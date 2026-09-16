# 2147. 分隔长廊的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-ways-to-divide-a-long-corridor/
- 题目 slug：`number-of-ways-to-divide-a-long-corridor`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1915
- 外部题解来源：https://leetcode.cn/problems/number-of-ways-to-divide-a-long-corridor/solutions/1226675/an-zhao-ti-yi-mo-ni-o1-e-wai-kong-jian-b-5oqd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次遍历，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/number-of-ways-to-divide-a-long-corridor/solutions/1226675/an-zhao-ti-yi-mo-ni-o1-e-wai-kong-jian-b-5oqd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`an-zhao-ti-yi-mo-ni-o1-e-wai-kong-jian-b-5oqd`
- topic id：`1226675`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![lc2147.png](https://pic.leetcode.cn/1765429793-SuasaU-lc2147.png)

在示例 1 中，我们可以在第 $2$ 个座位和第 $3$ 个座位之间的任意空隙放置一个屏风，空隙个数为两个座位的下标之差 $4-1=3$。

如果座位更多，例如 $\textit{corridor} = \texttt{SSPPSSPPPSS}$，我们可以：

- 在第 $2$ 个座位和第 $3$ 个座位之间的任意空隙放置一个屏风，空隙个数为两个座位的下标之差 $4-1=3$。
- 在第 $4$ 个座位和第 $5$ 个座位之间的任意空隙放置一个屏风，空隙个数为两个座位的下标之差 $9-5=4$。
- 这两个屏风如何放置互相独立，根据**乘法原理**，划分走廊的方案数为 $3\cdot 4 = 12$。

一般地，对于第 $3,5,7,\ldots$ 个座位，可以在其到其左侧最近座位之间的任意空隙放置一个屏风，空隙个数为两个座位的下标之差。总的方案数为每个屏风的放法之积。

不合法的情况：

1. 没有座位。不满足题目「每一段内都恰好有两个座位」的要求。
2. 一共有奇数个座位。这会导致某一段只有一个座位，不满足要求。

代码实现时，注意取模。为什么可以在**中途取模**？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```py [sol-Python3]
class Solution:
    def numberOfWays(self, corridor: str) -> int:
        MOD = 1_000_000_007
        ans = 1
        cnt_s = last_s = 0

        for i, ch in enumerate(corridor):
            if ch == 'S':
                cnt_s += 1
                # 对于第 3,5,7,... 个座位，可以在其到其左侧最近座位之间的任意空隙放置屏风
                if cnt_s >= 3 and cnt_s % 2:
                    ans = ans * (i - last_s) % MOD
                last_s = i  # 记录上一个座位的位置
 
        if cnt_s == 0 or cnt_s % 2:  # 座位个数不能为 0 或奇数
            return 0
        return ans
```

```java [sol-Java]
class Solution {
    public int numberOfWays(String corridor) {
        final int MOD = 1_000_000_007;
        long ans = 1;
        int cntS = 0;
        int lastS = 0;

        for (int i = 0; i < corridor.length(); i++) {
            if (corridor.charAt(i) == 'S') {
                cntS++;
                // 对于第 3,5,7,... 个座位，可以在其到其左侧最近座位之间的任意空隙放置屏风
                if (cntS >= 3 && cntS % 2 > 0) {
                    ans = ans * (i - lastS) % MOD;
                }
                lastS = i; // 记录上一个座位的位置
            }
        }

        if (cntS == 0 || cntS % 2 > 0) { // 座位个数不能为 0 或奇数
            return 0;
        }
        return (int) ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numberOfWays(string corridor) {
        constexpr int MOD = 1'000'000'007;
        long long ans = 1;
        int cnt_s = 0, last_s = 0;

        for (int i = 0; i < corridor.size(); i++) {
            if (corridor[i] == 'S') {
                cnt_s++;
                // 对于第 3,5,7,... 个座位，可以在其到其左侧最近座位之间的任意空隙放置屏风
                if (cnt_s >= 3 && cnt_s % 2) {
                    ans = ans * (i - last_s) % MOD;
                }
                last_s = i; // 记录上一个座位的位置
            }
        }

        if (cnt_s == 0 || cnt_s % 2) { // 座位个数不能为 0 或奇数
            return 0;
        }
        return ans;
    }
};
```

```c [sol-C]
#define MOD 1000000007

int numberOfWays(char* corridor) {
    long long ans = 1;
    int cnt_s = 0, last_s = 0;

    for (int i = 0; corridor[i]; i++) {
        if (corridor[i] == 'S') {
            cnt_s++;
            // 对于第 3,5,7,... 个座位，可以在其到其左侧最近座位之间的任意空隙放置屏风
            if (cnt_s >= 3 && cnt_s % 2) {
                ans = ans * (i - last_s) % MOD;
            }
            last_s = i; // 记录上一个座位的位置
        }
    }

    if (cnt_s == 0 || cnt_s % 2) { // 座位个数不能为 0 或奇数
        return 0;
    }
    return ans;
}
```

```go [sol-Go]
func numberOfWays(corridor string) int {
	const mod = 1_000_000_007
	ans, cntS, lastS := 1, 0, 0

	for i, ch := range corridor {
		if ch == 'S' {
			cntS++
			// 对于第 3,5,7,... 个座位，可以在其到其左侧最近座位之间的任意空隙放置屏风
			if cntS >= 3 && cntS%2 > 0 {
				ans = ans * (i - lastS) % mod
			}
			lastS = i // 记录上一个座位的位置
		}
	}

	if cntS == 0 || cntS%2 > 0 { // 座位个数不能为 0 或奇数
		return 0
	}
	return ans
}
```

```js [sol-JavaScript]
var numberOfWays = function(corridor) {
    const MOD = 1_000_000_007;
    let ans = 1, cntS = 0, lastS = 0;

    for (let i = 0; i < corridor.length; i++) {
        if (corridor[i] === 'S') {
            cntS++;
            // 对于第 3,5,7,... 个座位，可以在其到其左侧最近座位之间的任意空隙放置屏风
            if (cntS >= 3 && cntS % 2) {
                ans = ans * (i - lastS) % MOD;
            }
            lastS = i; // 记录上一个座位的位置
        }
    }

    if (cntS === 0 || cntS % 2) { // 座位个数不能为 0 或奇数
        return 0;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn number_of_ways(corridor: String) -> i32 {
        const MOD: i64 = 1_000_000_007;
        let mut ans = 1;
        let mut cnt_s = 0;
        let mut last_s = 0;

        for (i, ch) in corridor.bytes().enumerate() {
            if ch == b'S' {
                cnt_s += 1;
                // 对于第 3,5,7,... 个座位，可以在其到其左侧最近座位之间的任意空隙放置屏风
                if cnt_s >= 3 && cnt_s % 2 > 0 {
                    ans = ans * ((i - last_s) as i64) % MOD;
                }
                last_s = i; // 记录上一个座位的位置
            }
        }

        if cnt_s == 0 || cnt_s % 2 > 0 { // 座位个数不能为 0 或奇数
            return 0;
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{corridor}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面数学题单的「**§2.1 乘法原理**」。

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
