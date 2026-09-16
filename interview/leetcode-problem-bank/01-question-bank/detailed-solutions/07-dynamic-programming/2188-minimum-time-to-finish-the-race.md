# 2188. 完成比赛的最少时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-time-to-finish-the-race/
- 题目 slug：`minimum-time-to-finish-the-race`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.1 一维 DP
- 难度分：2315
- 外部题解来源：https://leetcode.cn/problems/minimum-time-to-finish-the-race/solutions/1295939/jie-he-xing-zhi-qiao-miao-dp-by-endlessc-b963/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[结合性质巧妙 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-time-to-finish-the-race/solutions/1295939/jie-he-xing-zhi-qiao-miao-dp-by-endlessc-b963/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jie-he-xing-zhi-qiao-miao-dp-by-endlessc-b963`
- topic id：`1295939`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

#### 提示 1

连续使用同一个轮胎时，耗费的时间是指数增长的。

#### 提示 2

连续使用同一个轮胎 $i$ 跑 $x$ 圈，第 $x$ 圈的耗时不应超过 $\textit{changeTime} + f_i$，否则直接更换轮胎会更优。结合提示 1，连续使用同一个轮胎的次数不会很大。

#### 提示 3

考虑线性 DP。

---

#### 上界分析

连续使用同一个轮胎 $i$ 跑 $x$ 圈，第 $x$ 圈的耗时不应超过 $\textit{changeTime} + f_i$，即

$$
f_i\cdot r_i^{x-1} \le \textit{changeTime} + f_i
$$

考虑 $x$ 至多能是多少。由于 $f_i$ 越小 $x$ 的上界越大，以及 $r_i$ 越小 $x$ 的上界越大，那么取 $f_i=1,r_i=2$，则有

$$
2^{x-1}\le \textit{changeTime} + 1
$$

解得

$$
x\le \log_2(\textit{changeTime}+1)+1
$$

由于 $x$ 是个整数，因此 $x$ 的上界为 $\lfloor \log_2(\textit{changeTime}+1)+1 \rfloor$。

根据题目的数据范围，代码实现时可将上界视为 $17$。

#### 算法

首先预处理出连续使用同一个轮胎跑 $x$ 圈的最小耗时，记作 $\textit{minSec}[x]$，这可以通过遍历每个轮胎计算出来。

然后定义 $f[i]$ 表示跑 $i$ 圈的最小耗时。为方便计算，初始值 $f[0]=-\textit{changeTime}$。

考虑最后一个轮胎连续跑了 $j$ 圈，我们可以从 $f[i-j]$ 转移过来，因此有转移方程

$$
f[i] = \textit{changeTime} + \min\limits_{j=1}^{\min(17,i)} f[i-j] + \textit{minSec}[j]
$$

最后答案为 $f[\textit{numLaps}]$。

```py [sol-Python3]
class Solution:
    def minimumFinishTime(self, tires: List[List[int]], changeTime: int, numLaps: int) -> int:
        min_sec = [inf] * 18
        for f, r in tires:
            x, time, s = 1, f, 0
            while time <= changeTime + f:
                s += time
                min_sec[x] = min(min_sec[x], s)
                time *= r
                x += 1

        f = [0] * (numLaps + 1)
        f[0] = -changeTime
        for i in range(1, numLaps + 1):
            f[i] = changeTime + min(f[i - j] + min_sec[j] for j in range(1, min(18, i + 1)))
        return f[numLaps]
```

```java [sol-Java]
class Solution {
    public int minimumFinishTime(int[][] tires, int changeTime, int numLaps) {
        int[] minSec = new int[18];
        Arrays.fill(minSec, Integer.MAX_VALUE / 2); // 除二是防止下面计算状态转移时溢出
        for (int[] tire : tires) {
            long time = tire[0];
            int sum = 0;
            for (int x = 1; time <= changeTime + tire[0]; x++) {
                sum += time;
                minSec[x] = Math.min(minSec[x], sum);
                time *= tire[1];
            }
        }

        int[] f = new int[numLaps + 1];
        Arrays.fill(f, Integer.MAX_VALUE);
        f[0] = -changeTime;
        for (int i = 1; i <= numLaps; i++) {
            for (int j = 1; j <= Math.min(17, i); j++) {
                f[i] = Math.min(f[i], f[i - j] + minSec[j]);
            }
            f[i] += changeTime;
        }
        return f[numLaps];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumFinishTime(vector<vector<int>>& tires, int changeTime, int numLaps) {
        vector<int> minSec(18, INT_MAX / 2); // 除二是防止下面计算状态转移时溢出
        for (auto& tire : tires) {
            long time = tire[0];
            for (int x = 1, sum = 0; time <= changeTime + tire[0]; x++) {
                sum += time;
                minSec[x] = min(minSec[x], sum);
                time *= tire[1];
            }
        }

        vector<int> f(numLaps + 1, INT_MAX);
        f[0] = -changeTime;
        for (int i = 1; i <= numLaps; i++) {
            for (int j = 1; j <= min(17, i); j++) {
                f[i] = min(f[i], f[i - j] + minSec[j]);
            }
            f[i] += changeTime;
        }
        return f[numLaps];
    }
};
```

```go [sol-Go]
func minimumFinishTime(tires [][]int, changeTime, numLaps int) int {
	minSec := [18]int{}
	for i := range minSec {
		minSec[i] = math.MaxInt / 2
	}
	for _, tire := range tires {
		f, r := tire[0], tire[1]
		for x, time, sum := 1, f, 0; time <= changeTime+f; x++ {
			sum += time
			minSec[x] = min(minSec[x], sum)
			time *= r
		}
	}

	f := make([]int, numLaps+1)
	f[0] = -changeTime
	for i := 1; i <= numLaps; i++ {
		f[i] = math.MaxInt
		for j := 1; j <= 17 && j <= i; j++ {
			f[i] = min(f[i], f[i-j]+minSec[j])
		}
		f[i] += changeTime
	}
	return f[numLaps]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+\textit{numLaps})k)$，其中 $n$ 是 $\textit{tires}$ 的长度，$k=17$。
- 空间复杂度：$\mathcal{O}(\textit{numLaps}+k)$。

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

本题来自 `七、其他线性 DP / §7.1 一维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.1 一维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
