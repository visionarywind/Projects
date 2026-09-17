# 838. 推多米诺

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/push-dominoes/
- 题目 slug：`push-dominoes`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1638
- 外部题解来源：https://leetcode.cn/problems/push-dominoes/solutions/3667176/fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-bztd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/push-dominoes/solutions/3667176/fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-bztd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-bztd`
- topic id：`3667176`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 分类讨论

下文把 $\texttt{.}$ 叫做点。

根据题意，有四种情况：

- $\texttt{L...L}$：中间的点全部变成 $\texttt{L}$。
- $\texttt{R...R}$：中间的点全部变成 $\texttt{R}$。
- $\texttt{R...L}$：前一半的点全部变成 $\texttt{R}$，后一半的点全部变成 $\texttt{L}$。特别地，如果有奇数个点，则正中间的点不变。
- $\texttt{L...R}$：不变。

## 实现技巧

为方便处理开头和结尾的点，可以在字符串前后各加一个哨兵，即定义 $s = \texttt{L} + \textit{dominoes} + \texttt{R}$。

初始化 $\textit{pre}=0$，表示上一个 $\texttt{L}$ 或 $\texttt{R}$ 的位置。

遍历 $s$：

- 如果 $s[i]$ 是点，跳过。
- 否则，如果 $s[i]=s[\textit{pre}]$，说明从 $\textit{pre}$ 到 $i$ 是 $\texttt{L...L}$ 或者 $\texttt{R...R}$，把中间的点全部变成 $s[i]$。
- 否则，如果 $s[i]=\texttt{L}$，说明从 $\textit{pre}$ 到 $i$ 是 $\texttt{R...L}$，把前一半的点全部变成 $\texttt{R}$，后一半的点全部变成 $\texttt{L}$。
    - 具体地，有 $i-\textit{pre}-1$ 个点，一半就是 $\textit{half} = \left\lfloor\dfrac{i-\textit{pre}-1}{2}\right\rfloor$。
    - 前一半的点从 $\textit{pre}+1$ 到 $\textit{pre}+\textit{half} = \left\lfloor\dfrac{\textit{pre}+i-1}{2}\right\rfloor$。
    - 后一半的点从 $i - \textit{half} = \left\lfloor\dfrac{\textit{pre}+i}{2}\right\rfloor+1$ 到 $i-1$。
- 否则，从 $\textit{pre}$ 到 $i$ 是 $\texttt{L...R}$，不变。
- 记录 $\textit{pre}=i$，继续循环。

```py [sol-Python3]
class Solution:
    def pushDominoes(self, dominoes: str) -> str:
        s = list('L' + dominoes + 'R')  # 前后各加一个哨兵
        pre = 0  # 上一个 L 或 R 的位置
        for i, ch in enumerate(s):
            if ch == '.':
                continue
            if ch == s[pre]:  # L...L 或 R...R
                s[pre + 1: i] = ch * (i - pre - 1)  # 全变成 s[i]
            elif ch == 'L':  # R...L
                half = (i - pre - 1) // 2
                s[pre + 1: pre + half + 1] = 'R' * half  # 前一半变 R
                s[i - half: i] = 'L' * half  # 后一半变 L
            pre = i
        return ''.join(s[1:-1])  # 去掉前后哨兵
```

```java [sol-Java]
class Solution {
    public String pushDominoes(String dominoes) {
        char[] s = ("L" + dominoes + "R").toCharArray(); // 前后各加一个哨兵
        int pre = 0; // 上一个 L 或 R 的位置
        for (int i = 1; i < s.length; i++) {
            if (s[i] == '.') {
                continue;
            }
            if (s[i] == s[pre]) { // L...L 或 R...R
                Arrays.fill(s, pre + 1, i, s[i]); // 全变成 s[i]
            } else if (s[i] == 'L') { // R...L
                Arrays.fill(s, pre + 1, (pre + i + 1) / 2, 'R'); // 前一半变 R
                Arrays.fill(s, (pre + i) / 2 + 1, i, 'L'); // 后一半变 L
            }
            pre = i;
        }
        return new String(s, 1, s.length - 2); // 去掉前后哨兵
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string pushDominoes(string dominoes) {
        string s = "L" + dominoes + "R"; // 前后各加一个哨兵（不加哨兵的写法见另一份代码）
        int n = s.size();
        int pre = 0; // 上一个 L 或 R 的位置
        for (int i = 1; i < n; i++) {
            if (s[i] == '.') {
                continue;
            }
            if (s[i] == s[pre]) { // L...L 或 R...R
                fill(s.begin() + pre + 1, s.begin() + i, s[i]); // 全变成 s[i]
            } else if (s[i] == 'L') { // R...L
                fill(s.begin() + pre + 1, s.begin() + (pre + i + 1) / 2, 'R'); // 前一半变 R
                fill(s.begin() + (pre + i) / 2 + 1, s.begin() + i, 'L'); // 后一半变 L
            }
            pre = i;
        }
        return s.substr(1, n - 2); // 去掉前后哨兵
    }
};
```

```cpp [sol-C++ 原地]
class Solution {
public:
    string pushDominoes(string s) {
        int n = s.size();
        int pre = -1; // 上一个 L 或 R 的位置
        for (int i = 0; i <= n; i++) {
            char ch = i < n ? s[i] : 'R';
            if (ch == '.') {
                continue;
            }
            if (ch == (pre < 0 ? 'L' : s[pre])) { // L...L 或 R...R
                fill(s.begin() + pre + 1, s.begin() + i, ch); // 全变成 s[i]
            } else if (ch == 'L') { // R...L
                fill(s.begin() + pre + 1, s.begin() + (pre + i + 1) / 2, 'R'); // 前一半变 R
                fill(s.begin() + (pre + i) / 2 + 1, s.begin() + i, 'L'); // 后一半变 L
            }
            pre = i;
        }
        return s;
    }
};
```

```c [sol-C]
char* pushDominoes(char* dominoes) {
    int n = strlen(dominoes) + 2;
    // 前后各加一个哨兵（不加哨兵的写法见另一份代码）
    char* s = malloc(n + 1);
    s[0] = 'L';
    strcpy(s + 1, dominoes);
    s[n - 1] = 'R';
    s[n] = '\0';

    int pre = 0; // 上一个 L 或 R 的位置
    for (int i = 1; i < n; i++) {
        if (s[i] == '.') {
            continue;
        }
        if (s[i] == s[pre]) { // L...L 或 R...R
            memset(s + pre + 1, s[i], i - pre - 1); // 全变成 s[i]
        } else if (s[i] == 'L') { // R...L
            int half = (i - pre - 1) / 2;
            memset(s + pre + 1, 'R', half); // 前一半变 R
            memset(s + i - half, 'L', half); // 后一半变 L
        }
        pre = i;
    }

    s[n - 1] = '\0';
    return s + 1;
}
```

```c [sol-C 原地]
char* pushDominoes(char* s) {
    int n = strlen(s);
    int pre = -1; // 上一个 L 或 R 的位置
    for (int i = 0; i <= n; i++) {
        char ch = i < n ? s[i] : 'R';
        if (ch == '.') {
            continue;
        }
        if (ch == (pre < 0 ? 'L' : s[pre])) { // L...L 或 R...R
            memset(s + pre + 1, ch, i - pre - 1); // 全变成 s[i]
        } else if (ch == 'L') { // R...L
            int half = (i - pre - 1) / 2;
            memset(s + pre + 1, 'R', half); // 前一半变 R
            memset(s + i - half, 'L', half); // 后一半变 L
        }
        pre = i;
    }
    return s;
}
```

```go [sol-Go]
func fill(s []byte, ch byte) {
    for i := range s {
        s[i] = ch
    }
}

func pushDominoes(dominoes string) string {
    s := []byte("L" + dominoes + "R") // 前后各加一个哨兵
    n := len(s)
    pre := 0 // 上一个 L 或 R 的位置
    for i, ch := range s {
        if ch == '.' {
            continue
        }
        if ch == s[pre] { // L...L 或 R...R
            fill(s[pre:i], ch) // 全变成 s[i]
        } else if ch == 'L' { // R...L
            fill(s[pre+1:(pre+i+1)/2], 'R') // 前一半变 R
            fill(s[(pre+i)/2+1:i], 'L')     // 后一半变 L
        }
        pre = i
    }
    return string(s[1 : n-1]) // 去掉前后哨兵
}
```

```js [sol-JavaScript]
var pushDominoes = function(dominoes) {
    const s = ('L' + dominoes + 'R').split(''); // 前后各加一个哨兵
    const n = s.length;
    let pre = 0; // 上一个 L 或 R 的位置
    for (let i = 1; i < n; i++) {
        if (s[i] === '.') {
            continue;
        }
        if (s[i] === s[pre]) { // L...L 或 R...R
            s.fill(s[i], pre + 1, i); // 全变成 s[i]
        } else if (s[i] === 'L') { // R...L
            s.fill('R', pre + 1, (pre + i + 1) / 2); // 前一半变 R
            s.fill('L', (pre + i) / 2 + 1, i); // 后一半变 L
        }
        pre = i;
    }
    return s.slice(1, n - 1).join(''); // 去掉前后哨兵
};
```

```rust [sol-Rust]
impl Solution {
    pub fn push_dominoes(dominoes: String) -> String {
        fn fill(s: &mut [u8], ch: u8) {
            for x in s.iter_mut() {
                *x = ch;
            }
        }

        let mut s = format!("L{}R", dominoes).into_bytes(); // 前后各加一个哨兵
        let n = s.len();
        let mut pre = 0; // 上一个 L 或 R 的位置
        for i in 1..n {
            let ch = s[i];
            if ch == b'.' {
                continue;
            }
            if ch == s[pre] { // L...L 或 R...R
                fill(&mut s[pre + 1..i], ch); // 全变成 s[i]
            } else if ch == b'L' { // R...L
                fill(&mut s[pre + 1..=(pre + i - 1) / 2], b'R'); // 前一半变 R
                fill(&mut s[(pre + i) / 2 + 1..i], b'L'); // 后一半变 L
            }
            pre = i;
        }
        unsafe { String::from_utf8_unchecked(s[1..n - 1].to_vec()) } // 去掉前后哨兵
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{dominoes}$ 的长度。每个点至多被修改一次，总的循环次数是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$，取决于实现。

## 相似题目

[2211. 统计道路上的碰撞次数](https://leetcode.cn/problems/count-collisions-on-a-road/)

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

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
