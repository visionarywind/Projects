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
