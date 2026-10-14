# 798. 得分最高的最小轮调

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/smallest-rotation-with-highest-score/
- 题目 slug：`smallest-rotation-with-highest-score`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.2 进阶
- 难度分：2130
- 外部题解来源：https://leetcode.cn/problems/smallest-rotation-with-highest-score/solutions/3972865/chai-fen-shu-zu-pythonjavaccgojsrust-by-o0zvr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[差分数组（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/smallest-rotation-with-highest-score/solutions/3972865/chai-fen-shu-zu-pythonjavaccgojsrust-by-o0zvr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chai-fen-shu-zu-pythonjavaccgojsrust-by-o0zvr`
- topic id：`3972865`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

暴力做法是，枚举 $k=0,1,2,\ldots,n-1$，遍历轮调后的数组，计算得分。时间复杂度为 $\mathcal{O}(n^2)$，太慢了。

定义 $\textit{score}[k]$ 为轮调 $k$ 次后的得分。

横看成岭侧成峰，单看 $\textit{nums}[i]$，它会如何影响数组 $\textit{score}$？哪些 $\textit{score}[k]$ 会增加 $1$？

设 $x = \textit{nums}[i]$。把元素 $x$ 轮调到下标区间 $[x, n-1]$ 中，就能得到 $1$ 分。

分类讨论：

- 如果 $i$ 在区间 $[x, n-1]$ 左侧，即 $i < x$，那么轮调 $[i+1, i+n-x]$ 中的任意整数次，就可以把下标 $i$ 移到 $[x, n-1]$ 中。换句话说，对于 $[i+1, i+n-x]$ 中的每个整数 $k$，把 $\textit{score}[k]$ 增加 $1$。**注**：下标 $i$ 移到下标 $0$ 要轮调 $i$ 次，再轮调一次就到 $n-1$ 了，所以下标 $i$ 移到下标 $n-1$ 要轮调 $i+1$ 次；下标 $i$ 往右移到下标 $x$ 要 $x-i$ 步，所以轮调 $n-(x-i) = i+n-x$ 次，下标 $i$ 移到下标 $x$。
- 如果 $i$ 在区间 $[x, n-1]$ 中，即 $i \ge x$，那么轮调 $[i+1-x, i]$ 中的任意整数次，就把下标 $i$ 移到 $[x, n-1]$ 外面了。我们可以先把所有 $\textit{score}[k]$ 都增加 $1$，然后把下标区间 $[i+1-x, i]$ 中的每个 $\textit{score}[k]$ 都减少 $1$。**注**：下标 $i$ 移到下标 $x-1$ 要轮调 $i+1-x$ 次；下标 $i$ 移到下标 $0$ 要轮调 $i$ 次。

这里用到了**区间增加（减少）同一个数**的操作，这可以用**差分数组**实现，请看 [原理讲解](https://leetcode.cn/problems/car-pooling/solution/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)。计算差分数组的前缀和，就得到了数组 $\textit{score}$。

> 注意题目保证 $\textit{nums}[i] < n$。

```py [sol-Python3]
class Solution:
    def bestRotation(self, nums: List[int]) -> int:
        n = len(nums)
        diff = [0] * (n + 1)
        for i, x in enumerate(nums):
            # 把 x 轮调到 [x, n-1] 中，就能得到 1 分
            if i < x:  # i 在 [x, n-1] 左侧
                # 需要轮调 [i+1, i+n-x] 次
                diff[i + 1] += 1
                diff[i + 1 + n - x] -= 1
            else:  # i 在 [x, n-1] 中
                # 先把每个 score[k] 都加一
                diff[0] += 1
                # 轮调 [i+1-x, i] 次无法得分，减掉
                diff[i + 1 - x] -= 1
                diff[i + 1] += 1

        max_sum = best_k = s = 0
        for i, d in enumerate(diff):
            s += d
            # 此时 s 为轮调 i 次后的得分
            if s > max_sum:
                max_sum = s
                best_k = i
        return best_k
```

```java [sol-Java]
class Solution {
    public int bestRotation(int[] nums) {
        int n = nums.length;
        int[] diff = new int[n + 1];
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            // 把 x 轮调到 [x, n-1] 中，就能得到 1 分
            if (i < x) { // i 在 [x, n-1] 左侧
                // 需要轮调 [i+1, i+n-x] 次
                diff[i + 1]++;
                diff[i + 1 + n - x]--;
            } else { // i 在 [x, n-1] 中
                // 先把每个 score[k] 都加一
                diff[0]++;
                // 轮调 [i+1-x, i] 次无法得分，减掉
                diff[i + 1 - x]--;
                diff[i + 1]++;
            }
        }

        int maxSum = 0;
        int bestK = 0;
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += diff[i];
            // 此时 sum 为轮调 i 次后的得分
            if (sum > maxSum) {
                maxSum = sum;
                bestK = i;
            }
        }
        return bestK;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int bestRotation(vector<int>& nums) {
        int n = nums.size();
        vector<int> diff(n + 1);
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            // 把 x 轮调到 [x, n-1] 中，就能得到 1 分
            if (i < x) { // i 在 [x, n-1] 左侧
                // 需要轮调 [i+1, i+n-x] 次
                diff[i + 1]++;
                diff[i + 1 + n - x]--;
            } else { // i 在 [x, n-1] 中
                // 先把每个 score[k] 都加一
                diff[0]++;
                // 轮调 [i+1-x, i] 次无法得分，减掉
                diff[i + 1 - x]--;
                diff[i + 1]++;
            }
        }

        int max_sum = 0, best_k = 0;
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += diff[i];
            // 此时 sum 为轮调 i 次后的得分
            if (sum > max_sum) {
                max_sum = sum;
                best_k = i;
            }
        }
        return best_k;
    }
};
```

```c [sol-C]
int bestRotation(int* nums, int numsSize) {
    int n = numsSize;
    int* diff = calloc(n + 1, sizeof(int));
    for (int i = 0; i < n; i++) {
        int x = nums[i];
        // 把 x 轮调到 [x, n-1] 中，就能得到 1 分
        if (i < x) { // i 在 [x, n-1] 左侧
            // 需要轮调 [i+1, i+n-x] 次
            diff[i + 1]++;
            diff[i + 1 + n - x]--;
        } else { // i 在 [x, n-1] 中
            // 先把每个 score[k] 都加一
            diff[0]++;
            // 轮调 [i+1-x, i] 次无法得分，减掉
            diff[i + 1 - x]--;
            diff[i + 1]++;
        }
    }

    int max_sum = 0, best_k = 0;
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += diff[i];
        // 此时 sum 为轮调 i 次后的得分
        if (sum > max_sum) {
            max_sum = sum;
            best_k = i;
        }
    }

    free(diff);
    return best_k;
}
```

```go [sol-Go]
func bestRotation(nums []int) int {
	n := len(nums)
	diff := make([]int, n+1)
	for i, x := range nums {
		// 把 x 轮调到 [x, n-1] 中，就能得到 1 分
		if i < x { // i 在 [x, n-1] 左侧
			// 需要轮调 [i+1, i+n-x] 次
			diff[i+1]++
			diff[i+1+n-x]--
		} else { // i 在 [x, n-1] 中
			// 先把每个 score[k] 都加一
			diff[0]++
			// 轮调 [i+1-x, i] 次无法得分，减掉
			diff[i+1-x]--
			diff[i+1]++
		}
	}

	maxSum, bestK := 0, 0
	sum := 0
	for i, d := range diff {
		sum += d
		// 此时 sum 为轮调 i 次后的得分
		if sum > maxSum {
			maxSum, bestK = sum, i
		}
	}
	return bestK
}
```

```js [sol-JavaScript]
var bestRotation = function(nums) {
    const n = nums.length;
    const diff = Array(n + 1).fill(0);
    for (let i = 0; i < n; i++) {
        const x = nums[i];
        // 把 x 轮调到 [x, n-1] 中，就能得到 1 分
        if (i < x) { // i 在 [x, n-1] 左侧
            // 需要轮调 [i+1, i+n-x] 次
            diff[i + 1]++;
            diff[i + 1 + n - x]--;
        } else { // i 在 [x, n-1] 中
            // 先把每个 score[k] 都加一
            diff[0]++;
            // 轮调 [i+1-x, i] 次无法得分，减掉
            diff[i + 1 - x]--;
            diff[i + 1]++;
        }
    }

    let maxSum = 0;
    let bestK = 0;
    let sum = 0;
    for (let i = 0; i < n; i++) {
        sum += diff[i];
        // 此时 sum 为轮调 i 次后的得分
        if (sum > maxSum) {
            maxSum = sum;
            bestK = i;
        }
    }
    return bestK;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn best_rotation(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut diff = vec![0; n + 1];
        for (i, x) in nums.into_iter().enumerate() {
            let x = x as usize;
            // 把 x 轮调到 [x, n-1] 中，就能得到 1 分
            if i < x { // i 在 [x, n-1] 左侧
                // 需要轮调 [i+1, i+n-x] 次
                diff[i + 1] += 1;
                diff[i + 1 + n - x] -= 1;
            } else { // i 在 [x, n-1] 中
                // 先把每个 score[k] 都加一
                diff[0] += 1;
                // 轮调 [i+1-x, i] 次无法得分，减掉
                diff[i + 1 - x] -= 1;
                diff[i + 1] += 1;
            }
        }

        let mut max_sum = 0;
        let mut best_k = 0;
        let mut sum = 0;
        for (i, d) in diff.into_iter().enumerate() {
            sum += d;
            // 此时 sum 为轮调 i 次后的得分
            if sum > max_sum {
                max_sum = sum;
                best_k = i;
            }
        }
        best_k as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

[1674. 使数组互补的最少操作次数](https://leetcode.cn/problems/minimum-moves-to-make-array-complementary/)

更多相似题目，见下面数据结构题单的「**§2.1 一维差分**」。

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
