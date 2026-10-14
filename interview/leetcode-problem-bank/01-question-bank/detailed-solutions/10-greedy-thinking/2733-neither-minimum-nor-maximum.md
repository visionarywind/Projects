# 2733. 既不是最小值也不是最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/neither-minimum-nor-maximum/
- 题目 slug：`neither-minimum-nor-maximum`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1148
- 外部题解来源：https://leetcode.cn/problems/neither-minimum-nor-maximum/solutions/2305393/o1-shi-jian-zuo-fa-pythonjavacgo-by-endl-sa7h/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(1) 时间做法（Python/Java/C++/Go）](https://leetcode.cn/problems/neither-minimum-nor-maximum/solutions/2305393/o1-shi-jian-zuo-fa-pythonjavacgo-by-endl-sa7h/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-shi-jian-zuo-fa-pythonjavacgo-by-endl-sa7h`
- topic id：`2305393`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

### 思路

由于数组元素各不相同，所以答案一定在前三个数中，不妨取前三个数的中间值。

```py [sol-Python3]
class Solution:
    def findNonMinOrMax(self, nums: List[int]) -> int:
        return sorted(nums[:3])[1] if len(nums) > 2 else -1
```

```java [sol-Java]
class Solution {
    public int findNonMinOrMax(int[] nums) {
        if (nums.length < 3) return -1;
        Arrays.sort(nums, 0, 3); // 只对前三个数排序
        return nums[1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findNonMinOrMax(vector<int> &nums) {
        if (nums.size() < 3) return -1;
        sort(nums.begin(), nums.begin() + 3); // 只对前三个数排序
        return nums[1];
    }
};
```

```go [sol-Go]
func findNonMinOrMax(nums []int) int {
	if len(nums) < 3 {
		return -1
	}
	sort.Ints(nums[:3]) // 只对前三个数排序
	return nums[1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。
