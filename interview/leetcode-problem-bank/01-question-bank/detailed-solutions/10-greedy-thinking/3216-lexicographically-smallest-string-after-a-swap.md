# 3216. 交换后字典序最小的字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/lexicographically-smallest-string-after-a-swap/
- 题目 slug：`lexicographically-smallest-string-after-a-swap`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1243
- 外部题解来源：https://leetcode.cn/problems/lexicographically-smallest-string-after-a-swap/solutions/2843075/tan-xin-pythonjavacgo-by-endlesscheng-jqga/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心，附思考题（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/lexicographically-smallest-string-after-a-swap/solutions/2843075/tan-xin-pythonjavacgo-by-endlesscheng-jqga/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavacgo-by-endlesscheng-jqga`
- topic id：`2843075`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

只能交换一次。要使字典序最小，需要满足下面两个要求：

1. 交换的两个数字，**左边必须大于右边**，否则交换不会使字典序变小。
2. 交换的位置**越靠左越好**。比如示例 1 的 $45320$，交换 $5$ 和 $3$ 得到 $43520$，而交换更靠右的 $2$ 和 $0$ 得到 $45302$，这比 $43520$ 更大。

细节：字符对应的数字的奇偶性，等于字符的 ASCII 值的奇偶性。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1LZ421u7Ut/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def getSmallestString(self, s: str) -> str:
        t = list(s)
        for i in range(1, len(t)):
            x, y = t[i - 1], t[i]
            if x > y and ord(x) % 2 == ord(y) % 2:
                t[i - 1], t[i] = y, x
                break
        return ''.join(t)
```

```java [sol-Java]
class Solution {
    public String getSmallestString(String s) {
        char[] t = s.toCharArray();
        for (int i = 1; i < t.length; i++) {
            char x = t[i - 1];
            char y = t[i];
            if (x > y && x % 2 == y % 2) {
                t[i - 1] = y;
                t[i] = x;
                break;
            }
        }
        return new String(t);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string getSmallestString(string s) {
        for (int i = 1; i < s.length(); i++) {
            char x = s[i - 1], y = s[i];
            if (x > y && x % 2 == y % 2) {
                swap(s[i - 1], s[i]);
                break;
            }
        }
        return s;
    }
};
```

```c [sol-C]
char* getSmallestString(char* s) {
    for (int i = 1; s[i]; i++) {
        char x = s[i - 1], y = s[i];
        if (x > y && x % 2 == y % 2) {
            s[i - 1] = y;
            s[i] = x;
            break;
        }
    }
    return s;
}
```

```go [sol-Go]
func getSmallestString(s string) string {
    t := []byte(s)
    for i := 1; i < len(t); i++ {
        x, y := t[i-1], t[i]
        if x > y && x%2 == y%2 {
            t[i-1], t[i] = y, x
            break
        }
    }
    return string(t)
}
```

```js [sol-JavaScript]
var getSmallestString = function(s) {
    const t = s.split('');
    for (let i = 1; i < t.length; i++) {
        const x = t[i - 1], y = t[i];
        if (x > y && x.charCodeAt(0) % 2 === y.charCodeAt(0) % 2) {
            t[i - 1] = y;
            t[i] = x;
            break;
        }
    }
    return t.join('');
};
```

```rust [sol-Rust]
impl Solution {
    pub fn get_smallest_string(mut s: String) -> String {
        unsafe {
            let t = s.as_bytes_mut();
            for i in 1..t.len() {
                let x = t[i - 1];
                let y = t[i];
                if x > y && x % 2 == y % 2 {
                    t[i - 1] = y;
                    t[i] = x;
                    break;
                }
            }
            s
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$，取决于能否直接修改 $s$。

## 思考题

如果交换的是**任意两位**呢？

见 [670. 最大交换](https://leetcode.cn/problems/maximum-swap/)。

更多相似题目，见 [贪心题单](https://leetcode.cn/circle/discuss/g6KTKL/) 中的「**§3.1 字典序最小/最大**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
