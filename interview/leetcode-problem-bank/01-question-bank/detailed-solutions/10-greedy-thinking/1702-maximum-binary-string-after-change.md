# 1702. 修改后的最大二进制字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-binary-string-after-change/
- 题目 slug：`maximum-binary-string-after-change`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.1 从特殊到一般
- 难度分：1825
- 外部题解来源：https://leetcode.cn/problems/maximum-binary-string-after-change/solutions/2732155/tan-xin-jian-ji-xie-fa-pythonjavacgojsru-szie/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心，简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-binary-string-after-change/solutions/2732155/tan-xin-jian-ji-xie-fa-pythonjavacgojsru-szie/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-jian-ji-xie-fa-pythonjavacgojsru-szie`
- topic id：`2732155`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

## 提示 1

答案不会包含 $00$（连续的 $0$）。

**证明**：反证法。假设答案包含 $00$，我们可以通过操作 1 将其变为 $10$，从而得到更大的答案。所以答案不会包含 $00$。

## 提示 2

答案至多包含一个 $0$。

**证明**：反证法。假设至少有两个 $0$，我们随意选择其中两个 $0$，由提示 1 可知这两个 $0$ 不相邻。例如 $10110$，我们可以通过操作 2 将右边的 $0$「移动」到第一个 $0$ 的右边，即 $10011$，然后通过操作 1 将其变为 $11011$。由于左边更高位的 $0$ 变成了 $1$，所以我们得到了比 $10110$ 更大的答案。一般地，在有多个 $0$ 的情况下，总是可以通过操作 2 让最高位的 $0$ 的右侧也是 $0$，然后通过操作 1 让最高位的 $0$ 变成 $1$，从而得到更大的答案。所以答案至多包含一个 $0$。

## 提示 3

如果 $\textit{binary}$ 全是 $1$，那么返回 $\textit{binary}$ 即可。

如果 $\textit{binary}$ 中有 $0$，由于操作 1 和操作 2 的结果都包含 $0$，所以我们无法把所有的 $0$ 都变成 $1$。结合提示 2，**最终答案会恰好包含一个** $0$。

此外，提示 2 相当于给出了一个让二进制更大的方案：只要还有两个 $0$，那么用操作 2 把右边的 $0$ 往左移，当出现 $00$ 时就通过操作 1 把左边的 $0$ 变成 $1$，这会让二进制更大。

设 $\textit{binary}$ 从左到右第一个 $0$ 的下标为 $i$，为了得到更大的二进制，下标在 $[i,n-1]$ 中的 $1$ 会随着 $0$ 的左移被「挤到」$\textit{binary}$ 的末尾。例如

$$
101010\xrightarrow{操作\ 2} 100110\xrightarrow{操作\ 1} 110110\xrightarrow{操作\ 2} 110011\xrightarrow{操作\ 1} 111011
$$

或者

$$
101010\xrightarrow{操作\ 2} 100011\xrightarrow{操作\ 1} 111011
$$

注意 $101010\rightarrow 111110$ 是无法做到的，在末尾 $0$ 不移动的情况下，我们无法把前面的 $0$ 全部变成 $1$。一般地，在有多个不相邻 $0$ 的情况下，不移动末尾 $0$ 又能把前面所有 $0$ 都变成 $1$ 是做不到的，因为 $0$ 只能左移不能右移。

设 $[i,n-1]$ 中有 $\textit{cnt}_1$ 个 $1$，那么当上述操作完成时，有 $\textit{cnt}_1$ 个 $1$ 被挤到答案的末尾，那唯一的 $0$ 就在这 $\textit{cnt}_1$ 个 $1$ 的左边。所以最后答案**从左到右**依次为：

- $n-1-\textit{cnt}_1$ 个 $1$。
- $1$ 个 $0$。
- $\textit{cnt}_1$ 个 $1$。

```py [sol-Python3]
class Solution:
    def maximumBinaryString(self, binary: str) -> str:
        i = binary.find('0')
        if i < 0:  # binary 全是 '1'
            return binary
        cnt1 = binary.count('1', i)  # 统计 binary[i:] 中 '1' 的个数
        return '1' * (len(binary) - 1 - cnt1) + '0' + '1' * cnt1
```

```java [sol-Java]
class Solution {
    public String maximumBinaryString(String binary) {
        int i = binary.indexOf('0');
        if (i < 0) { // binary 全是 '1'
            return binary;
        }
        char[] s = binary.toCharArray();
        int cnt1 = 0;
        for (i++; i < s.length; i++) {
            cnt1 += s[i] - '0'; // 统计 [i, n-1] 中 '1' 的个数
        }
        return "1".repeat(s.length - 1 - cnt1) + '0' + "1".repeat(cnt1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string maximumBinaryString(string binary) {
        int i = binary.find('0');
        if (i < 0) { // binary 全是 '1'
            return binary;
        }
        int cnt1 = count(binary.begin() + i, binary.end(), '1'); // 统计 [i, n-1] 中 '1' 的个数
        return string(binary.size() - 1 - cnt1, '1') + '0' + string(cnt1, '1');
    }
};
```

```go [sol-Go]
func maximumBinaryString(binary string) string {
    i := strings.Index(binary, "0")
    if i < 0 { // binary 全是 '1'
        return binary
    }
    cnt1 := strings.Count(binary[i:], "1") // 统计 binary[i:] 中 '1' 的个数
    return strings.Repeat("1", len(binary)-1-cnt1) + "0" + strings.Repeat("1", cnt1)
}
```

```js [sol-JavaScript]
var maximumBinaryString = function(binary) {
    let i = binary.indexOf('0');
    if (i < 0) { // binary 全是 '1'
        return binary;
    }
    let cnt1 = 0;
    for (i++; i < binary.length; i++) {
        if (binary[i] === '1') {
            cnt1++; // 统计 [i, n-1] 中 '1' 的个数
        }
    }
    return '1'.repeat(binary.length - 1 - cnt1) + '0' + '1'.repeat(cnt1);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_binary_string(binary: String) -> String {
        if let Some(i) = binary.find('0') {
            let cnt1 = binary[i..].bytes().filter(|&c| c == b'1').count(); // 统计 binary[i..] 中 '1' 的个数
            return "1".repeat(binary.len() - 1 - cnt1) + "0" + &"1".repeat(cnt1);
        }
        binary // binary 全是 '1'
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{binary}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.1 从特殊到一般`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.1 从特殊到一般`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
