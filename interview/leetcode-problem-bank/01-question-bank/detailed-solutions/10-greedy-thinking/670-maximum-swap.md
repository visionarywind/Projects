# 670. 最大交换

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-swap/
- 题目 slug：`maximum-swap`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-swap/solutions/2614470/yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-c9b1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次遍历，简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-swap/solutions/2614470/yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-c9b1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-c9b1`
- topic id：`2614470`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

举例说明，假设 $\textit{num}=9952767$。为了得到最大值，我们来看看怎么贪心：

1. 从左往右考察 $\textit{num}$ 的每个数字，如果一个数字右边没有比它大的，那么肯定无需交换它。比如这里的 $9$，无论它和谁交换都不能让 $\textit{num}$ 更大。
2. 反之，如果一个数字右边有比它大的数，那么肯定要交换它，比如这里的 $5$，右边有比它大的 $7$ 和 $6$，为了让 $\textit{num}$ 尽量大，和 $7$ 交换最优。
3. 但是，如果有多个 $7$ 呢？我们应该和哪个 $7$ 交换呢？
   - 如果和第一个 $7$ 交换，我们得到的是 $9972567$。
   - 如果和第二个 $7$ 交换，我们得到的是 $9972765$。
   - 和**最后一个** $7$ 交换是最优的。
    
设 $\textit{num}$ 的十进制字符串为 $s$。算法如下：

1. 倒序遍历 $s$，同时维护最大数的下标 $\textit{maxIdx}$。它只在遇到更大的数字才更新，遇到相同数字不会更新，从而满足上面讨论的「最后一个」。
2. 如果发现 $s[i] < s[\textit{maxIdx}]$，满足交换要求，我们先把这两个下标保存在变量 $p$ 和 $q$ 中。注：$p$ 在遍历前的初始值为 $-1$。
3. 继续向左遍历，如果又遇到 $s[i] < s[\textit{maxIdx}]$，就更新 $p=i,\ q=\textit{maxIdx}$，因为 $s[i]$ 越靠左越好，我们要交换的是从左到右第一个右边有比它大的数字。
4. 遍历结束，如果无需交换，即 $p=-1$，那么直接返回 $\textit{num}$。否则交换 $s[p]$ 和 $s[q]$，然后把 $s$ 转换成数字返回。

```py [sol-Python3]
class Solution:
    def maximumSwap(self, num: int) -> int:
        s = str(num)
        max_idx = len(s) - 1
        p = q = -1
        for i in range(len(s) - 2, -1, -1):
            if s[i] > s[max_idx]:  # s[i] 是目前最大数字
                max_idx = i
            elif s[i] < s[max_idx]:  # s[i] 右边有比它大的
                p, q = i, max_idx  # 更新 p 和 q
        if p == -1:  # 这意味着 s 是降序的
            return num
        s = list(s)
        s[p], s[q] = s[q], s[p]  # 交换 s[p] 和 s[q]
        return int(''.join(s))
```

```java [sol-Java]
class Solution {
    public int maximumSwap(int num) {
        char[] s = Integer.toString(num).toCharArray();
        int maxIdx = s.length - 1;
        int p = -1, q = 0;
        for (int i = s.length - 2; i >= 0; i--) {
            if (s[i] > s[maxIdx]) { // s[i] 是目前最大数字
                maxIdx = i;
            } else if (s[i] < s[maxIdx]) { // s[i] 右边有比它大的
                p = i;
                q = maxIdx; // 更新 p 和 q
            }
        }
        if (p == -1) { // 这意味着 s 是降序的
            return num;
        }
        char temp = s[p];
        s[p] = s[q];
        s[q] = temp; // 交换 s[p] 和 s[q]
        return Integer.parseInt(new String(s));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSwap(int num) {
        string s = to_string(num);
        int n = s.length();
        int max_idx = n - 1;
        int p = -1, q;
        for (int i = n - 2; i >= 0; i--) {
            if (s[i] > s[max_idx]) { // s[i] 是目前最大数字
                max_idx = i;
            } else if (s[i] < s[max_idx]) { // s[i] 右边有比它大的
                p = i;
                q = max_idx; // 更新 p 和 q
            }
        }
        if (p == -1) { // 这意味着 s 是降序的
            return num;
        }
        swap(s[p], s[q]); // 交换 s[p] 和 s[q]
        return stoi(s);
    }
};
```

```go [sol-Go]
func maximumSwap(num int) int {
    s := strconv.Itoa(num)
    maxIdx := len(s) - 1
    p, q := -1, 0
    for i := len(s) - 2; i >= 0; i-- {
        if s[i] > s[maxIdx] { // s[i] 是目前最大数字
            maxIdx = i
        } else if s[i] < s[maxIdx] { // s[i] 右边有比它大的
            p, q = i, maxIdx // 更新 p 和 q
        }
    }
    if p == -1 { // 这意味着 s 是降序的
        return num
    }
    t := []byte(s)
    t[p], t[q] = t[q], t[p]
    ans, _ := strconv.Atoi(string(t))
    return ans
}
```

```js [sol-JavaScript]
var maximumSwap = function (num) {
    const s = num.toString();
    let maxIdx = s.length - 1;
    let p = -1, q = 0;
    for (let i = s.length - 2; i >= 0; i--) {
        if (s[i] > s[maxIdx]) { // s[i] 是目前最大数字
            maxIdx = i;
        } else if (s[i] < s[maxIdx]) { // s[i] 右边有比它大的
            p = i;
            q = maxIdx; // 更新 p 和 q
        }
    }
    if (p === -1) { // 这意味着 s 是降序的
        return num;
    }
    let t = s.split('');
    [t[p], t[q]] = [t[q], t[p]];
    return parseInt(t.join(''));
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_swap(num: i32) -> i32 {
        let mut s = num.to_string().into_bytes();
        let n = s.len();
        let mut max_idx = n - 1;
        let mut p = n;
        let mut q = 0;
        for i in (0..n - 1).rev() {
            if s[i] > s[max_idx] { // s[i] 是目前最大数字
                max_idx = i;
            } else if s[i] < s[max_idx] { // s[i] 右边有比它大的
                p = i;
                q = max_idx; // 更新 p 和 q
            }
        }
        if p == n { // 这意味着 s 是降序的
            return num;
        }
        s.swap(p, q); // 交换 s[p] 和 s[q]
        unsafe { String::from_utf8_unchecked(s).parse().unwrap() }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \textit{num})$。$\textit{num}$ 的十进制长度为 $\mathcal{O}(\log \textit{num})$。
- 空间复杂度：$\mathcal{O}(\log \textit{num})$。

## 思考题

1. 如果要交换的两个数，下标相差不能超过 $k$ 呢？
2. 如果可以交换两次呢？

欢迎在评论区发表你的思路。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

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
