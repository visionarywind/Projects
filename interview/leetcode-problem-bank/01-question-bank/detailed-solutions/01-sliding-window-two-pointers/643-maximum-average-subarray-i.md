# 643. 子数组最大平均数 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-average-subarray-i/
- 题目 slug：`maximum-average-subarray-i`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-average-subarray-i/solutions/3061219/mo-ban-ding-chang-hua-dong-chuang-kou-py-1jxk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】定长滑动窗口（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-average-subarray-i/solutions/3061219/mo-ban-ding-chang-hua-dong-chuang-kou-py-1jxk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-ding-chang-hua-dong-chuang-kou-py-1jxk`
- topic id：`3061219`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 定长滑窗套路

我总结成三步：**入-更新-出**。

1. **入**：下标为 $i$ 的元素进入窗口，窗口元素和 $s$ 增加 $\textit{nums}[i]$。如果 $i<k-1$ 则重复第一步。
2. **更新**：更新答案。本题由于窗口长度固定为 $k$，可以统计窗口元素和的最大值 $\textit{maxS}$，最后返回的时候再除以 $k$。
3. **出**：下标为 $i-k+1$ 的元素离开窗口，窗口元素和 $s$ 减少 $\textit{nums}[i-k+1]$。

以上三步适用于所有定长滑窗题目。

详细讲解见[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)

⚠**注意**：本题 $\textit{nums}$ 中有负数，代码中的 $\textit{maxS}$ 要初始化成 $-\infty$。

```py [sol-Python3]
# 请选择 Python3 提交代码，而不是 Python（即 Python2），后者已经被淘汰了
class Solution:
    def findMaxAverage(self, nums: List[int], k: int) -> float:
        max_s = -inf  # 窗口元素和的最大值
        s = 0  # 维护窗口元素和
        for i, x in enumerate(nums):
            # 1. 进入窗口
            s += x
            if i < k - 1:  # 窗口大小不足 k
                continue
            # 2. 更新答案
            max_s = max(max_s, s)
            # 3. 离开窗口
            s -= nums[i - k + 1]
        return max_s / k
```

```java [sol-Java]
class Solution {
    public double findMaxAverage(int[] nums, int k) {
        int maxS = Integer.MIN_VALUE; // 窗口元素和的最大值
        int s = 0; // 维护窗口元素和
        for (int i = 0; i < nums.length; i++) {
            // 1. 进入窗口
            s += nums[i];
            if (i < k - 1) { // 窗口大小不足 k
                continue;
            }
            // 2. 更新答案
            maxS = Math.max(maxS, s);
            // 3. 离开窗口
            s -= nums[i - k + 1];
        }
        return (double) maxS / k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double findMaxAverage(vector<int>& nums, int k) {
        int max_s = INT_MIN; // 窗口元素和的最大值
        int s = 0; // 维护窗口元素和
        for (int i = 0; i < nums.size(); i++) {
            // 1. 进入窗口
            s += nums[i];
            if (i < k - 1) { // 窗口大小不足 k
                continue;
            }
            // 2. 更新答案
            max_s = max(max_s, s);
            // 3. 离开窗口
            s -= nums[i - k + 1];
        }
        return (double) max_s / k;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

double findMaxAverage(int* nums, int numsSize, int k) {
    int max_s = INT_MIN; // 窗口元素和的最大值
    int s = 0; // 维护窗口元素和
    for (int i = 0; i < numsSize; i++) {
        // 1. 进入窗口
        s += nums[i];
        if (i < k - 1) { // 窗口大小不足 k
            continue;
        }
        // 2. 更新答案
        max_s = MAX(max_s, s);
        // 3. 离开窗口
        s -= nums[i - k + 1];
    }
    return (double) max_s / k;
}
```

```go [sol-Go]
func findMaxAverage(nums []int, k int) float64 {
    maxS := math.MinInt // 窗口元素和的最大值
    s := 0 // 维护窗口元素和
    for i, x := range nums {
        // 1. 进入窗口
        s += x
        if i < k-1 { // 窗口大小不足 k
            continue
        }
        // 2. 更新答案
        maxS = max(maxS, s)
        // 3. 离开窗口
        s -= nums[i-k+1]
    }
    return float64(maxS) / float64(k)
}
```

```js [sol-JavaScript]
var findMaxAverage = function(nums, k) {
    let maxS = -Infinity; // 窗口元素和的最大值
    let s = 0; // 维护窗口元素和
    for (let i = 0; i < nums.length; i++) {
        // 1. 进入窗口
        s += nums[i];
        if (i < k - 1) { // 窗口大小不足 k
            continue;
        }
        // 2. 更新答案
        maxS = Math.max(maxS, s);
        // 3. 离开窗口
        s -= nums[i - k + 1];
    }
    return maxS / k;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_max_average(nums: Vec<i32>, k: i32) -> f64 {
        let k = k as usize;
        let mut max_s = i32::MIN; // 窗口元素和的最大值
        let mut s = 0; // 维护窗口元素和
        for (i, &x) in nums.iter().enumerate() {
            // 1. 进入窗口
            s += x;
            if i + 1 < k { // 窗口大小不足 k
                continue;
            }
            // 2. 更新答案
            max_s = max_s.max(s);
            // 3. 离开窗口
            s -= nums[i + 1 - k];
        }
        max_s as f64 / k as f64
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

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
