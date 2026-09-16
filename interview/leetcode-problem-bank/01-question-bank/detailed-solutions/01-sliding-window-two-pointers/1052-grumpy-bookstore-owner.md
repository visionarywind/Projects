# 1052. 爱生气的书店老板

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/grumpy-bookstore-owner/
- 题目 slug：`grumpy-bookstore-owner`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：1418
- 外部题解来源：https://leetcode.cn/problems/grumpy-bookstore-owner/solutions/2751888/ding-chang-hua-dong-chuang-kou-fu-ti-dan-rch7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[定长滑动窗口，附题单！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/grumpy-bookstore-owner/solutions/2751888/ding-chang-hua-dong-chuang-kou-fu-ti-dan-rch7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ding-chang-hua-dong-chuang-kou-fu-ti-dan-rch7`
- topic id：`2751888`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题可以拆分成两个问题：

1. 老板不生气时的顾客数量之和 $s_0$。这些顾客可以感到满意。
2. 长度为 $\textit{minutes}$ 的连续子数组中，老板生气时的顾客数量之和 $s_1$ 的最大值 $\textit{maxS}_1$。这些顾客可以感到满意。

最终答案为 $s_0 + \textit{maxS}_1$。

第二个问题可以用**定长滑动窗口**解决。

例如输入为 $\textit{customers}=[3,1,4,1,5,9],\ \textit{grumpy}=[1,1,1,1,1,1],\ \textit{minutes}=3$：

1. 计算第一个长为 $3$ 的子数组的元素和 $3+1+4=8$。
2. 计算第二个长为 $3$ 的子数组的元素和，我们可以在第一个子数组元素和的基础上，增加 $\textit{customers}[3]=1$，减少 $\textit{customers}[0]=3$，得到 $8+1-3=6$。
3. 计算第三个长为 $3$ 的子数组的元素和，我们可以在第二个子数组元素和的基础上，增加 $\textit{customers}[4]=5$，减少 $\textit{customers}[1]=1$，得到 $6+5-1=10$。
4. 计算第四个长为 $3$ 的子数组的元素和，我们可以在第三个子数组元素和的基础上，增加 $\textit{customers}[5]=9$，减少 $\textit{customers}[2]=4$，得到 $10+9-4=15$。

最大的长为 $3$ 的子数组和为 $15$。

## 答疑

**问**：为什么要分开计算不生气和生气时的顾客？

**答**：因为不生气时的顾客可以在窗口外面，需独立计算。

```py [sol-Python3]
class Solution:
    def maxSatisfied(self, customers: List[int], grumpy: List[int], minutes: int) -> int:
        s = [0, 0]
        max_s1 = 0
        for i, (c, g) in enumerate(zip(customers, grumpy)):
            s[g] += c
            left = i - minutes + 1  # 窗口左端点
            if left < 0:  # 窗口长度不足 minutes
                continue
            max_s1 = max(max_s1, s[1])
            if grumpy[left]:
                s[1] -= customers[left]  # 窗口最左边元素离开窗口
        return s[0] + max_s1
```

```java [sol-Java]
class Solution {
    public int maxSatisfied(int[] customers, int[] grumpy, int minutes) {
        int[] s = new int[2];
        int maxS1 = 0;
        for (int i = 0; i < customers.length; i++) {
            s[grumpy[i]] += customers[i];
            int left = i - minutes + 1; // 窗口左端点
            if (left < 0) { // 窗口长度不足 minutes
                continue;
            }
            maxS1 = Math.max(maxS1, s[1]);
            // 窗口最左边元素离开窗口
            s[1] -= grumpy[left] > 0 ? customers[left] : 0;
        }
        return s[0] + maxS1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSatisfied(vector<int>& customers, vector<int>& grumpy, int minutes) {
        int s[2]{}, max_s1 = 0;
        for (int i = 0; i < customers.size(); i++) {
            s[grumpy[i]] += customers[i];
            int left = i - minutes + 1; // 窗口左端点
            if (left < 0) { // 窗口长度不足 minutes
                continue;
            }
            max_s1 = max(max_s1, s[1]);
            // 窗口最左边元素离开窗口
            s[1] -= grumpy[left] ? customers[left] : 0;
        }
        return s[0] + max_s1;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxSatisfied(int* customers, int customersSize, int* grumpy, int grumpySize, int minutes) {
    int s[2] = {}, max_s1 = 0;
    for (int i = 0; i < customersSize; i++) {
        s[grumpy[i]] += customers[i];
        int left = i - minutes + 1; // 窗口左端点
        if (left < 0) { // 窗口长度不足 minutes
            continue;
        }
        max_s1 = MAX(max_s1, s[1]);
        // 窗口最左边元素离开窗口
        s[1] -= grumpy[left] ? customers[left] : 0;
    }
    return s[0] + max_s1;
}
```

```go [sol-Go]
func maxSatisfied(customers []int, grumpy []int, minutes int) int {
    s := [2]int{}
    maxS1 := 0
    for i, c := range customers {
        s[grumpy[i]] += c
        left := i - minutes + 1 // 窗口左端点
        if left < 0 { // 窗口长度不足 minutes
            continue
        }
        maxS1 = max(maxS1, s[1])
        if grumpy[left] > 0 {
            s[1] -= customers[left] // 窗口最左边元素离开窗口
        }
    }
    return s[0] + maxS1
}
```

```js [sol-JavaScript]
var maxSatisfied = function(customers, grumpy, minutes) {
    const s = [0, 0];
    let maxS1 = 0;
    for (let i = 0; i < customers.length; i++) {
        s[grumpy[i]] += customers[i];
        const left = i - minutes + 1; // 窗口左端点
        if (left < 0) { // 窗口长度不足 minutes
            continue;
        }
        maxS1 = Math.max(maxS1, s[1]);
        // 窗口最左边元素离开窗口
        s[1] -= grumpy[left] ? customers[left] : 0;
    }
    return s[0] + maxS1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_satisfied(customers: Vec<i32>, grumpy: Vec<i32>, minutes: i32) -> i32 {
        let k = minutes as usize;
        let mut s = [0, 0];
        let mut max_s1 = 0;
        for (i, (&c, &g)) in customers.iter().zip(grumpy.iter()).enumerate() {
            s[g as usize] += c;
            if i < k - 1 { // 窗口长度不足 minutes
                continue;
            }
            max_s1 = max_s1.max(s[1]);
            if grumpy[i - k + 1] == 1 {
                s[1] -= customers[i - k + 1]; // 窗口最左边元素离开窗口
            }
        }
        s[0] + max_s1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{customers}$ 的长度。
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

本题来自 `一、定长滑动窗口 / §1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、定长滑动窗口 / §1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
