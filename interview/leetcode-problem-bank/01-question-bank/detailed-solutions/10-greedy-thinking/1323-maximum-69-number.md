# 1323. 6 和 9 组成的最大数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-69-number/
- 题目 slug：`maximum-69-number`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1194
- 外部题解来源：https://leetcode.cn/problems/maximum-69-number/solutions/3746134/liang-chong-fang-fa-yong-zi-fu-chuan-bu-jwopg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：用字符串/不用字符串（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-69-number/solutions/3746134/liang-chong-fang-fa-yong-zi-fu-chuan-bu-jwopg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-yong-zi-fu-chuan-bu-jwopg`
- topic id：`3746134`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

## 分析

要想把数字变大，只能把 $6$ 改成 $9$。

比如 $\textit{num}=9669$：

- 改高位的 $6$，得到 $9969$。
- 改低位的 $6$，得到 $9699$。
- $9969 > 9699$。

改高位的 $6$ 比改低位的 $6$ 更好。由于至多改一次，所以改最高位的 $6$。若 $\textit{num}$ 无 $6$，则 $\textit{num}$ 不变。

## 方法一：用字符串

```py [sol-Py3]
class Solution:
    def maximum69Number(self, num: int) -> int:
        s = str(num).replace('6', '9', 1)  # 替换第一个 6
        return int(s)
```

```py [sol-Py3 写法二]
class Solution:
    def maximum69Number(self, num: int) -> int:
        s = str(num)
        i = s.find('6')
        if i < 0:
            return num
        return int(s[:i] + '9' + s[i + 1:])
```

```java [sol-Java]
class Solution {
    public int maximum69Number(int num) {
        String s = String.valueOf(num).replaceFirst("6", "9"); // 替换第一个 6
        return Integer.parseInt(s);
    }
}
```

```java [sol-Java 写法二]
class Solution {
    public int maximum69Number(int num) {
        String s = Integer.toString(num);
        int i = s.indexOf('6');
        if (i < 0) {
            return num;
        }
        s = s.substring(0, i) + "9" + s.substring(i + 1);
        return Integer.parseInt(s);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximum69Number(int num) {
        string s = to_string(num);
        int i = s.find('6');
        if (i == string::npos) {
            return num;
        }
        s[i] = '9';
        return stoi(s);
    }
};
```

```c [sol-C]
int maximum69Number(int num) {
    char s[6];
    sprintf(s, "%d", num);
    char* p = strchr(s, '6');
    if (p == NULL) {
        return num;
    }
    *p = '9';
    return atoi(s);
}
```

```go [sol-Go]
func maximum69Number(num int) int {
	s := strconv.Itoa(num)
	s = strings.Replace(s, "6", "9", 1) // 替换第一个 6
	ans, _ := strconv.Atoi(s)
	return ans
}
```

```js [sol-JS]
var maximum69Number = function(num) {
    const s = String(num).replace('6', '9'); // 替换第一个 6
    return parseInt(s);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum69_number(num: i32) -> i32 {
        num.to_string()
           .replacen("6", "9", 1) // 替换第一个 6
           .parse()
           .unwrap()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \textit{num})$。
- 空间复杂度：$\mathcal{O}(\log \textit{num})$。

## 方法二：不用字符串

可以不用转成字符串处理，而是不断取最低位（模 $10$），去掉最低位（除以 $10$），直到数字为 $0$。

例如 $\textit{num}=9669$：

1. 初始化 $x=\textit{num}$。
2. 通过 $x\bmod 10$ 取到个位数 $9$，然后把 $x$ 除以 $10$（下取整），得到 $x=966$。
3. 再次 $x\bmod 10$ 取到十位数 $6$，然后把 $x$ 除以 $10$（下取整），得到 $x=96$。
4. 再次 $x\bmod 10$ 取到百位数 $6$，然后把 $x$ 除以 $10$（下取整），得到 $x=9$。
5. 最后 $x\bmod 10$ 取到千位数 $9$，然后把 $x$ 除以 $10$（下取整），得到 $x=0$。此时完成了遍历 $\textit{num}$ 的每个数位，退出循环。

在这个过程中，维护一个变量 $\textit{base}$，初始值为 $1$，在循环末尾把 $\textit{base}$ 乘以 $10$。每当我们遍历到一个等于 $6$ 的数位，就保存此刻的 $\textit{base}$，即更新 $\textit{maxBase} = \textit{base}$。其中 $\textit{maxBase}$ 初始值为 $0$。由于我们从低位往高位遍历，所以最终的 $\textit{maxBase}$ 就是最高位的 $6$ 对应的 $\textit{base}$。在上面的例子中，我们可以得到 $\textit{maxBase} = 100$。

最终答案为 

$$
\textit{num} + \textit{maxBase}\cdot 3
$$

在上面的例子中，答案为 $9669 + 100\cdot 3 = 9969$。

> 注：如果 $\textit{num}$ 中没有 $6$，由于我们初始化 $\textit{maxBase}=0$，最终答案为 $\textit{num}$。

```py [sol-Python3]
class Solution:
    def maximum69Number(self, num: int) -> int:
        max_base = 0
        base = 1
        x = num
        while x:
            x, d = divmod(x, 10)
            if d == 6:
                max_base = base
            base *= 10
        return num + max_base * 3
```

```java [sol-Java]
class Solution {
    public int maximum69Number(int num) {
        int maxBase = 0;
        int base = 1;
        for (int x = num; x > 0; x /= 10) {
            if (x % 10 == 6) {
                maxBase = base;
            }
            base *= 10;
        }
        return num + maxBase * 3;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximum69Number(int num) {
        int max_base = 0;
        int base = 1;
        for (int x = num; x > 0; x /= 10) {
            if (x % 10 == 6) {
                max_base = base;
            }
            base *= 10;
        }
        return num + max_base * 3;
    }
};
```

```c [sol-C]
int maximum69Number(int num) {
    int max_base = 0;
    int base = 1;
    for (int x = num; x > 0; x /= 10) {
        if (x % 10 == 6) {
            max_base = base;
        }
        base *= 10;
    }
    return num + max_base * 3;
}
```

```go [sol-Go]
func maximum69Number(num int) int {
	maxBase := 0
	base := 1
	for x := num; x > 0; x /= 10 {
		if x%10 == 6 {
			maxBase = base
		}
		base *= 10
	}
	return num + maxBase*3
}
```

```js [sol-JavaScript]
var maximum69Number = function(num) {
    let maxBase = 0;
    let base = 1;
    for (let x = num; x > 0; x = Math.floor(x / 10)) {
        if (x % 10 === 6) {
            maxBase = base;
        }
        base *= 10;
    }
    return num + maxBase * 3;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum69_number(num: i32) -> i32 {
        let mut max_base = 0;
        let mut base = 1;
        let mut x = num;
        while x > 0 {
            if x % 10 == 6 {
                max_base = base;
            }
            base *= 10;
            x /= 10;
        }
        num + max_base * 3
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \textit{num})$。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

1. 改成求最小数字。
2. 额外输入一个正整数 $k$，至多翻转 $k$ 个数，返回可以得到的最大数字。
3. 给定正整数 $\textit{low}$ 和 $\textit{high}$，计算闭区间 $[\textit{low},\textit{high}]$ 中的所有整数 $x$ 的 $\texttt{maximum69Number}(x)$ 之和。

欢迎在评论区分享你的思路/代码。

## 专题训练

见下面贪心题单的「**§3.1 字典序最小/最大**」。

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

本题来自 `三、字符串贪心 / §3.1 字典序最小/最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、字符串贪心 / §3.1 字典序最小/最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
