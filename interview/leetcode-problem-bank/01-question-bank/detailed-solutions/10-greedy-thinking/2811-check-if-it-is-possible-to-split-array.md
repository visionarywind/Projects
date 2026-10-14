# 2811. 判断是否能拆分数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-it-is-possible-to-split-array/
- 题目 slug：`check-if-it-is-possible-to-split-array`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1543
- 外部题解来源：https://leetcode.cn/problems/check-if-it-is-possible-to-split-array/solutions/2375178/nao-jin-ji-zhuan-wan-by-endlesscheng-0l19/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯（Python/Java/C++/Go）](https://leetcode.cn/problems/check-if-it-is-possible-to-split-array/solutions/2375178/nao-jin-ji-zhuan-wan-by-endlesscheng-0l19/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-by-endlesscheng-0l19`
- topic id：`2375178`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1Yr4y1o7aP/) 第二题。

先特判 $n\le 2$ 的情况，这是满足要求的。

对于 $n\ge 3$ 的情况，无论按照何种方式分割，一定会在某个时刻，分割出一个长为 $2$ 的子数组。

如果 $\textit{nums}$ 中任何长为 $2$ 的子数组的元素和都小于 $m$，那么无法满足要求。

否则，可以用这个子数组作为「核心」，像剥洋葱一样，**一个一个地**去掉 $\textit{nums}$ 的首尾元素，最后得到这个子数组。由于子数组的元素和 $\ge m$，所以每次分割出一个元素时，剩余的子数组的元素和也必然是 $\ge m$ 的，满足要求。

所以问题变成：判断数组中是否有两个相邻数字 $\ge m$。

```py [sol-Python3]
class Solution:
    def canSplitArray(self, nums: List[int], m: int) -> bool:
        return len(nums) <= 2 or any(x + y >= m for x, y in pairwise(nums))
```

```java [sol-Java]
class Solution {
    public boolean canSplitArray(List<Integer> nums, int m) {
        int n = nums.size();
        if (n <= 2) return true;
        for (int i = 1; i < n; i++)
            if (nums.get(i - 1) + nums.get(i) >= m)
                return true;
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canSplitArray(vector<int> &nums, int m) {
        int n = nums.size();
        if (n <= 2) return true;
        for (int i = 1; i < n; i++)
            if (nums[i - 1] + nums[i] >= m)
                return true;
        return false;
    }
};
```

```go [sol-Go]
func canSplitArray(nums []int, m int) bool {
	n := len(nums)
	if n <= 2 {
		return true
	}
	for i := 1; i < n; i++ {
		if nums[i-1]+nums[i] >= m {
			return true
		}
	}
	return false
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。
