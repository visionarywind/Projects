# 1819. 序列中不同最大公约数的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-different-subsequences-gcds/
- 题目 slug：`number-of-different-subsequences-gcds`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.6 最大公约数（GCD）
- 难度分：2540
- 外部题解来源：https://leetcode.cn/problems/number-of-different-subsequences-gcds/solutions/2061079/ji-bai-100mei-ju-gcdxun-huan-you-hua-pyt-get7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[击败100%！枚举GCD+循环优化（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-different-subsequences-gcds/solutions/2061079/ji-bai-100mei-ju-gcdxun-huan-you-hua-pyt-get7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-bai-100mei-ju-gcdxun-huan-you-hua-pyt-get7`
- topic id：`2061079`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 横看成岭侧成峰。

由于非空子序列的数量高达 $2^n-1$，直接回溯枚举是会超时的。不妨换一个视角，考虑**值域**。

多个数的最大公约数等于 $g$，也反过来说明这些数都是 $g$ 的倍数。例如 $[8,12,6]$ 的最大公约数是 $2$，这些数都是 $2$ 的倍数。

### 【值域】【倍数】

那么，能不能反过来，枚举 $g$ 的倍数呢？（找在 $\textit{nums}$ 中的倍数）

- $1,2,3,\cdots$
- $2,4,6,\cdots$
- $3,6,9,\cdots$

看上去运行时间是平方级别，会超时。

### 先别急着否定，这里得来点数学。

设 $U=\max(\textit{nums})$，那么 $1$ 的倍数需要枚举 $\left\lfloor\dfrac{U}{1}\right\rfloor$ 个，$2$ 的倍数需要枚举 $\left\lfloor\dfrac{U}{2}\right\rfloor$ 个，……，把这些加起来，去掉下取整，有

$$
\left\lfloor\dfrac{U}{1}\right\rfloor + \left\lfloor\dfrac{U}{2}\right\rfloor +\cdots + \left\lfloor\dfrac{U}{U}\right\rfloor \le U\cdot\left(\dfrac{1}{1} + \dfrac{1}{2} + \cdots + \dfrac{1}{U}\right)
$$

右边括号中的叫做 [调和级数部分和](https://baike.baidu.com/item/%E8%B0%83%E5%92%8C%E7%BA%A7%E6%95%B0/8019971?fr=aladdin#3)，可以看成是 $\mathcal{O}(\log U)$ 的，因此枚举倍数的时间复杂度为 $\mathcal{O}(U\log U)$，不会超时。

那么就枚举 $i=1,2,\cdots,U$ 及其倍数，当作子序列中的数。

### 子序列中的数越多，$g$ 就可能越小，就越可能等于 $i$。

例如，如果枚举 $i=2$ 的倍数，其中 $8$ 和 $12$ 是在 $\textit{nums}$ 中的，但由于 $8$ 和 $12$ 的最大公约数等于 $4$，所以无法找到一个子序列，其最大公约数为 $i$。但如果还有 $6$ 也在 $\textit{nums}$ 中，那么最大公约数等于 $2$，这样 $i$ 就可以是一个子序列的最大公约数了。

代码实现时，需要用哈希表或者数组，记录每个数是否在 $\textit{nums}$ 中，从而加快判断。数组的效率会更高一些。

### 优化前

```py [sol-Python3]
class Solution:
    def countDifferentSubsequenceGCDs(self, nums: List[int]) -> int:
        mx = max(nums)
        has = [False] * (mx + 1)
        for x in nums:
            has[x] = True

        ans = 0
        for i in range(1, mx + 1):
            g = 0  # 0 和任何数 x 的最大公约数都是 x
            for j in range(i, mx + 1, i):  # 枚举 i 的倍数 j
                if has[j]:  # 如果 j 在 nums 中
                    g = gcd(g, j)  # 更新最大公约数
                    if g == i:  # 找到一个答案（g 无法继续减小）
                        ans += 1
                        break  # 提前退出循环
        return ans
```

```java [sol-Java]
class Solution {
    public int countDifferentSubsequenceGCDs(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }
        boolean[] has = new boolean[mx + 1];
        for (int x : nums) {
            has[x] = true;
        }
        
        int ans = 0;
        for (int i = 1; i <= mx; i++) {
            int g = 0; // 0 和任何数 x 的最大公约数都是 x
            for (int j = i; j <= mx && g != i; j += i) { // 枚举 i 的倍数 j
                if (has[j]) { // 如果 j 在 nums 中
                    g = gcd(g, j); // 更新最大公约数
                }
            }
            if (g == i) {
                ans++; // 找到一个答案
            }
        }
        return ans;
    }

    // 迭代写法比递归写法快
    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countDifferentSubsequenceGCDs(vector<int>& nums) {
        int mx = ranges::max(nums);
        vector<int> has(mx + 1);
        for (int x : nums) {
            has[x] = true;
        }
        
        int ans = 0;
        for (int i = 1; i <= mx; i++) {
            int g = 0; // 0 和任何数 x 的最大公约数都是 x
            for (int j = i; j <= mx && g != i; j += i) { // 枚举 i 的倍数 j
                if (has[j]) { // 如果 j 在 nums 中
                    g = __gcd(g, j); // 更新最大公约数
                    // 注：__gcd 比 gcd 更快
                }
            }
            if (g == i) {
                ans++; // 找到一个答案
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countDifferentSubsequenceGCDs(nums []int) (ans int) {
    mx := slices.Max(nums)
    has := make([]bool, mx+1)
    for _, x := range nums {
        has[x] = true
    }

    for i := 1; i <= mx; i++ {
        g := 0 // 0 和任何数 x 的最大公约数都是 x
        for j := i; j <= mx && g != i; j += i { // 枚举 i 的倍数 j
            if has[j] { // 如果 j 在 nums 中
                g = gcd(g, j) // 更新最大公约数
            }
        }
        if g == i { // 找到一个答案
            ans++
        }
    }
    return
}

func gcd(a, b int) int {
    for a != 0 {
        a, b = b%a, a
    }
    return b
}
```

### 优化

答案可以由两部分组成：

- 子序列的长度为 $1$，此时最大公约数等于 $\textit{nums}[i]$，这部分可以给答案贡献 $m$ 个，这里 $m$ 为 $\textit{nums}$ 中不同元素的个数。
- 子序列的长度至少为 $2$，为了避免重复统计，此时最大公约数 $i$ 必须不在 $\textit{nums}$ 中。此外，要想使最大公约数为 $i$，$\textit{nums}$ 中**最小**要有 $2i$ 和 $3i$ 这两个数，这样最大公约数才能是 $i$。因此，**$i$ 只需要枚举到 $\left\lfloor\dfrac{U}{3}\right\rfloor$**。

凭借这个优化，下面的代码可以在时间上击败 $100\%$（截至本文发布时）。

```py [sol-Python3]
class Solution:
    def countDifferentSubsequenceGCDs(self, nums: List[int]) -> int:
        ans = 0
        mx = max(nums)
        has = [False] * (mx + 1)
        for x in nums:
            if not has[x]:
                has[x] = True
                ans += 1  # 单独一个数也算

        for i in range(1, mx // 3 + 1):  # 优化循环上界
            if has[i]:
                continue
            g = 0  # 0 和任何数 x 的最大公约数都是 x
            for j in range(i * 2, mx + 1, i):  # 枚举 i 的倍数 j
                if has[j]:  # 如果 j 在 nums 中
                    g = gcd(g, j)  # 更新最大公约数
                    if g == i:  # 找到一个答案（g 无法继续减小）
                        ans += 1
                        break  # 提前退出循环
        return ans
```

```java [sol-Java]
class Solution {
    public int countDifferentSubsequenceGCDs(int[] nums) {
        int ans = 0;
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }
        boolean[] has = new boolean[mx + 1];
        for (int x : nums) {
            if (!has[x]) {
                has[x] = true;
                ans++; // 单独一个数也算
            }
        }

        for (int i = 1; i <= mx / 3; i++) { // 优化循环上界
            if (has[i]) {
                continue;
            }
            int g = 0; // 0 和任何数 x 的最大公约数都是 x
            for (int j = i * 2; j <= mx && g != i; j += i) { // 枚举 i 的倍数 j
                if (has[j]) { // 如果 j 在 nums 中
                    g = gcd(g, j); // 更新最大公约数
                }
            }
            if (g == i) {
                ans++; // 找到一个答案
            }
        }
        return ans;
    }

    // 迭代写法比递归写法快
    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countDifferentSubsequenceGCDs(vector<int>& nums) {
        int ans = 0;
        int mx = ranges::max(nums);
        vector<int> has(mx + 1);
        for (int x : nums) {
            if (!has[x]) {
                has[x] = true;
                ans++; // 单独一个数也算
            }
        }

        for (int i = 1; i <= mx / 3; i++) { // 优化循环上界
            if (has[i]) {
                continue;
            }
            int g = 0; // 0 和任何数 x 的最大公约数都是 x
            for (int j = i * 2; j <= mx && g != i; j += i) { // 枚举 i 的倍数 j
                if (has[j]) { // 如果 j 在 nums 中
                    g = __gcd(g, j); // 更新最大公约数
                    // 注：__gcd 比 gcd 更快
                }
            }
            if (g == i) {
                ans++; // 找到一个答案
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countDifferentSubsequenceGCDs(nums []int) (ans int) {
    mx := slices.Max(nums)
    has := make([]bool, mx+1)
    for _, x := range nums {
        if !has[x] {
            has[x] = true
            ans++
        }
    }

    for i := 1; i <= mx/3; i++ {
        if has[i] {
            continue
        }
        g := 0 // 0 和任何数 x 的最大公约数都是 x
        for j := i * 2; j <= mx && g != i; j += i { // 枚举 i 的倍数 j
            if has[j] { // 如果 j 在 nums 中
                g = gcd(g, j) // 更新最大公约数
            }
        }
        if g == i { // 找到一个答案
            ans++
        }
    }
    return
}

func gcd(a, b int) int {
    for a != 0 {
        a, b = b%a, a
    }
    return b
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+U\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。二重循环的时间复杂度分两部分，第一部分是**二重循环的次数**，根据 [调和级数部分和](https://baike.baidu.com/item/%E8%B0%83%E5%92%8C%E7%BA%A7%E6%95%B0/8019971?fr=aladdin#3)，循环次数为 $\mathcal{O}(U\log U)$；第二部分是**计算 $g$ 的总时间**，由于 $g$ 每次计算要么不变，要么至少减半，所以实际上 $g$ 在内层循环中至多减半 $\mathcal{O}(\log U)$ 次，计算 $g$ 的时间对于每个 $i$ 都是 $\mathcal{O}(\log U)$ 的，因此这部分的时间复杂度也为 $\mathcal{O}(U\log U)$。最后，加上遍历 $\textit{nums}$ 的 $\mathcal{O}(n)$ 时间，总的时间复杂度为 $\mathcal{O}(n+U\log U)$。
- 空间复杂度：$\mathcal{O}(U)$。

更多题目，见下面数学题单中的 GCD 小节。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.6 最大公约数（GCD）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.6 最大公约数（GCD）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
