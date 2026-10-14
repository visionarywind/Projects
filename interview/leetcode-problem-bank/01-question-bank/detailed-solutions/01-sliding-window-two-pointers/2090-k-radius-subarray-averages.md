# 2090. 半径为 k 的子数组平均值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/k-radius-subarray-averages/
- 题目 slug：`k-radius-subarray-averages`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.1 基础
- 难度分：1358
- 外部题解来源：https://leetcode.cn/problems/k-radius-subarray-averages/solutions/1126276/on-hua-dong-chuang-kou-by-endlesscheng-jtr5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 定长滑动窗口（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/k-radius-subarray-averages/solutions/1126276/on-hua-dong-chuang-kou-by-endlesscheng-jtr5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-hua-dong-chuang-kou-by-endlesscheng-jtr5`
- topic id：`1126276`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题相当于一个长为 $2k+1$ 的滑动窗口。

1. **入**：下标为 $i$ 的元素进入窗口，窗口元素和 $s$ 增加 $\textit{nums}[i]$。如果 $i<2k$ 则重复第一步。
2. **更新**：本题只需记录答案，即 $\textit{avgs}[i-k]=\left\lfloor\dfrac{s}{2k+1}\right\rfloor$。其中 $i-k$ 是因为 $i$ 对应的是子数组右端点，而记录答案的位置是子数组的正中间。
3. **出**：下标为 $i-2k$ 的元素离开窗口，窗口元素和 $s$ 减少 $\textit{nums}[i-2k]$。

以上三步适用于所有定长滑窗题目。

详细讲解见[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)

⚠**注意**：窗口元素和会超过 $32$ 位整数最大值，需要用 $64$ 位整数保存窗口元素和。

```py [sol-Python3]
class Solution:
    def getAverages(self, nums: List[int], k: int) -> List[int]:
        avgs = [-1] * len(nums)
        s = 0  # 维护窗口元素和
        for i, x in enumerate(nums):
            # 1. 进入窗口
            s += x
            if i < k * 2:  # 窗口大小不足 2k+1
                continue
            # 2. 记录答案
            avgs[i - k] = s // (k * 2 + 1)
            # 3. 离开窗口
            s -= nums[i - k * 2]
        return avgs
```

```java [sol-Java]
class Solution {
    public int[] getAverages(int[] nums, int k) {
        int n = nums.length;
        int[] avgs = new int[n];
        Arrays.fill(avgs, -1);
        long s = 0; // 维护窗口元素和（注意类型！）
        for (int i = 0; i < n; i++) {
            // 1. 进入窗口
            s += nums[i];
            if (i < k * 2) { // 窗口大小不足 2k+1
                continue;
            }
            // 2. 记录答案
            avgs[i - k] = (int) (s / (k * 2 + 1));
            // 3. 离开窗口
            s -= nums[i - k * 2];
        }
        return avgs;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> getAverages(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> avgs(n, -1);
        long long s = 0; // 维护窗口元素和（注意类型！）
        for (int i = 0; i < n; i++) {
            // 1. 进入窗口
            s += nums[i];
            if (i < k * 2) { // 窗口大小不足 2k+1
                continue;
            }
            // 2. 记录答案
            avgs[i - k] = s / (k * 2 + 1);
            // 3. 离开窗口
            s -= nums[i - k * 2];
        }
        return avgs;
    }
};
```

```c [sol-C]
int* getAverages(int* nums, int numsSize, int k, int* returnSize) {
    *returnSize = numsSize;
    int* avgs = malloc(numsSize * sizeof(int));
    memset(avgs, -1, numsSize * sizeof(int));
    long long s = 0; // 维护窗口元素和（注意类型！）
    for (int i = 0; i < numsSize; i++) {
        // 1. 进入窗口
        s += nums[i];
        if (i < k * 2) { // 窗口大小不足 2k+1
            continue;
        }
        // 2. 记录答案
        avgs[i - k] = s / (k * 2 + 1);
        // 3. 离开窗口
        s -= nums[i - k * 2];
    }
    return avgs;
}
```

```go [sol-Go]
func getAverages(nums []int, k int) []int {
    avgs := make([]int, len(nums))
    for i := range avgs {
        avgs[i] = -1
    }
    s := 0 // 维护窗口元素和
    for i, x := range nums {
        // 1. 进入窗口
        s += x
        if i < k*2 { // 窗口大小不足 2k+1
            continue
        }
        // 2. 记录答案
        avgs[i-k] = s / (k*2 + 1)
        // 3. 离开窗口
        s -= nums[i-k*2]
    }
    return avgs
}
```

```js [sol-JavaScript]
var getAverages = function(nums, k) {
    const n = nums.length;
    const avgs = Array(n).fill(-1);
    let s = 0; // 维护窗口元素和
    for (let i = 0; i < n; i++) {
        // 1. 进入窗口
        s += nums[i];
        if (i < k * 2) { // 窗口大小不足 2k+1
            continue;
        }
        // 2. 记录答案
        avgs[i - k] = Math.floor(s / (k * 2 + 1));
        // 3. 离开窗口
        s -= nums[i - k * 2];
    }
    return avgs;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn get_averages(nums: Vec<i32>, k: i32) -> Vec<i32> {
        let k = k as usize;
        let mut avgs = vec![-1; nums.len()];
        let mut s = 0; // 维护窗口元素和
        for (i, &x) in nums.iter().enumerate() {
            // 1. 进入窗口
            s += x as i64;
            if i < k * 2 { // 窗口大小不足 2k+1
                continue;
            }
            // 2. 记录答案
            avgs[i - k] = (s / (k * 2 + 1) as i64) as _;
            // 3. 离开窗口
            s -= nums[i - k * 2] as i64;
        }
        avgs
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

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
