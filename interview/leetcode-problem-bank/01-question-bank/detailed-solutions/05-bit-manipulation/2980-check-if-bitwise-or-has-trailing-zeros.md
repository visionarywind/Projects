# 2980. 检查按位或是否存在尾随零

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-bitwise-or-has-trailing-zeros/
- 题目 slug：`check-if-bitwise-or-has-trailing-zeros`
- 来源专题：位运算
- 来源分类路径：三、与或（AND/OR）的性质
- 难度分：1234
- 外部题解来源：https://leetcode.cn/problems/check-if-bitwise-or-has-trailing-zeros/solutions/2585847/pan-duan-shi-fou-you-zhi-shao-liang-ge-o-aggh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[判断是否有至少两个偶数（Python/Java/C++/Go）](https://leetcode.cn/problems/check-if-bitwise-or-has-trailing-zeros/solutions/2585847/pan-duan-shi-fou-you-zhi-shao-liang-ge-o-aggh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pan-duan-shi-fou-you-zhi-shao-liang-ge-o-aggh`
- topic id：`2585847`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[本题视频讲解](https://www.bilibili.com/video/BV1XG411B7bX/)

$\textit{nums}$ 中的奇数肯定不能参与或运算，这会导致或运算结果的最低位必然是 $1$。

所以只有偶数能参与或运算，这样最低位必然是 $0$。

所以判断 $\textit{nums}$ 中是否至少有两个偶数即可。

```py [sol-Python3]
class Solution:
    def hasTrailingZeros(self, nums: List[int]) -> bool:
        return len(nums) - sum(x % 2 for x in nums) >= 2
```

```java [sol-Java]
class Solution {
    public boolean hasTrailingZeros(int[] nums) {
        int even = nums.length;
        for (int x : nums) {
            even -= x % 2;
        }
        return even >= 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool hasTrailingZeros(vector<int> &nums) {
        int even = nums.size();
        for (int x: nums) {
            even -= x % 2;
        }
        return even >= 2;
    }
};
```

```go [sol-Go]
func hasTrailingZeros(nums []int) bool {
	even := len(nums)
	for _, x := range nums {
		even -= x % 2
	}
	return even >= 2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。
