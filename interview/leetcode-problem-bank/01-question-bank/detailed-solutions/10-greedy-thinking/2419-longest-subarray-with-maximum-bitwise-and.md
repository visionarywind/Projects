# 2419. 按位与最大的最长子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-subarray-with-maximum-bitwise-and/
- 题目 slug：`longest-subarray-with-maximum-bitwise-and`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1496
- 外部题解来源：https://leetcode.cn/problems/longest-subarray-with-maximum-bitwise-and/solutions/1848011/nao-jin-ji-zhuan-wan-by-endlesscheng-75dq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯，从两次遍历到一次遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/longest-subarray-with-maximum-bitwise-and/solutions/1848011/nao-jin-ji-zhuan-wan-by-endlesscheng-75dq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-by-endlesscheng-75dq`
- topic id：`1848011`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

AND（$&$）运算的性质是，**参与 AND 运算的元素越多，AND 结果越小**（不会变得更大）。

比如 $3\ &\ 2 = 2$，$3\ &\ 2\ &\ 1 = 0$。

所以我们通过 AND 运算能得到的最大值，就是 $\textit{nums}$ 的最大值 $\textit{mx}$。

多个相同 $\textit{mx}$ 计算 AND，结果仍然是 $\textit{mx}$；而 $\textit{mx}$ 与另一个小于 $\textit{mx}$ 的数计算 AND，结果会小于 $\textit{mx}$。所以问题变成：

- 计算 $\textit{nums}$ 的最长连续子数组的长度，该子数组只包含 $\textit{mx}$。

**算法**：

1. 遍历 $\textit{nums}$，计算 $\textit{nums}$ 的最大值 $\textit{mx}$。
2. 遍历 $\textit{nums}$，同时用一个 $\textit{cnt}$ 变量统计连续相同 $\textit{mx}$ 的个数。
3. 如果 $\textit{nums}[i] = \textit{mx}$，把 $\textit{cnt}$ 加一；否则连续相同 $\textit{mx}$ 断开，把 $\textit{cnt}$ 重置为 $0$。
4. 每次 $\textit{cnt}$ 加一后，用 $\textit{cnt}$ 更新答案的最大值。

## 写法一：两次遍历

```py [sol-Python3]
class Solution:
    def longestSubarray(self, nums: List[int]) -> int:
        mx = max(nums)
        ans = cnt = 0
        for x in nums:
            if x == mx:
                cnt += 1
                ans = max(ans, cnt)
            else:
                cnt = 0  # 连续 mx 断开了，重新统计
        return ans
```

```java [sol-Java]
class Solution {
    public int longestSubarray(int[] nums) {
        int mx = Arrays.stream(nums).max().getAsInt();
        int ans = 0;
        int cnt = 0;
        for (int x : nums) {
            if (x == mx) {
                cnt++;
                ans = Math.max(ans, cnt);
            } else {
                cnt = 0; // 连续 mx 断开了，重新统计
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestSubarray(vector<int>& nums) {
        int mx = ranges::max(nums);
        int ans = 0, cnt = 0;
        for (int x : nums) {
            if (x == mx) {
                cnt++;
                ans = max(ans, cnt);
            } else {
                cnt = 0; // 连续 mx 断开了，重新统计
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int longestSubarray(int* nums, int numsSize) {
    int mx = nums[0];
    for (int i = 1; i < numsSize; i++) {
        mx = MAX(mx, nums[i]);
    }

    int ans = 0, cnt = 0;
    for (int i = 0; i < numsSize; i++) {
        if (nums[i] == mx) {
            cnt++;
            ans = MAX(ans, cnt);
        } else {
            cnt = 0; // 连续 mx 断开了，重新统计
        }
    }
    return ans;
}
```

```go [sol-Go]
func longestSubarray(nums []int) (ans int) {
	mx := slices.Max(nums)
	cnt := 0
	for _, x := range nums {
		if x == mx {
			cnt++
			ans = max(ans, cnt)
		} else {
			cnt = 0 // 连续 mx 断开了，重新统计
		}
	}
	return
}
```

```js [sol-JavaScript]
var longestSubarray = function(nums) {
    const mx = Math.max(...nums);
    let ans = 0, cnt = 0;
    for (const x of nums) {
        if (x === mx) {
            cnt++;
            ans = Math.max(ans, cnt);
        } else {
            cnt = 0; // 连续 mx 断开了，重新统计
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn longest_subarray(nums: Vec<i32>) -> i32 {
        let mx = *nums.iter().max().unwrap();
        let mut ans = 0;
        let mut cnt = 0;
        for x in nums {
            if x == mx {
                cnt += 1;
                ans = ans.max(cnt);
            } else {
                cnt = 0; // 连续 mx 断开了，重新统计
            }
        }
        ans
    }
}
```

## 写法二：一次遍历

```py [sol-Python3]
class Solution:
    def longestSubarray(self, nums: List[int]) -> int:
        ans = mx = cnt = 0
        for x in nums:
            if x > mx:
                # 发现新的 mx，重新统计所有内容
                mx = x
                ans = cnt = 1
            elif x == mx:
                cnt += 1
                if cnt > ans:  # 手写 max 更快
                    ans = cnt
            else:
                cnt = 0  # 连续 mx 断开了，重新统计
        return ans
```

```java [sol-Java]
class Solution {
    public int longestSubarray(int[] nums) {
        int ans = 0;
        int mx = 0;
        int cnt = 0;
        for (int x : nums) {
            if (x > mx) {
                // 发现新的 mx，重新统计所有内容
                mx = x;
                cnt = 1;
                ans = 1;
            } else if (x == mx) {
                cnt++;
                ans = Math.max(ans, cnt);
            } else {
                cnt = 0; // 连续 mx 断开了，重新统计
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestSubarray(vector<int>& nums) {
        int ans = 0, mx = 0, cnt = 0;
        for (int x : nums) {
            if (x > mx) {
                // 发现新的 mx，重新统计所有内容
                mx = x;
                ans = cnt = 1;
            } else if (x == mx) {
                cnt++;
                ans = max(ans, cnt);
            } else {
                cnt = 0; // 连续 mx 断开了，重新统计
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int longestSubarray(int* nums, int numsSize) {
    int ans = 0, mx = 0, cnt = 0;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        if (x > mx) {
            // 发现新的 mx，重新统计所有内容
            mx = x;
            ans = cnt = 1;
        } else if (x == mx) {
            cnt++;
            ans = MAX(ans, cnt);
        } else {
            cnt = 0; // 连续 mx 断开了，重新统计
        }
    }
    return ans;
}
```

```go [sol-Go]
func longestSubarray(nums []int) (ans int) {
	mx, cnt := 0, 0
	for _, x := range nums {
		if x > mx {
			// 发现新的 mx，重新统计所有内容
			mx = x
			cnt = 1
			ans = 1
		} else if x == mx {
			cnt++
			ans = max(ans, cnt)
		} else {
			cnt = 0 // 连续 mx 断开了，重新统计
		}
	}
	return
}
```

```js [sol-JavaScript]
var longestSubarray = function(nums) {
    let ans = 0, mx = 0, cnt = 0;
    for (const x of nums) {
        if (x > mx) {
            // 发现新的 mx，重新统计所有内容
            mx = x;
            ans = cnt = 1;
        } else if (x === mx) {
            cnt++;
            ans = Math.max(ans, cnt);
        } else {
            cnt = 0; // 连续 mx 断开了，重新统计
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn longest_subarray(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut mx = 0;
        let mut cnt = 0;
        for x in nums {
            if x > mx {
                // 发现新的 mx，重新统计所有内容
                mx = x;
                ans = 1;
                cnt = 1;
            } else if x == mx {
                cnt += 1;
                ans = ans.max(cnt);
            } else {
                cnt = 0; // 连续 mx 断开了，重新统计
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面贪心与思维题单的「**§5.2 脑筋急转弯**」。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.2 脑筋急转弯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.2 脑筋急转弯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
