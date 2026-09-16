# 2594. 修车的最少时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-time-to-repair-cars/
- 题目 slug：`minimum-time-to-repair-cars`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.1 求最小 / 答疑
- 难度分：1915
- 外部题解来源：https://leetcode.cn/problems/minimum-time-to-repair-cars/solutions/2177199/er-fen-da-an-pythonjavacgo-by-endlessche-keqf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案+优化（附题单！）Python/Java/C++/Go/JS](https://leetcode.cn/problems/minimum-time-to-repair-cars/solutions/2177199/er-fen-da-an-pythonjavacgo-by-endlessche-keqf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-pythonjavacgo-by-endlessche-keqf`
- topic id：`2177199`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

- 关于二分的基础知识，请看[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。
- 本题讲解请看[ 双周赛 100 ](https://www.bilibili.com/video/BV1WM411H7UE/) 第四题。

## 思路

请注意，能力值越低，修车越快。（按照英文描述，$\textit{ranks}_i$ 应该翻译成「排名」，排名越靠前，修车越快。）

如果已知修车的时间 $t$，我们可以算出每个人在 $t$ 分钟内能修好多少辆车。例如一个能力值 $r=3$ 的人可以在 $t=16$ 分钟内修好 $2$ 辆车，但无法修好 $3$ 辆车。

根据题意，需要满足 

$$
rn^2 \le t
$$

解得

$$
n\le \sqrt\dfrac{t}{r}
$$

所以能力值为 $r$ 的机械工最多可以修

$$
\left\lfloor\sqrt\dfrac{t}{r}\right\rfloor
$$

辆车。

累加每个人在 $t$ 分钟内能修的车的个数，如果

$$
\sum_{i=0}^{i=n-1}\left\lfloor\sqrt\dfrac{t}{\textit{ranks}[i]}\right\rfloor \ge \textit{cars}
$$

则说明可以在 $t$ 分钟修完所有车。

上式表明，$t$ 越大，能修的车越多，越能满足要求。有了这样的**单调性**，我们就可以二分答案了。

二分上界为 $\min(\textit{ranks}) \cdot \textit{cars}^2$，即让能力值最低（修车最快）的人修好所有车所需要的时间。

#### 答疑

**问**：开方直接取整的做法是否会有精度误差？

**答**：代码中对整数开方，只要整数转浮点没有丢失精度（在 $2^{53}-1$ 内），开方出来的整数部分就是正确的。具体可以参考 IEEE 754。

```py [sol-Python3]
class Solution:
    def repairCars(self, ranks: List[int], cars: int) -> int:
        left = 0
        right = min(ranks) * cars * cars
        while left + 1 < right:  # 开区间
            mid = (left + right) // 2
            if sum(isqrt(mid // r) for r in ranks) >= cars:
                right = mid  # 满足要求
            else:
                left = mid
        return right  # 最小的满足要求的值
```

```py [sol-Python3 库函数]
class Solution:
    def repairCars(self, ranks: List[int], cars: int) -> int:
        s = lambda t: sum(isqrt(t // r) for r in ranks)
        return bisect_left(range(min(ranks) * cars * cars), cars, key=s)
```

```java [sol-Java]
class Solution {
    public long repairCars(int[] ranks, int cars) {
        int minR = ranks[0];
        for (int r : ranks) {
            minR = Math.min(minR, r);
        }
        long left = 0;
        long right = (long) minR * cars * cars;
        while (left + 1 < right) { // 开区间
            long mid = (left + right) >> 1;
            long s = 0;
            for (int r : ranks) {
                s += Math.sqrt(mid / r);
            }
            if (s >= cars) {
                right = mid; // 满足要求
            } else { 
                left = mid;
            }
        }
        return right; // 最小的满足要求的值
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long repairCars(vector<int> &ranks, int cars) {
        int min_r = *min_element(ranks.begin(), ranks.end());
        long long left = 0, right = 1LL * min_r * cars * cars;
        while (left + 1 < right) { // 开区间
            long long mid = (left + right) / 2, s = 0;
            for (int r : ranks)
                s += sqrt(mid / r);
            (s >= cars ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func repairCars(ranks []int, cars int) int64 {
	minR := ranks[0]
	for _, r := range ranks {
		if r < minR {
			minR = r
		}
	}
	left := 0
	right := minR * cars * cars
	for left+1 < right { // 开区间
		mid := (left + right) / 2
		s := 0
		for _, r := range ranks {
			s += int(math.Sqrt(float64(mid / r)))
		}
		if s >= cars {
			right = mid // 满足要求
		} else {
			left = mid
		}
	}
	return int64(right) // 最小的满足要求的值
}
```

```go [sol-Go 库函数]
func repairCars(ranks []int, cars int) int64 {
	minR := ranks[0]
	for _, r := range ranks {
		if r < minR {
			minR = r
		}
	}
	return int64(sort.Search(minR*cars*cars, func(t int) bool {
		s := 0
		for _, r := range ranks {
			s += int(math.Sqrt(float64(t / r)))
		}
		return s >= cars
	}))
}
```

```js [sol-JavaScript]
var repairCars = function (ranks, cars) {
    let left = 0;
    let right = Math.min(...ranks) * cars * cars;
    while (left + 1 < right) { // 开区间
        const mid = Math.floor((left + right) / 2);
        let s = 0;
        for (const r of ranks) {
            s += Math.floor(Math.sqrt(mid / r));
        }
        if (s >= cars) {
            right = mid; // 满足要求
        } else {
            left = mid;
        }
    }
    return right; // 最小的满足要求的值
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log(mc^2))$，其中 $n$ 为 $\textit{ranks}$ 的长度，$m=\min(\textit{ranks})$，$c=\textit{cars}$。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 优化

能力值相同的人，在 $t$ 分钟内修好的车的个数是一样的。

根据数据范围，$\textit{ranks}$ 中至多有 $100$ 个不同的数字，我们可以统计 $\textit{ranks}$ 中每个数字的出现次数，这样每次二分至多循环 $100$ 次。

此外，如果循环中发现 $s\ge \textit{cars}$，可以提前退出循环。

```py [sol-Python3]
class Solution:
    def repairCars(self, ranks: List[int], cars: int) -> int:
        cnt = Counter(ranks)
        left = 0
        right = min(cnt) * cars * cars
        while left + 1 < right:
            mid = (left + right) // 2
            if sum(isqrt(mid // r) * c for r, c in cnt.items()) >= cars:
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def repairCars(self, ranks: List[int], cars: int) -> int:
        cnt = Counter(ranks)
        s = lambda t: sum(isqrt(t // r) * c for r, c in cnt.items())
        return bisect_left(range(min(cnt) * cars * cars), cars, key=s)
```

```java [sol-Java]
class Solution {
    public long repairCars(int[] ranks, int cars) {
        int[] cnt = new int[101]; // 数组比哈希表更快
        int minR = ranks[0];
        for (int r : ranks) {
            cnt[r]++;
            minR = Math.min(minR, r);
        }
        long left = 0;
        long right = (long) minR * cars * cars;
        while (left + 1 < right) {
            long mid = (left + right) >> 1;
            long s = 0;
            for (int r = minR; r <= 100 && s < cars; r++) { // 至多循环 100 次
                s += (long) Math.sqrt(mid / r) * cnt[r];
            }
            if (s >= cars) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long repairCars(vector<int> &ranks, int cars) {
        int min_r = ranks[0], cnt[101]{}; // 数组比哈希表更快
        for (int r : ranks) {
            min_r = min(min_r, r);
            cnt[r]++;
        }
        long long left = 0, right = 1LL * min_r * cars * cars;
        while (left + 1 < right) {
            long long mid = (left + right) / 2, s = 0;
            for (int r = min_r; r <= 100 && s < cars; r++) // 至多循环 100 次
                s += (long long) sqrt(mid / r) * cnt[r];
            (s >= cars ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func repairCars(ranks []int, cars int) int64 {
	cnt := [101]int{} // 数组比哈希表更快
	minR := ranks[0]
	for _, r := range ranks {
		cnt[r]++
		if r < minR {
			minR = r
		}
	}
	left := 0
	right := minR * cars * cars
	for left+1 < right {
		mid := (left + right) / 2
		s := 0
		for r := minR; r <= 100 && s < cars; r++ { // 至多循环 100 次
			s += int(math.Sqrt(float64(mid/r))) * cnt[r]
		}
		if s >= cars {
			right = mid
		} else {
			left = mid
		}
	}
	return int64(right)
}
```

```go [sol-Go 库函数]
func repairCars(ranks []int, cars int) int64 {
	cnt := [101]int{} // 数组比哈希表更快
	minR := ranks[0]
	for _, r := range ranks {
		cnt[r]++
		if r < minR {
			minR = r
		}
	}
	return int64(sort.Search(minR*cars*cars, func(t int) bool {
		s := 0
		for r := minR; r <= 100 && s < cars; r++ {
			s += int(math.Sqrt(float64(t/r))) * cnt[r]
		}
		return s >= cars
	}))
}
```

```js [sol-JavaScript]
var repairCars = function (ranks, cars) {
    const cnt = new Array(101).fill(0); // 数组比哈希表更快
    for (const r of ranks) {
        cnt[r]++;
    }
    const minR = Math.min(...ranks);
    let left = 0;
    let right = minR * cars * cars;
    while (left + 1 < right) {
        const mid = Math.floor((left + right) / 2);
        let s = 0;
        for (let r = minR; r <= 100 && s < cars; r++) { // 至多循环 100 次
            s += Math.floor(Math.sqrt(mid / r)) * cnt[r];
        }
        if (s >= cars) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + M\log(mc^2))$，其中 $n$ 为 $\textit{ranks}$ 的长度，$M=\max(\textit{ranks})$，$m=\min(\textit{ranks})$，$c=\textit{cars}$。
- 空间复杂度：$\mathcal{O}(M)$。

## 题单

#### 二分答案（按照难度分排序）
- [875. 爱吃香蕉的珂珂](https://leetcode.cn/problems/koko-eating-bananas/)
- [1283. 使结果不超过阈值的最小除数](https://leetcode.cn/problems/find-the-smallest-divisor-given-a-threshold/)
- [2187. 完成旅途的最少时间](https://leetcode.cn/problems/minimum-time-to-complete-trips/)
- [2226. 每个小孩最多能分到多少糖果](https://leetcode.cn/problems/maximum-candies-allocated-to-k-children/)
- [1870. 准时到达的列车最小时速](https://leetcode.cn/problems/minimum-speed-to-arrive-on-time/)
- [1011. 在 D 天内送达包裹的能力](https://leetcode.cn/problems/capacity-to-ship-packages-within-d-days/)
- [2064. 分配给商店的最多商品的最小值](https://leetcode.cn/problems/minimized-maximum-of-products-distributed-to-any-store/)
- [1760. 袋子里最少数目的球](https://leetcode.cn/problems/minimum-limit-of-balls-in-a-bag/)
- [1482. 制作 m 束花所需的最少天数](https://leetcode.cn/problems/minimum-number-of-days-to-make-m-bouquets/)
- [1642. 可以到达的最远建筑](https://leetcode.cn/problems/furthest-building-you-can-reach/)
- [1898. 可移除字符的最大数目](https://leetcode.cn/problems/maximum-number-of-removable-characters/)
- [778. 水位上升的泳池中游泳](https://leetcode.cn/problems/swim-in-rising-water/)
- [2258. 逃离火灾](https://leetcode.cn/problems/escape-the-spreading-fire/)

#### 最小化最大值
- [2439. 最小化数组中的最大值](https://leetcode.cn/problems/minimize-maximum-of-array/)
- [2513. 最小化两个数组中的最大值](https://leetcode.cn/problems/minimize-the-maximum-of-two-arrays/)
- [2560. 打家劫舍 IV](https://leetcode.cn/problems/house-robber-iv/)
- [2616. 最小化数对的最大差值](https://leetcode.cn/problems/minimize-the-maximum-difference-of-pairs/)

#### 最大化最小值
- [1552. 两球之间的磁力](https://leetcode.cn/problems/magnetic-force-between-two-balls/)
- [2517. 礼盒的最大甜蜜度](https://leetcode.cn/problems/maximum-tastiness-of-candy-basket/)
- [2528. 最大化城市的最小供电站数目](https://leetcode.cn/problems/maximize-the-minimum-powered-city/)

#### 第 K 小/大（部分题目还可以用堆解决）
- [373. 查找和最小的 K 对数字](https://leetcode.cn/problems/find-k-pairs-with-smallest-sums/)
- [378. 有序矩阵中第 K 小的元素](https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/)
- [719. 找出第 K 小的数对距离](https://leetcode.cn/problems/find-k-th-smallest-pair-distance/)
- [786. 第 K 个最小的素数分数](https://leetcode.cn/problems/k-th-smallest-prime-fraction/)
- [1439. 有序矩阵中的第 k 个最小数组和](https://leetcode.cn/problems/find-the-kth-smallest-sum-of-a-matrix-with-sorted-rows/)
- [2040. 两个有序数组的第 K 小乘积](https://leetcode.cn/problems/kth-smallest-product-of-two-sorted-arrays/)
- [2386. 找出数组的第 K 大和](https://leetcode.cn/problems/find-the-k-sum-of-an-array/)

[我的其它题解（按 tag 分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

#### 思考题

如果 $\textit{ranks}[i]=i+1$，是否存在某种数学做法？

## 本地原创解析

### 1. 题意重述

本题来自 `二、二分答案 / §2.1 求最小 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.1 求最小 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
