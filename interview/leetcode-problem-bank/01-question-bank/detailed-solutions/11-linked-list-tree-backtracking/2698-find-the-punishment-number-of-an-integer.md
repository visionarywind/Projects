# 2698. 求一个整数的惩罚数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-punishment-number-of-an-integer/
- 题目 slug：`find-the-punishment-number-of-an-integer`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.3 划分型回溯
- 难度分：1679
- 外部题解来源：https://leetcode.cn/problems/find-the-punishment-number-of-an-integer/solutions/2277792/yu-chu-li-hui-su-by-endlesscheng-ro3s/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理+递归，两种写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-the-punishment-number-of-an-integer/solutions/2277792/yu-chu-li-hui-su-by-endlesscheng-ro3s/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-hui-su-by-endlesscheng-ro3s`
- topic id：`2277792`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

把 $i^2$ 转成字符串 $s$，然后写一个递归，枚举 $s$ 分割出的第一个子串、第二个子串、……，把每个子串对应的整数值 $x$ 加到 $\textit{sum}$ 中。

递归到终点时，如果 $\textit{sum}=i$，则说明 $i$ 符合要求。

如果你不清楚怎么写这个递归，请看 [回溯算法套路①子集型回溯【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)。

代码实现时，可以预处理 $[1,1000]$ 内的所有数的惩罚数。

[视频讲解](https://www.bilibili.com/video/BV1Qm4y1t7cx/) 第三题。

## 写法一：使用字符串

```py [sol-Python3]
PRE_SUM = [0] * 1001
for i in range(1, 1001):
    s = str(i * i)
    n = len(s)
    def dfs(p: int, sum: int) -> bool:
        if p == n:  # 递归终点
            return sum == i  # i 符合要求
        x = 0
        for j in range(p, n):  # 枚举分割出从 s[p] 到 s[j] 的子串
            x = x * 10 + int(s[j])  # 子串对应的整数值
            if dfs(j + 1, sum + x):
                return True
        return False
    PRE_SUM[i] = PRE_SUM[i - 1] + (i * i if dfs(0, 0) else 0)

class Solution:
    def punishmentNumber(self, n: int) -> int:
        return PRE_SUM[n]
```

```java [sol-Java]
class Solution {
    private static final int[] PRE_SUM = new int[1001];

    static {
        for (int i = 1; i <= 1000; i++) {
            char[] s = Integer.toString(i * i).toCharArray();
            PRE_SUM[i] = PRE_SUM[i - 1] + (dfs(s, i, 0, 0) ? i * i : 0);
        }
    }

    private static boolean dfs(char[] s, int i, int p, int sum) {
        if (p == s.length) { // 递归终点
            return sum == i; // i 符合要求
        }
        int x = 0;
        for (int j = p; j < s.length; j++) { // 枚举分割出从 s[p] 到 s[j] 的子串
            x = x * 10 + s[j] - '0'; // 子串对应的整数值
            if (dfs(s, i, j + 1, sum + x)) {
                return true;
            }
        }
        return false;
    }

    public int punishmentNumber(int n) {
        return PRE_SUM[n];
    }
}
```

```cpp [sol-C++]
int PRE_SUM[1001];

int init = []() {
    for (int i = 1; i <= 1000; i++) {
        string s = to_string(i * i);
        int n = s.length();
        auto dfs = [&](this auto&& dfs, int p, int sum) -> bool {
            if (p == n) { // 递归终点
                return sum == i; // i 符合要求
            }
            int x = 0;
            for (int j = p; j < n; j++) { // 枚举分割出从 s[p] 到 s[j] 的子串
                x = x * 10 + s[j] - '0'; // 子串对应的整数值
                if (dfs(j + 1, sum + x)) {
                    return true;
                }
            }
            return false;
        };
        PRE_SUM[i] = PRE_SUM[i - 1] + (dfs(0, 0) ? i * i : 0);
    }
    return 0;
}();

class Solution {
public:
    int punishmentNumber(int n) {
        return PRE_SUM[n];
    }
};
```

```go [sol-Go]
var preSum [1001]int

func init() {
	for i := 1; i <= 1000; i++ {
		s := strconv.Itoa(i * i)
		n := len(s)
		var dfs func(int, int) bool
		dfs = func(p, sum int) bool {
			if p == n { // 递归终点
				return sum == i // i 符合要求
			}
			x := 0
			for j := p; j < n; j++ { // 枚举分割出从 s[p] 到 s[j] 的子串
				x = x*10 + int(s[j]-'0') // 子串对应的整数值
				if dfs(j+1, sum+x) {
					return true
				}
			}
			return false
		}
		preSum[i] = preSum[i-1]
		if dfs(0, 0) { // i 符合要求
			preSum[i] += i * i // 计算前缀和
		}
	}
}

func punishmentNumber(n int) int {
	return preSum[n]
}
```

```js [sol-JavaScript]
function dfs(p, sum, s, i) {
    const n = s.length;
    if (p === n) { // 递归终点
        return sum === i; // i 符合要求
    }
    let x = 0;
    for (let j = p; j < n; j++) { // 枚举分割出从 s[p] 到 s[j] 的子串
        x = x * 10 + parseInt(s[j]); // 子串对应的整数值
        if (dfs(j + 1, sum + x, s, i)) {
            return true;
        }
    }
    return false;
}

const PRE_SUM = new Array(1001).fill(0);
for (let i = 1; i <= 1000; i++) {
    const s = (i * i).toString();
    PRE_SUM[i] = PRE_SUM[i - 1] + (dfs(0, 0, s, i) ? i * i : 0);
}

var punishmentNumber = function (n) {
    return PRE_SUM[n];
};
```

```rust [sol-Rust]
fn dfs(p: usize, sum: i32, i: i32, s: &Vec<u8>) -> bool {
    if p == s.len() { // 递归终点
        return sum == i; // i 符合要求
    }
    let mut x = 0;
    for j in p..s.len() { // 枚举分割出从 s[p] 到 s[j] 的子串
        x = x * 10 + (s[j] & 0xf) as i32; // 子串对应的整数值
        if dfs(j + 1, sum + x, i, s) {
            return true;
        }
    }
    false
}

static mut initialized: bool = false;
static mut pre_sum: [i32; 1001] = [0; 1001];

fn init_once() {
    unsafe {
        if initialized { // 之前初始化过了
            return;
        }
        initialized = true;
        for i in 1..1001 {
            let s = (i * i).to_string().bytes().collect();
            pre_sum[i as usize] = pre_sum[i as usize - 1] + if dfs(0, 0, i, &s) { i * i } else { 0 };
        }
    }
}

impl Solution {
    pub fn punishment_number(n: i32) -> i32 {
        init_once();
        unsafe { pre_sum[n as usize] }
    }
}
```

## 写法二：不使用字符串

```py [sol-Python3]
PRE_SUM = [0] * 1001
for i in range(1, 1001):
    def dfs(val: int, sum: int) -> bool:
        if val == 0:  # 递归终点
            return sum == i  # i 符合要求
        x = 0
        pow10 = 1
        while val:
            val, d = divmod(val, 10)
            x += d * pow10
            if dfs(val, sum + x):
                return True
            pow10 *= 10
        return False
    PRE_SUM[i] = PRE_SUM[i - 1] + (i * i if dfs(i * i, 0) else 0)

class Solution:
    def punishmentNumber(self, n: int) -> int:
        return PRE_SUM[n]
```

```java [sol-Java]
class Solution {
    private static final int[] PRE_SUM = new int[1001];

    static {
        for (int i = 1; i <= 1000; i++) {
            PRE_SUM[i] = PRE_SUM[i - 1] + (dfs(i * i, i) ? i * i : 0);
        }
    }

    private static boolean dfs(int val, int sum) {
        if (val == 0) { // 递归终点
            return sum == 0; // i 符合要求
        }
        int pow10 = 1;
        for (int x = 0; val > 0; val /= 10) {
            x += val % 10 * pow10;
            if (dfs(val / 10, sum - x)) {
                return true;
            }
            pow10 *= 10;
        }
        return false;
    }

    public int punishmentNumber(int n) {
        return PRE_SUM[n];
    }
}
```

```cpp [sol-C++]
int PRE_SUM[1001];

int init = []() {
    for (int i = 1; i <= 1000; i++) {
        auto dfs = [&](this auto&& dfs, int val, int sum) -> bool {
            if (val == 0) { // 递归终点
                return sum == i; // i 符合要求
            }
            int pow10 = 1;
            for (int x = 0; val > 0; val /= 10) {
                x += val % 10 * pow10;
                if (dfs(val / 10, sum + x)) {
                    return true;
                }
                pow10 *= 10;
            }
            return false;
        };
        PRE_SUM[i] = PRE_SUM[i - 1] + (dfs(i * i, 0) ? i * i : 0);
    }
    return 0;
}();

class Solution {
public:
    int punishmentNumber(int n) {
        return PRE_SUM[n];
    }
};
```

```go [sol-Go]
var preSum [1001]int

func init() {
	for i := 1; i <= 1000; i++ {
		var dfs func(int, int) bool
		dfs = func(val, sum int) bool {
			if val == 0 { // 递归终点
				return sum == i // i 符合要求
			}
			for x, pow10 := 0, 1; val > 0; val /= 10 {
				x += val % 10 * pow10
				if dfs(val/10, sum+x) {
					return true
				}
				pow10 *= 10
			}
			return false
		}
		preSum[i] = preSum[i-1]
		if dfs(i*i, 0) { // i 符合要求
			preSum[i] += i * i // 计算前缀和
		}
	}
}

func punishmentNumber(n int) int {
	return preSum[n]
}
```

```js [sol-JavaScript]
function dfs(val, sum) {
    if (val === 0) { // 递归终点
        return sum === 0; // i 符合要求
    }
    let pow10 = 1;
    for (let x = 0; val > 0; val = Math.floor(val / 10)) {
        x += val % 10 * pow10;
        if (dfs(Math.floor(val / 10), sum - x)) {
            return true;
        }
        pow10 *= 10;
    }
    return false;
}

const PRE_SUM = new Array(1001).fill(0);
for (let i = 1; i <= 1000; i++) {
    PRE_SUM[i] = PRE_SUM[i - 1] + (dfs(i * i, i) ? i * i : 0);
}

var punishmentNumber = function (n) {
    return PRE_SUM[n];
};
```

```rust [sol-Rust]
fn dfs(mut val: i32, mut sum: i32) -> bool {
    if val == 0 { // 递归终点
        return sum == 0; // i 符合要求
    }
    let mut pow10 = 1;
    let mut x = 0;
    while val > 0 {
        x += val % 10 * pow10;
        val /= 10;
        if (dfs(val, sum - x)) {
            return true;
        }
        pow10 *= 10;
    }
    false
}

static mut initialized: bool = false;
static mut pre_sum: [i32; 1001] = [0; 1001];

fn init_once() {
    unsafe {
        if initialized { // 之前初始化过了
            return;
        }
        initialized = true;
        for i in 1..1001 {
            pre_sum[i as usize] = pre_sum[i as usize - 1] + if dfs(i * i, i) { i * i } else { 0 };
        }
    }
}

impl Solution {
    pub fn punishment_number(n: i32) -> i32 {
        init_once();
        unsafe { pre_sum[n as usize] }
    }
}
```

#### 复杂度分析

预处理的时间：$\mathcal{O}(U^{1 + 2\log_{10} 2})\approx\mathcal{O}(U^{1.602})$，其中 $U=1000$。对于数字 $i^2$，它的十进制字符串的长度为 $m=\lfloor1+2\log_{10} i\rfloor$。我在【基础算法精讲】中讲过，划分型题目的本质就是枚举子集，所以递归需要 $\mathcal{O}(2^m)=\mathcal{O}(i^{2\log_{10} 2})$ 的时间，对其积分可知，整个预处理需要 $\mathcal{O}(U^{1 + 2\log_{10} 2})$ 的时间。

力扣的计时规则是，预处理的时间不计入，所以两种写法都是 $\mathcal{O}(1)$ 时间。

- 时间复杂度：$\mathcal{O}(1)$
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `四、回溯 / §4.3 划分型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.3 划分型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
