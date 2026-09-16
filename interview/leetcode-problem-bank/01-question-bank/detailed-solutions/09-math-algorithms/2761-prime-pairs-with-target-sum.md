# 2761. 和等于目标值的质数对

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/prime-pairs-with-target-sum/
- 题目 slug：`prime-pairs-with-target-sum`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.2 预处理质数（筛质数）
- 难度分：1505
- 外部题解来源：https://leetcode.cn/problems/prime-pairs-with-target-sum/solutions/2327232/yu-chu-li-zhi-shu-mei-ju-by-endlesscheng-cq9b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理质数+枚举+特判奇数（Python/Java/C++/Go）](https://leetcode.cn/problems/prime-pairs-with-target-sum/solutions/2327232/yu-chu-li-zhi-shu-mei-ju-by-endlesscheng-cq9b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-zhi-shu-mei-ju-by-endlesscheng-cq9b`
- topic id：`2327232`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先可以预处理出 $10^6$ 内的所有质数，这个之前周赛出过，我讲了**埃氏筛**和**线性筛**两种做法，可以看 [【周赛 326】](https://www.bilibili.com/video/BV1H8411E7hn/)第四题。

[两种筛法的代码模板（Python/Java/C++/Go）](https://leetcode.cn/problems/closest-prime-numbers-in-range/solution/yu-chu-li-zhi-shu-mei-ju-by-endlesscheng-uw2b/)

然后就是暴力枚举质数 $x$ 和 $y=n-x$ 了，如果 $x\le y$ 且 $y$ 是质数，那么就把 $[x,y]$ 加入答案。

代码实现时，有一些优化之处：如果 $n$ 是奇数，由于只有奇数+偶数=奇数，而偶数中只有 $2$ 是质数，所以如果 $n$ 是奇数时，至多只有一个质数对 $(2,n-2)$。

注：把 $n$ 分解成两个质数的方案有多少呢？请看 [OEIS A061358](https://oeis.org/A061358/graph)。

```py [sol-Python3]
MX = 10 ** 6 + 1
primes = []
is_prime = [True] * MX
for i in range(2, MX):
    if is_prime[i]:
        primes.append(i)
        for j in range(i * i, MX, i):
            is_prime[j] = False

class Solution:
    def findPrimePairs(self, n: int) -> List[List[int]]:
        if n % 2:
            return [[2, n - 2]] if n > 4 and is_prime[n - 2] else []
        ans = []
        for x in primes:
            y = n - x
            if y < x:
                break
            if is_prime[y]:
                ans.append([x, y])
        return ans
```

```java [sol-Java]
class Solution {
    private final static int MX = (int) 1e6;
    private final static int[] primes = new int[78498];
    private final static boolean[] np = new boolean[MX + 1];

    static {
        var pi = 0;
        for (var i = 2; i <= MX; ++i) {
            if (!np[i]) {
                primes[pi++] = i;
                for (var j = i; j <= MX / i; ++j) // 避免溢出的写法
                    np[i * j] = true;
            }
        }
    }

    public List<List<Integer>> findPrimePairs(int n) {
        if (n % 2 > 0)
            return n > 4 && !np[n - 2] ? List.of(List.of(2, n - 2)) : List.of();
        var ans = new ArrayList<List<Integer>>();
        for (int x : primes) {
            int y = n - x;
            if (y < x) break;
            if (!np[y]) ans.add(List.of(x, y));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
const int MX = 1e6;
vector<int> primes;
bool np[MX + 1];

int init = []() {
    for (int i = 2; i <= MX; i++) {
        if (!np[i]) {
            primes.push_back(i);
            for (int j = i; j <= MX / i; j++) // 避免溢出的写法
                np[i * j] = true;
        }
    }
    return 0;
}();

class Solution {
public:
    vector<vector<int>> findPrimePairs(int n) {
        vector<vector<int>> ans;
        if (n % 2) {
            if (n > 4 && !np[n - 2])
                ans.push_back({2, n - 2});
            return ans;
        }
        for (int x: primes) {
            int y = n - x;
            if (y < x) break;
            if (!np[y]) ans.push_back({x, y});
        }
        return ans;
    }
};
```

```go [sol-Go]
const mx = 1e6
var primes []int
var isP = [mx + 1]bool{}

func init() {
	for i := 2; i <= mx; i++ {
		isP[i] = true
	}
	for i := 2; i <= mx; i++ {
		if isP[i] {
			primes = append(primes, i)
			for j := i * i; j <= mx; j += i {
				isP[j] = false
			}
		}
	}
}

func findPrimePairs(n int) (ans [][]int) {
	if n%2 > 0 {
		if n > 4 && isP[n-2] {
			return [][]int{{2, n - 2}}
		}
		return
	}
	for _, x := range primes {
		y := n - x
		if y < x {
			break
		}
		if isP[y] {
			ans = append(ans, []int{x, y})
		}
	}
	return
}
```

#### 复杂度分析

这里忽略预处理质数的时间和空间。

- 时间复杂度：$\mathcal{O}\left(\dfrac{n}{\log n}\right)$。$n$ 以内有 $\mathcal{O}\left(\dfrac{n}{\log n}\right)$ 个质数。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

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
