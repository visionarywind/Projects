# 2382. 删除操作后的最大子段和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-segment-sum-after-removals/
- 题目 slug：`maximum-segment-sum-after-removals`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.4 逆向思维
- 难度分：2136
- 外部题解来源：https://leetcode.cn/problems/maximum-segment-sum-after-removals/solutions/1763638/by-endlesscheng-p61j/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[倒序回答 + 并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-segment-sum-after-removals/solutions/1763638/by-endlesscheng-p61j/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-p61j`
- topic id：`1763638`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

#### 提示 1 

删除不好做，添加比较好做。不妨倒着思考，删除变成了添加。

#### 提示 2

添加时可能会合并两个子段。

我们需要考虑如何动态维护每个子段的元素和，并高效地合并两个子段。

#### 提示 3

用并查集，添加下标 $x=\textit{removeQueries}[i]$ 时，用并查集合并 $x$ 和 $x+1$，并把 $\textit{nums}[x]$ 加到子段和中。

以 $\textit{removeQueries}=[3,1,2,0]$ 为例说明。倒序遍历，我们会先合并下标 $0$ 和 $1$，这样相当于创建了一个下标子段 $[0]$；然后合并 $2$ 和 $3$，创建了下标子段 $[2]$；然后合并 $1$ 和 $2$，由于 $0$ 和 $1$ 已经合并了，这一操作会把 $0 1 2$ 都合并起来，最终形成下标子段 $[0,1,2]$。注意，这一合并过程中会形成若干条「链」，每一条链去掉最右边的元素就等价于实际的下标子段。

另外一种理解方式是，把链看成是一列火车，这列火车有一节「幽灵火车头」，不算在实际的子段中。如果要合并两条链，就需要把左边这节幽灵火车头作为一节实际的车厢加到右边这列火车中，因此我们只需要合并 $x$ 和 $x+1$，不需要合并 $x$ 和 $x-1$。

最后，对于 $\textit{ans}[i]$，要么取上一个 $\textit{ans}[i+1]$ 的最大子段和，要么取合并后的子段和，这两者取最大值。

```py [sol1-Python3]
class Solution:
    def maximumSegmentSum(self, nums: List[int], removeQueries: List[int]) -> List[int]:
        n = len(nums)
        fa = list(range(n + 1))
        sum = [0] * (n + 1)
        def find(x: int) -> int:
            if fa[x] != x:
                fa[x] = find(fa[x])
            return fa[x]
        ans = [0] * n
        for i in range(n - 1, 0, -1):
            x = removeQueries[i]
            to = find(x + 1)
            fa[x] = to  # 合并 x 和 x+1
            sum[to] += sum[x] + nums[x]
            ans[i - 1] = max(ans[i], sum[to])
        return ans
```

```java [sol1-Java]
class Solution {
    int[] fa;

    public long[] maximumSegmentSum(int[] nums, int[] removeQueries) {
        var n = nums.length;
        fa = new int[n + 1];
        for (var i = 0; i <= n; i++) fa[i] = i;
        var sum = new long[n + 1];

        var ans = new long[n];
        for (var i = n - 1; i > 0; --i) {
            var x = removeQueries[i];
            var to = find(x + 1);
            fa[x] = to; // 合并 x 和 x+1
            sum[to] += sum[x] + nums[x];
            ans[i - 1] = Math.max(ans[i], sum[to]);
        }
        return ans;
    }

    int find(int x) {
        if (fa[x] != x) fa[x] = find(fa[x]);
        return fa[x];
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<long long> maximumSegmentSum(vector<int> &nums, vector<int> &removeQueries) {
        int n = nums.size();
        int fa[n + 1];
        iota(fa, fa + n + 1, 0);
        long long sum[n + 1];
        memset(sum, 0, sizeof(sum));
        function<int(int)> find = [&](int x) -> int { return fa[x] == x ? x : fa[x] = find(fa[x]); };

        vector<long long> ans(n);
        for (int i = n - 1; i > 0; --i) {
            int x = removeQueries[i];
            int to = find(x + 1);
            fa[x] = to; // 合并 x 和 x+1
            sum[to] += sum[x] + nums[x];
            ans[i - 1] = max(ans[i], sum[to]);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func maximumSegmentSum(nums []int, removeQueries []int) (ans []int64) {
	n := len(nums)
	fa := make([]int, n+1)
	for i := range fa {
		fa[i] = i
	}
	sum := make([]int64, n+1)
	var find func(int) int
	find = func(x int) int {
		if fa[x] != x {
			fa[x] = find(fa[x])
		}
		return fa[x]
	}

	ans = make([]int64, n)
	for i := n - 1; i > 0; i-- {
		x := removeQueries[i]
		to := find(x + 1)
		fa[x] = to // 合并 x 和 x+1
		sum[to] += sum[x] + int64(nums[x])
		ans[i-1] = max(ans[i], sum[to])
	}
	return
}

func max(a, b int64) int64 { if b > a { return b }; return a }
```

#### 相似题目

- [2334. 元素值大于变化阈值的子数组](https://leetcode.cn/problems/subarray-with-elements-greater-than-varying-threshold/)
- [1562. 查找大小为 M 的最新分组](https://leetcode.cn/problems/find-latest-group-of-size-m/)
