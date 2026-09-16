# 1652. 拆炸弹

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/defuse-the-bomb/
- 题目 slug：`defuse-the-bomb`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/defuse-the-bomb/solutions/2765762/on-ding-chang-hua-dong-chuang-kou-python-y2py/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 定长滑动窗口（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/defuse-the-bomb/solutions/2765762/on-ding-chang-hua-dong-chuang-kou-python-y2py/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-ding-chang-hua-dong-chuang-kou-python-y2py`
- topic id：`2765762`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 举例

例如 $\textit{code}=[3,1,4,1,5,9],\ k=3$：

1. 计算 $\textit{ans}[0]$，即子数组 $[1,4,1]$ 的元素和 $1+4+1=6$。
2. 计算 $\textit{ans}[1]$，即子数组 $[4,1,5]$ 的元素和，我们可以在 $[1,4,1]$ 的基础上，增加 $\textit{code}[4]=5$，减少 $\textit{code}[1]=1$，得到 $6+5-1=10$。
3. 计算 $\textit{ans}[2]$，即子数组 $[1,5,9]$ 的元素和，我们可以在 $[4,1,5]$ 的基础上，增加 $\textit{code}[5]=9$，减少 $\textit{code}[2]=4$，得到 $10+9-4=15$。
4. 计算 $\textit{ans}[3]$，即子数组 $[5,9,3]$ 的元素和，我们可以在 $[1,5,9]$ 的基础上，增加 $\textit{code}[6\bmod 6]=\textit{code}[0] = 3$，减少 $\textit{code}[3]=1$，得到 $15+3-1=17$。
5. 计算 $\textit{ans}[4]$，即子数组 $[9,3,1]$ 的元素和，我们可以在 $[5,9,3]$ 的基础上，增加 $\textit{code}[7\bmod 6]=\textit{code}[1] = 1$，减少 $\textit{code}[4]=5$，得到 $17+1-5=13$。
6. 计算 $\textit{ans}[5]$，即子数组 $[3,1,4]$ 的元素和，我们可以在 $[9,3,1]$ 的基础上，增加 $\textit{code}[8\bmod 6]=\textit{code}[2] = 4$，减少 $\textit{code}[5]=9$，得到 $13+4-9=8$。

## 分析

注意到，无论 $k>0$ 还是 $k<0$，**窗口都在向右移动**，只有初始位置不同。所以找到第一个窗口的下标范围，就可以开滑了！

- $k>0$，第一个窗口为左闭右开区间 $[1,k+1)$。
- $k<0$，第一个窗口为左闭右开区间 $[n-|k|,n)$。
- 无论 $k$ 是正是负，窗口的大小都是 $|k|$。

在窗口向右滑动时，设移入窗口的元素下标为 $r\bmod n$，则移出窗口的元素下标为 $(r-|k|)\bmod n$。

代码实现时，$k=0$ 的特判可以省略。

```py [sol-Python3]
class Solution:
    def decrypt(self, code: List[int], k: int) -> List[int]:
        n = len(code)
        r = k + 1 if k > 0 else n  # 第一个窗口的右开端点
        k = abs(k)
        s = sum(code[r - k: r])  # 第一个窗口的元素和

        ans = [0] * n
        for i in range(n):
            ans[i] = s
            s += code[r % n] - code[(r - k) % n]
            r += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int[] decrypt(int[] code, int k) {
        int n = code.length;
        // 计算第一个窗口的右开端点
        int r = k > 0 ? k + 1 : n;
        k = Math.abs(k);

        // 计算第一个窗口的元素和
        int s = 0;
        for (int i = r - k; i < r; i++) {
            s += code[i]; 
        }

        int[] ans = new int[n];
        for (int i = 0; i < n; i++) {
            ans[i] = s;
            s += code[r % n] - code[(r - k) % n];
            r++;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> decrypt(vector<int>& code, int k) {
        int n = code.size();
        // 计算第一个窗口的右开端点
        int r = k > 0 ? k + 1 : n;
        k = abs(k);

        // 计算第一个窗口的元素和
        int s = reduce(code.begin() + r - k, code.begin() + r);

        vector<int> ans(n);
        for (int i = 0; i < n; i++) {
            ans[i] = s;
            s += code[r % n] - code[(r - k) % n];
            r++;
        }
        return ans;
    }
};
```

```go [sol-Go]
func decrypt(code []int, k int) []int {
    n := len(code)
    // 计算第一个窗口的右开端点
    r := k + 1
    if k < 0 {
        r = n
        k = -k
    }

    // 计算第一个窗口的元素和
    s := 0
    for _, x := range code[r-k : r] {
        s += x
    }

    ans := make([]int, n)
    for i := range ans {
        ans[i] = s
        s += code[r%n] - code[(r-k)%n]
        r++
    }
    return ans
}
```

```js [sol-JavaScript]
var decrypt = function(code, k) {
    const n = code.length;
    // 计算第一个窗口的右开端点
    let r = k > 0 ? k + 1 : n;
    k = Math.abs(k);

    // 计算第一个窗口的元素和
    let s = 0;
    for (let i = r - k; i < r; i++) {
        s += code[i];
    }

    const ans = Array(n);
    for (let i = 0; i < n; i++) {
        ans[i] = s;
        s += code[r % n] - code[(r - k) % n];
        r++;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn decrypt(code: Vec<i32>, k: i32) -> Vec<i32> {
        let n = code.len();
        // 计算第一个窗口的右开端点
        let mut r = if k > 0 { k as usize + 1 } else { n };
        let k = k.abs() as usize;

        // 计算第一个窗口的元素和
        let mut s = code[r - k..r].iter().sum::<i32>();

        let mut ans = vec![0; n];
        for i in 0..n {
            ans[i] = s;
            s += code[r % n] - code[(r - k) % n];
            r += 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{code}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。Python 忽略切片的空间。

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
