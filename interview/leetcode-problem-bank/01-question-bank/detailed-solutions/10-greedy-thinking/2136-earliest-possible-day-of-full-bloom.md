# 2136. 全部开花的最早一天

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/earliest-possible-day-of-full-bloom/
- 题目 slug：`earliest-possible-day-of-full-bloom`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.7 交换论证法
- 难度分：2033
- 外部题解来源：https://leetcode.cn/problems/earliest-possible-day-of-full-bloom/solutions/1200254/tan-xin-ji-qi-zheng-ming-by-endlesscheng-hfwe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心及其证明（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/earliest-possible-day-of-full-bloom/solutions/1200254/tan-xin-ji-qi-zheng-ming-by-endlesscheng-hfwe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-ji-qi-zheng-ming-by-endlesscheng-hfwe`
- topic id：`1200254`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

## 需要交替播种吗？

假设有两枚种子要播种，$1$ 号种子需要 $3$ 天，$2$ 号种子需要 $2$ 天。

- 如果播种顺序为 $11122$，即先播种完 $1$ 号种子，再播种 $2$ 号种子，那么 $1$ 号种子在第 $3$ 天播种完毕，$2$ 号种子在第 $5$ 天播种完毕，完成播种共需 $5$ 天。
- 如果交替播种，比如 $12112$，那么 $2$ 号种子完成播种的时间是不变的，但对于 $1$ 号种子来说，完成播种的时间却延后了一天。

这说明如果要交替播种，至少有一枚种子的完成时间要延后，并且所有种子完成播种的时间是不变的。也就是说，交替播种不仅没有得到任何好处，反而会拖慢其中一些种子的播种进度。

所以不应交替播种，应当种完一枚种子再开始另一枚。

## 谁先播种？

对于两枚种子，设其播种所需天数为 $p_1$ 和 $p_2$，生长所需天数为 $g_1$ 和 $g_2$。

不妨设 $g_1\ge g_2$。我们来比较哪种播种顺序更优：

- 先 $1$ 后 $2$ 时的最晚开花时间：
    $$
    \max(p_1+g_1,p_1+p_2+g_2)
    $$
- 先 $2$ 后 $1$ 时的最晚开花时间：
    $$
    \max(p_1+p_2+g_1,p_2+g_2)
    $$
    由于 $g_1\ge g_2$ 且 $p_1>0$，所以 $p_1+p_2+g_1>p_2+g_2$，上式可化简为 
    $$
    p_1+p_2+g_1
    $$

由于 $p_1+g_1 < p_1+p_2+g_1$ 且 $p_1+p_2+g_2 \le p_1+p_2+g_1$，因此

$$
\max(p_1+g_1,p_1+p_2+g_2) \le p_1+p_2+g_1 = \max(p_1+p_2+g_1,p_2+g_2)
$$

上式表明，按照先 $1$ 后 $2$ 的顺序播种，最晚开花时间不会晚于按照先 $2$ 后 $1$ 播种时的最晚开花时间。

这意味着**按照生长天数从大到小排序**后，交换任意两枚种子的播种顺序，不会让最晚开花时间提前。

假设存在其它更优的种子排列，我们可以交换「生长天数小且排在前面的种子」与「生长天数大且排在后面的种子」，从而得到更早的最晚开花时间，因此假设不成立，按照生长天数从大到小的顺序播种是最优的。

对于两枚生长天数相同的种子，由于无论按照何种顺序播种，这两枚种子的最晚开花时间都是相同的，因此无需考虑生长天数相同的种子的播种顺序，在排序时，仅需按生长天数从大到小排序。

```py [sol-Python3]
class Solution:
    def earliestFullBloom(self, plantTime: List[int], growTime: List[int]) -> int:
        ans = days = 0
        for p, g in sorted(zip(plantTime, growTime), key=lambda z: -z[1]):
            days += p  # 累加播种天数
            ans = max(ans, days + g)  # 再加上生长天数，就是这个种子的开花时间
        return ans
```

```java [sol-Java]
class Solution {
    public int earliestFullBloom(int[] plantTime, int[] growTime) {
        int n = plantTime.length;
        var id = new Integer[n];
        Arrays.setAll(id, i -> i);
        Arrays.sort(id, (i, j) -> growTime[j] - growTime[i]);
        int ans = 0, days = 0;
        for (int i : id) {
            days += plantTime[i]; // 累加播种天数
            ans = Math.max(ans, days + growTime[i]); // 再加上生长天数，就是这个种子的开花时间
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int earliestFullBloom(vector<int> &plantTime, vector<int> &growTime) {
        vector<int> id(plantTime.size());
        iota(id.begin(), id.end(), 0); // id[i] = i
        sort(id.begin(), id.end(), [&](int i, int j) { return growTime[i] > growTime[j]; });
        int ans = 0, days = 0;
        for (int i : id) {
            days += plantTime[i]; // 累加播种天数
            ans = max(ans, days + growTime[i]); // 再加上生长天数，就是这个种子的开花时间
        }
        return ans;
    }
};
```

```go [sol-Go]
func earliestFullBloom(plantTime, growTime []int) (ans int) {
	type pair struct{ p, g int }
	a := make([]pair, len(plantTime))
	for i, p := range plantTime {
		a[i] = pair{p, growTime[i]}
	}
	sort.Slice(a, func(i, j int) bool { return a[i].g > a[j].g })
	days := 0
	for _, p := range a {
		days += p.p // 累加播种天数
		ans = max(ans, days+p.g) // 再加上生长天数，就是这个种子的开花时间
	}
	return
}

func max(a, b int) int { if b > a { return b }; return a }
```

```js [sol-JavaScript]
var earliestFullBloom = function (plantTime, growTime) {
    let ans = 0, days = 0;
    for (const [p, g] of _.zip(plantTime, growTime).sort((a, b) => b[1] - a[1])) {
        days += p; // 累加播种天数
        ans = Math.max(ans, days + g); // 再加上生长天数，就是这个种子的开花时间
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn earliest_full_bloom(plant_time: Vec<i32>, grow_time: Vec<i32>) -> i32 {
        let mut id: Vec<usize> = (0..grow_time.len()).collect();
        id.sort_unstable_by(|&i, &j| grow_time[j].cmp(&grow_time[i]));
        let mut ans = 0;
        let mut days = 0;
        for &i in &id {
            days += plant_time[i]; // 累加播种天数
            ans = ans.max(days + grow_time[i]); // 再加上生长天数，就是这个种子的开花时间
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{plantTime}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [1665. 完成所有任务的最少初始能量](https://leetcode.cn/problems/minimum-initial-energy-to-finish-tasks/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.7 交换论证法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.7 交换论证法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
