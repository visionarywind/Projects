# 1679. K 和数对的最大数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/max-number-of-k-sum-pairs/
- 题目 slug：`max-number-of-k-sum-pairs`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：1346
- 外部题解来源：https://leetcode.cn/problems/max-number-of-k-sum-pairs/solutions/2701378/on-yi-ci-bian-li-pythonjavacgojsrust-by-r7obc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 一次遍历（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/max-number-of-k-sum-pairs/solutions/2701378/on-yi-ci-bian-li-pythonjavacgojsrust-by-r7obc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-yi-ci-bian-li-pythonjavacgojsrust-by-r7obc`
- topic id：`2701378`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

例如 $\textit{nums}=[1,2,2,1],\ k=3$。

从左到右遍历数组，同时用一个哈希表统计元素及其出现次数。

1. 遍历到 $\textit{nums}[0]=1$，加入哈希表。
2. 遍历到 $\textit{nums}[1]=2$，查找到哈希表中有数字 $1$ 满足 $1+2=k$，那么从哈希表中去掉一个 $1$，答案加一。注意这一步没有把 $2$ 加入哈希表。
3. 遍历到 $\textit{nums}[2]=2$，由于哈希表中没有 $1$，把 $2$ 加入哈希表。
4. 遍历到 $\textit{nums}[3]=1$，查找到哈希表中有数字 $2$ 满足 $2+1=k$，那么从哈希表中去掉一个 $2$，答案加一。注意这一步没有把 $1$ 加入哈希表。

一般地，和 [1. 两数之和](https://leetcode.cn/problems/two-sum/) 一样，每次先看看哈希表中是否有 $k-\textit{nums}[i]$，有就去掉哈希表中的一个 $k-\textit{nums}[i]$，同时把答案加一，没有就把 $\textit{nums}[i]$ 加入哈希表。

```py [sol-Python3]
class Solution:
    def maxOperations(self, nums: List[int], k: int) -> int:
        cnt = defaultdict(int)
        ans = 0
        for x in nums:
            if cnt[k - x]:
                cnt[k - x] -= 1
                ans += 1
            else:
                cnt[x] += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int maxOperations(int[] nums, int k) {
        HashMap<Integer, Integer> cnt = new HashMap<>();
        int ans = 0;
        for (int x : nums) {
            int c = cnt.getOrDefault(k - x, 0);
            if (c > 0) {
                cnt.put(k - x, c - 1);
                ans++;
            } else {
                cnt.merge(x, 1, Integer::sum);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxOperations(vector<int> &nums, int k) {
        unordered_map<int, int> cnt;
        int ans = 0;
        for (int x : nums) {
            auto it = cnt.find(k - x);
            if (it != cnt.end() && it->second) {
                it->second--;
                ans++;
            } else {
                cnt[x]++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxOperations(nums []int, k int) (ans int) {
    cnt := map[int]int{}
    for _, x := range nums {
        if cnt[k-x] > 0 {
            cnt[k-x]--
            ans++
        } else {
            cnt[x]++
        }
    }
    return
}
```

```js [sol-JavaScript]
var maxOperations = function (nums, k) {
    const cnt = new Map();
    let ans = 0;
    for (const x of nums) {
        const c = cnt.get(k - x)
        if (c) {
            cnt.set(k - x, c - 1);
            ans++;
        } else {
            cnt.set(x, (cnt.get(x) ?? 0) + 1);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn max_operations(nums: Vec<i32>, k: i32) -> i32 {
        let mut cnt = HashMap::new();
        let mut ans = 0;
        for x in nums {
            if let Some(c) = cnt.get_mut(&(k - x)) {
                if *c > 0 {
                    *c -= 1;
                    ans += 1;
                    continue;
                }
            }
            *cnt.entry(x).or_insert(0) += 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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
