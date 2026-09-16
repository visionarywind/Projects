# 2105. 给植物浇水 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/watering-plants-ii/
- 题目 slug：`watering-plants-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：1507
- 外部题解来源：https://leetcode.cn/problems/watering-plants-ii/solutions/1153072/shuang-zhi-zhen-mo-ni-by-endlesscheng-9l76/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[相向双指针模拟（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/watering-plants-ii/solutions/1153072/shuang-zhi-zhen-mo-ni-by-endlesscheng-9l76/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shuang-zhi-zhen-mo-ni-by-endlesscheng-9l76`
- topic id：`1153072`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

1. 初始化答案 $\textit{ans}=0$，Alice 水罐的初始水量 $a = \textit{capacityA}$，Bob 水罐的初始水量 $b = \textit{capacityB}$。
2. 初始化左右指针 $i=0,\ j=n-1$。
3. 循环直到 $i\ge j$。每次循环，对于 Alice，如果 $a < \textit{plants}[i]$，那么 Alice 需要重新灌满水罐，$a$ 重置为 $\textit{capacityA}$，答案加一。然后把 $a$ 减少 $\textit{plants}[i]$，左指针 $i$ 加一。对于 Bob，如果 $b < \textit{plants}[j]$，那么 Bob 需要重新灌满水罐，$b$ 重置为 $\textit{capacityB}$，答案加一。然后把 $b$ 减少 $\textit{plants}[j]$，右指针 $j$ 减一。
4. 循环结束后，如果 $i=j$ 且 $\max(a,b) < \textit{plants}[i]$，则需要重新灌满水罐，答案加一。
5. 返回答案。

⚠**注意**：灌满水后要**立刻浇水**，不能跳过。

```py [sol-Python3]
class Solution:
    def minimumRefill(self, plants: List[int], capacityA: int, capacityB: int) -> int:
        ans = 0
        a, b = capacityA, capacityB
        i, j = 0, len(plants) - 1
        while i < j:
            # Alice 给植物 i 浇水
            if a < plants[i]:
                # 没有足够的水，重新灌满水罐
                ans += 1
                a = capacityA
            a -= plants[i]
            i += 1
            # Bob 给植物 j 浇水
            if b < plants[j]:
                # 没有足够的水，重新灌满水罐
                ans += 1
                b = capacityB
            b -= plants[j]
            j -= 1
        # 如果 Alice 和 Bob 到达同一株植物，那么当前水罐中水更多的人会给这株植物浇水
        if i == j and max(a, b) < plants[i]:
            # 没有足够的水，重新灌满水罐
            ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumRefill(int[] plants, int capacityA, int capacityB) {
        int ans = 0;
        int a = capacityA;
        int b = capacityB;
        int i = 0;
        int j = plants.length - 1;
        while (i < j) {
            // Alice 给植物 i 浇水
            if (a < plants[i]) {
                // 没有足够的水，重新灌满水罐
                ans++;
                a = capacityA;
            }
            a -= plants[i++];
            // Bob 给植物 j 浇水
            if (b < plants[j]) {
                // 没有足够的水，重新灌满水罐
                ans++;
                b = capacityB;
            }
            b -= plants[j--];
        }
        // 如果 Alice 和 Bob 到达同一株植物，那么当前水罐中水更多的人会给这株植物浇水
        if (i == j && Math.max(a, b) < plants[i]) {
            // 没有足够的水，重新灌满水罐
            ans++;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumRefill(vector<int>& plants, int capacityA, int capacityB) {
        int ans = 0;
        int a = capacityA, b = capacityB;
        int i = 0, j = plants.size() - 1;
        while (i < j) {
            // Alice 给植物 i 浇水
            if (a < plants[i]) {
                // 没有足够的水，重新灌满水罐
                ans++;
                a = capacityA;
            }
            a -= plants[i++];
            // Bob 给植物 j 浇水
            if (b < plants[j]) {
                // 没有足够的水，重新灌满水罐
                ans++;
                b = capacityB;
            }
            b -= plants[j--];
        }
        // 如果 Alice 和 Bob 到达同一株植物，那么当前水罐中水更多的人会给这株植物浇水
        if (i == j && max(a, b) < plants[i]) {
            // 没有足够的水，重新灌满水罐
            ans++;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumRefill(plants []int, capacityA, capacityB int) (ans int) {
    a, b := capacityA, capacityB
    i, j := 0, len(plants)-1
    for i < j {
        // Alice 给植物 i 浇水
        if a < plants[i] {
            // 没有足够的水，重新灌满水罐
            ans++
            a = capacityA
        }
        a -= plants[i]
        i++
        // Bob 给植物 j 浇水
        if b < plants[j] {
            // 没有足够的水，重新灌满水罐
            ans++
            b = capacityB
        }
        b -= plants[j]
        j--
    }
    // 如果 Alice 和 Bob 到达同一株植物，那么当前水罐中水更多的人会给这株植物浇水
    if i == j && max(a, b) < plants[i] {
        // 没有足够的水，重新灌满水罐
        ans++
    }
    return
}
```

```js [sol-JavaScript]
var minimumRefill = function(plants, capacityA, capacityB) {
    let ans = 0;
    let a = capacityA, b = capacityB;
    let i = 0, j = plants.length - 1;
    while (i < j) {
        // Alice 给植物 i 浇水
        if (a < plants[i]) {
            // 没有足够的水，重新灌满水罐
            ans++;
            a = capacityA;
        }
        a -= plants[i++];
        // Bob 给植物 j 浇水
        if (b < plants[j]) {
            // 没有足够的水，重新灌满水罐
            ans++;
            b = capacityB;
        }
        b -= plants[j--];
    }
    // 如果 Alice 和 Bob 到达同一株植物，那么当前水罐中水更多的人会给这株植物浇水
    if (i === j && Math.max(a, b) < plants[i]) {
        // 没有足够的水，重新灌满水罐
        ans++;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_refill(plants: Vec<i32>, capacity_a: i32, capacity_b: i32) -> i32 {
        let mut ans = 0;
        let mut a = capacity_a;
        let mut b = capacity_b;
        let mut i = 0;
        let mut j = plants.len() - 1;
        while i < j {
            // Alice 给植物 i 浇水
            if a < plants[i] {
                // 没有足够的水，重新灌满水罐
                ans += 1;
                a = capacity_a;
            }
            a -= plants[i];
            i += 1;
            // Bob 给植物 j 浇水
            if b < plants[j] {
                // 没有足够的水，重新灌满水罐
                ans += 1;
                b = capacity_b;
            }
            b -= plants[j];
            j -= 1;
        }
        // 如果 Alice 和 Bob 到达同一株植物，那么当前水罐中水更多的人会给这株植物浇水
        if i == j && a.max(b) < plants[i] {
            // 没有足够的水，重新灌满水罐
            ans += 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{plants}$ 的长度。
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

本题来自 `三、单序列双指针 / §3.2 相向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.2 相向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
