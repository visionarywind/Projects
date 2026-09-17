# 2081. k 镜像数字的和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-k-mirror-numbers/
- 题目 slug：`sum-of-k-mirror-numbers`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.1 回文数
- 难度分：2210
- 外部题解来源：https://leetcode.cn/problems/sum-of-k-mirror-numbers/solutions/1113431/da-biao-zuo-fa-by-endlesscheng-7ojf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】枚举回文数（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-k-mirror-numbers/solutions/1113431/da-biao-zuo-fa-by-endlesscheng-7ojf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`da-biao-zuo-fa-by-endlesscheng-7ojf`
- topic id：`1113431`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

由于数据范围很小，可以直接把 $k$ 在 $[2,9]$，$n$ 在 $[1,30]$ 中的所有答案预处理出来，这一共有 $8\cdot 30=240$ 个。

从小到大枚举回文数，构造方法如下：

- 枚举 $\textit{base}=10^0,10^1,10^2,\cdots$
- **枚举回文数的左半边**。
- 对于 $[\textit{base},10\cdot \textit{base})$ 范围内的每个数，将除了末尾数位的其余部分反转，拼到原数字末尾，这样可以生成奇数长度回文数。例如 $\textit{base}=10$，可以生成在 $[101,999]$ 中的所有回文数。
- 对于 $[\textit{base},10\cdot \textit{base})$ 范围内的每个数，将所有数位反转，拼到原数字末尾，这样可以生成偶数长度回文数。例如 $\textit{base}=10$，可以生成在 $[1001,9999]$ 中的所有回文数。

对每个回文数，判断其是否为 $k=2,3,\ldots,9$ 镜像数字。做法见 [9. 回文数](https://leetcode.cn/problems/palindrome-number/)，[我的题解](https://leetcode.cn/problems/palindrome-number/solutions/3682487/bi-guan-fang-ti-jie-shao-xun-huan-yi-ci-02nkc/)。也可以转成字符串再判断。

当每组 $k$ 镜像数字都计算出 $30$ 个时，结束枚举。

然后对每组 $k$ 镜像数字，计算这 $30$ 个数的前缀和，即为答案。

**注**：经计算，枚举回文数的左半边时，要从 $1$ 枚举到 $644545$，最后一个回文数为 $64454545446$。注意这个回文数超出了 $32$ 位整数范围。

```py [sol-Python3]
MAX_N = 30
ans = [[] for _ in range(10)]

# 力扣 9. 回文数
def is_k_palindrome(x: int, k: int) -> bool:
    if x % k == 0:
        return False
    rev = 0
    while rev < x // k:
        rev = rev * k + x % k
        x //= k
    return rev == x or rev == x // k

def do_palindrome(x: int) -> bool:
    done = True
    for k in range(2, 10):
        if len(ans[k]) < MAX_N and is_k_palindrome(x, k):
            ans[k].append(x)
        if len(ans[k]) < MAX_N:
            done = False
    if not done:
        return False
    for k in range(2, 10):
        ans[k] = list(accumulate(ans[k]))
    return True

def init() -> None:
    base = 1
    while True:
        # 生成奇数长度回文数，例如 base = 10，生成的范围是 101 ~ 999
        for i in range(base, base * 10):
            s = str(i)
            x = int(s + s[::-1][1:])
            if do_palindrome(x):
                return
        # 生成偶数长度回文数，例如 base = 10，生成的范围是 1001 ~ 9999
        for i in range(base, base * 10):
            s = str(i)
            x = int(s + s[::-1])
            if do_palindrome(x):
                return
        base *= 10
init()

class Solution:
    def kMirror(self, k: int, n: int) -> int:
        return ans[k][n - 1]
```

```java [sol-Java]
class Solution {
    private static final int MAX_N = 30;
    private static final List<Long>[] ans = new ArrayList[10];
    private static boolean initialized = false;

    // 这样写比 static block 快
    private void init() {
        if (initialized) {
            return;
        }
        initialized = true;

        Arrays.setAll(ans, i -> new ArrayList<>());
        for (int base = 1; ; base *= 10) {
            // 生成奇数长度回文数，例如 base = 10，生成的范围是 101 ~ 999
            for (int i = base; i < base * 10; i++) {
                long x = i;
                for (int t = i / 10; t > 0; t /= 10) {
                    x = x * 10 + t % 10;
                }
                if (doPalindrome(x)) {
                    return;
                }
            }
            // 生成偶数长度回文数，例如 base = 10，生成的范围是 1001 ~ 9999
            for (int i = base; i < base * 10; i++) {
                long x = i;
                for (int t = i; t > 0; t /= 10) {
                    x = x * 10 + t % 10;
                }
                if (doPalindrome(x)) {
                    return;
                }
            }
        }
    }

    private boolean doPalindrome(long x) {
        boolean done = true;
        for (int k = 2; k < 10; k++) {
            if (ans[k].size() < MAX_N && isKPalindrome(x, k)) {
                ans[k].add(x);
            }
            if (ans[k].size() < MAX_N) {
                done = false;
            }
        }
        if (!done) {
            return false;
        }

        for (int k = 2; k < 10; k++) {
            // 原地求前缀和
            List<Long> s = ans[k];
            for (int i = 1; i < MAX_N; i++) {
                s.set(i, s.get(i) + s.get(i - 1));
            }
        }
        return true;
    }

    // 力扣 9. 回文数
    private boolean isKPalindrome(long x, int k) {
        if (x % k == 0) {
            return false;
        }
        int rev = 0;
        while (rev < x / k) {
            rev = rev * k + (int) (x % k);
            x /= k;
        }
        return rev == x || rev == x / k;
    }

    public long kMirror(int k, int n) {
        init();
        return ans[k].get(n - 1);
    }
}
```

```cpp [sol-C++]
const int MAX_N = 30;
vector<long long> ans[10];

// 力扣 9. 回文数
bool is_k_palindrome(long long x, int k) {
    if (x % k == 0) {
        return false;
    }
    int rev = 0;
    while (rev < x / k) {
        rev = rev * k + x % k;
        x /= k;
    }
    return rev == x || rev == x / k;
}

bool do_palindrome(long long x) {
    bool done = true;
    for (int k = 2; k < 10; k++) {
        if (ans[k].size() < MAX_N && is_k_palindrome(x, k)) {
            ans[k].push_back(x);
        }
        if (ans[k].size() < MAX_N) {
            done = false;
        }
    }
    if (!done) {
        return false;
    }

    for (int k = 2; k < 10; k++) {
        // 原地求前缀和
        partial_sum(ans[k].begin(), ans[k].end(), ans[k].begin());
    }
    return true;
}

auto init = []() {
    for (int base = 1; ; base *= 10) {
        // 生成奇数长度回文数，例如 base = 10，生成的范围是 101 ~ 999
        for (int i = base; i < base * 10; i++) {
            long long x = i;
            for (int t = i / 10; t > 0; t /= 10) {
                x = x * 10 + t % 10;
            }
            if (do_palindrome(x)) {
                return 0;
            }
        }
        // 生成偶数长度回文数，例如 base = 10，生成的范围是 1001 ~ 9999
        for (int i = base; i < base * 10; i++) {
            long long x = i;
            for (int t = i; t > 0; t /= 10) {
                x = x * 10 + t % 10;
            }
            if (do_palindrome(x)) {
                return 0;
            }
        }
    }
}();

class Solution {
public:
    long long kMirror(int k, int n) {
        return ans[k][n - 1];
    }
};
```

```go [sol-Go]
const maxN = 30

var ans [10][]int

// 力扣 9. 回文数
func isKPalindrome(x, k int) bool {
	if x%k == 0 {
		return false
	}
	rev := 0
	for rev < x/k {
		rev = rev*k + x%k
		x /= k
	}
	return rev == x || rev == x/k
}

func doPalindrome(x int) bool {
	done := true
	for k := 2; k < 10; k++ {
		if len(ans[k]) < maxN && isKPalindrome(x, k) {
			ans[k] = append(ans[k], x)
		}
		if len(ans[k]) < maxN {
			done = false
		}
	}
	if !done {
		return false
	}

	for k := 2; k < 10; k++ {
		// 计算前缀和 
		for i := 1; i < maxN; i++ {
			ans[k][i] += ans[k][i-1]
		}
	}
	return true
}

func init() {
	for k := 2; k < 10; k++ {
		ans[k] = make([]int, 0, maxN) // 预分配空间
	}
	for base := 1; ; base *= 10 {
		// 生成奇数长度回文数，例如 base = 10，生成的范围是 101 ~ 999
		for i := base; i < base*10; i++ {
			x := i
			for t := i / 10; t > 0; t /= 10 {
				x = x*10 + t%10
			}
			if doPalindrome(x) {
				return
			}
		}
		// 生成偶数长度回文数，例如 base = 10，生成的范围是 1001 ~ 9999
		for i := base; i < base*10; i++ {
			x := i
			for t := i; t > 0; t /= 10 {
				x = x*10 + t%10
			}
			if doPalindrome(x) {
				return
			}
		}
	}
}

func kMirror(k, n int) int64 {
	return int64(ans[k][n-1])
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

见下面数学题单的「**§7.1 回文数**」。

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

本题来自 `七、杂项 / §7.1 回文数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.1 回文数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
