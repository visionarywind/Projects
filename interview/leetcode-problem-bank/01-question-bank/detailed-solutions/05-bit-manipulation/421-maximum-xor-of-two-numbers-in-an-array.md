# 421. 数组中两个数的最大异或值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-xor-of-two-numbers-in-an-array/
- 题目 slug：`maximum-xor-of-two-numbers-in-an-array`
- 来源专题：位运算
- 来源分类路径：五、试填法
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-xor-of-two-numbers-in-an-array/solutions/2511644/tu-jie-jian-ji-gao-xiao-yi-tu-miao-dong-1427d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】简洁高效，一图秒懂！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-xor-of-two-numbers-in-an-array/solutions/2511644/tu-jie-jian-ji-gao-xiao-yi-tu-miao-dong-1427d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-jian-ji-gao-xiao-yi-tu-miao-dong-1427d`
- topic id：`2511644`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![位运算入门位运算教程位运算图解 灵茶山艾府 灵神](https://pic.leetcode.cn/1698978713-ouzPib-lc421-c.png)

## 算法

1. 初始化答案 $\textit{ans}=0$。
2. 从最高位开始枚举 $i$，也就是 $\max(\textit{nums})$ 的二进制长度减一。
3. 设 $\textit{newAns} = \textit{ans} + 2^i$，看能否从 $\textit{nums}$ 中选两个数（低于 $i$ 的比特位当作 $0$），满足这两个数的异或和等于 $\textit{newAns}$。如果可以，则更新 $\textit{ans}$ 为 $\textit{newAns}$，否则 $\textit{ans}$ 保持不变。 
4. 判断【两数异或】的做法和力扣第一题【两数之和】是一样的，请看 [我的题解](https://leetcode.cn/problems/two-sum/solution/dong-hua-cong-liang-shu-zhi-he-zhong-wo-0yvmj/)。用 $\oplus$ 表示异或，如果 $a\oplus b = \textit{newAns}$，那么两边同时异或 $b$，由于 $b\oplus b = 0$，所以得到 $a = \textit{newAns} \oplus b$（相当于把两数之和代码中的减法改成异或）。这样就可以一边枚举 $b$，一边在哈希表中查找 $\textit{newAns} \oplus b$ 了。

请注意，$\textit{ans}$ **继承**了上一位算出的内容，后面比特位的计算不是只看当前位，之前算过的高位也得满足。

## 答疑

**问**：如何理解代码中这些位运算的含义？

**答**：关于位运算的技巧整理，请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

**问**：为什么不能从二进制的最低位开始思考？

**答**：如果从最低位开始枚举，可能高位的异或就只能是 $0$ 了，无法满足题目「最大」的要求。

```py [sol-Python3]
class Solution:
    def findMaximumXOR(self, nums: List[int]) -> int:
        ans = mask = 0
        high_bit = max(nums).bit_length() - 1
        for i in range(high_bit, -1, -1):  # 从最高位开始枚举
            mask |= 1 << i
            new_ans = ans | (1 << i)  # 这个比特位可以是 1 吗？
            seen = set()
            for x in nums:
                x &= mask  # 低于 i 的比特位置为 0
                if new_ans ^ x in seen:
                    ans = new_ans  # 这个比特位可以是 1
                    break
                seen.add(x)
        return ans
```

```java [sol-Java]
class Solution {
    public int findMaximumXOR(int[] nums) {
        int max = 0;
        for (int x : nums) {
            max = Math.max(max, x);
        }
        int highBit = 31 - Integer.numberOfLeadingZeros(max);

        int ans = 0, mask = 0;
        Set<Integer> seen = new HashSet<>();
        for (int i = highBit; i >= 0; i--) { // 从最高位开始枚举
            seen.clear();
            mask |= 1 << i;
            int newAns = ans | (1 << i); // 这个比特位可以是 1 吗？
            for (int x : nums) {
                x &= mask; // 低于 i 的比特位置为 0
                if (seen.contains(newAns ^ x)) {
                    ans = newAns; // 这个比特位可以是 1
                    break;
                }
                seen.add(x);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findMaximumXOR(vector<int>& nums) {
        int high_bit = __lg(ranges::max(nums));
        int ans = 0, mask = 0;
        unordered_set<int> seen;
        for (int i = high_bit; i >= 0; i--) { // 从最高位开始枚举
            seen.clear();
            mask |= 1 << i;
            int new_ans = ans | (1 << i); // 这个比特位可以是 1 吗？
            for (int x : nums) {
                x &= mask; // 低于 i 的比特位置为 0
                if (seen.contains(new_ans ^ x)) {
                    ans = new_ans; // 这个比特位可以是 1
                    break;
                }
                seen.insert(x);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func findMaximumXOR(nums []int) (ans int) {
    highBit := bits.Len(uint(slices.Max(nums))) - 1
    seen := map[int]bool{}
    mask := 0
    for i := highBit; i >= 0; i-- { // 从最高位开始枚举
        clear(seen)
        mask |= 1 << i
        newAns := ans | 1<<i // 这个比特位可以是 1 吗？
        for _, x := range nums {
            x &= mask // 低于 i 的比特位置为 0
            if seen[newAns^x] {
                ans = newAns // 这个比特位可以是 1
                break
            }
            seen[x] = true
        }
    }
    return
}
```

```js [sol-JavaScript]
var findMaximumXOR = function (nums) {
    const highBit = 31 - Math.clz32(Math.max(...nums));
    const seen = new Set();
    let ans = 0, mask = 0;
    for (let i = highBit; i >= 0; i--) { // 从最高位开始枚举
        seen.clear();
        mask |= 1 << i;
        const newAns = ans | (1 << i); // 这个比特位可以是 1 吗？
        for (let x of nums) {
            x &= mask; // 低于 i 的比特位置为 0
            if (seen.has(newAns ^ x)) {
                ans = newAns; // 这个比特位可以是 1
                break;
            }
            seen.add(x);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashSet;

impl Solution {
    pub fn find_maximum_xor(nums: Vec<i32>) -> i32 {
        let mx = nums.iter().max().unwrap();
        let high_bit = 31 - mx.leading_zeros() as i32;

        let mut ans = 0;
        let mut mask = 0;
        let mut seen = HashSet::new();
        for i in (0..=high_bit).rev() { // 从最高位开始枚举
            seen.clear();
            mask |= 1 << i;
            let new_ans = ans | (1 << i); // 这个比特位可以是 1 吗？
            for &x in &nums {
                let x = x & mask; // 低于 i 的比特位置为 0
                if seen.contains(&(new_ans ^ x)) {
                    ans = new_ans; // 这个比特位可以是 1
                    break;
                }
                seen.insert(x);
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。外层循环需要循环 $\mathcal{O}(\log U)$ 次。
- 空间复杂度：$\mathcal{O}(n)$。哈希表中至多有 $n$ 个数。

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
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、试填法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、试填法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
