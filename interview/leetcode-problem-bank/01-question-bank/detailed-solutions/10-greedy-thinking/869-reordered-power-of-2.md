# 869. 重新排序得到 2 的幂

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reordered-power-of-2/
- 题目 slug：`reordered-power-of-2`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.4 逆向思维
- 难度分：1505
- 外部题解来源：https://leetcode.cn/problems/reordered-power-of-2/solutions/3748346/ni-xiang-si-wei-pythonjavacgo-by-endless-ehey/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维（Python/Java/C++/Go）](https://leetcode.cn/problems/reordered-power-of-2/solutions/3748346/ni-xiang-si-wei-pythonjavacgo-by-endless-ehey/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ni-xiang-si-wei-pythonjavacgo-by-endless-ehey`
- topic id：`3748346`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

**题意**：把 $n$ 视作一个十进制字符串，判断 $n$ 能否重排为 $1,2,4,8,16,32,\dots$ 这种 $2$ 的幂。不能有前导零。

换句话说，$n$ 和某个 $2^k$ 是**字母异位词**，见 [49. 字母异位词分组](https://leetcode.cn/problems/group-anagrams/)。如果 $n$ 的十进制字符串排序后的字符串，等于某个 $2^k$ 的十进制字符串排序后的字符串，那么返回 $\texttt{true}$。否则返回 $\texttt{false}$。

对于本题的数据范围，我们可以预处理 $[2^0,2^{29}]$ 中的所有 $2$ 的幂的十进制字符串排序后的字符串，保存到一个哈希集合中。预处理后，只需判断 $n$ 的十进制字符串排序后的字符串是否在哈希集合中。

> **注 1**：也可以用长为 $10$ 的数组统计每个数位的出现次数，避免排序。
> 
> **注 2**：也可以做到与 $10^9$ 无关。由于十进制长度与 $n$ 相同的 $2$ 的幂至多只有 $4$ 种（比如最高位为 $1,2,4,8$），所以至多枚举 $4$ 个 $2$ 的幂，看其排序后能否与 $n$ 排序后的结果相同（或者统计数位出现次数）。

```py [sol-Python3]
pow_two_sorted_str_set = {''.join(sorted(str(1 << i))) for i in range(30)}

class Solution:
    def reorderedPowerOf2(self, n: int) -> bool:
        s = ''.join(sorted(str(n)))
        return s in pow_two_sorted_str_set
```

```java [sol-Java]
class Solution {
    private static final Set<String> powTwoSortedStrSet = new HashSet<>();

    static {
        final int MAX_N = 1_000_000_000;
        for (int i = 1; i < MAX_N; i <<= 1) {
            String s = intToSortedStr(i);
            powTwoSortedStrSet.add(s);
        }
    }

    private static String intToSortedStr(int n) {
        char[] s = String.valueOf(n).toCharArray();
        Arrays.sort(s);
        return new String(s);
    }

    public boolean reorderedPowerOf2(int n) {
        String s = intToSortedStr(n);
        return powTwoSortedStrSet.contains(s);
    }
}
```

```cpp [sol-C++]
string int_to_sorted_str(int n) {
    string s = to_string(n);
    ranges::sort(s);
    return s;
}

unordered_set<string> pow_two_sorted_str_set;

int init = []() {
    const int MAX_N = 1'000'000'000;
    for (int i = 1; i < MAX_N; i <<= 1) {
        string s = int_to_sorted_str(i);
        pow_two_sorted_str_set.insert(s);
    }
    return 0;
}();

class Solution {
public:
    bool reorderedPowerOf2(int n) {
        string s = int_to_sorted_str(n);
        return pow_two_sorted_str_set.contains(s);
    }
};
```

```go [sol-Go]
var powTwoSortedStrSet = map[string]bool{}

func init() {
    const maxN = 1_000_000_000
    for i := 1; i < maxN; i <<= 1 {
        s := intToSortedStr(i)
        powTwoSortedStrSet[s] = true
    }
}

func intToSortedStr(n int) string {
    s := []byte(strconv.Itoa(n))
    slices.Sort(s)
    return string(s)
}

func reorderedPowerOf2(n int) bool {
    s := intToSortedStr(n)
    return powTwoSortedStrSet[s]
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(M\log M)$，其中 $M=\log n$ 是 $n$ 的十进制长度。也可以用长为 $C=10$ 的数组统计每个数位的出现次数，避免排序。
- 空间复杂度：$\mathcal{O}(M)$。

## 专题训练

见下面贪心与思维题单的「**§5.3 逆向思维**」。

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

本题来自 `五、思维题 / §5.4 逆向思维`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.4 逆向思维`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
