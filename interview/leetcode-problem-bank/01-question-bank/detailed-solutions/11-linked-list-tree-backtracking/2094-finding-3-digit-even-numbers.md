# 2094. 找出 3 位偶数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/finding-3-digit-even-numbers/
- 题目 slug：`finding-3-digit-even-numbers`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.7 搜索
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/finding-3-digit-even-numbers/solutions/1139403/mei-ju-suo-you-san-wei-shu-ou-shu-by-end-8n7d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力枚举 / 回溯（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/finding-3-digit-even-numbers/solutions/1139403/mei-ju-suo-you-san-wei-shu-ou-shu-by-end-8n7d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-suo-you-san-wei-shu-ou-shu-by-end-8n7d`
- topic id：`1139403`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 方法一：暴力枚举

枚举所有三位偶数 $i=100,102,104,\ldots,998$。

统计 $i$ 中的每个数字 $d=0,1,2,\ldots,9$ 的出现次数。比如 $i=666$，其中 $d=6$ 出现了 $3$ 次，但如果 $\textit{digits}$ 中只有 $2$ 个 $6$，那么 $i$ 无法由 $\textit{digits}$ 中的数字组成，不能加到答案中。

为了知道 $\textit{digits}$ 中的数字个数，可以在枚举 $i$ 之前，统计 $\textit{digits}$ 中的每个数字的个数。

```py [sol-Py3]
class Solution:
    def findEvenNumbers(self, digits: List[int]) -> List[int]:
        cnt = Counter(digits)
        ans = []
        # 枚举所有三位数偶数
        for i in range(100, 1000, 2):
            # digits 有充足的数字组成 i
            if Counter(map(int, str(i))) <= cnt:
                ans.append(i)
        return ans
```

```py [sol-Py3 两行]
class Solution:
    def findEvenNumbers(self, digits: List[int]) -> List[int]:
        cnt = Counter(digits)
        return [i for i in range(100, 1000, 2) if Counter(map(int, str(i))) <= cnt]
```

```py [sol-Py3 不用字符串]
class Solution:
    def findEvenNumbers(self, digits: List[int]) -> List[int]:
        cnt = [0] * 10
        for d in digits:
            cnt[d] += 1

        ans = []
        for i in range(100, 1000, 2):  # 枚举所有三位数偶数 i
            c = [0] * 10
            x = i
            while x > 0:
                x, d = divmod(x, 10)
                c[d] += 1
                # 如果 i 中 d 的个数比 digits 中的还多，那么 i 无法由 digits 中的数字组成
                if c[d] > cnt[d]:
                    break
            else:  # 没有中途 break
                ans.append(i)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] findEvenNumbers(int[] digits) {
        int[] cnt = new int[10];
        for (int d : digits) {
            cnt[d]++;
        }

        List<Integer> ans = new ArrayList<>();
        next:
        for (int i = 100; i < 1000; i += 2) { // 枚举所有三位数偶数 i
            int[] c = new int[10];
            for (int x = i; x > 0; x /= 10) { // 枚举 i 的每一位 d
                int d = x % 10;
                // 如果 i 中 d 的个数比 digits 中的还多，那么 i 无法由 digits 中的数字组成
                if (++c[d] > cnt[d]) { 
                    continue next; // 枚举下一个偶数
                }
            }
            ans.add(i);
        }
        return ans.stream().mapToInt(i -> i).toArray();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findEvenNumbers(vector<int>& digits) {
        int cnt[10]{};
        for (int d : digits) {
            cnt[d]++;
        }

        vector<int> ans;
        for (int i = 100; i < 1000; i += 2) { // 枚举所有三位数偶数 i
            int c[10]{};
            bool ok = true;
            for (int x = i; x > 0; x /= 10) { // 枚举 i 的每一位 d
                int d = x % 10;
                // 如果 i 中 d 的个数比 digits 中的还多，那么 i 无法由 digits 中的数字组成
                if (++c[d] > cnt[d]) { 
                    ok = false;
                    break;
                }
            }
            if (ok) {
                ans.push_back(i);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func findEvenNumbers(digits []int) (ans []int) {
    cnt := [10]int{}
    for _, d := range digits {
        cnt[d]++
    }

next:
    for i := 100; i < 1000; i += 2 { // 枚举所有三位数偶数 i
        c := [10]int{}
        for x := i; x > 0; x /= 10 { // 枚举 i 的每一位 d
            d := x % 10
            c[d]++
            // 如果 i 中 d 的个数比 digits 中的还多，那么 i 无法由 digits 中的数字组成
            if c[d] > cnt[d] {
                continue next // 枚举下一个偶数
            }
        }
        ans = append(ans, i)
    }
    return
}
```

```js [sol-JavaScript]
var findEvenNumbers = function(digits) {
    const cnt = Array(10).fill(0);
    for (const d of digits) {
        cnt[d]++;
    }

    const ans = [];
    for (let i = 100; i < 1000; i += 2) { // 枚举所有三位数偶数 i
        const c = Array(10).fill(0);
        let ok = true;
        for (let x = i; x > 0; x = Math.floor(x / 10)) { // 枚举 i 的每一位 d
            const d = x % 10;
            // 如果 i 中 d 的个数比 digits 中的还多，那么 i 无法由 digits 中的数字组成
            if (++c[d] > cnt[d]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            ans.push(i);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_even_numbers(digits: Vec<i32>) -> Vec<i32> {
        let mut cnt = [0; 10];
        for d in digits {
            cnt[d as usize] += 1;
        }

        let mut ans = vec![];
        for i in (100..1000).step_by(2) { // 枚举所有三位数偶数 i
            let mut c = [0; 10];
            let mut x = i;
            let mut ok = true;
            while x > 0 {
                let d = x % 10;
                c[d] += 1;
                // 如果 i 中 d 的个数比 digits 中的还多，那么 i 无法由 digits 中的数字组成
                if c[d] > cnt[d] {
                    ok = false;
                    break;
                }
                x /= 10;
            }
            if ok {
                ans.push(i as i32);
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + k 10^k)$，其中 $n$ 是 $\textit{digits}$ 的长度，$k=3$。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。$\textit{cnt}$ 的大小视作 $\mathcal{O}(1)$。

## 方法二：回溯

**前置知识**：[回溯【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)

换一个角度，枚举百位数填什么，十位数填什么，个位数填什么。

- 百位数不能填 $0$。
- 十位数随便填。
- 个位数只能填偶数。

用回溯实现上述枚举，注意填的数字个数必须有剩余，或者说不能超过 $\textit{digits}$ 中的数字个数。

```py [sol-Python3]
class Solution:
    def findEvenNumbers(self, digits: List[int]) -> List[int]:
        cnt = [0] * 10
        for d in digits:
            cnt[d] += 1

        ans = []
        # i=0 百位，i=1 十位，i=2 个位，x 表示当前正在构造的数字
        def dfs(i: int, x: int):
            if i == 3:
                ans.append(x)
                return
            for d, c in enumerate(cnt):
                if c > 0 and (i == 0 and d > 0 or i == 1 or i == 2 and d % 2 == 0):
                    cnt[d] -= 1  # 消耗一个数字 d
                    dfs(i + 1, x * 10 + d)
                    cnt[d] += 1  # 复原
        dfs(0, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] findEvenNumbers(int[] digits) {
        int[] cnt = new int[10];
        for (int d : digits) {
            cnt[d]++;
        }

        List<Integer> ans = new ArrayList<>();
        dfs(0, 0, cnt, ans);
        return ans.stream().mapToInt(i -> i).toArray();
    }

    // i=0 百位，i=1 十位，i=2 个位，x 表示当前正在构造的数字
    private void dfs(int i, int x, int[] cnt, List<Integer> ans) {
        if (i == 3) {
            ans.add(x);
            return;
        }
        for (int d = 0; d < 10; d++) {
            if (cnt[d] > 0 && (i == 0 && d > 0 || i == 1 || i == 2 && d % 2 == 0)) {
                cnt[d]--; // 消耗一个数字 d
                dfs(i + 1, x * 10 + d, cnt, ans);
                cnt[d]++; // 复原
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findEvenNumbers(vector<int>& digits) {
        int cnt[10]{};
        for (int d : digits) {
            cnt[d]++;
        }

        vector<int> ans;
        // i=0 百位，i=1 十位，i=2 个位，x 表示当前正在构造的数字
        auto dfs = [&](this auto&& dfs, int i, int x) -> void {
            if (i == 3) {
                ans.push_back(x);
                return;
            }
            for (int d = 0; d < 10; d++) {
                if (cnt[d] > 0 && (i == 0 && d > 0 || i == 1 || i == 2 && d % 2 == 0)) {
                    cnt[d]--; // 消耗一个数字 d
                    dfs(i + 1, x * 10 + d);
                    cnt[d]++; // 复原
                }
            }
        };
        dfs(0, 0);
        return ans;
    }
};
```

```go [sol-Go]
func findEvenNumbers(digits []int) (ans []int) {
    cnt := make([]int, 10)
    for _, d := range digits {
        cnt[d]++
    }

    // i=0 百位，i=1 十位，i=2 个位，x 表示当前正在构造的数字
    var dfs func(i, x int)
    dfs = func(i, x int) {
        if i == 3 {
            ans = append(ans, x)
            return
        }
        for d, c := range cnt {
            if c > 0 && (i == 0 && d > 0 || i == 1 || i == 2 && d%2 == 0) {
                cnt[d]-- // 消耗一个数字 d
                dfs(i+1, x*10+d)
                cnt[d]++ // 复原
            }
        }
    }
    dfs(0, 0)
    return
}
```

```js [sol-JavaScript]
var findEvenNumbers = function(digits) {
    const cnt = Array(10).fill(0);
    for (const d of digits) {
        cnt[d]++;
    }

    const ans = [];
    // i=0 百位，i=1 十位，i=2 个位，x 表示当前正在构造的数字
    function dfs(i, x) {
        if (i === 3) {
            ans.push(x);
            return;
        }
        for (let d = 0; d < 10; d++) {
            if (cnt[d] > 0 && (i === 0 && d > 0 || i === 1 || i === 2 && d % 2 === 0)) {
                cnt[d]--; // 消耗一个数字 d
                dfs(i + 1, x * 10 + d);
                cnt[d]++; // 复原
            }
        }
    }
    dfs(0, 0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_even_numbers(digits: Vec<i32>) -> Vec<i32> {
        let mut cnt = [0; 10];
        for d in digits {
            cnt[d as usize] += 1;
        }

        // i=0 百位，i=1 十位，i=2 个位，x 表示当前正在构造的数字
        fn dfs(i: usize, x: i32, cnt: &mut [i32; 10], ans: &mut Vec<i32>) {
            if i == 3 {
                ans.push(x);
                return;
            }
            for d in 0..10 {
                if cnt[d] > 0 && (i == 0 && d > 0 || i == 1 || i == 2 && d % 2 == 0) {
                    cnt[d] -= 1; // 消耗一个数字 d
                    dfs(i + 1, x * 10 + d as i32, cnt, ans);
                    cnt[d] += 1; // 复原
                }
            }
        }
        let mut ans = vec![];
        dfs(0, 0, &mut cnt, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + 10^k)$，其中 $n$ 是 $\textit{digits}$ 的长度，$k=3$。搜索树是一棵高度为 $k$ 的 $10$ 叉树，有 $\mathcal{O}(10^k)$ 个节点，遍历这棵树需要 $\mathcal{O}(10^k)$ 的时间。
- 空间复杂度：$\mathcal{O}(k)$。返回值不计入。$\textit{cnt}$ 数组的大小视作 $\mathcal{O}(1)$。递归需要 $\mathcal{O}(k)$ 的栈空间。

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

本题来自 `四、回溯 / §4.7 搜索`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.7 搜索`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
