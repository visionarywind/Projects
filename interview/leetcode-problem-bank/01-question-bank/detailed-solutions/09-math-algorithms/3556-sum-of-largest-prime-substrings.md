# 3556. 最大质数子字符串之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-largest-prime-substrings/
- 题目 slug：`sum-of-largest-prime-substrings`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.1 判断质数
- 难度分：1440
- 外部题解来源：https://leetcode.cn/problems/sum-of-largest-prime-substrings/solutions/3685356/pan-duan-zhi-shu-pythonjavacgo-by-endles-0w8f/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[判断质数 + 预处理质数加速判断（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-largest-prime-substrings/solutions/3685356/pan-duan-zhi-shu-pythonjavacgo-by-endles-0w8f/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pan-duan-zhi-shu-pythonjavacgo-by-endles-0w8f`
- topic id：`3685356`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识**：[如何判断质数](https://leetcode.cn/problems/prime-in-diagonal/solutions/2216347/pan-duan-zhi-shu-by-endlesscheng-m6nt/)。

本题可以枚举所有 $\dfrac{n(n+1)}{2}$ 个子串，判断子串对应的十进制数是否为质数。

把质数加到一个列表中，排序去重后，计算前三大之和，即为答案。

也可以用哈希表或者有序集合去重。

注意 $1$ 不是质数。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1cqjgzdEPP/)，欢迎点赞关注~

## 写法一

```py [sol-Python3]
def is_prime(n: int) -> bool:
    for i in range(2, isqrt(n) + 1):
        if n % i == 0:
            return False
    return n >= 2

class Solution:
    def sumOfLargestPrimes(self, s: str) -> int:
        primes = set()
        for i in range(len(s)):
            x = 0
            for ch in s[i:]:
                x = x * 10 + int(ch)
                if is_prime(x):
                    primes.add(x)
        return sum(sorted(primes)[-3:])
```

```java [sol-Java]
class Solution {
    private boolean isPrime(long n) {
        for (long i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        return n >= 2;
    }

    public long sumOfLargestPrimes(String S) {
        char[] s = S.toCharArray();
        TreeSet<Long> set = new TreeSet<>();
        for (int i = 0; i < s.length; i++) {
            long x = 0;
            for (int j = i; j < s.length; j++) {
                x = x * 10 + (s[j] - '0');
                if (isPrime(x)) {
                    set.add(x);
                }
            }
        }

        return set.descendingSet()
                  .stream()
                  .limit(3)
                  .mapToLong(Long::longValue)
                  .sum();
    }
}
```

```cpp [sol-C++]
class Solution {
    bool is_prime(long long n) {
        for (long long i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        return n >= 2;
    }

public:
    long long sumOfLargestPrimes(string s) {
        vector<long long> primes;
        for (int i = 0; i < s.size(); i++) {
            long long x = 0;
            for (int j = i; j < s.size(); j++) {
                x = x * 10 + (s[j] - '0');
                if (is_prime(x)) {
                    primes.push_back(x);
                }
            }
        }

        // 排序，去重，计算前三大之和
        ranges::sort(primes, greater());
        int n = ranges::unique(primes).begin() - primes.begin();
        return reduce(primes.begin(), primes.begin() + min(n, 3), 0LL);
    }
};
```

```go [sol-Go]
func isPrime(n int) bool {
	for i := 2; i*i <= n; i++ {
		if n%i == 0 {
			return false
		}
	}
	return n >= 2
}

func sumOfLargestPrimes(s string) (ans int64) {
	primes := []int{}
	n := len(s)
	for i := range n {
		x := 0
		for _, b := range s[i:] {
			x = x*10 + int(b-'0')
			if isPrime(x) {
				primes = append(primes, x)
			}
		}
	}

	slices.Sort(primes)
	primes = slices.Compact(primes) // 去重

	for _, p := range primes[max(len(primes)-3, 0):] {
		ans += int64(p)
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(10^{n/2})$，其中 $n$ 是 $s$ 的长度。乍一看，时间复杂度是 $\mathcal{O}(n^2\cdot 10^{n/2})$，但实际上有 $n$ 个长为 $1$ 的子串，$n-1$ 个长为 $2$ 的子串，……，$1$ 个长为 $n$ 的子串。每个长为 $k$ 的子串需要 $\mathcal{O}(10^{k/2})$ 的时间判断质数，累加得
   $$  
   S = n\cdot 10^{1/2} + (n-1)\cdot 10^{2/2} + \cdots + 1\cdot 10^{n/2}
   $$
   用**错位相减法**可以求出 $S=\mathcal{O}(10^{n/2})$。详细推导过程请看 [视频讲解](https://www.bilibili.com/video/BV1cqjgzdEPP/)。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 写法二

也可以用筛法预处理 $10^{n/2} \le 10^5$ 以内的质数。如果整数 $m$ 没有被 $\sqrt m$ 以内的质数整除，那么 $m$ 就是质数。

```py [sol-Python3]
MX = 100_001
is_p = [True] * MX
is_p[0] = is_p[1] = False
prime_numbers = []
for i in range(2, MX):
    if is_p[i]:
        prime_numbers.append(i)
        for j in range(i * i, MX, i):
            is_p[j] = False

def is_prime(n: int) -> bool:
     if n < MX:
         return is_p[n]
     for p in prime_numbers:
         if p * p > n:
             break
         if n % p == 0:
             return False
     return True

class Solution:
    def sumOfLargestPrimes(self, s: str) -> int:
        primes = set()
        for i in range(len(s)):
            x = 0
            for ch in s[i:]:
                x = x * 10 + int(ch)
                if is_prime(x):
                    primes.add(x)
        return sum(sorted(primes)[-3:])
```

```java [sol-Java]
class Solution {
    private static final int MX = 100_000;
    private static final boolean[] np = new boolean[MX + 1];
    private static final List<Integer> primeNumbers = new ArrayList<>();
    private static boolean initialized = false;

    private void init() {
        if (initialized) {
            return;
        }
        initialized = true;

        np[0] = np[1] = true;
        for (int i = 2; i <= MX; i++) {
            if (!np[i]) {
                primeNumbers.add(i);
                for (int j = i; j <= MX / i; j++) { // 避免溢出的写法
                    np[i * j] = true;
                }
            }
        }
    }

    private boolean isPrime(long n) {
        if (n <= MX) {
            return !np[(int) n];
        }
        for (int p : primeNumbers) {
            if (p > n / p) {
                break;
            }
            if (n % p == 0) {
                return false;
            }
        }
        return true;
    }

    public long sumOfLargestPrimes(String S) {
        init();
        char[] s = S.toCharArray();
        TreeSet<Long> set = new TreeSet<>();
        for (int i = 0; i < s.length; i++) {
            long x = 0;
            for (int j = i; j < s.length; j++) {
                x = x * 10 + (s[j] - '0');
                if (isPrime(x)) {
                    set.add(x);
                }
            }
        }

        return set.descendingSet()
                .stream()
                .limit(3)
                .mapToLong(Long::longValue)
                .sum();
    }
}
```

```cpp [sol-C++]
const int MX = 100'000;
bool np[MX + 1];
vector<int> prime_numbers;

int init = []() {
    np[0] = np[1] = true;
    for (int i = 2; i <= MX; i++) {
        if (!np[i]) {
            prime_numbers.push_back(i);
            for (int j = i; j <= MX / i; j++) { // 避免溢出的写法
                np[i * j] = true;
            }
        }
    }
    return 0;
}();

class Solution {
    bool is_prime(long long n) {
        if (n <= MX) {
            return !np[n];
        }
        for (long long p : prime_numbers) {
            if (p * p > n) {
                break;
            }
            if (n % p == 0) {
                return false;
            }
        }
        return true;
    }

public:
    long long sumOfLargestPrimes(string s) {
        vector<long long> primes;
        for (int i = 0; i < s.size(); i++) {
            long long x = 0;
            for (int j = i; j < s.size(); j++) {
                x = x * 10 + (s[j] - '0');
                if (is_prime(x)) {
                    primes.push_back(x);
                }
            }
        }

        // 排序，去重，计算前三大之和
        ranges::sort(primes, greater());
        int n = ranges::unique(primes).begin() - primes.begin();
        return reduce(primes.begin(), primes.begin() + min(n, 3), 0LL);
    }
};
```

```go [sol-Go]
const mx = 100_000

var np = [mx + 1]bool{true, true}
var primeNumbers []int

func init() {
	for i := 2; i <= mx; i++ {
		if !np[i] {
			primeNumbers = append(primeNumbers, i)
			for j := i * i; j <= mx; j += i {
				np[j] = true
			}
		}
	}
}

func isPrime(n int) bool {
	if n <= mx {
		return !np[n]
	}
	for _, p := range primeNumbers {
		if p*p > n {
			break
		}
		if n%p == 0 {
			return false
		}
	}
	return true
}

func sumOfLargestPrimes(s string) (ans int64) {
	primes := []int{}
	n := len(s)
	for i := range n {
		x := 0
		for _, b := range s[i:] {
			x = x*10 + int(b-'0')
			if isPrime(x) {
				primes = append(primes, x)
			}
		}
	}

	slices.Sort(primes)
	primes = slices.Compact(primes) // 去重

	for _, p := range primes[max(len(primes)-3, 0):] {
		ans += int64(p)
	}
	return
}
```

#### 复杂度分析

忽略预处理的时间和空间。

- 时间复杂度：$\mathcal{O}\left(\dfrac{10^{n/2}}{n}\right)$，其中 $n$ 是 $s$ 的长度。根据质数密度，枚举量除以 $\log 10^{n/2} = n/2 \cdot \log 10 = \mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

更多相似题目，见下面数学题单的「**§1.1 判断质数**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.1 判断质数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.1 判断质数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
