# 2523. 范围内最接近的两个质数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/closest-prime-numbers-in-range/
- 题目 slug：`closest-prime-numbers-in-range`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.2 预处理质数（筛质数）
- 难度分：1650
- 外部题解来源：https://leetcode.cn/problems/closest-prime-numbers-in-range/solutions/2040087/yu-chu-li-zhi-shu-mei-ju-by-endlesscheng-uw2b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理质数 + 枚举（Python/Java/C++/Go）](https://leetcode.cn/problems/closest-prime-numbers-in-range/solutions/2040087/yu-chu-li-zhi-shu-mei-ju-by-endlesscheng-uw2b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-zhi-shu-mei-ju-by-endlesscheng-uw2b`
- topic id：`2040087`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

分两步：

1. 筛质数，做法见 [204. 计数质数](https://leetcode.cn/problems/count-primes/)。
2. 找 $[\textit{left},\textit{right}]$ 范围内的最小质数间隙（prime gap），暴力枚举范围内的所有相邻质数即可。

附：[视频讲解](https://www.bilibili.com/video/BV1H8411E7hn)

#### 优化

1. 找范围内的第一个质数可以用二分查找。
2. 可以往质数表末尾额外插入 $2$ 个 $10^6+1$，这样无需判断下标是否越界。

```py [sol1-Python3]
MX = 10 ** 6 + 1
primes = []
is_prime = [True] * MX
for i in range(2, MX):
    if is_prime[i]:
        primes.append(i)
        for j in range(i * i, MX, i):
            is_prime[j] = False
primes.extend((MX, MX))  # 保证下面下标不会越界

class Solution:
    def closestPrimes(self, left: int, right: int) -> List[int]:
        p = q = -1
        i = bisect_left(primes, left)
        while primes[i + 1] <= right:
            if p < 0 or primes[i + 1] - primes[i] < q - p:
                p, q = primes[i], primes[i + 1]
            i += 1
        return [p, q]
```

```java [sol1-Java]
class Solution {
    private final static int MX = (int) 1e6;
    private final static int[] primes = new int[78500];

    static {
        var np = new boolean[MX + 1];
        var pi = 0;
        for (var i = 2; i <= MX; ++i)
            if (!np[i]) {
                primes[pi++] = i;
                for (var j = i; j <= MX / i; ++j) // 避免溢出的写法
                    np[i * j] = true;
            }
        primes[pi++] = MX + 1;
        primes[pi++] = MX + 1; // 保证下面下标不会越界
    }

    public int[] closestPrimes(int left, int right) {
        int p = -1, q = -1;
        for (var i = lowerBound(primes, left); primes[i + 1] <= right; ++i)
            if (p < 0 || primes[i + 1] - primes[i] < q - p) {
                p = primes[i];
                q = primes[i + 1];
            }
        return new int[]{p, q};
    }

    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, int target) {
        int left = -1, right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target)
                left = mid; // 范围缩小到 (mid, right)
            else
                right = mid; // 范围缩小到 (left, mid)
        }
        return right;
    }
}
```

```cpp [sol1-C++]
const int MX = 1e6;
vector<int> primes;

int init = []() {
    bool np[MX + 1]{};
    for (int i = 2; i <= MX; ++i)
        if (!np[i]) {
            primes.push_back(i);
            for (int j = i; j <= MX / i; ++j) // 避免溢出的写法
                np[i * j] = true;
        }
    primes.push_back(MX + 1);
    primes.push_back(MX + 1); // 保证下面下标不会越界
    return 0;
}();

class Solution {
public:
    vector<int> closestPrimes(int left, int right) {
        int p = -1, q = -1;
        int i = lower_bound(primes.begin(), primes.end(), left) - primes.begin();
        for (; primes[i + 1] <= right; ++i)
            if (p < 0 || primes[i + 1] - primes[i] < q - p) {
                p = primes[i];
                q = primes[i + 1];
            }
        return {p, q};
    }
};
```

```go [sol1-Go]
const mx = 1e6 + 1
var primes = make([]int, 0, 78500)

func init() {
	np := [mx]bool{}
	for i := 2; i < mx; i++ {
		if !np[i] {
			primes = append(primes, i)
			for j := i * i; j < mx; j += i {
				np[j] = true
			}
		}
	}
	primes = append(primes, mx, mx) // 保证下面下标不会越界
}

func closestPrimes(left, right int) []int {
	p, q := -1, -1
	for i := sort.SearchInts(primes, left); primes[i+1] <= right; i++ {
		if p < 0 || primes[i+1]-primes[i] < q-p {
			p, q = primes[i], primes[i+1]
		}
	}
	return []int{p, q}
}
```

也可以用线性筛（欧拉筛）做，具体原理见 [视频讲解](https://www.bilibili.com/video/BV1H8411E7hn)。

```py [sol2-Python3]
MX = 10 ** 6 + 1
primes = []
is_prime = [True] * MX
for i in range(2, MX):
    if is_prime[i]:
        primes.append(i)
    for p in primes:
        if i * p >= MX: break
        is_prime[i * p] = False
        if i % p == 0: break
primes.extend((MX, MX))  # 保证下面下标不会越界

class Solution:
    def closestPrimes(self, left: int, right: int) -> List[int]:
        p = q = -1
        i = bisect_left(primes, left)
        while primes[i + 1] <= right:
            if p < 0 or primes[i + 1] - primes[i] < q - p:
                p, q = primes[i], primes[i + 1]
            i += 1
        return [p, q]
```

```java [sol2-Java]
class Solution {
    private final static int MX = (int) 1e6;
    private final static int[] primes = new int[78500];

    static {
        var np = new boolean[MX + 1];
        var pi = 0;
        for (var i = 2; i <= MX; ++i) {
            if (!np[i]) primes[pi++] = i;
            for (var j = 0; j < pi; ++j) {
                var p = primes[j];
                if (i * p > MX) break;
                np[i * p] = true;
                if (i % p == 0) break;
            }
        }
        primes[pi++] = MX + 1;
        primes[pi++] = MX + 1; // 保证下面下标不会越界
    }

    public int[] closestPrimes(int left, int right) {
        int p = -1, q = -1;
        for (var i = lowerBound(primes, left); primes[i + 1] <= right; ++i)
            if (p < 0 || primes[i + 1] - primes[i] < q - p) {
                p = primes[i];
                q = primes[i + 1];
            }
        return new int[]{p, q};
    }

    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, int target) {
        int left = -1, right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target)
                left = mid; // 范围缩小到 (mid, right)
            else
                right = mid; // 范围缩小到 (left, mid)
        }
        return right;
    }
}
```

```cpp [sol2-C++]
const int MX = 1e6;
vector<int> primes;

int init = []() {
    bool np[MX + 1]{};
    for (int i = 2; i <= MX; ++i) {
        if (!np[i]) primes.push_back(i);
        for (int p: primes) {
            if (i * p > MX) break;
            np[i * p] = true;
            if (i % p == 0) break;
        }
    }
    primes.push_back(MX + 1);
    primes.push_back(MX + 1); // 保证下面下标不会越界
    return 0;
}();

class Solution {
public:
    vector<int> closestPrimes(int left, int right) {
        int p = -1, q = -1;
        int i = lower_bound(primes.begin(), primes.end(), left) - primes.begin();
        for (; primes[i + 1] <= right; ++i)
            if (p < 0 || primes[i + 1] - primes[i] < q - p) {
                p = primes[i];
                q = primes[i + 1];
            }
        return {p, q};
    }
};
```

```go [sol2-Go]
const mx = 1e6 + 1
var primes = make([]int, 0, 78500)

func init() {
	np := [mx]bool{}
	for i := 2; i < mx; i++ {
		if !np[i] {
			primes = append(primes, i)
		}
		for _, p := range primes {
			if i*p >= mx {
				break
			}
			np[i*p] = true
			if i%p == 0 {
				break
			}
		}
	}
	primes = append(primes, mx, mx) // 保证下面下标不会越界
}

func closestPrimes(left, right int) []int {
	p, q := -1, -1
	for i := sort.SearchInts(primes, left); primes[i+1] <= right; i++ {
		if p < 0 || primes[i+1]-primes[i] < q-p {
			p, q = primes[i], primes[i+1]
		}
	}
	return []int{p, q}
}
```


#### 复杂度分析

- 时间复杂度：$O(\textit{right})$，忽略预处理的时间复杂度。严谨地说，由于范围内有 $O\left(\dfrac{\textit{right}}{\log\textit{right}}-\dfrac{\textit{left}}{\log\textit{left}}\right)$ 个质数（根据质数密度），所以遍历的时间复杂度为 $O\left(\dfrac{\textit{right}}{\log\textit{right}}-\dfrac{\textit{left}}{\log\textit{left}}\right)$，再算上二分质数的时间 $O(\log\pi(U))$（$\pi(U)$ 表示 $U=10^6$ 内的质数个数，这有 $78498$ 个），总的时间复杂度为 $O\left(\log\pi(U) + \dfrac{\textit{right}}{\log\textit{right}}-\dfrac{\textit{left}}{\log\textit{left}}\right)$。
- 空间复杂度：$O(1)$，忽略预处理的空间复杂度。仅用到若干变量。

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.2 预处理质数（筛质数）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.2 预处理质数（筛质数）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
