# 4044. 统计好循环移位的数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-good-cyclic-rotations/
- 题目 slug：`count-good-cyclic-rotations`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-good-cyclic-rotations/solutions/4023626/liang-chong-fang-fa-qian-zhui-he-ding-ch-wh1y/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：前缀和 / 定长滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/count-good-cyclic-rotations/solutions/4023626/liang-chong-fang-fa-qian-zhui-he-ding-ch-wh1y/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-qian-zhui-he-ding-ch-wh1y`
- topic id：`4023626`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：前缀和

任意循环移位后的数组，都是 $\textit{nums} + \textit{nums}$ 的子数组。

预处理 $\textit{nums} + \textit{nums}$ 的 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 后，可以 $\mathcal{O}(1)$ 计算任意子数组的和。

注意 $\textit{nums} + \textit{nums}$ 的第一个长为 $n$ 的子数组和最后一个长为 $n$ 的子数组是同一个，为避免重复统计，不考虑最后一个长为 $n$ 的子数组。

下午两点 [B站@灵茶山艾府](https://space.bilibili.com/206214) 直播讲题，欢迎关注~

```py [sol-Python3]
class Solution:
    def countGoodRotations(self, nums: list[int]) -> int:
        s = list(accumulate(nums + nums, initial=0))

        n = len(nums)
        ans = 0
        for i in range(n, n * 2):
            # sum[i-n/2]-sum[i-n] > sum[i]-sum[i-n/2]
            if s[i - n // 2] * 2 > s[i] + s[i - n]:
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int countGoodRotations(int[] nums) {
        int n = nums.length;
        long[] sum = new long[n * 2 + 1];
        for (int i = 0; i < n * 2; i++) {
            sum[i + 1] = sum[i] + nums[i % n];
        }

        int ans = 0;
        for (int i = n; i < n * 2; i++) {
            // sum[i-n/2]-sum[i-n] > sum[i]-sum[i-n/2]
            if (sum[i - n / 2] * 2 > sum[i] + sum[i - n]) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countGoodRotations(vector<int>& nums) {
        int n = nums.size();
        vector<long long> sum(n * 2 + 1);
        for (int i = 0; i < n * 2; i++) {
            sum[i + 1] = sum[i] + nums[i % n];
        }

        int ans = 0;
        for (int i = n; i < n * 2; i++) {
            // sum[i-n/2]-sum[i-n] > sum[i]-sum[i-n/2]
            if (sum[i - n / 2] * 2 > sum[i] + sum[i - n]) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countGoodRotations(nums []int) (ans int) {
	n := len(nums)
	sum := make([]int, n*2+1)
	for i := range n * 2 {
		sum[i+1] = sum[i] + nums[i%n]
	}

	for i := n; i < n*2; i++ {
		// sum[i-n/2]-sum[i-n] > sum[i]-sum[i-n/2]
		if sum[i-n/2]*2 > sum[i]+sum[i-n] {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：定长滑动窗口

视作两个长为 $n/2$ 的**定长滑动窗口**同时向右滑动。原理请看[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)

```py [sol-Python3]
class Solution:
    def countGoodRotations(self, nums: list[int]) -> int:
        n = len(nums)
        m = n // 2
        ans = sum1 = sum2 = 0
        for i in range(m, n * 2 - 1):
            # 1. 入
            sum1 += nums[(i - m) % n]
            sum2 += nums[i % n]

            left = i - n + 1
            if left < 0:  # 尚未形成第一个窗口
                continue

            # 2. 更新答案
            if sum1 > sum2:
                ans += 1

            # 3. 出
            sum1 -= nums[left]
            sum2 -= nums[(left + m) % n]
        return ans
```

```java [sol-Java]
class Solution {
    public int countGoodRotations(int[] nums) {
        int n = nums.length;
        long sum1 = 0;
        long sum2 = 0;
        int ans = 0;
        for (int i = n / 2; i < n * 2 - 1; i++) {
            // 1. 入
            sum1 += nums[(i - n / 2) % n];
            sum2 += nums[i % n];

            int left = i - n + 1;
            if (left < 0) { // 尚未形成第一个窗口
                continue;
            }

            // 2. 更新答案
            if (sum1 > sum2) {
                ans++;
            }

            // 3. 出
            sum1 -= nums[left];
            sum2 -= nums[(left + n / 2) % n];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countGoodRotations(vector<int>& nums) {
        int n = nums.size();
        long long sum1 = 0, sum2 = 0;
        int ans = 0;
        for (int i = n / 2; i < n * 2 - 1; i++) {
            // 1. 入
            sum1 += nums[(i - n / 2) % n];
            sum2 += nums[i % n];

            int left = i - n + 1;
            if (left < 0) { // 尚未形成第一个窗口
                continue;
            }

            // 2. 更新答案
            if (sum1 > sum2) {
                ans++;
            }

            // 3. 出
            sum1 -= nums[left];
            sum2 -= nums[(left + n / 2) % n];
        }
        return ans;
    }
};
```

```go [sol-Go]
func countGoodRotations(nums []int) (ans int) {
	n := len(nums)
	sum1, sum2 := 0, 0
	for i := n / 2; i < n*2-1; i++ {
		// 1. 入
		sum1 += nums[(i-n/2)%n]
		sum2 += nums[i%n]

		left := i - n + 1
		if left < 0 { // 尚未形成第一个窗口
			continue
		}

		// 2. 更新答案
		if sum1 > sum2 {
			ans++
		}

		// 3. 出
		sum1 -= nums[left]
		sum2 -= nums[(left+n/2)%n]
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

[3652. 按策略买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-using-strategy/)

## 专题训练

1. 数据结构题单的「**一、前缀和**」。
2. 滑动窗口题单的「**一、定长滑动窗口**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
