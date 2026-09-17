# 2952. 需要添加的硬币的最小数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-coins-to-be-added/
- 题目 slug：`minimum-number-of-coins-to-be-added`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.6 归纳法
- 难度分：1784
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-coins-to-be-added/solutions/2551707/yong-gui-na-fa-si-kao-pythonjavacgo-by-e-8etj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[用归纳法思考（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-coins-to-be-added/solutions/2551707/yong-gui-na-fa-si-kao-pythonjavacgo-by-e-8etj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-gui-na-fa-si-kao-pythonjavacgo-by-e-8etj`
- topic id：`2551707`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

## 观察

为方便描述，把 $0$ 也算作可以得到的数。

假设现在得到了区间 $[0,s-1]$ 中的所有整数，如果此时遍历到整数 $x=\textit{coins}[i]$，那么把 $[0,s-1]$ 中的每个整数都增加 $x$，我们就得到了区间 $[x,s+x-1]$ 中的所有整数。

## 思路

把 $\textit{coins}$ 从小到大排序，遍历 $x=\textit{coins}[i]$。分类讨论，看是否要添加数字：

- 如果 $x \le s$，那么合并 $[0,s-1]$ 和 $[x,s+x-1]$ 这两个区间，我们可以得到 $[0,s+x-1]$ 中的所有整数。
- 如果 $x > s$，或者遍历完了 $\textit{coins}$ 数组，这意味着我们无法得到 $s$，那么就一定要把 $s$ 加到数组中（加一个比 $s$ 还小的数字就没法得到更大的数，不够贪），这样就可以得到了 $[s,2s-1]$ 中的所有整数，再与 $[0,s-1]$ 合并，可以得到 $[0,2s-1]$ 中的所有整数。然后再考虑 $x$ 和 $2s$ 的大小关系，继续分类讨论。

当 $s > \textit{target}$ 时，我们就得到了 $[1,target]$ 中的所有整数，退出循环。

附：[视频讲解](https://www.bilibili.com/video/BV1og4y1Z7SZ/)

```py [sol-Python3]
class Solution:
    def minimumAddedCoins(self, coins: List[int], target: int) -> int:
        coins.sort()
        ans, s, i = 0, 1, 0
        while s <= target:
            if i < len(coins) and coins[i] <= s:
                s += coins[i]
                i += 1
            else:
                s *= 2  # 必须添加 s
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumAddedCoins(int[] coins, int target) {
        Arrays.sort(coins);
        int ans = 0, s = 1, i = 0;
        while (s <= target) {
            if (i < coins.length && coins[i] <= s) {
                s += coins[i++];
            } else {
                s *= 2; // 必须添加 s
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
    int minimumAddedCoins(vector<int> &coins, int target) {
        ranges::sort(coins);
        int ans = 0, s = 1, i = 0;
        while (s <= target) {
            if (i < coins.size() && coins[i] <= s) {
                s += coins[i++];
            } else {
                s *= 2; // 必须添加 s
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumAddedCoins(coins []int, target int) (ans int) {
	slices.Sort(coins)
	s, i := 1, 0
	for s <= target {
		if i < len(coins) && coins[i] <= s {
			s += coins[i]
			i++
		} else {
			s *= 2 // 必须添加 s
			ans++
		}
	}
	return
}
```

```js [sol-JavaScript]
var minimumAddedCoins = function(coins, target) {
    coins.sort((a, b) => a - b);
    let ans = 0, s = 1, i = 0;
    while (s <= target) {
        if (i < coins.length && coins[i] <= s) {
            s += coins[i++];
        } else {
            s *= 2; // 必须添加 s
            ans++;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_added_coins(mut coins: Vec<i32>, target: i32) -> i32 {
        coins.sort_unstable();
        let mut ans = 0;
        let mut s = 1;
        let mut i = 0;
        while s <= target {
            if i < coins.len() && coins[i] <= s {
                s += coins[i];
                i += 1;
            } else {
                s *= 2; // 必须添加 s
                ans += 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + \log \textit{target})$，其中 $n$ 为 $\textit{coins}$ 的长度。$s$ 至多翻倍 $\mathcal{O}(\log \textit{target})$ 次。瓶颈主要在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 相似题目

- [330. 按要求补齐数组](https://leetcode.cn/problems/patching-array/)
- [1798. 你能构造出连续值的最大数目](https://leetcode.cn/problems/maximum-number-of-consecutive-values-you-can-make/)

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `四、数学贪心 / §4.6 归纳法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.6 归纳法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
