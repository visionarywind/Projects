# 2765. 最长交替子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-alternating-subarray/
- 题目 slug：`longest-alternating-subarray`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1581
- 外部题解来源：https://leetcode.cn/problems/longest-alternating-subarray/solutions/2615916/jiao-ni-yi-ci-xing-ba-dai-ma-xie-dui-on-r57bz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一次性把代码写对！O(n) 分组循环（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/longest-alternating-subarray/solutions/2615916/jiao-ni-yi-ci-xing-ba-dai-ma-xie-dui-on-r57bz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-ci-xing-ba-dai-ma-xie-dui-on-r57bz`
- topic id：`2615916`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**适用场景**：按照题目要求，数组会被分割成若干组，且每一组的判断/处理逻辑是一样的。

**核心思想**：

- 外层循环负责遍历组之前的准备工作（记录开始位置），和遍历组之后的统计工作（更新答案最大值）。
- 内层循环负责遍历组，找出这一组最远在哪结束。

这个写法的好处是，各个逻辑块分工明确，也不需要特判最后一组。以我的经验，这个写法是所有写法中最不容易出 bug 的，推荐大家记住。

对于本题来说，在内层循环时，假设这一组的第一个数是 $3$，那么这一组的数字必须形如 $3,4,3,4,\cdots$，也就是

$$
\textit{nums}[i] = \textit{nums}[i-2]
$$

另外，对于 $[3,4,3,4,5,4,5]$ 这样的数组，第一组交替子数组为 $[3,4,3,4]$，第二组交替子数组为 $[4,5,4,5]$，这两组有一个数是**重叠**的，所以下面代码在外层循环末尾要把 $i$ 减一。

```py [sol-Python3]
class Solution:
    def alternatingSubarray(self, nums: List[int]) -> int:
        ans = -1
        i, n = 0, len(nums)
        while i < n - 1:
            if nums[i + 1] - nums[i] != 1:
                i += 1  # 直接跳过
                continue
            i0 = i  # 记录这一组的开始位置
            i += 2  # i 和 i+1 已经满足要求，从 i+2 开始判断
            while i < n and nums[i] == nums[i - 2]:
                i += 1
            # 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
            ans = max(ans, i - i0)
            i -= 1
        return ans 
```

```java [sol-Java]
class Solution {
    public int alternatingSubarray(int[] nums) {
        int ans = -1;
        int i = 0, n = nums.length;
        while (i < n - 1) {
            if (nums[i + 1] - nums[i] != 1) {
                i++; // 直接跳过
                continue;
            }
            int i0 = i; // 记录这一组的开始位置
            i += 2; // i 和 i+1 已经满足要求，从 i+2 开始判断
            while (i < n && nums[i] == nums[i - 2]) {
                i++;
            }
            // 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
            ans = Math.max(ans, i - i0);
            i--;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int alternatingSubarray(vector<int> &nums) {
        int ans = -1;
        int i = 0, n = nums.size();
        while (i < n - 1) {
            if (nums[i + 1] - nums[i] != 1) {
                i++; // 直接跳过
                continue;
            }
            int i0 = i; // 记录这一组的开始位置
            i += 2; // i 和 i+1 已经满足要求，从 i+2 开始判断
            while (i < n && nums[i] == nums[i - 2]) {
                i++;
            }
            // 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
            ans = max(ans, i - i0);
            i--;
        }
        return ans;
    }
};
```

```go [sol-Go]
func alternatingSubarray(nums []int) int {
    ans := -1
    i, n := 0, len(nums)
    for i < n-1 {
        if nums[i+1]-nums[i] != 1 {
            i++ // 直接跳过
            continue
        }
        i0 := i // 记录这一组的开始位置
        i += 2  // i 和 i+1 已经满足要求，从 i+2 开始判断
        for i < n && nums[i] == nums[i-2] {
            i++
        }
        // 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
        ans = max(ans, i-i0)
        i--
    }
    return ans
}
```

```js [sol-JavaScript]
var alternatingSubarray = function(nums) {
    const n = nums.length;
    let ans = -1, i = 0;
    while (i < n - 1) {
        if (nums[i + 1] - nums[i] !== 1) {
            i++; // 直接跳过
            continue;
        }
        const i0 = i; // 记录这一组的开始位置
        i += 2; // i 和 i+1 已经满足要求，从 i+2 开始判断
        while (i < n && nums[i] === nums[i - 2]) {
            i++;
        }
        // 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
        ans = Math.max(ans, i - i0);
        i--;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn alternating_subarray(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut ans = -1;
        let mut i = 0;
        while i < n - 1 {
            if nums[i + 1] - nums[i] != 1 {
                i += 1; // 直接跳过
                continue;
            }
            let i0 = i; // 记录这一组的开始位置
            i += 2; // i 和 i+1 已经满足要求，从 i+2 开始判断
            while i < n && nums[i] == nums[i - 2] {
                i += 1;
            }
            // 从 i0 到 i-1 是满足题目要求的（并且无法再延长的）子数组
            ans = ans.max((i - i0) as i32);
            i -= 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。时间复杂度乍一看是 $\mathcal{O}(n^2)$，但注意变量 $i$ 减少的次数是 $\mathcal{O}(n)$ 的，其它情况一直在增加，由于 $i$ 最大是 $n$，所以增加的次数是 $\mathcal{O}(n)$，所以二重循环总共循环 $\mathcal{O}(n)$ 次，时间复杂度是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 练习

一般来说，分组循环的模板如下（可根据题目调整）：

```py
n = len(nums)
i = 0
while i < n:
    start = i
    while i < n and ...:
        i += 1
    # 从 start 到 i-1 是一组
    # 下一组从 i 开始，无需 i += 1
```

学会一个模板是远远不够的，需要大量练习才能灵活运用。

- [1446. 连续字符](https://leetcode.cn/problems/consecutive-characters/) 1165
- [1869. 哪种连续子字符串更长](https://leetcode.cn/problems/longer-contiguous-segments-of-ones-than-zeros/) 1205
- [1957. 删除字符使字符串变好](https://leetcode.cn/problems/delete-characters-to-make-fancy-string/) 1358
- [2110. 股票平滑下跌阶段的数目](https://leetcode.cn/problems/number-of-smooth-descent-periods-of-a-stock/) 1408
- [228. 汇总区间](https://leetcode.cn/problems/summary-ranges/)
- [2760. 最长奇偶子数组](https://leetcode.cn/problems/longest-even-odd-subarray-with-threshold/) 1420
- [1887. 使数组元素相等的减少操作次数](https://leetcode.cn/problems/reduction-operations-to-make-the-array-elements-equal/) 1428
- [2038. 如果相邻两个颜色均相同则删除当前颜色](https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/) 1468
- [1759. 统计同质子字符串的数目](https://leetcode.cn/problems/count-number-of-homogenous-substrings/) 1491
- [1578. 使绳子变成彩色的最短时间](https://leetcode.cn/problems/minimum-time-to-make-rope-colorful/) 1574
- [1839. 所有元音按顺序排布的最长子字符串](https://leetcode.cn/problems/longest-substring-of-all-vowels-in-order/) 1580

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
