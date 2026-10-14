# 485. 最大连续 1 的个数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/max-consecutive-ones/
- 题目 slug：`max-consecutive-ones`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/max-consecutive-ones/solutions/2993237/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-b8i9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题，简单做（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/max-consecutive-ones/solutions/2993237/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-b8i9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-b8i9`
- topic id：`2993237`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

遍历 $\textit{nums}$，同时维护目前连续 $1$ 的个数 $\textit{cnt}$，初始值为 $0$。

- 如果 $\textit{nums}[i]=0$，把 $\textit{cnt}$ 重置为 $0$。
- 如果 $\textit{nums}[i]=1$，把 $\textit{cnt}$ 加一，然后用 $\textit{cnt}$ 更新答案的最大值。

示例 1 计算过程如下：

| $i$  | $\textit{nums}[i]$  | $\textit{cnt}$ |
|---|---|---|
| $0$  | $1$  | $1$  |
| $1$  | $1$  | $2$  |
| $2$  | $0$  | $0$  |
| $3$  | $1$  | $1$  |
| $4$  | $1$  | $2$  |
| $5$  | $1$  | $3$  |

其中最大的 $\textit{cnt}=3$，即为答案。

```py [sol-Python3]
class Solution:
    def findMaxConsecutiveOnes(self, nums: List[int]) -> int:
        ans = cnt = 0
        for x in nums:
            if x:
                cnt += 1  # 连续 1 的个数增加
                ans = max(ans, cnt)  # 更新答案的最大值
            else:
                cnt = 0  # 重置
        return ans
```

```java [sol-Java]
class Solution {
    public int findMaxConsecutiveOnes(int[] nums) {
        int ans = 0;
        int cnt = 0;
        for (int x : nums) {
            if (x == 0) {
                cnt = 0; // 重置
            } else {
                cnt++; // 连续 1 的个数增加
                ans = Math.max(ans, cnt); // 更新答案的最大值
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findMaxConsecutiveOnes(vector<int>& nums) {
        int ans = 0, cnt = 0;
        for (int x : nums) {
            if (x) {
                cnt++; // 连续 1 的个数增加
                ans = max(ans, cnt); // 更新答案的最大值
            } else {
                cnt = 0; // 重置
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int findMaxConsecutiveOnes(int* nums, int numsSize) {
    int ans = 0, cnt = 0;
    for (int i = 0; i < numsSize; i++) {
        if (nums[i]) {
            cnt++; // 连续 1 的个数增加
            ans = MAX(ans, cnt); // 更新答案的最大值
        } else {
            cnt = 0; // 重置
        }
    }
    return ans;
}
```

```go [sol-Go]
func findMaxConsecutiveOnes(nums []int) (ans int) {
    cnt := 0
    for _, x := range nums {
        if x == 0 {
            cnt = 0 // 重置
        } else {
            cnt++ // 连续 1 的个数增加
            ans = max(ans, cnt) // 更新答案的最大值
        }
    }
    return
}
```

```js [sol-JavaScript]
var findMaxConsecutiveOnes = function(nums) {
    let ans = 0, cnt = 0;
    for (const x of nums) {
        if (x) {
            cnt++; // 连续 1 的个数增加
            ans = Math.max(ans, cnt); // 更新答案的最大值
        } else {
            cnt = 0; // 重置
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_max_consecutive_ones(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut cnt = 0;
        for x in nums {
            if x == 0 {
                cnt = 0; // 重置
            } else {
                cnt += 1; // 连续 1 的个数增加
                ans = ans.max(cnt); // 更新答案的最大值
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如果你可以删掉一个数，最大连续 $1$ 的个数是多少？

见 [1493. 删掉一个元素以后全为 1 的最长子数组](https://leetcode.cn/problems/longest-subarray-of-1s-after-deleting-one-element/)。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
