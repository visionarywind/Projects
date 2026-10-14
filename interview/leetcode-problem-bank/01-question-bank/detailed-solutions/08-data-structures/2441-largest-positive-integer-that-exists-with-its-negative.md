# 2441. 与对应负数同时存在的最大正整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-positive-integer-that-exists-with-its-negative/
- 题目 slug：`largest-positive-integer-that-exists-with-its-negative`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：1168
- 外部题解来源：https://leetcode.cn/problems/largest-positive-integer-that-exists-with-its-negative/solutions/1895719/by-endlesscheng-jjtb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 一次遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/largest-positive-integer-that-exists-with-its-negative/solutions/1895719/by-endlesscheng-jjtb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-jjtb`
- topic id：`1895719`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

### 视频讲解

见[【周赛 315】](https://www.bilibili.com/video/BV1Ae4y1i7PM/)。

### 思路

用一个哈希表记录出现过的数字。一边遍历，一边看 $-\textit{nums}[i]$ 是否在哈希表中，如果在，就更新答案的最大值为 $|\textit{nums}[i]|$。

```py [sol1-Python3]
class Solution:
    def findMaxK(self, nums: List[int]) -> int:
        ans = -1
        s = set()
        for x in nums:
            if -x in s:
                ans = max(ans, abs(x))
            s.add(x)
        return ans
```

```java [sol1-Java]
class Solution {
    public int findMaxK(int[] nums) {
        int ans = -1;
        var s = new HashSet<Integer>();
        for (int x : nums) {
            if (s.contains(-x))
                ans = Math.max(ans, Math.abs(x));
            s.add(x);
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int findMaxK(vector<int> &nums) {
        int ans = -1;
        unordered_set<int> s;
        for (int x: nums) {
            if (s.count(-x))
                ans = max(ans, abs(x));
            s.insert(x);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func findMaxK(nums []int) int {
	ans := -1
	has := map[int]bool{}
	for _, x := range nums {
		if abs(x) > ans && has[-x] {
			ans = abs(x)
		}
		has[x] = true
	}
	return ans
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

---

[往期每日一题题解](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注[ biIibiIi@灵茶山艾府](https://space.bilibili.com/206214)，高质量算法教学，持续输出中~
