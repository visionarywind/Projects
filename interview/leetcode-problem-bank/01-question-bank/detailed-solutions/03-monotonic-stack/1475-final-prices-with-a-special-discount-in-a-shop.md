# 1475. 商品折扣后的最终价格

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/final-prices-with-a-special-discount-in-a-shop/
- 题目 slug：`final-prices-with-a-special-discount-in-a-shop`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/final-prices-with-a-special-discount-in-a-shop/solutions/3831614/liang-chong-dan-diao-zhan-xie-fa-cong-yo-dpdx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种单调栈写法：从右到左 / 从左到右（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/final-prices-with-a-special-discount-in-a-shop/solutions/3831614/liang-chong-dan-diao-zhan-xie-fa-cong-yo-dpdx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-dan-diao-zhan-xie-fa-cong-yo-dpdx`
- topic id：`3831614`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

本题要求的是在 $i$ 右边最近的 $\le \textit{prices}[i]$ 的数，这是单调栈的标准应用，原理请看 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)。

## 写法一：从右到左

栈中记录折扣值。

```py [sol-Python3]
class Solution:
    def finalPrices(self, prices: List[int]) -> List[int]:
        n = len(prices)
        ans = [0] * n
        st = [0]  # 哨兵，作为没有折扣时的栈顶值
        for i in range(n - 1, -1, -1):
            p = prices[i]
            while st[-1] > p:
                st.pop()  # p 比栈顶小，比栈顶更能成为左侧元素的折扣值
            # 循环结束后，栈顶的价格 <= p，作为折扣值
            ans[i] = p - st[-1]
            st.append(p)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] finalPrices(int[] prices) {
        int n = prices.length;
        int[] ans = new int[n];
        Deque<Integer> st = new ArrayDeque<>();
        st.push(0); // 哨兵，作为没有折扣时的栈顶值
        for (int i = n - 1; i >= 0; i--) {
            int p = prices[i];
            while (st.peek() > p) {
                st.pop(); // p 比栈顶小，比栈顶更能成为左侧元素的折扣值
            }
            // 循环结束后，栈顶的价格 <= p，作为折扣值
            ans[i] = p - st.peek();
            st.push(p);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> finalPrices(vector<int>& prices) {
        int n = prices.size();
        vector<int> ans(n);
        stack<int> st;
        st.push(0); // 哨兵，作为没有折扣时的栈顶值
        for (int i = n - 1; i >= 0; i--) {
            int p = prices[i];
            while (st.top() > p) {
                st.pop(); // p 比栈顶小，比栈顶更能成为左侧元素的折扣值
            }
            // 循环结束后，栈顶的价格 <= p，作为折扣值
            ans[i] = p - st.top();
            st.push(p);
        }
        return ans;
    }
};
```

```c [sol-C]
int* finalPrices(int* prices, int pricesSize, int* returnSize) {
    *returnSize = pricesSize;
    int* ans = malloc(pricesSize * sizeof(int));
    int* st = malloc((pricesSize + 1) * sizeof(int));
    st[0] = 0; // 哨兵，作为没有折扣时的栈顶值
    int top = 0; // 栈顶下标

    for (int i = pricesSize - 1; i >= 0; i--) {
        int p = prices[i];
        while (st[top] > p) {
            top--; // p 比栈顶小，比栈顶更能成为左侧元素的折扣值
        }
        // 循环结束后，栈顶的价格 <= p，作为折扣值
        ans[i] = p - st[top];
        st[++top] = p; // p 入栈
    }

    free(st);
    return ans;
}
```

```go [sol-Go]
func finalPrices(prices []int) []int {
	n := len(prices)
	ans := make([]int, n)
	st := []int{0} // 哨兵，作为没有折扣时的栈顶值
	for i := n - 1; i >= 0; i-- {
		p := prices[i]
		for st[len(st)-1] > p {
			st = st[:len(st)-1] // p 比栈顶小，比栈顶更能成为左侧元素的折扣值
		}
		// 循环结束后，栈顶的价格 <= p，作为折扣值
		ans[i] = p - st[len(st)-1]
		st = append(st, p)
	}
	return ans
}
```

```js [sol-JavaScript]
var finalPrices = function(prices) {
    const n = prices.length;
    const ans = Array(n).fill(0);
    const st = [0]; // 哨兵，作为没有折扣时的栈顶值
    for (let i = n - 1; i >= 0; i--) {
        const p = prices[i];
        while (st[st.length - 1] > p) {
            st.pop(); // p 比栈顶小，比栈顶更能成为左侧元素的折扣值
        }
        // 循环结束后，栈顶的价格 <= p，作为折扣值
        ans[i] = p - st[st.length - 1];
        st.push(p);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn final_prices(prices: Vec<i32>) -> Vec<i32> {
        let mut ans = vec![0; prices.len()];
        let mut st = vec![0]; // 哨兵，作为没有折扣时的栈顶值
        for (i, p) in prices.into_iter().enumerate().rev() {
            while *st.last().unwrap() > p {
                st.pop(); // p 比栈顶小，比栈顶更能成为左侧元素的折扣值
            }
            // 循环结束后，栈顶的价格 <= p，作为折扣值
            ans[i] = p - st.last().unwrap();
            st.push(p);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{prices}$ 的长度。虽然我们写了个二重循环，但站在每个元素的视角看，这个元素在二重循环中最多入栈出栈各一次，因此循环次数**之和**是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二：从左到右

栈中记录还没算出折扣值的那些数的下标。当前遍历的元素作为折扣值。

相当于栈是一个 todolist，在循环的过程中，现在还不知道答案是多少，在后面的循环中会算出答案。

```py [sol-Python3]
class Solution:
    def finalPrices(self, prices: List[int]) -> List[int]:
        st = []  # todolist
        for i, p in enumerate(prices):  
            while st and prices[st[-1]] >= p:
                prices[st.pop()] -= p  # p 是栈顶的折扣值
            st.append(i)
        return prices
```

```java [sol-Java]
class Solution {
    public int[] finalPrices(int[] prices) {
        // 更快的写法见【Java 数组】
        Deque<Integer> st = new ArrayDeque<>(); // todolist
        for (int i = 0; i < prices.length; i++) {
            int p = prices[i];
            while (!st.isEmpty() && prices[st.peek()] >= p) {
                prices[st.pop()] -= p; // p 是栈顶的折扣值
            }
            st.push(i);
        }
        return prices;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int[] finalPrices(int[] prices) {
        int n = prices.length;
        int[] st = new int[n];
        int top = -1; // 栈顶下标
        for (int i = 0; i < n; i++) {
            int p = prices[i];
            while (top >= 0 && prices[st[top]] >= p) {
                prices[st[top--]] -= p; // p 是栈顶的折扣值
            }
            st[++top] = i; // i 入栈
        }
        return prices;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> finalPrices(vector<int>& prices) {
        stack<int> st; // todolist
        for (int i = 0; i < prices.size(); i++) {
            int p = prices[i];
            while (!st.empty() && prices[st.top()] >= p) {
                prices[st.top()] -= p; // p 是栈顶的折扣值
                st.pop();
            }
            st.push(i);
        }
        return prices;
    }
};
```

```c [sol-C]
int* finalPrices(int* prices, int pricesSize, int* returnSize) {
    int* st = malloc(pricesSize * sizeof(int)); // todolist
    int top = -1; // 栈顶下标

    for (int i = 0; i < pricesSize; i++) {
        int p = prices[i];
        while (top >= 0 && prices[st[top]] >= p) {
            prices[st[top--]] -= p; // p 是栈顶的折扣值
        }
        st[++top] = i; // i 入栈
    }

    free(st);
    *returnSize = pricesSize;
    return prices;
}
```

```go [sol-Go]
func finalPrices(prices []int) []int {
	st := []int{} // todolist
	for i, p := range prices {
		for len(st) > 0 && prices[st[len(st)-1]] >= p {
			prices[st[len(st)-1]] -= p // p 是栈顶的折扣值
			st = st[:len(st)-1]
		}
		st = append(st, i)
	}
	return prices
}
```

```js [sol-JavaScript]
var finalPrices = function(prices) {
    const st = []; // todolist
    for (let i = 0; i < prices.length; i++) {
        const p = prices[i];
        while (st.length && prices[st[st.length - 1]] >= p) {
            prices[st.pop()] -= p; // p 是栈顶的折扣值
        }
        st.push(i);
    }
    return prices;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn final_prices(mut prices: Vec<i32>) -> Vec<i32> {
        let mut st = vec![]; // todolist
        for i in 0..prices.len() {
            let p = prices[i];
            while !st.is_empty() && prices[*st.last().unwrap()] >= p {
                prices[st.pop().unwrap()] -= p; // p 是栈顶的折扣值
            }
            st.push(i);
        }
        prices
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{prices}$ 的长度。虽然我们写了个二重循环，但站在每个元素的视角看，这个元素在二重循环中最多入栈出栈各一次，因此循环次数**之和**是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(\min(n,U))$，其中 $U$ 是 $\textit{prices}$ 中的不同元素个数。注意这种写法栈中没有重复元素。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、单调栈 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、单调栈 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
