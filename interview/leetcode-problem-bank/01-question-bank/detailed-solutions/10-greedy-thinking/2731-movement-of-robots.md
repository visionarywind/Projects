# 2731. 移动机器人

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/movement-of-robots/
- 题目 slug：`movement-of-robots`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1923
- 外部题解来源：https://leetcode.cn/problems/movement-of-robots/solutions/2304680/nao-jin-ji-zhuan-wan-pai-xu-tong-ji-pyth-we55/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯+排序统计（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/movement-of-robots/solutions/2304680/nao-jin-ji-zhuan-wan-pai-xu-tong-ji-pyth-we55/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-pai-xu-tong-ji-pyth-we55`
- topic id：`2304680`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

[视频讲解（第三题）](https://www.bilibili.com/video/BV18u411Y7Gt/)

## 提示 1

题目最后要求机器人之间的距离，此时把任意两个机器人的位置交换，并不会对答案产生影响。

> 假设 $d$ 秒后机器人的位置数组为 $[1,2,3]$，那么交换成 $[2,1,3]$，所有机器人之间两两距离之和保持不变。

既然如此，那么可以把机器人都看成是**完全一样的，无法区分**。

## 提示 2

相撞等价于**机器人互相穿过对方**，因为我们无法区分机器人。

所以可以无视相撞的规则，把每个机器人都看成是独立运动的。

类似的思路在 [1503. 所有蚂蚁掉下来前的最后一刻](https://leetcode.cn/problems/last-moment-before-all-ants-fall-out-of-a-plank/) 中出现过。

## 提示 3

设 $d$ 秒后机器人的位置数组为 $a$，根据提示 1，可以把数组 $a$ 从小到大排序，再计算所有机器人之间两两距离之和。

从小到大枚举 $a[i]$，此时左边有 $i$ 个数都不超过 $a[i]$，那么 $a[i]$ 与其左侧机器人的距离之和为

$$
\begin{aligned}
&(a[i] - a[0])+ (a[i] - a[1]) + \cdots + (a[i] - a[i-1])\\
=&\ i\cdot a[i] - (a[0] + a[1] + \cdots + a[i-1])
\end{aligned}
$$

其中 $a[0] + a[1] + \cdots + a[i-1]$ 可以一边遍历 $a$，一边计算出来。

计算时，为了避免溢出，需要取模。原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

## 答疑

**问**：排序之前，为什么不能对 $a[i]$ 取模？

**答**：注意 $a$ 中第 $i$ 小的数要乘上 $i$，取模后每个元素的大小关系就乱了，原来第 $i$ 小的数要乘的就不一定是 $i$ 了，所以会算出错误的结果。

```py [sol-Python3]
class Solution:
    def sumDistance(self, nums: List[int], s: str, d: int) -> int:
        for i, c in enumerate(s):
            nums[i] += d if c == 'R' else -d
        nums.sort()

        ans = s = 0
        for i, x in enumerate(nums):
            ans += i * x - s
            s += x
        return ans % (10 ** 9 + 7)
```

```java [sol-Java]
class Solution {
    public int sumDistance(int[] nums, String s, int d) {
        final long MOD = (long) 1e9 + 7;
        int n = nums.length;
        long[] a = new long[n]; // 用 long 是因为 nums[i]+d 可能是 2e9+1e9，溢出了
        for (int i = 0; i < n; i++) {
            a[i] = (long) nums[i] + (s.charAt(i) == 'L' ? -d : d);
        }
        Arrays.sort(a);

        long ans = 0, sum = 0;
        for (int i = 0; i < n; i++) {
            ans = (ans + i * a[i] - sum) % MOD;
            sum += a[i];
        }
        return (int) ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumDistance(vector<int> &nums, string s, int d) {
        const int MOD = 1e9 + 7;
        int n = nums.size();
        vector<long long> a(n); // 用 long long 是因为 nums[i]+d 可能是 2e9+1e9，溢出了
        for (int i = 0; i < n; i++)
            a[i] = (long long) nums[i] + d * ((s[i] & 2) - 1); // L=-1, R=1
        sort(a.begin(), a.end());

        long long ans = 0, sum = 0;
        for (int i = 0; i < n; i++) {
            ans = (ans + i * a[i] - sum) % MOD;
            sum += a[i];
        }
        return ans;
    }
};
```

```go [sol-Go]
func sumDistance(nums []int, s string, d int) (ans int) {
	const mod = 1_000_000_007
	for i, c := range s {
		nums[i] += d * int(c&2-1) // L=-1, R=1
	}
	sort.Ints(nums)

	sum := 0
	for i, x := range nums {
		ans = (ans + i*x - sum) % mod
		sum += x
	}
	return
}
```

```js [sol-JavaScript]
var sumDistance = function(nums, s, d) {
    for (let i = 0; i < s.length; i++) {
        nums[i] += s[i] === 'R' ? d : -d;
    }
    nums.sort((a, b) => a - b);

    let ans = 0, sum = 0;
    for (let i = 0; i < nums.length; i++) {
        ans = (ans + i * nums[i] - sum) % (1e9 + 7);
        sum += nums[i];
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn sum_distance(nums: Vec<i32>, s: String, d: i32) -> i32 {
        const MOD: i64 = 1_000_000_007;
        let mut a = vec![0; nums.len()];
        let s = s.as_bytes();
        for (i, &x) in nums.iter().enumerate() {
            let d = if s[i] == b'L' as u8 { -d } else { d };
            a[i] = x as i64 + d as i64;
        }
        a.sort_unstable();

        let mut ans = 0i64;
        let mut sum = 0i64;
        for (i, &x) in a.iter().enumerate() {
            ans = (ans + i as i64 * x - sum) % MOD;
            sum += x;
        }
        ans as i32
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。如果需要用一个新的 $\textit{nums}$ 数组记录则需要 $\mathcal{O}(n)$ 空间，否则为 $\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

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
