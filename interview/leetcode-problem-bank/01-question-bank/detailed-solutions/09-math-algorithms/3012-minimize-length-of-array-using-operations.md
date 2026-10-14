# 3012. 通过操作使数组长度最小

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-length-of-array-using-operations/
- 题目 slug：`minimize-length-of-array-using-operations`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1833
- 外部题解来源：https://leetcode.cn/problems/minimize-length-of-array-using-operations/solutions/2613059/on-nao-jin-ji-zhuan-wan-pythonjavacgo-by-2lea/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 脑筋急转弯（Python/Java/C++/Go）](https://leetcode.cn/problems/minimize-length-of-array-using-operations/solutions/2613059/on-nao-jin-ji-zhuan-wan-pythonjavacgo-by-2lea/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-nao-jin-ji-zhuan-wan-pythonjavacgo-by-2lea`
- topic id：`2613059`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

[视频讲解](https://www.bilibili.com/video/BV1oV411D7gB/) 第三题。

考虑这个例子：$\textit{nums}=[2,3,4,5,6]$，每次操作都可以选择 $2$ 和另一个数字 $x$，由于 $x>2$，所以 $2\bmod x = 2$，于是操作等价于：

- 移除 $x$。

所以最后必定只会剩下 $2$。

所以，如果数组中的最小值只有一个，我们可以操作成只剩下一个数，返回 $1$。

但如果最小值不止一个呢？如果能构造出一个小于 $m = \min(\textit{nums})$ 的正整数，那么也可以返回 $1$。

**结论**：当且仅当 $\textit{nums}$ 中有不是 $m$ 的倍数的数，我们才能构造出一个小于 $m$ 的正整数。

**证明**：如果有不是 $m$ 的倍数的数 $x$，那么 $0 < x\bmod m < m$，构造成功。如果所有数都是 $m$ 的倍数，那么任意两个数的模都是 $m$ 的倍数，我们无法得到一个在 $[1,m-1]$ 内的数。

如果所有数都是 $m$ 的倍数，我们可以先用 $m$ 把大于 $m$ 的数都移除，然后剩下的 $\textit{cnt}$ 个 $m$ 两两一对消除，最后剩下 $\left\lceil\dfrac{\textit{cnt}}{2}\right\rceil$ 个数。

```py [sol-Python3]
class Solution:
    def minimumArrayLength(self, nums: List[int]) -> int:
        m = min(nums)
        for x in nums:
            if x % m:
                return 1
        return (nums.count(m) + 1) // 2
```

```java [sol-Java]
class Solution {
    public int minimumArrayLength(int[] nums) {
        int m = Integer.MAX_VALUE;
        for (int x : nums) {
            m = Math.min(m, x);
        }

        for (int x : nums) {
            if (x % m > 0) {
                return 1;
            }
        }

        int cnt = 0;
        for (int x : nums) {
            if (x == m) {
                cnt++;
            }
        }
        return (cnt + 1) / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumArrayLength(vector<int> &nums) {
        int m = ranges::min(nums);
        for (int x : nums) {
            if (x % m) {
                return 1;
            }
        }
        return (ranges::count(nums, m) + 1) / 2;
    }
};
```

```go [sol-Go]
func minimumArrayLength(nums []int) int {
	m := slices.Min(nums)
	for _, x := range nums {
		if x%m > 0 {
			return 1
		}
	}
	cnt := 0
	for _, x := range nums {
		if x == m {
			cnt++
		}
	}
	return (cnt + 1) / 2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)
