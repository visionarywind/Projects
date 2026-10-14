# 16. 最接近的三数之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/3sum-closest/
- 题目 slug：`3sum-closest`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/3sum-closest/solutions/2337801/ji-zhi-you-hua-ji-yu-san-shu-zhi-he-de-z-qgqi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[极致优化！基于三数之和的做法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/3sum-closest/solutions/2337801/ji-zhi-you-hua-ji-yu-san-shu-zhi-he-de-z-qgqi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-zhi-you-hua-ji-yu-san-shu-zhi-he-de-z-qgqi`
- topic id：`2337801`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置题目

请先完成 [15. 三数之和](https://leetcode.cn/problems/3sum/)。

[视频讲解](https://www.bilibili.com/video/BV1bP411c7oJ/)，[代码](https://leetcode.cn/problems/3sum/solution/shuang-zhi-zhen-xiang-bu-ming-bai-yi-ge-pno55/)

## 思路

思路和 [15. 三数之和](https://leetcode.cn/problems/3sum/solution/shuang-zhi-zhen-xiang-bu-ming-bai-yi-ge-pno55/) 类似，排序后，枚举 $\textit{nums}[i]$ 作为第一个数，那么问题变成找到另外两个数，使得这三个数的和与 $\textit{target}$ 最接近，这同样可以用**双指针**解决。

设 $s=\textit{nums}[i]+\textit{nums}[j]+\textit{nums}[k]$。如果 $|s - \textit{target}| < |\textit{ans} - \textit{target}|$，那么更新 $\textit{ans} = s$。$\textit{ans}$ 初始值为 $\infty$。

然后分类讨论：

- 如果 $s=\textit{target}$，那么答案就是 $\textit{target}$，直接返回 $\textit{target}$。
- 如果 $s>\textit{target}$，那么和三数之和一样，把 $k$ 减一。
- 否则 $s<\textit{target}$，那么和三数之和一样，把 $j$ 加一。

除此以外，还有一些类似 [15. 三数之和我的题解](https://leetcode.cn/problems/3sum/solutions/1968332/shuang-zhi-zhen-xiang-bu-ming-bai-yi-ge-pno55/) 的优化：

1. 如果 $i>0$ 且 $\textit{nums}[i]=\textit{nums}[i-1]$，那么 $\textit{nums}[i]$ 和后面数字相加的结果，必然在之前算过，所以无需跑下面的双指针，直接 `continue` 外层循环。
2. 设 $s=\textit{nums}[i]+\textit{nums}[i+1]+\textit{nums}[i+2]$。如果 $s > \textit{target}$，由于数组已经排序，后面无论怎么选，选出的三数之和不会比 $s$ 还小，所以不会找到比 $s$ 更优的答案了。所以只要 $s > \textit{target}$，就可以直接 `break` 外层循环了。在 `break` 前判断 $s$ 是否离 $\textit{target}$ 更近，如果更近，那么更新答案为 $s$。
3. 设 $s=\textit{nums}[i]+\textit{nums}[n-2]+\textit{nums}[n-1]$。如果 $s < \textit{target}$，由于数组已经排序，$\textit{nums}[i]$ 加上后面任意两个数都不超过 $s$，所以下面的双指针就不需要跑了，无法找到比 $s$ 更优的答案。但是后面还有更大的 $\textit{nums}[i]$，可能找到一个离 $\textit{target}$ 更近的三数之和，所以还需要继续枚举，`continue` 外层循环。在 `continue` 前判断 $s$ 是否离 $\textit{target}$ 更近，如果更近，那么更新答案为 $s$。

```py [sol-Python3]
class Solution:
    def threeSumClosest(self, nums: List[int], target: int) -> int:
        nums.sort()

        n = len(nums)
        ans = inf
        for i in range(n - 2):
            x = nums[i]
            # 优化
            if i and x == nums[i - 1]:
                continue  

            # 优化
            s = x + nums[i + 1] + nums[i + 2]
            if s > target:  # 后面无论怎么选，选出的三数之和不会比 s 还小
                if s - target < abs(ans - target):
                    ans = s
                break

            # 优化
            s = x + nums[-2] + nums[-1]
            if s < target:  # x 加上后面任意两个数都不超过 s，所以下面的双指针就不需要跑了
                if target - s < abs(ans - target):
                    ans = s
                continue

            # 双指针
            j, k = i + 1, n - 1
            while j < k:
                s = x + nums[j] + nums[k]
                if s == target:
                    return target
                if abs(s - target) < abs(ans - target):  # s 离 target 更近
                    ans = s
                if s > target:
                    k -= 1
                else:  # s < target
                    j += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int threeSumClosest(int[] nums, int target) {
        Arrays.sort(nums);

        int n = nums.length;
        int ans = Integer.MAX_VALUE / 2; // 除 2 防止减去一个负数溢出
        for (int i = 0; i < n - 2; i++) {
            int x = nums[i];
            // 优化
            if (i > 0 && x == nums[i - 1]) {
                continue; 
            }

            // 优化
            int s = x + nums[i + 1] + nums[i + 2];
            if (s > target) { // 后面无论怎么选，选出的三数之和不会比 s 还小
                if (s - target < Math.abs(ans - target)) {
                    ans = s;
                }
                break;
            }

            // 优化
            s = x + nums[n - 2] + nums[n - 1];
            if (s < target) { // x 加上后面任意两个数都不超过 s，所以下面的双指针就不需要跑了
                if (target - s < Math.abs(ans - target)) {
                    ans = s;
                }
                continue;
            }

            // 双指针
            int j = i + 1;
            int k = n - 1;
            while (j < k) {
                s = x + nums[j] + nums[k];
                if (s == target) {
                    return target;
                }
                if (Math.abs(s - target) < Math.abs(ans - target)) { // s 离 target 更近
                    ans = s;
                }
                if (s > target) {
                    k--;
                } else { // s < target
                    j++;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int threeSumClosest(vector<int>& nums, int target) {
        ranges::sort(nums);

        int n = nums.size();
        int ans = INT_MAX / 2; // 除 2 防止减去一个负数溢出
        for (int i = 0; i < n - 2; i++) {
            int x = nums[i];
            // 优化
            if (i > 0 && x == nums[i - 1]) {
                continue;
            }

            // 优化
            int s = x + nums[i + 1] + nums[i + 2];
            if (s > target) { // 后面无论怎么选，选出的三数之和不会比 s 还小
                if (s - target < abs(ans - target)) {
                    ans = s;
                }
                break;
            }

            // 优化
            s = x + nums[n - 2] + nums[n - 1];
            if (s < target) { // x 加上后面任意两个数都不超过 s，所以下面的双指针就不需要跑了
                if (target - s < abs(ans - target)) {
                    ans = s;
                }
                continue;
            }

            // 双指针
            int j = i + 1, k = n - 1;
            while (j < k) {
                s = x + nums[j] + nums[k];
                if (s == target) {
                    return target;
                }
                if (abs(s - target) < abs(ans - target)) { // s 离 target 更近
                    ans = s;
                }
                if (s > target) {
                    k--;
                } else { // s < target
                    j++;
                }
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int threeSumClosest(int* nums, int numsSize, int target) {
    qsort(nums, numsSize, sizeof(int), cmp);

    int n = numsSize;
    int ans = INT_MAX / 2; // 除 2 防止减去一个负数溢出
    for (int i = 0; i < numsSize - 2; i++) {
        int x = nums[i];
        // 优化
        if (i > 0 && x == nums[i - 1]) {
            continue;
        }

        // 优化
        int s = x + nums[i + 1] + nums[i + 2];
        if (s > target) { // 后面无论怎么选，选出的三个数的和不会比 s 还小
            if (s - target < abs(ans - target)) {
                ans = s;
            }
            break;
        }

        // 优化
        s = x + nums[n - 2] + nums[n - 1];
        if (s < target) { // x 加上后面任意两个数都不超过 s，所以下面的双指针就不需要跑了
            if (target - s < abs(ans - target)) {
                ans = s;
            }
            continue;
        }

        // 双指针
        int j = i + 1, k = n - 1;
        while (j < k) {
            s = x + nums[j] + nums[k];
            if (s == target) {
                return target;
            }
            if (abs(s - target) < abs(ans - target)) { // s 离 target 更近
                ans = s;
            }
            if (s > target) {
                k--;
            } else { // s < target
                j++;
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func threeSumClosest(nums []int, target int) int {
    slices.Sort(nums)

    n := len(nums)
    ans := math.MaxInt / 2 // 除 2 防止减去一个负数溢出
    for i, x := range nums[:n-2] {
        // 优化
        if i > 0 && x == nums[i-1] {
            continue
        }

        // 优化
        s := x + nums[i+1] + nums[i+2]
        if s > target { // 后面无论怎么选，选出的三数之和不会比 s 还小
            if s-target < abs(ans-target) {
                ans = s
            }
            break
        }

        // 优化
        s = x + nums[n-2] + nums[n-1]
        if s < target { // x 加上后面任意两个数都不超过 s，所以下面的双指针就不需要跑了
            if target-s < abs(ans-target) {
                ans = s
            }
            continue
        }

        // 双指针
        j, k := i+1, n-1
        for j < k {
            s = x + nums[j] + nums[k]
            if s == target {
                return target
            }
            if abs(s-target) < abs(ans-target) { // s 离 target 更近
                ans = s
            }
            if s > target {
                k--
            } else { // s < target
                j++
            }
        }
    }
    return ans
}

func abs(x int) int {
    if x < 0 {
        return -x
    }
    return x
}
```

```js [sol-JavaScript]
var threeSumClosest = function(nums, target) {
    nums.sort((a, b) => a - b);

    const n = nums.length;
    let ans = Infinity;
    for (let i = 0; i < n - 2; i++) {
        const x = nums[i];
        // 优化
        if (i > 0 && x === nums[i - 1]) {
            continue;
        }

        // 优化
        let s = x + nums[i + 1] + nums[i + 2];
        if (s > target) { // 后面无论怎么选，选出的三数之和不会比 s 还小
            if (s - target < Math.abs(ans - target)) {
                ans = s;
            }
            break;
        }

        // 优化
        s = x + nums[n - 2] + nums[n - 1];
        if (s < target) { // x 加上后面任意两个数都不超过 s，所以下面的双指针就不需要跑了
            if (target - s < Math.abs(ans - target)) {
                ans = s;
            }
            continue;
        }

        // 双指针
        let j = i + 1, k = n - 1;
        while (j < k) {
            s = x + nums[j] + nums[k];
            if (s === target) {
                return target;
            }
            if (Math.abs(s - target) < Math.abs(ans - target)) { // s 离 target 更近
                ans = s;
            }
            if (s > target) {
                k--;
            } else { // s < target
                j++;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn three_sum_closest(mut nums: Vec<i32>, target: i32) -> i32 {
        nums.sort_unstable();

        let n = nums.len();
        let mut ans = i32::MAX / 2; // 除 2 防止减去一个负数溢出
        for i in 0..n - 2 {
            let x = nums[i];
            // 优化
            if i > 0 && x == nums[i - 1] {
                continue;
            }

            // 优化
            let mut s = x + nums[i + 1] + nums[i + 2];
            if s > target { // 后面无论怎么选，选出的三个数的和不会比 s 还小
                if s - target < (ans - target).abs() {
                    ans = s;
                }
                break;
            }

            // 优化
            s = x + nums[n - 2] + nums[n - 1];
            if s < target { // x 加上后面任意两个数都不超过 s，所以下面的双指针就不需要跑了
                if target - s < (ans - target).abs() {
                    ans = s;
                }
                continue;
            }

            // 双指针
            let mut j = i + 1;
            let mut k = n - 1;
            while j < k {
                s = x + nums[j] + nums[k];
                if s == target {
                    return target;
                }
                if (s - target).abs() < (ans - target).abs() { // s 离 target 更近
                    ans = s;
                }
                if s > target {
                    k -= 1;
                } else { // s < target
                    j += 1;
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{nums}$ 的长度。排序 $\mathcal{O}(n\log n)$。外层循环 $\mathcal{O}(n)$ 枚举第一个数，内层循环 $\mathcal{O}(n)$ 双指针。所以总的时间复杂度为 $\mathcal{O}(n\log n + n^2) = \mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。返回值不计入。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
