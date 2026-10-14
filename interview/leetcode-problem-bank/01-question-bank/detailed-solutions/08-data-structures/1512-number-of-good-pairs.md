# 1512. 好数对的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-good-pairs/
- 题目 slug：`number-of-good-pairs`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：1161
- 外部题解来源：https://leetcode.cn/problems/number-of-good-pairs/solutions/2974653/mei-ju-you-wei-hu-zuo-pythonjavaccgojsru-7u5v/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 做法：枚举右，维护左（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/number-of-good-pairs/solutions/2974653/mei-ju-you-wei-hu-zuo-pythonjavaccgojsru-7u5v/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-you-wei-hu-zuo-pythonjavaccgojsru-7u5v`
- topic id：`2974653`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 题意

统计满足 $i<j$ 且 $\textit{nums}[i]=\textit{nums}[j]$ 的数对个数。

## 暴力思路

写个二重循环，枚举 $j$，统计左边（因为要求 $i<j$）有多少个数等于 $\textit{nums}[j]$。

这需要 $\mathcal{O}(n^2)$ 的时间。

## 优化

例如 $\textit{nums}=[1,1,2,1]$：

1. 遍历到 $\textit{nums}[0]= 1$ 时，左边没有数字。
2. 遍历到 $\textit{nums}[1]= 1$ 时，左边有 $1$ 个 $1$，找到了 $1$ 个好数对 $(0,1)$。
3. 遍历到 $\textit{nums}[2]= 2$ 时，左边没有 $2$。
4. 遍历到 $\textit{nums}[3]= 1$ 时，左边有 $2$ 个 $1$，找到了 $2$ 个好数对 $(0,3)$ 和 $(1,3)$。
5. 所以答案为 $1+2=3$。

一般地，遍历到元素 $x$ 时，把答案增加之前遍历过的 $x$ 的个数。我们可以在遍历的过程中，用一个哈希表（或者数组）统计每个元素的出现次数。

## 答疑

**问**：为什么代码要先更新答案，再更新 $\textit{cnt}$？

**答**：题目要求 $i<j$，如果先更新 $\textit{cnt}$，再更新答案，就把 $\textit{nums}[j]$ 自己也统计进来了，相当于把 $i=j$ 的情况也认为是好数对。

```py [sol-Python3]
class Solution:
    def numIdenticalPairs(self, nums: List[int]) -> int:
        ans = 0
        cnt = defaultdict(int)
        for x in nums:  # x = nums[j]
            # 此时 cnt[x] 表示之前遍历过的 x 的个数，加到 ans 中
            # 如果先执行 cnt[x] += 1，再执行 ans += cnt[x]，就把 i=j 这种情况也统计进来了，算出的答案会偏大
            ans += cnt[x]
            cnt[x] += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int numIdenticalPairs(int[] nums) {
        int ans = 0;
        int[] cnt = new int[101];
        for (int x : nums) { // x = nums[j]
            // 此时 cnt[x] 表示之前遍历过的 x 的个数，加到 ans 中
            // 如果先执行 cnt[x]++，再执行 ans += cnt[x]，就把 i=j 这种情况也统计进来了，算出的答案会偏大
            ans += cnt[x];
            cnt[x]++;
        }
        return ans;
    }
}
```

```java [sol-Java 写法二]
class Solution {
    public int numIdenticalPairs(int[] nums) {
        int ans = 0;
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            int c = cnt.getOrDefault(x, 0);
            ans += c;
            cnt.put(x, c + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numIdenticalPairs(vector<int>& nums) {
        int ans = 0;
        unordered_map<int, int> cnt;
        for (int x : nums) { // x = nums[j]
            // 此时 cnt[x] 表示之前遍历过的 x 的个数，加到 ans 中
            // 如果先执行 cnt[x]++，再执行 ans += cnt[x]，就把 i=j 这种情况也统计进来了，算出的答案会偏大
            ans += cnt[x];
            cnt[x]++;
        }
        return ans;
    }
};
```

```c [sol-C]
int numIdenticalPairs(int* nums, int numsSize) {
    int ans = 0;
    int cnt[101] = {};
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        // 此时 cnt[x] 表示之前遍历过的 x 的个数，加到 ans 中
        // 如果先执行 cnt[x]++，再执行 ans += cnt[x]，就把 i=j 这种情况也统计进来了，算出的答案会偏大
        ans += cnt[x];
        cnt[x]++;
    }
    return ans;
}
```

```go [sol-Go]
func numIdenticalPairs(nums []int) (ans int) {
    cnt := map[int]int{}
    for _, x := range nums { // x = nums[j]
        // 此时 cnt[x] 表示之前遍历过的 x 的个数，加到 ans 中
        // 如果先执行 cnt[x]++，再执行 ans += cnt[x]，就把 i=j 这种情况也统计进来了，算出的答案会偏大
        ans += cnt[x]
        cnt[x]++
    }
    return
}
```

```js [sol-JavaScript]
var numIdenticalPairs = function(nums) {
    let ans = 0;
    const cnt = new Map();
    for (const x of nums) { // x = nums[j]
        // 此时 cnt[x] 表示之前遍历过的 x 的个数，加到 ans 中
        // 如果先执行 cnt[x]++，再执行 ans += cnt[x]，就把 i=j 这种情况也统计进来了，算出的答案会偏大
        const c = cnt.get(x) ?? 0
        ans += c;
        cnt.set(x, c + 1);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn num_identical_pairs(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut cnt = HashMap::new();
        for x in nums { // x = nums[j]
            // 此时 *e 表示之前遍历过的 x 的个数，加到 ans 中
            // 如果先执行 *e += 1，再执行 ans += *e，就把 i=j 这种情况也统计进来了，算出的答案会偏大
            let e = cnt.entry(x).or_insert(0);
            ans += *e;
            *e += 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面数据结构题单中的「**§0.1 枚举右，维护左**」。

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
