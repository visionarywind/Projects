# 2835. 使子序列的和等于目标的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-form-subsequence-with-target-sum/
- 题目 slug：`minimum-operations-to-form-subsequence-with-target-sum`
- 来源专题：位运算
- 来源分类路径：八、思维题
- 难度分：2207
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-form-subsequence-with-target-sum/solutions/2413344/tan-xin-by-endlesscheng-immn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-form-subsequence-with-target-sum/solutions/2413344/tan-xin-by-endlesscheng-immn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-by-endlesscheng-immn`
- topic id：`2413344`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 前置知识

请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

## 思路

由于可以把一个数一分为二，所以整个数组可以全部变成 $1$。因此如果 $\textit{nums}$ 的元素和小于 $\textit{target}$，则无解，返回 $-1$。否则一定有解。

然后从低位到高位贪心：

- 如果 $\textit{target}$ 的第 $i$ 位是 $0$，跳过。
- 如果 $\textit{target}$ 的第 $i$ 位是 $1$，那么先看看所有 $\le 2^i$ 的元素和能否 $\ge \textit{target}\& \textit{mask}$，其中 $\textit{mask}=2^{i+1}-1$。如果能，那么必然可以合并出 $\textit{target}\&  \textit{mask}$，无需操作（见 [视频](https://www.bilibili.com/video/BV1Em4y1T7Bq/) 中的证明）。
- 如果不能，那么就需要把一个更大的数（设它是 $2^j$）不断地一分为二，直到分解出 $2^i$ 为止。
- 注意分解完后，$2^i,2^{i+1},2^{i+2},\cdots,2^{j-1}$ 这些 $2$ 的幂我们都有了。所以后面 $i+1,i+2,\cdots, j-1$ 这些比特位都无需判断了，可以直接从第 $j$ 个比特位开始判断。

```py [sol-Python3]
class Solution:
    def minOperations(self, nums: List[int], target: int) -> int:
        if sum(nums) < target:
            return -1
        cnt = Counter(nums)
        ans = s = i = 0
        while 1 << i <= target:
            s += cnt[1 << i] << i
            mask = (1 << (i + 1)) - 1
            i += 1
            if s >= target & mask:
                continue
            ans += 1  # 一定要找更大的数操作
            while cnt[1 << i] == 0:
                ans += 1  # 还没找到，继续找更大的数
                i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minOperations(List<Integer> nums, int target) {
        long s = 0;
        int[] cnt = new int[31];
        for (int x : nums) {
            s += x;
            cnt[Integer.numberOfTrailingZeros(x)]++;
        }
        if (s < target) {
            return -1;
        }
        int ans = 0, i = 0;
        s = 0;
        while ((1L << i) <= target) {
            s += (long) cnt[i] << i;
            long mask = (1L << ++i) - 1;
            if (s >= (target & mask)) {
                continue;
            }
            ans++; // 一定要找更大的数操作
            for (; cnt[i] == 0; i++) {
                ans++; // 还没找到，继续找更大的数
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(vector<int>& nums, int target) {
        if (accumulate(nums.begin(), nums.end(), 0LL) < target) {
            return -1;
        }
        int cnt[31]{};
        for (int x : nums) {
            cnt[__builtin_ctz(x)]++;
        }
        int ans = 0, i = 0;
        long long s = 0;
        while ((1LL << i) <= target) {
            s += (long long) cnt[i] << i;
            int mask = (1LL << ++i) - 1;
            if (s >= (target & mask)) {
                continue;
            }
            ans++; // 一定要找更大的数操作
            for (; cnt[i] == 0; i++) {
                ans++; // 还没找到，继续找更大的数
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minOperations(nums []int, target int) (ans int) {
	s := 0
	cnt := [31]int{}
	for _, v := range nums {
		s += v
		cnt[bits.TrailingZeros(uint(v))]++
	}
	if s < target {
		return -1
	}
	s = 0
	for i := 0; 1<<i <= target; {
		s += cnt[i] << i
		mask := 1<<(i+1) - 1
		if s >= target&mask {
			i++
			continue
		}
		ans++
		for i++; cnt[i] == 0; i++ {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+\log \textit{target})$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(\log \textit{target})$。

## 分类题单

以下题单没有特定的顺序，可以按照个人喜好刷题。

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
