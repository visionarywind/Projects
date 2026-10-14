# 904. 水果成篮

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/fruit-into-baskets/
- 题目 slug：`fruit-into-baskets`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础
- 难度分：1516
- 外部题解来源：https://leetcode.cn/problems/fruit-into-baskets/solutions/3042744/hua-dong-chuang-kou-ha-xi-biao-pythonjav-uzmw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[滑动窗口+哈希表（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/fruit-into-baskets/solutions/3042744/hua-dong-chuang-kou-ha-xi-biao-pythonjav-uzmw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-dong-chuang-kou-ha-xi-biao-pythonjav-uzmw`
- topic id：`3042744`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 题意

找一个最长连续子数组，满足子数组中至多有两种数字。返回子数组的长度。

## 思路

**前置知识**：[滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

子数组越长，包含的元素越多，越可能不满足题目要求；反之，子数组越短，包含的元素越少，越可能满足题目要求。本题属于「越短越合法」，有这种性质的题目，可以用滑动窗口解决。

枚举子数组的右端点 $\textit{right}$。同时用一个哈希表 $\textit{cnt}$ 维护子数组内每个元素的出现次数。

如果 $\textit{fruits}[right]$ 加入哈希表后，发现哈希表的大小超过了 $2$，那么子数组不满足要求。移动子数组的左端点 $\textit{left}$，把 $\textit{fruits}[\textit{left}]$ 的出现次数减一，直到哈希表中的元素种数等于 $2$。

⚠**注意**：如果 $\textit{fruits}[\textit{left}]$ 的出现次数变成 $0$，需要从 $\textit{cnt}$ 中移除，表示子数组内少了一种元素。如果不移除，我们无法通过 $\textit{cnt}$ 的大小判断窗口中的元素种数。

```py [sol-Python3]
class Solution:
    def totalFruit(self, fruits: List[int]) -> int:
        ans = left = 0
        cnt = defaultdict(int)
        for right, in_ in enumerate(fruits):
            cnt[in_] += 1  # fruits[right] 进入窗口
            while len(cnt) > 2:  # 不满足要求
                out = fruits[left]
                cnt[out] -= 1  # fruits[left] 离开窗口
                if cnt[out] == 0:
                    del cnt[out]
                left += 1
            ans = max(ans, right - left + 1)
        return ans
```

```java [sol-Java]
class Solution {
    public int totalFruit(int[] fruits) {
        int ans = 0;
        int left = 0;
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int right = 0; right < fruits.length; right++) {
            cnt.merge(fruits[right], 1, Integer::sum); // fruits[right] 进入窗口
            while (cnt.size() > 2) { // 不满足要求
                int out = fruits[left];
                cnt.merge(out, -1, Integer::sum); // fruits[left] 离开窗口
                if (cnt.get(out) == 0) {
                    cnt.remove(out);
                }
                left++;
            }
            ans = Math.max(ans, right - left + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int totalFruit(vector<int>& fruits) {
        int ans = 0, left = 0;
        unordered_map<int, int> cnt;
        for (int right = 0; right < fruits.size(); right++) {
            cnt[fruits[right]]++; // fruits[right] 进入窗口
            while (cnt.size() > 2) { // 不满足要求
                int out = fruits[left];
                cnt[out]--; // fruits[left] 离开窗口
                if (cnt[out] == 0) {
                    cnt.erase(out);
                }
                left++;
            }
            ans = max(ans, right - left + 1);
        }
        return ans;
    }
};
```

```go [sol-Go]
func totalFruit(fruits []int) (ans int) {
    cnt := map[int]int{}
    left := 0
    for right, in := range fruits {
        cnt[in]++ // fruits[right] 进入窗口
        for len(cnt) > 2 { // 不满足要求
            out := fruits[left]
            cnt[out]-- // fruits[left] 离开窗口
            if cnt[out] == 0 {
                delete(cnt, out)
            }
            left++
        }
        ans = max(ans, right-left+1)
    }
    return
}
```

```js [sol-JavaScript]
var totalFruit = function(fruits) {
    let ans = 0, left = 0;
    const cnt = new Map();
    for (let right = 0; right < fruits.length; right++) {
        cnt.set(fruits[right], (cnt.get(fruits[right]) ?? 0) + 1); // fruits[right] 进入窗口
        while (cnt.size > 2) { // 不满足要求
            const out = fruits[left];
            cnt.set(out, cnt.get(out) - 1); // fruits[left] 离开窗口
            if (cnt.get(out) === 0) {
                cnt.delete(out);
            }
            left++;
        }
        ans = Math.max(ans, right - left + 1);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn total_fruit(fruits: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut left = 0;
        let mut cnt = HashMap::new();
        for (right, &x) in fruits.iter().enumerate() {
            *cnt.entry(x).or_insert(0) += 1; // fruits[right] 进入窗口
            while cnt.len() > 2 { // 不满足要求
                let out = fruits[left];
                *cnt.entry(out).or_insert(0) -= 1; // fruits[left] 离开窗口
                if cnt[&out] == 0 {
                    cnt.remove(&out);
                }
                left += 1;
            }
            ans = ans.max(right - left + 1);
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{fruits}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。在任意时刻，哈希表中至多有 $3$ 个键值对（$3$ 种不同元素）。

## 专题训练

见下面滑动窗口题单中的「**§2.1 越短越合法/求最长/最大**」。

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
