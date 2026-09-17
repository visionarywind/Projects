# 2763. 所有子数组中不平衡数字之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-imbalance-numbers-of-all-subarrays/
- 题目 slug：`sum-of-imbalance-numbers-of-all-subarrays`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.5 贡献法
- 难度分：2700
- 外部题解来源：https://leetcode.cn/problems/sum-of-imbalance-numbers-of-all-subarrays/solutions/2327214/bao-li-mei-ju-pythonjavacgo-by-endlessch-2r7p/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举/贡献法（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-imbalance-numbers-of-all-subarrays/solutions/2327214/bao-li-mei-ju-pythonjavacgo-by-endlessch-2r7p/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bao-li-mei-ju-pythonjavacgo-by-endlessch-2r7p`
- topic id：`2327214`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 方法一：枚举

由于 $n$ 至多为 $1000$，我们可以从左到右枚举子数组左端点 $i$，然后从 $i+1$ 开始向右枚举子数组右端点 $j$。一边枚举 $j$，一边维护不平衡度 $\textit{cnt}$：

- 如果 $x=\textit{nums}[j]$ 之前出现过，那么子数组排序后必然会和另一个 $x$ 相邻，$\textit{cnt}$ 不变；
- 如果 $x=\textit{nums}[j]$ 之前没出现过，那么看 $x-1$ 和 $x+1$ 是否出现过：
  - 都没有，$\textit{cnt}$ 加一；
  - 只有一个，$\textit{cnt}$ 不变；
  - 两个都有，$\textit{cnt}$ 减一。

遍历过程中，累加 $\textit{cnt}$，即为答案。

```py [sol-Python3]
class Solution:
    def sumImbalanceNumbers(self, nums: List[int]) -> int:
        ans, n = 0, len(nums)
        for i, x in enumerate(nums):
            vis = [False] * (n + 2)
            vis[x] = True
            cnt = 0
            for j in range(i + 1, n):
                x = nums[j]
                if not vis[x]:
                    cnt += 1 - vis[x - 1] - vis[x + 1]
                    vis[x] = True
                ans += cnt
        return ans
```

```java [sol-Java]
class Solution {
    public int sumImbalanceNumbers(int[] nums) {
        int ans = 0, n = nums.length;
        var vis = new boolean[n + 2];
        for (int i = 0; i < n; i++) {
            Arrays.fill(vis, false);
            vis[nums[i]] = true;
            int cnt = 0;
            for (int j = i + 1; j < n; j++) {
                int x = nums[j];
                if (!vis[x]) {
                    cnt++;
                    if (vis[x - 1]) cnt--;
                    if (vis[x + 1]) cnt--;
                    vis[x] = true;
                }
                ans += cnt;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumImbalanceNumbers(vector<int> &nums) {
        int ans = 0, n = nums.size();
        bool vis[n + 2];
        for (int i = 0; i < n; i++) {
            memset(vis, 0, sizeof(vis));
            vis[nums[i]] = true;
            int cnt = 0;
            for (int j = i + 1; j < n; j++) {
                int x = nums[j];
                if (!vis[x]) {
                    cnt += 1 - vis[x - 1] - vis[x + 1];
                    vis[x] = true;
                }
                ans += cnt;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func sumImbalanceNumbers(nums []int) (ans int) {
	n := len(nums)
	for i, x := range nums {
		vis := make([]int, n+2)
		vis[x] = 1
		cnt := 0
		for j := i + 1; j < n; j++ {
			if x := nums[j]; vis[x] == 0 {
				cnt += 1 - vis[x-1] - vis[x+1]
				vis[x] = 1
			}
			ans += cnt
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：贡献法

视频讲解见[【周赛 352】](https://www.bilibili.com/video/BV1ej411m7zV/)第四题。

```py [sol-Python3]
class Solution:
    def sumImbalanceNumbers(self, nums: List[int]) -> int:
        n = len(nums)
        right = [0] * n  # nums[i] 右侧的 x 和 x-1 的最近下标（不存在时为 n）
        idx = [n] * (n + 1)
        for i in range(n - 1, -1, -1):
            x = nums[i]
            right[i] = min(idx[x], idx[x - 1])
            idx[x] = i

        ans = 0
        idx = [-1] * (n + 1)
        for i, (x, r) in enumerate(zip(nums, right)):
            # 统计 x 能产生多少贡献
            ans += (i - idx[x - 1]) * (r - i)  # 子数组左端点个数 * 子数组右端点个数
            idx[x] = i
        # 上面计算的时候，每个子数组的最小值必然可以作为贡献，而这是不合法的
        # 所以每个子数组都多算了 1 个不合法的贡献
        return ans - n * (n + 1) // 2
```

```java [sol-Java]
class Solution {
    public int sumImbalanceNumbers(int[] nums) {
        int n = nums.length;
        var right = new int[n];
        var idx = new int[n + 1];
        Arrays.fill(idx, n);
        for (int i = n - 1; i >= 0; i--) {
            int x = nums[i];
            // right[i] 表示 nums[i] 右侧的 x 和 x-1 的最近下标（不存在时为 n）
            right[i] = Math.min(idx[x], idx[x - 1]);
            idx[x] = i;
        }

        int ans = 0;
        Arrays.fill(idx, -1);
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            // 统计 x 能产生多少贡献
            ans += (i - idx[x - 1]) * (right[i] - i); // 子数组左端点个数 * 子数组右端点个数
            idx[x] = i;
        }
        // 上面计算的时候，每个子数组的最小值必然可以作为贡献，而这是不合法的
        // 所以每个子数组都多算了 1 个不合法的贡献
        return ans - n * (n + 1) / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumImbalanceNumbers(vector<int> &nums) {
        int n = nums.size(), right[n], idx[n + 1];
        fill(idx, idx + n + 1, n);
        for (int i = n - 1; i >= 0; i--) {
            int x = nums[i];
            // right[i] 表示 nums[i] 右侧的 x 和 x-1 的最近下标（不存在时为 n）
            right[i] = min(idx[x], idx[x - 1]);
            idx[x] = i;
        }

        int ans = 0;
        memset(idx, -1, sizeof(idx));
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            // 统计 x 能产生多少贡献
            ans += (i - idx[x - 1]) * (right[i] - i); // 子数组左端点个数 * 子数组右端点个数
            idx[x] = i;
        }
        // 上面计算的时候，每个子数组的最小值必然可以作为贡献，而这是不合法的
        // 所以每个子数组都多算了 1 个不合法的贡献
        return ans - n * (n + 1) / 2;
    }
};
```

```go [sol-Go]
func sumImbalanceNumbers(nums []int) (ans int) {
	n := len(nums)
	right := make([]int, n)
	idx := make([]int, n+1)
	for i := range idx {
		idx[i] = n
	}
	for i := n - 1; i >= 0; i-- {
		x := nums[i]
		// right[i] 表示 nums[i] 右侧的 x 和 x-1 的最近下标（不存在时为 n）
		right[i] = min(idx[x], idx[x-1])
		idx[x] = i
	}

	for i := range idx {
		idx[i] = -1
	}
	for i, x := range nums {
		// 统计 x 能产生多少贡献
		ans += (i - idx[x-1]) * (right[i] - i) // 子数组左端点个数 * 子数组右端点个数
		idx[x] = i
	}
	// 上面计算的时候，每个子数组的最小值必然可以作为贡献，而这是不合法的
	// 所以每个子数组都多算了 1 个不合法的贡献
	return ans - n*(n+1)/2
}

func min(a, b int) int { if b < a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

把 `sarr[i+1] - sarr[i] > 1` 改成 `sarr[i+1] - sarr[i] > k` 要怎么做？欢迎在评论区发表你的思路。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.5 贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.5 贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
