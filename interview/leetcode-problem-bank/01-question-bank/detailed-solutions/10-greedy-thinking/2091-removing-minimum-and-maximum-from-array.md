# 2091. 从数组中移除最大值和最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/removing-minimum-and-maximum-from-array/
- 题目 slug：`removing-minimum-and-maximum-from-array`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1384
- 外部题解来源：https://leetcode.cn/problems/removing-minimum-and-maximum-from-array/solutions/1126273/san-chong-tan-xin-ce-lue-qu-zui-xiao-zhi-fhnn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种情况（Python/Java/C++/Go）](https://leetcode.cn/problems/removing-minimum-and-maximum-from-array/solutions/1126273/san-chong-tan-xin-ce-lue-qu-zui-xiao-zhi-fhnn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-chong-tan-xin-ce-lue-qu-zui-xiao-zhi-fhnn`
- topic id：`1126273`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

设 $\textit{nums}$ 的长度为 $n$，其最小值的下标为 $p$，最大值的下标为 $q$。假设 $p\le q$（若不满足则交换）。

根据题意，有三种删除策略：

- 删除包含 $p$ 和 $q$ 的前缀，长为 $q+1$。
- 删除包含 $p$ 和 $q$ 的后缀，长为 $n-p$。
- 删除包含 $p$ 的前缀，以及包含 $q$ 的后缀，长为 $(p+1)+(n-q)$。

取三者最小值，即为答案。

```py [sol-Python3]
class Solution:
    def minimumDeletions(self, nums: List[int]) -> int:
        n = len(nums)
        p = q = 0
        for i, x in enumerate(nums):
            if x < nums[p]:
                p = i
            elif x > nums[q]:
                q = i

        if p > q:
            p, q = q, p  # 保证 p <= q，方便下面计算
        return min(q + 1, n - p, p + 1 + n - q)
```

```py [sol-Python3 写法二]
class Solution:
    def minimumDeletions(self, nums: List[int]) -> int:
        n = len(nums)
        p = nums.index(min(nums))
        q = nums.index(max(nums))

        if p > q:
            p, q = q, p  # 保证 p <= q，方便下面计算
        return min(q + 1, n - p, p + 1 + n - q)
```

```java [sol-Java]
class Solution {
    public int minimumDeletions(int[] nums) {
        int n = nums.length;
        int p = 0;
        int q = 0;
        for (int i = 0; i < n; i++) {
            if (nums[i] < nums[p]) {
                p = i;
            } else if (nums[i] > nums[q]) {
                q = i;
            }
        }

        if (p > q) {
            // 保证 p <= q，方便下面计算
            int tmp = p;
            p = q;
            q = tmp;
        }
        return Math.min(Math.min(q + 1, n - p), p + 1 + n - q);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumDeletions(vector<int>& nums) {
        int n = nums.size();
        int p = 0, q = 0;
        for (int i = 0; i < n; i++) {
            if (nums[i] < nums[p]) {
                p = i;
            } else if (nums[i] > nums[q]) {
                q = i;
            }
        }

        if (p > q) {
            swap(p, q); // 保证 p <= q，方便下面计算
        }
        return min({q + 1, n - p, p + 1 + n - q});
    }
};
```

```go [sol-Go]
func minimumDeletions(nums []int) int {
	n := len(nums)
	p, q := 0, 0
	for i, x := range nums {
		if x < nums[p] {
			p = i
		} else if x > nums[q] {
			q = i
		}
	}

	if p > q {
		p, q = q, p // 保证 p <= q，方便下面计算
	}
	return min(q+1, n-p, p+1+n-q)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面贪心与思维题单的「**§5.8 分类讨论**」。

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
