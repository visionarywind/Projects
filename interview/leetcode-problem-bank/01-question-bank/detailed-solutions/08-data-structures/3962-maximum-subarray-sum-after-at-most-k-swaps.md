# 3962. 至多 K 次交换后最大子数组和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-subarray-sum-after-at-most-k-swaps/
- 题目 slug：`maximum-subarray-sum-after-at-most-k-swaps`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）
- 难度分：2672
- 外部题解来源：https://leetcode.cn/problems/maximum-subarray-sum-after-at-most-k-swaps/solutions/3983444/zhi-yu-shu-zhuang-shu-zu-by-endlesscheng-txva/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[值域树状数组 + 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-subarray-sum-after-at-most-k-swaps/solutions/3983444/zhi-yu-shu-zhuang-shu-zu-by-endlesscheng-txva/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-yu-shu-zhuang-shu-zu-by-endlesscheng-txva`
- topic id：`3983444`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于 $n\le 1500$，我们可以枚举子数组的左右端点。

外层循环枚举左端点，内层循环枚举右端点，不断向右扩大子数组长度。

贪心地，**把子数组内较小的数与子数组外较大的数交换**。所以要用两个数据结构，分别维护：

- 子数组内的前 $k$ **小**元素和。
- 子数组外的前 $k$ **大**元素和。

这可以用**对顶堆**或者**值域树状数组**维护，题解用的后者。我们可以在值域树状数组上二分，从而求出第 $k$ 小以及前 $k$ 小元素和，原理见 [OI-wiki](https://oi-wiki.org/ds/fenwick/#%E5%8D%95%E7%82%B9%E4%BF%AE%E6%94%B9%E6%9F%A5%E8%AF%A2%E5%85%A8%E5%B1%80%E7%AC%AC-k-%E5%B0%8F)。

但是，如果子数组内的第 $k$ 小比子数组外的第 $k$ 大还要大，那么不能交换。这意味着，实际交换次数可能小于 $k$。

难道要二分交换次数吗？这样总体时间复杂度是 $\mathcal{O}(n^2\log ^2 n)$，太慢了。

注意到，当子数组长度增加一时，只有一个元素从外面进入子数组，交换次数至多增加 $1$ 或者减少 $1$。

设元素进入子数组前，需要交换 $\textit{needSwap}$ 次。

- 如果子数组内的第 $\textit{needSwap}+1$ 个数 $<$ 子数组外的第 $\textit{needSwap}+1$ 个数，那么需要再交换一次，把 $\textit{needSwap}$ 加一。
- 如果子数组内的第 $\textit{needSwap}$ 个数 $\ge$ 子数组外的第 $\textit{needSwap}$ 个数，那么这一对元素不应交换，把 $\textit{needSwap}$ 减一。

[本题视频讲解](https://www.bilibili.com/video/BV1ptJw6hENZ/?t=20m58s)，欢迎点赞关注~

## 优化前

```py [sol-Python3]
# ⚠ 超时了！请看优化后的代码
class FenwickTree:
    def __init__(self, sorted_nums: List[int]):
        self.n = n = len(sorted_nums)
        self.high_bit = 1 << (n.bit_length() - 1)
        self.sorted_nums = sorted_nums
        self.cnt = [0] * (n + 1)
        self.sum = [0] * (n + 1)

    # 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    # 如果 num < 0，表示减少 -num 个 val
    # 注意 val = sorted_nums[i - 1]，无需手动传入
    def update(self, i: int, num: int) -> None:
        val = self.sorted_nums[i - 1]
        while i <= self.n:
            self.cnt[i] += num
            self.sum[i] += num * val
            i += i & -i

    # 返回第 k 小的数（k 从 1 开始）
    def kth(self, k: int) -> int:
        i = 0
        b = self.high_bit
        while b > 0:
            nxt = i | b
            if nxt <= self.n and self.cnt[nxt] < k:
                k -= self.cnt[nxt]
                i = nxt
            b >>= 1
        return self.sorted_nums[i]

    # 返回前 k 小的数之和（k 从 1 开始）
    def pre_sum(self, k: int) -> int:
        s = i = 0
        b = self.high_bit
        while b > 0:
            nxt = i | b
            if nxt <= self.n and self.cnt[nxt] < k:
                k -= self.cnt[nxt]
                s += self.sum[nxt]
                i = nxt
            b >>= 1
        # 加上等于第 k 小的数
        return s + self.sorted_nums[i] * k

    def copy(self) -> FenwickTree:
        t = FenwickTree(self.sorted_nums)
        t.cnt[:] = self.cnt
        t.sum[:] = self.sum
        return t


class Solution:
    def maxSum(self, nums: list[int], k: int) -> int:
        # 离散化
        n = len(nums)
        sorted_nums = sorted(set(nums))
        rank = [0] * n  # rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        all_tree = FenwickTree(sorted_nums)  # 包含所有元素的树状数组
        total = 0
        for i, x in enumerate(nums):
            rank[i] = bisect_left(sorted_nums, x) + 1
            all_tree.update(rank[i], 1)
            total += x

        ans = -inf

        # 枚举子数组左端点
        for left in range(n):
            in_tree = FenwickTree(sorted_nums)
            out_tree = all_tree.copy()
            need_swap = sub_sum = 0

            # 枚举子数组右端点
            for right in range(left, n):
                # x 从子数组外移到子数组内
                x = nums[right]
                rk = rank[right]
                sub_sum += x
                in_tree.update(rk, 1)
                out_tree.update(rk, -1)

                inc = False
                sz = right - left + 1
                if need_swap < k and need_swap < sz and need_swap < n - sz:
                    # 能否多交换一次
                    if in_tree.kth(need_swap + 1) < out_tree.kth(n - sz - need_swap):
                        inc = True
                        need_swap += 1

                if not inc and need_swap > 0:
                    # 是否要减少交换次数
                    if in_tree.kth(need_swap) >= out_tree.kth(n - sz - need_swap + 1):
                        need_swap -= 1

                # 计算通过交换导致的元素和的增量
                delta = 0
                if need_swap > 0:
                    in_sum = in_tree.pre_sum(need_swap)
                    out_sum = total - sub_sum - out_tree.pre_sum(n - sz - need_swap)
                    delta = out_sum - in_sum

                ans = max(ans, sub_sum + delta)

        return ans

# ⚠ 超时了！请看优化后的代码
```

```java [sol-Java]
class FenwickTree {
    private final int highBit;
    private final int[] sorted;
    private final int[] cnt;
    private final long[] sum;

    public FenwickTree(int[] sorted) {
        int n = sorted.length;
        highBit = Integer.highestOneBit(n);
        this.sorted = sorted;
        cnt = new int[n + 1];
        sum = new long[n + 1];
    }

    // 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    // 如果 num < 0，表示减少 -num 个 val
    // 注意 val = sorted[i - 1]，无需手动传入
    public void update(int i, int num) {
        int val = sorted[i - 1];
        for (; i < cnt.length; i += i & -i) {
            cnt[i] += num;
            sum[i] += (long) num * val;
        }
    }

    // 返回第 k 小的数（k 从 1 开始）
    public int kth(int k) {
        int i = 0;
        for (int b = highBit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.length && cnt[nxt] < k) {
                k -= cnt[nxt];
                i = nxt;
            }
        }
        return sorted[i];
    }

    // 返回前 k 小的数之和（k 从 1 开始）
    public long preSum(int k) {
        long s = 0;
        int i = 0;
        for (int b = highBit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.length && cnt[nxt] < k) {
                k -= cnt[nxt];
                s += sum[nxt];
                i = nxt;
            }
        }
        // 加上等于第 k 小的数
        return s + (long) sorted[i] * k;
    }

    public FenwickTree copy() {
        FenwickTree f = new FenwickTree(sorted);
        System.arraycopy(cnt, 0, f.cnt, 0, cnt.length);
        System.arraycopy(sum, 0, f.sum, 0, sum.length);
        return f;
    }
}

class Solution {
    public long maxSum(int[] nums, int k) {
        // 离散化
        int n = nums.length;
        int[] sorted = nums.clone();
        Arrays.sort(sorted);
        int[] rank = new int[n]; // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        FenwickTree allTree = new FenwickTree(sorted); // 包含所有元素的树状数组
        long total = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            rank[i] = Arrays.binarySearch(sorted, x) + 1;
            allTree.update(rank[i], 1);
            total += x;
        }

        long ans = Long.MIN_VALUE;

        // 枚举子数组左端点
        for (int left = 0; left < n; left++) {
            FenwickTree inTree = new FenwickTree(sorted);
            FenwickTree outTree = allTree.copy();
            int needSwap = 0;
            long subSum = 0;

            // 枚举子数组右端点
            for (int right = left; right < n; right++) {
                // x 从子数组外移到子数组内
                int x = nums[right];
                int rk = rank[right];
                subSum += x;
                inTree.update(rk, 1);
                outTree.update(rk, -1);

                boolean inc = false;
                int sz = right - left + 1;
                if (needSwap < k && needSwap < sz && needSwap < n - sz) {
                    // 能否多交换一次
                    if (inTree.kth(needSwap + 1) < outTree.kth(n - sz - needSwap)) {
                        inc = true;
                        needSwap++;
                    }
                }

                if (!inc && needSwap > 0) {
                    // 是否要减少交换次数
                    if (inTree.kth(needSwap) >= outTree.kth(n - sz - needSwap + 1)) {
                        needSwap--;
                    }
                }

                // 计算通过交换导致的元素和的增量
                long delta = 0;
                if (needSwap > 0) {
                    long inSum = inTree.preSum(needSwap);
                    long outSum = total - subSum - outTree.preSum(n - sz - needSwap);
                    delta = outSum - inSum;
                }

                ans = Math.max(ans, subSum + delta);
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class FenwickTree {
    const vector<int>& sorted;
    const int high_bit;
    vector<int> cnt;
    vector<long long> sum;

public:
    FenwickTree(const vector<int>& sorted) :
        sorted(sorted),
        high_bit(1 << (bit_width(sorted.size()) - 1)),
        cnt(sorted.size() + 1),
        sum(sorted.size() + 1) {}

    // 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    // 如果 num < 0，表示减少 -num 个 val
    // 注意 val = sorted[i - 1]，无需手动传入
    void update(int i, int num) {
        auto val = sorted[i - 1];
        for (; i < cnt.size(); i += i & -i) {
            cnt[i] += num;
            sum[i] += 1LL * num * val;
        }
    }

    // 返回第 k 小的数（k 从 1 开始）
    int kth(int k) const {
        int i = 0;
        for (int b = high_bit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.size() && cnt[nxt] < k) {
                k -= cnt[nxt];
                i = nxt;
            }
        }
        return sorted[i];
    }

    // 返回前 k 小的数之和（k 从 1 开始）
    long long pre_sum(int k) const {
        long long s = 0;
        int i = 0;
        for (int b = high_bit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.size() && cnt[nxt] < k) {
                k -= cnt[nxt];
                s += sum[nxt];
                i = nxt;
            }
        }
        // 加上等于第 k 小的数
        return s + 1LL * sorted[i] * k;
    }
};

class Solution {
public:
    long long maxSum(vector<int>& nums, int k) {
        // 离散化
        int n = nums.size();
        vector<int> sorted = nums;
        ranges::sort(sorted);
        sorted.erase(ranges::unique(sorted).begin(), sorted.end());
        vector<int> rank(n); // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        FenwickTree all_tree(sorted); // 包含所有元素的树状数组
        long long total = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            rank[i] = ranges::lower_bound(sorted, x) - sorted.begin() + 1;
            all_tree.update(rank[i], 1);
            total += x;
        }

        long long ans = LLONG_MIN;

        // 枚举子数组左端点
        for (int left = 0; left < n; left++) {
            FenwickTree in_tree(sorted);
            FenwickTree out_tree = all_tree;
            int need_swap = 0;
            long long sub_sum = 0;

            // 枚举子数组右端点
            for (int right = left; right < n; right++) {
                // x 从子数组外移到子数组内
                int x = nums[right];
                int rk = rank[right];
                sub_sum += x;
                in_tree.update(rk, 1);
                out_tree.update(rk, -1);

                bool inc = false;
                int sz = right - left + 1;
                if (need_swap < k && need_swap < sz && need_swap < n - sz) {
                    // 能否多交换一次
                    if (in_tree.kth(need_swap + 1) < out_tree.kth(n - sz - need_swap)) {
                        inc = true;
                        need_swap++;
                    }
                }

                if (!inc && need_swap > 0) {
                    // 是否要减少交换次数
                    if (in_tree.kth(need_swap) >= out_tree.kth(n - sz - need_swap + 1)) {
                        need_swap--;
                    }
                }

                // 计算通过交换导致的元素和的增量
                long long delta = 0;
                if (need_swap > 0) {
                    long long in_sum = in_tree.pre_sum(need_swap);
                    long long out_sum = total - sub_sum - out_tree.pre_sum(n - sz - need_swap);
                    delta = out_sum - in_sum;
                }

                ans = max(ans, sub_sum + delta);
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
type pair struct{ cnt, sum int }
type fenwick struct {
	t       []pair
	sorted  []int
	highBit int
}

func newFenwickTree(sorted []int) fenwick {
	n := len(sorted)
	return fenwick{
		t:       make([]pair, n+1),
		sorted:  sorted,
		highBit: 1 << (bits.Len(uint(n)) - 1),
	}
}

// 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
// 如果 num < 0，表示减少 -num 个 val
// 注意 val = sorted[i-1]，无需手动传入
func (f *fenwick) update(i, num int) {
	val := f.sorted[i-1]
	for ; i < len(f.t); i += i & -i {
		f.t[i].cnt += num
		f.t[i].sum += num * val
	}
}

// 返回第 k 小的数（k 从 1 开始）
func (f *fenwick) kth(k int) int {
	i := 0
	for b := f.highBit; b > 0; b >>= 1 {
		if nxt := i | b; nxt < len(f.t) && f.t[nxt].cnt < k {
			k -= f.t[nxt].cnt
			i = nxt
		}
	}
	return f.sorted[i]
}

// 返回前 k 小的数之和（k 从 1 开始）
func (f *fenwick) preSum(k int) (s int) {
	i := 0
	for b := f.highBit; b > 0; b >>= 1 {
		if nxt := i | b; nxt < len(f.t) && f.t[nxt].cnt < k {
			k -= f.t[nxt].cnt
			s += f.t[nxt].sum
			i = nxt
		}
	}
	// 加上等于第 k 小的数
	s += f.sorted[i] * k
	return
}

func maxSum(nums []int, k int) int64 {
	// 离散化
	n := len(nums)
	sorted := slices.Clone(nums)
	slices.Sort(sorted)
	sorted = slices.Compact(sorted)
	rank := make([]int, n) // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
	allTree := newFenwickTree(sorted) // 包含所有元素的树状数组
	total := 0
	for i, x := range nums {
		rank[i] = sort.SearchInts(sorted, x) + 1
		allTree.update(rank[i], 1)
		total += x
	}

	inTree := newFenwickTree(sorted)
	outTree := newFenwickTree(sorted)
	ans := math.MinInt

	// 枚举子数组左端点
	for left := range nums {
		clear(inTree.t)
		copy(outTree.t, allTree.t)
		needSwap := 0
		subSum := 0

		// 枚举子数组右端点
		for right := left; right < n; right++ {
			// x 从子数组外移到子数组内
			x := nums[right]
			rk := rank[right]
			subSum += x
			inTree.update(rk, 1)
			outTree.update(rk, -1)

			inc := false
			sz := right - left + 1
			if needSwap < k && needSwap < sz && needSwap < n-sz {
				// 能否多交换一次
				if inTree.kth(needSwap+1) < outTree.kth(n-sz-needSwap) {
					inc = true
					needSwap++
				}
			}

			if !inc && needSwap > 0 {
				// 是否要减少交换次数
				if inTree.kth(needSwap) >= outTree.kth(n-sz-needSwap+1) {
					needSwap--
				}
			}

			// 计算通过交换导致的元素和的增量
			delta := 0
			if needSwap > 0 {
				inSum := inTree.preSum(needSwap)
				outSum := total - subSum - outTree.preSum(n-sz-needSwap)
				delta = outSum - inSum
			}

			ans = max(ans, subSum+delta)
		}
	}

	return int64(ans)
}
```

## 优化

### 1) 特判

设 $\textit{nums}$ 中的正数个数为 $p$。跑一个 [定长滑窗](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)，如果存在一个长为 $p$ 的窗口，其中 $正数个数+k\ge p$，则说明可以在 $k$ 次操作内把窗口外的正数全部换入窗口内，从而把所有正数都聚在一起，此时直接返回所有正数之和（这是答案的上界）。

特别地，如果 $p=0$，没有正数，则返回 $\max(\textit{nums})$（题目不允许子数组为空）。

在下面的讨论中，$\textit{nums}$ 包含正数。

### 2) 只需考虑正数与负数的交换

对于和最大的子数组 $[\ell,r]$，有如下性质：

1. $\textit{nums}[\ell-1]$ 和 $\textit{nums}[r+1]$（如果有）一定都 $\le 0$。**反证法**：如果其中有正数，则子数组扩大更好，矛盾。
2. $\textit{nums}[\ell]$ 和 $\textit{nums}[r]$ 一定都 $\ge 0$。**反证法**：如果其中有负数，则子数组缩小更好，矛盾。

这些性质有什么用？

1. 与其交换子数组内的正数（以及 $0$）与子数组外的正数，不如把子数组外的正数与 $\textit{nums}[\ell-1]$ 或者 $\textit{nums}[r+1]$ 交换，然后扩大子数组，包含这个正数，可以得到更大的子数组和。
2. 与其交换子数组内的负数与子数组外的负数，不如把子数组内的负数与 $\textit{nums}[\ell]$ 或者 $\textit{nums}[r]$ 交换，然后缩小子数组，移除这个负数，可以得到更大的子数组和。

所以我们只需考虑子数组内的负数与子数组外的正数的交换。

这样计算交换次数 $\textit{needSwap}$ 就很简单了。设子数组内的负数个数为 $\textit{negCnt}$，子数组外的正数个数为 $\textit{posCnt}$，那么

$$
\textit{needSwap} = \min(\textit{negCnt}, \textit{posCnt}, k)
$$

```py [sol-Python3]
class FenwickTree:
    def __init__(self, sorted_nums: List[int]):
        self.n = n = len(sorted_nums)
        self.high_bit = 1 << (n.bit_length() - 1)
        self.sorted_nums = sorted_nums
        self.cnt = [0] * (n + 1)
        self.sum = [0] * (n + 1)

    # 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    # 如果 num < 0，表示减少 -num 个 val
    # 注意 val = sorted_nums[i - 1]，无需手动传入
    def update(self, i: int, num: int) -> None:
        val = self.sorted_nums[i - 1]
        while i <= self.n:
            self.cnt[i] += num
            self.sum[i] += num * val
            i += i & -i

    # 返回前 k 小的数之和（k 从 1 开始）
    def pre_sum(self, k: int) -> int:
        s = i = 0
        b = self.high_bit
        while b > 0:
            nxt = i | b
            if nxt <= self.n and self.cnt[nxt] < k:
                k -= self.cnt[nxt]
                s += self.sum[nxt]
                i = nxt
            b >>= 1
        # 加上等于第 k 小的数
        return s + self.sorted_nums[i] * k

    def copy(self) -> FenwickTree:
        t = FenwickTree(self.sorted_nums)
        t.cnt[:] = self.cnt
        t.sum[:] = self.sum
        return t


class Solution:
    def maxSum(self, nums: list[int], k: int) -> int:
        # O(n) 特判：能否把正数都聚在一起
        all_pos_sum = all_pos_cnt = 0
        for x in nums:
            if x > 0:
                all_pos_sum += x
                all_pos_cnt += 1
        if all_pos_cnt == 0:  # 没有正数
            return max(nums)
        # 定长滑动窗口模板，窗口长度为 all_pos_cnt
        cnt = 0
        for i, x in enumerate(nums):
            if x > 0:
                cnt += 1
            left = i - all_pos_cnt + 1
            if left < 0:
                continue
            if cnt + k >= all_pos_cnt:  # 可以把正数都聚在一起
                return all_pos_sum
            if nums[left] > 0:
                cnt -= 1

        # 离散化
        n = len(nums)
        sorted_nums = sorted(set(nums))
        rank = [0] * n  # rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        all_pos_tree = FenwickTree(sorted_nums)  # 包含所有正数的树状数组
        for i, x in enumerate(nums):
            rank[i] = bisect_left(sorted_nums, x) + 1
            if x > 0:
                all_pos_tree.update(rank[i], 1)

        ans = -inf

        # 枚举子数组左端点
        for left in range(n):
            neg_tree = FenwickTree(sorted_nums)
            pos_tree = all_pos_tree.copy()
            pos_sum = all_pos_sum
            pos_cnt = all_pos_cnt
            neg_cnt = 0
            sub_sum = 0

            # 枚举子数组右端点
            for right in range(left, n):
                # x 从子数组外移到子数组内
                x = nums[right]
                rk = rank[right]
                sub_sum += x
                if x > 0:
                    pos_tree.update(rk, -1)
                    pos_sum -= x
                    pos_cnt -= 1
                elif x < 0:
                    neg_tree.update(rk, 1)
                    neg_cnt += 1

                # 计算通过交换导致的元素和的增量
                delta = 0
                need_swap = min(neg_cnt, pos_cnt, k)
                if need_swap > 0:
                    in_sum = neg_tree.pre_sum(need_swap)
                    out_sum = pos_sum - pos_tree.pre_sum(pos_cnt - need_swap)
                    delta = out_sum - in_sum

                ans = max(ans, sub_sum + delta)

        return ans
```

```java [sol-Java]
class FenwickTree {
    private final int highBit;
    private final int[] sorted;
    private final int[] cnt;
    private final long[] sum;

    public FenwickTree(int[] sorted) {
        int n = sorted.length;
        highBit = Integer.highestOneBit(n);
        this.sorted = sorted;
        cnt = new int[n + 1];
        sum = new long[n + 1];
    }

    // 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    // 如果 num < 0，表示减少 -num 个 val
    // 注意 val = sorted[i - 1]，无需手动传入
    public void update(int i, int num) {
        int val = sorted[i - 1];
        for (; i < cnt.length; i += i & -i) {
            cnt[i] += num;
            sum[i] += (long) num * val;
        }
    }

    // 返回前 k 小的数之和（k 从 1 开始）
    public long preSum(int k) {
        long s = 0;
        int i = 0;
        for (int b = highBit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.length && cnt[nxt] < k) {
                k -= cnt[nxt];
                s += sum[nxt];
                i = nxt;
            }
        }
        // 加上等于第 k 小的数
        return s + (long) sorted[i] * k;
    }

    public FenwickTree copy() {
        FenwickTree f = new FenwickTree(sorted);
        System.arraycopy(cnt, 0, f.cnt, 0, cnt.length);
        System.arraycopy(sum, 0, f.sum, 0, sum.length);
        return f;
    }
}

class Solution {
    public long maxSum(int[] nums, int k) {
        int n = nums.length;
        // O(n) 特判：能否把正数都聚在一起
        long allPosSum = 0;
        int allPosCnt = 0;
        int mx = Integer.MIN_VALUE;
        for (int x : nums) {
            if (x > 0) {
                allPosSum += x;
                allPosCnt++;
            } else {
                mx = Math.max(mx, x);
            }
        }
        if (allPosCnt == 0) { // 没有正数
            return mx;
        }
        // 定长滑动窗口模板，窗口长度为 allPosCnt
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            if (x > 0) {
                cnt++;
            }
            int left = i - allPosCnt + 1;
            if (left < 0) {
                continue;
            }
            if (cnt + k >= allPosCnt) { // 可以把正数都聚在一起
                return allPosSum;
            }
            if (nums[left] > 0) {
                cnt--;
            }
        }

        // 离散化
        int[] sorted = nums.clone();
        Arrays.sort(sorted);
        int[] rank = new int[n]; // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        FenwickTree allPosTree = new FenwickTree(sorted); // 包含所有正数的树状数组
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            rank[i] = Arrays.binarySearch(sorted, x) + 1;
            if (x > 0) {
                allPosTree.update(rank[i], 1);
            }
        }

        long ans = Long.MIN_VALUE;

        // 枚举子数组左端点
        for (int left = 0; left < n; left++) {
            FenwickTree negTree = new FenwickTree(sorted);
            FenwickTree posTree = allPosTree.copy();
            long posSum = allPosSum;
            int posCnt = allPosCnt;
            int negCnt = 0;
            long subSum = 0;

            // 枚举子数组右端点
            for (int right = left; right < n; right++) {
                // x 从子数组外移到子数组内
                int x = nums[right];
                int rk = rank[right];
                subSum += x;
                if (x > 0) {
                    posTree.update(rk, -1);
                    posSum -= x;
                    posCnt--;
                } else if (x < 0) {
                    negTree.update(rk, 1);
                    negCnt++;
                }

                // 计算通过交换导致的元素和的增量
                long delta = 0;
                int needSwap = Math.min(Math.min(negCnt, posCnt), k);
                if (needSwap > 0) {
                    long inSum = negTree.preSum(needSwap);
                    long outSum = posSum - posTree.preSum(posCnt - needSwap);
                    delta = outSum - inSum;
                }

                ans = Math.max(ans, subSum + delta);
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class FenwickTree {
    const vector<int>& sorted;
    const int high_bit;
    vector<int> cnt;
    vector<long long> sum;

public:
    FenwickTree(const vector<int>& sorted) :
        sorted(sorted),
        high_bit(1 << (bit_width(sorted.size()) - 1)),
        cnt(sorted.size() + 1),
        sum(sorted.size() + 1) {}

    // 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    // 如果 num < 0，表示减少 -num 个 val
    // 注意 val = sorted[i - 1]，无需手动传入
    void update(int i, int num) {
        auto val = sorted[i - 1];
        for (; i < cnt.size(); i += i & -i) {
            cnt[i] += num;
            sum[i] += 1LL * num * val;
        }
    }

    // 返回前 k 小的数之和（k 从 1 开始）
    long long pre_sum(int k) const {
        long long s = 0;
        int i = 0;
        for (int b = high_bit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.size() && cnt[nxt] < k) {
                k -= cnt[nxt];
                s += sum[nxt];
                i = nxt;
            }
        }
        // 加上等于第 k 小的数
        return s + 1LL * sorted[i] * k;
    }
};

class Solution {
public:
    long long maxSum(vector<int>& nums, int k) {
        int n = nums.size();
        // O(n) 特判：能否把正数都聚在一起
        int all_pos_sum = 0;
        int all_pos_cnt = 0;
        for (int x : nums) {
            if (x > 0) {
                all_pos_sum += x;
                all_pos_cnt++;
            }
        }
        if (all_pos_cnt == 0) { // 没有正数
            return ranges::max(nums);
        }
        // 定长滑动窗口模板，窗口长度为 all_pos_cnt
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            cnt += x > 0;
            int left = i - all_pos_cnt + 1;
            if (left < 0) {
                continue;
            }
            if (cnt + k >= all_pos_cnt) { // 可以把正数都聚在一起
                return all_pos_sum;
            }
            cnt -= nums[left] > 0;
        }

        // 离散化
        auto sorted = nums;
        ranges::sort(sorted);
        sorted.erase(ranges::unique(sorted).begin(), sorted.end());
        vector<int> rank(n); // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        FenwickTree all_pos_tree(sorted); // 包含所有正数的树状数组
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            rank[i] = ranges::lower_bound(sorted, x) - sorted.begin() + 1;
            if (x > 0) {
                all_pos_tree.update(rank[i], 1);
            }
        }

        long long ans = LLONG_MIN;

        // 枚举子数组左端点
        for (int left = 0; left < n; left++) {
            FenwickTree neg_tree(sorted);
            FenwickTree pos_tree = all_pos_tree;
            long long pos_sum = all_pos_sum;
            int pos_cnt = all_pos_cnt;
            int neg_cnt = 0;
            long long sub_sum = 0;

            // 枚举子数组右端点
            for (int right = left; right < n; right++) {
                // x 从子数组外移到子数组内
                int x = nums[right];
                int rk = rank[right];
                sub_sum += x;
                if (x > 0) {
                    pos_tree.update(rk, -1);
                    pos_sum -= x;
                    pos_cnt--;
                } else if (x < 0) {
                    neg_tree.update(rk, 1);
                    neg_cnt++;
                }

                long long delta = 0;
                int need_swap = min({neg_cnt, pos_cnt, k});
                if (need_swap > 0) {
                    long long in_sum = neg_tree.pre_sum(need_swap);
                    long long out_sum = pos_sum - pos_tree.pre_sum(pos_cnt - need_swap);
                    delta = out_sum - in_sum;
                }

                ans = max(ans, sub_sum + delta);
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
type pair struct{ cnt, sum int }
type fenwick struct {
	t       []pair
	sorted  []int
	highBit int
}

func newFenwickTree(sorted []int) fenwick {
	n := len(sorted)
	return fenwick{
		t:       make([]pair, n+1),
		sorted:  sorted,
		highBit: 1 << (bits.Len(uint(n)) - 1),
	}
}

// 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
// 如果 num < 0，表示减少 -num 个 val
// 注意 val = sorted[i-1]，无需手动传入
func (f *fenwick) update(i, num int) {
	val := f.sorted[i-1]
	for ; i < len(f.t); i += i & -i {
		f.t[i].cnt += num
		f.t[i].sum += num * val
	}
}

// 返回前 k 小的数之和（k 从 1 开始）
func (f *fenwick) preSum(k int) (s int) {
	i := 0
	for b := f.highBit; b > 0; b >>= 1 {
		if nxt := i | b; nxt < len(f.t) && f.t[nxt].cnt < k {
			k -= f.t[nxt].cnt
			s += f.t[nxt].sum
			i = nxt
		}
	}
	// 加上等于第 k 小的数
	s += f.sorted[i] * k
	return
}

func maxSum(nums []int, k int) int64 {
	// O(n) 特判：能否把正数都聚在一起
	allPosSum := 0
	allPosCnt := 0
	for _, x := range nums {
		if x > 0 {
			allPosSum += x
			allPosCnt++
		}
	}
	if allPosCnt == 0 { // 没有正数
		return int64(slices.Max(nums))
	}
	// 定长滑动窗口模板，窗口长度为 allPosCnt
	cnt := 0
	for i, x := range nums {
		if x > 0 {
			cnt++
		}
		left := i - allPosCnt + 1
		if left < 0 {
			continue
		}
		if cnt+k >= allPosCnt { // 可以把正数都聚在一起
			return int64(allPosSum)
		}
		if nums[left] > 0 {
			cnt--
		}
	}

	// 离散化
	n := len(nums)
	sorted := slices.Clone(nums)
	slices.Sort(sorted)
	sorted = slices.Compact(sorted)
	rank := make([]int, n) // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
	allPosTree := newFenwickTree(sorted) // 包含所有正数的树状数组
	for i, x := range nums {
		rank[i] = sort.SearchInts(sorted, x) + 1
		if x > 0 {
			allPosTree.update(rank[i], 1)
		}
	}

	negTree := newFenwickTree(sorted)
	posTree := newFenwickTree(sorted)
	ans := math.MinInt

	// 枚举子数组左端点
	for left := range nums {
		clear(negTree.t)
		copy(posTree.t, allPosTree.t)
		posSum := allPosSum
		posCnt := allPosCnt
		negCnt := 0
		subSum := 0

		// 枚举子数组右端点
		for right := left; right < n; right++ {
			// x 从子数组外移到子数组内
			x := nums[right]
			rk := rank[right]
			subSum += x
			if x > 0 {
				posTree.update(rk, -1)
				posSum -= x
				posCnt--
			} else if x < 0 {
				negTree.update(rk, 1)
				negCnt++
			}

			// 计算通过交换导致的元素和的增量
			delta := 0
			needSwap := min(negCnt, posCnt, k)
			if needSwap > 0 {
				inSum := negTree.preSum(needSwap)
				outSum := posSum - posTree.preSum(posCnt-needSwap)
				delta = outSum - inSum
			}

			ans = max(ans, subSum+delta)
		}
	}

	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数据结构题单的「**§5.7 对顶堆**」和「**§8.1 树状数组**」。

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

## 优化前

```py [sol-Python3]
# ⚠ 超时了！请看优化后的代码
class FenwickTree:
    def __init__(self, sorted_nums: List[int]):
        self.n = n = len(sorted_nums)
        self.high_bit = 1 << (n.bit_length() - 1)
        self.sorted_nums = sorted_nums
        self.cnt = [0] * (n + 1)
        self.sum = [0] * (n + 1)

    # 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    # 如果 num < 0，表示减少 -num 个 val
    # 注意 val = sorted_nums[i - 1]，无需手动传入
    def update(self, i: int, num: int) -> None:
        val = self.sorted_nums[i - 1]
        while i <= self.n:
            self.cnt[i] += num
            self.sum[i] += num * val
            i += i & -i

    # 返回第 k 小的数（k 从 1 开始）
    def kth(self, k: int) -> int:
        i = 0
        b = self.high_bit
        while b > 0:
            nxt = i | b
            if nxt <= self.n and self.cnt[nxt] < k:
                k -= self.cnt[nxt]
                i = nxt
            b >>= 1
        return self.sorted_nums[i]

    # 返回前 k 小的数之和（k 从 1 开始）
    def pre_sum(self, k: int) -> int:
        s = i = 0
        b = self.high_bit
        while b > 0:
            nxt = i | b
            if nxt <= self.n and self.cnt[nxt] < k:
                k -= self.cnt[nxt]
                s += self.sum[nxt]
                i = nxt
            b >>= 1
        # 加上等于第 k 小的数
        return s + self.sorted_nums[i] * k

    def copy(self) -> FenwickTree:
        t = FenwickTree(self.sorted_nums)
        t.cnt[:] = self.cnt
        t.sum[:] = self.sum
        return t


class Solution:
    def maxSum(self, nums: list[int], k: int) -> int:
        # 离散化
        n = len(nums)
        sorted_nums = sorted(set(nums))
        rank = [0] * n  # rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        all_tree = FenwickTree(sorted_nums)  # 包含所有元素的树状数组
        total = 0
        for i, x in enumerate(nums):
            rank[i] = bisect_left(sorted_nums, x) + 1
            all_tree.update(rank[i], 1)
            total += x

        ans = -inf

        # 枚举子数组左端点
        for left in range(n):
            in_tree = FenwickTree(sorted_nums)
            out_tree = all_tree.copy()
            need_swap = sub_sum = 0

            # 枚举子数组右端点
            for right in range(left, n):
                # x 从子数组外移到子数组内
                x = nums[right]
                rk = rank[right]
                sub_sum += x
                in_tree.update(rk, 1)
                out_tree.update(rk, -1)

                inc = False
                sz = right - left + 1
                if need_swap < k and need_swap < sz and need_swap < n - sz:
                    # 能否多交换一次
                    if in_tree.kth(need_swap + 1) < out_tree.kth(n - sz - need_swap):
                        inc = True
                        need_swap += 1

                if not inc and need_swap > 0:
                    # 是否要减少交换次数
                    if in_tree.kth(need_swap) >= out_tree.kth(n - sz - need_swap + 1):
                        need_swap -= 1

                # 计算通过交换导致的元素和的增量
                delta = 0
                if need_swap > 0:
                    in_sum = in_tree.pre_sum(need_swap)
                    out_sum = total - sub_sum - out_tree.pre_sum(n - sz - need_swap)
                    delta = out_sum - in_sum

                ans = max(ans, sub_sum + delta)

        return ans

# ⚠ 超时了！请看优化后的代码
```

```java [sol-Java]
class FenwickTree {
    private final int highBit;
    private final int[] sorted;
    private final int[] cnt;
    private final long[] sum;

    public FenwickTree(int[] sorted) {
        int n = sorted.length;
        highBit = Integer.highestOneBit(n);
        this.sorted = sorted;
        cnt = new int[n + 1];
        sum = new long[n + 1];
    }

    // 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    // 如果 num < 0，表示减少 -num 个 val
    // 注意 val = sorted[i - 1]，无需手动传入
    public void update(int i, int num) {
        int val = sorted[i - 1];
        for (; i < cnt.length; i += i & -i) {
            cnt[i] += num;
            sum[i] += (long) num * val;
        }
    }

    // 返回第 k 小的数（k 从 1 开始）
    public int kth(int k) {
        int i = 0;
        for (int b = highBit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.length && cnt[nxt] < k) {
                k -= cnt[nxt];
                i = nxt;
            }
        }
        return sorted[i];
    }

    // 返回前 k 小的数之和（k 从 1 开始）
    public long preSum(int k) {
        long s = 0;
        int i = 0;
        for (int b = highBit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.length && cnt[nxt] < k) {
                k -= cnt[nxt];
                s += sum[nxt];
                i = nxt;
            }
        }
        // 加上等于第 k 小的数
        return s + (long) sorted[i] * k;
    }

    public FenwickTree copy() {
        FenwickTree f = new FenwickTree(sorted);
        System.arraycopy(cnt, 0, f.cnt, 0, cnt.length);
        System.arraycopy(sum, 0, f.sum, 0, sum.length);
        return f;
    }
}

class Solution {
    public long maxSum(int[] nums, int k) {
        // 离散化
        int n = nums.length;
        int[] sorted = nums.clone();
        Arrays.sort(sorted);
        int[] rank = new int[n]; // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        FenwickTree allTree = new FenwickTree(sorted); // 包含所有元素的树状数组
        long total = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            rank[i] = Arrays.binarySearch(sorted, x) + 1;
            allTree.update(rank[i], 1);
            total += x;
        }

        long ans = Long.MIN_VALUE;

        // 枚举子数组左端点
        for (int left = 0; left < n; left++) {
            FenwickTree inTree = new FenwickTree(sorted);
            FenwickTree outTree = allTree.copy();
            int needSwap = 0;
            long subSum = 0;

            // 枚举子数组右端点
            for (int right = left; right < n; right++) {
                // x 从子数组外移到子数组内
                int x = nums[right];
                int rk = rank[right];
                subSum += x;
                inTree.update(rk, 1);
                outTree.update(rk, -1);

                boolean inc = false;
                int sz = right - left + 1;
                if (needSwap < k && needSwap < sz && needSwap < n - sz) {
                    // 能否多交换一次
                    if (inTree.kth(needSwap + 1) < outTree.kth(n - sz - needSwap)) {
                        inc = true;
                        needSwap++;
                    }
                }

                if (!inc && needSwap > 0) {
                    // 是否要减少交换次数
                    if (inTree.kth(needSwap) >= outTree.kth(n - sz - needSwap + 1)) {
                        needSwap--;
                    }
                }

                // 计算通过交换导致的元素和的增量
                long delta = 0;
                if (needSwap > 0) {
                    long inSum = inTree.preSum(needSwap);
                    long outSum = total - subSum - outTree.preSum(n - sz - needSwap);
                    delta = outSum - inSum;
                }

                ans = Math.max(ans, subSum + delta);
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class FenwickTree {
    const vector<int>& sorted;
    const int high_bit;
    vector<int> cnt;
    vector<long long> sum;

public:
    FenwickTree(const vector<int>& sorted) :
        sorted(sorted),
        high_bit(1 << (bit_width(sorted.size()) - 1)),
        cnt(sorted.size() + 1),
        sum(sorted.size() + 1) {}

    // 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    // 如果 num < 0，表示减少 -num 个 val
    // 注意 val = sorted[i - 1]，无需手动传入
    void update(int i, int num) {
        auto val = sorted[i - 1];
        for (; i < cnt.size(); i += i & -i) {
            cnt[i] += num;
            sum[i] += 1LL * num * val;
        }
    }

    // 返回第 k 小的数（k 从 1 开始）
    int kth(int k) const {
        int i = 0;
        for (int b = high_bit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.size() && cnt[nxt] < k) {
                k -= cnt[nxt];
                i = nxt;
            }
        }
        return sorted[i];
    }

    // 返回前 k 小的数之和（k 从 1 开始）
    long long pre_sum(int k) const {
        long long s = 0;
        int i = 0;
        for (int b = high_bit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.size() && cnt[nxt] < k) {
                k -= cnt[nxt];
                s += sum[nxt];
                i = nxt;
            }
        }
        // 加上等于第 k 小的数
        return s + 1LL * sorted[i] * k;
    }
};

class Solution {
public:
    long long maxSum(vector<int>& nums, int k) {
        // 离散化
        int n = nums.size();
        vector<int> sorted = nums;
        ranges::sort(sorted);
        sorted.erase(ranges::unique(sorted).begin(), sorted.end());
        vector<int> rank(n); // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        FenwickTree all_tree(sorted); // 包含所有元素的树状数组
        long long total = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            rank[i] = ranges::lower_bound(sorted, x) - sorted.begin() + 1;
            all_tree.update(rank[i], 1);
            total += x;
        }

        long long ans = LLONG_MIN;

        // 枚举子数组左端点
        for (int left = 0; left < n; left++) {
            FenwickTree in_tree(sorted);
            FenwickTree out_tree = all_tree;
            int need_swap = 0;
            long long sub_sum = 0;

            // 枚举子数组右端点
            for (int right = left; right < n; right++) {
                // x 从子数组外移到子数组内
                int x = nums[right];
                int rk = rank[right];
                sub_sum += x;
                in_tree.update(rk, 1);
                out_tree.update(rk, -1);

                bool inc = false;
                int sz = right - left + 1;
                if (need_swap < k && need_swap < sz && need_swap < n - sz) {
                    // 能否多交换一次
                    if (in_tree.kth(need_swap + 1) < out_tree.kth(n - sz - need_swap)) {
                        inc = true;
                        need_swap++;
                    }
                }

                if (!inc && need_swap > 0) {
                    // 是否要减少交换次数
                    if (in_tree.kth(need_swap) >= out_tree.kth(n - sz - need_swap + 1)) {
                        need_swap--;
                    }
                }

                // 计算通过交换导致的元素和的增量
                long long delta = 0;
                if (need_swap > 0) {
                    long long in_sum = in_tree.pre_sum(need_swap);
                    long long out_sum = total - sub_sum - out_tree.pre_sum(n - sz - need_swap);
                    delta = out_sum - in_sum;
                }

                ans = max(ans, sub_sum + delta);
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
type pair struct{ cnt, sum int }
type fenwick struct {
	t       []pair
	sorted  []int
	highBit int
}

func newFenwickTree(sorted []int) fenwick {
	n := len(sorted)
	return fenwick{
		t:       make([]pair, n+1),
		sorted:  sorted,
		highBit: 1 << (bits.Len(uint(n)) - 1),
	}
}

// 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
// 如果 num < 0，表示减少 -num 个 val
// 注意 val = sorted[i-1]，无需手动传入
func (f *fenwick) update(i, num int) {
	val := f.sorted[i-1]
	for ; i < len(f.t); i += i & -i {
		f.t[i].cnt += num
		f.t[i].sum += num * val
	}
}

// 返回第 k 小的数（k 从 1 开始）
func (f *fenwick) kth(k int) int {
	i := 0
	for b := f.highBit; b > 0; b >>= 1 {
		if nxt := i | b; nxt < len(f.t) && f.t[nxt].cnt < k {
			k -= f.t[nxt].cnt
			i = nxt
		}
	}
	return f.sorted[i]
}

// 返回前 k 小的数之和（k 从 1 开始）
func (f *fenwick) preSum(k int) (s int) {
	i := 0
	for b := f.highBit; b > 0; b >>= 1 {
		if nxt := i | b; nxt < len(f.t) && f.t[nxt].cnt < k {
			k -= f.t[nxt].cnt
			s += f.t[nxt].sum
			i = nxt
		}
	}
	// 加上等于第 k 小的数
	s += f.sorted[i] * k
	return
}

func maxSum(nums []int, k int) int64 {
	// 离散化
	n := len(nums)
	sorted := slices.Clone(nums)
	slices.Sort(sorted)
	sorted = slices.Compact(sorted)
	rank := make([]int, n) // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
	allTree := newFenwickTree(sorted) // 包含所有元素的树状数组
	total := 0
	for i, x := range nums {
		rank[i] = sort.SearchInts(sorted, x) + 1
		allTree.update(rank[i], 1)
		total += x
	}

	inTree := newFenwickTree(sorted)
	outTree := newFenwickTree(sorted)
	ans := math.MinInt

	// 枚举子数组左端点
	for left := range nums {
		clear(inTree.t)
		copy(outTree.t, allTree.t)
		needSwap := 0
		subSum := 0

		// 枚举子数组右端点
		for right := left; right < n; right++ {
			// x 从子数组外移到子数组内
			x := nums[right]
			rk := rank[right]
			subSum += x
			inTree.update(rk, 1)
			outTree.update(rk, -1)

			inc := false
			sz := right - left + 1
			if needSwap < k && needSwap < sz && needSwap < n-sz {
				// 能否多交换一次
				if inTree.kth(needSwap+1) < outTree.kth(n-sz-needSwap) {
					inc = true
					needSwap++
				}
			}

			if !inc && needSwap > 0 {
				// 是否要减少交换次数
				if inTree.kth(needSwap) >= outTree.kth(n-sz-needSwap+1) {
					needSwap--
				}
			}

			// 计算通过交换导致的元素和的增量
			delta := 0
			if needSwap > 0 {
				inSum := inTree.preSum(needSwap)
				outSum := total - subSum - outTree.preSum(n-sz-needSwap)
				delta = outSum - inSum
			}

			ans = max(ans, subSum+delta)
		}
	}

	return int64(ans)
}
```

## 优化

### 1) 特判

设 $\textit{nums}$ 中的正数个数为 $p$。跑一个 [定长滑窗](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)，如果存在一个长为 $p$ 的窗口，其中 $正数个数+k\ge p$，则说明可以在 $k$ 次操作内把窗口外的正数全部换入窗口内，从而把所有正数都聚在一起，此时直接返回所有正数之和（这是答案的上界）。

特别地，如果 $p=0$，没有正数，则返回 $\max(\textit{nums})$（题目不允许子数组为空）。

在下面的讨论中，$\textit{nums}$ 包含正数。

### 2) 只需考虑正数与负数的交换

对于和最大的子数组 $[\ell,r]$，有如下性质：

1. $\textit{nums}[\ell-1]$ 和 $\textit{nums}[r+1]$（如果有）一定都 $\le 0$。**反证法**：如果其中有正数，则子数组扩大更好，矛盾。
2. $\textit{nums}[\ell]$ 和 $\textit{nums}[r]$ 一定都 $\ge 0$。**反证法**：如果其中有负数，则子数组缩小更好，矛盾。

这些性质有什么用？

1. 与其交换子数组内的正数（以及 $0$）与子数组外的正数，不如把子数组外的正数与 $\textit{nums}[\ell-1]$ 或者 $\textit{nums}[r+1]$ 交换，然后扩大子数组，包含这个正数，可以得到更大的子数组和。
2. 与其交换子数组内的负数与子数组外的负数，不如把子数组内的负数与 $\textit{nums}[\ell]$ 或者 $\textit{nums}[r]$ 交换，然后缩小子数组，移除这个负数，可以得到更大的子数组和。

所以我们只需考虑子数组内的负数与子数组外的正数的交换。

这样计算交换次数 $\textit{needSwap}$ 就很简单了。设子数组内的负数个数为 $\textit{negCnt}$，子数组外的正数个数为 $\textit{posCnt}$，那么

$$
\textit{needSwap} = \min(\textit{negCnt}, \textit{posCnt}, k)
$$

```py [sol-Python3]
class FenwickTree:
    def __init__(self, sorted_nums: List[int]):
        self.n = n = len(sorted_nums)
        self.high_bit = 1 << (n.bit_length() - 1)
        self.sorted_nums = sorted_nums
        self.cnt = [0] * (n + 1)
        self.sum = [0] * (n + 1)

    # 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    # 如果 num < 0，表示减少 -num 个 val
    # 注意 val = sorted_nums[i - 1]，无需手动传入
    def update(self, i: int, num: int) -> None:
        val = self.sorted_nums[i - 1]
        while i <= self.n:
            self.cnt[i] += num
            self.sum[i] += num * val
            i += i & -i

    # 返回前 k 小的数之和（k 从 1 开始）
    def pre_sum(self, k: int) -> int:
        s = i = 0
        b = self.high_bit
        while b > 0:
            nxt = i | b
            if nxt <= self.n and self.cnt[nxt] < k:
                k -= self.cnt[nxt]
                s += self.sum[nxt]
                i = nxt
            b >>= 1
        # 加上等于第 k 小的数
        return s + self.sorted_nums[i] * k

    def copy(self) -> FenwickTree:
        t = FenwickTree(self.sorted_nums)
        t.cnt[:] = self.cnt
        t.sum[:] = self.sum
        return t


class Solution:
    def maxSum(self, nums: list[int], k: int) -> int:
        # O(n) 特判：能否把正数都聚在一起
        all_pos_sum = all_pos_cnt = 0
        for x in nums:
            if x > 0:
                all_pos_sum += x
                all_pos_cnt += 1
        if all_pos_cnt == 0:  # 没有正数
            return max(nums)
        # 定长滑动窗口模板，窗口长度为 all_pos_cnt
        cnt = 0
        for i, x in enumerate(nums):
            if x > 0:
                cnt += 1
            left = i - all_pos_cnt + 1
            if left < 0:
                continue
            if cnt + k >= all_pos_cnt:  # 可以把正数都聚在一起
                return all_pos_sum
            if nums[left] > 0:
                cnt -= 1

        # 离散化
        n = len(nums)
        sorted_nums = sorted(set(nums))
        rank = [0] * n  # rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        all_pos_tree = FenwickTree(sorted_nums)  # 包含所有正数的树状数组
        for i, x in enumerate(nums):
            rank[i] = bisect_left(sorted_nums, x) + 1
            if x > 0:
                all_pos_tree.update(rank[i], 1)

        ans = -inf

        # 枚举子数组左端点
        for left in range(n):
            neg_tree = FenwickTree(sorted_nums)
            pos_tree = all_pos_tree.copy()
            pos_sum = all_pos_sum
            pos_cnt = all_pos_cnt
            neg_cnt = 0
            sub_sum = 0

            # 枚举子数组右端点
            for right in range(left, n):
                # x 从子数组外移到子数组内
                x = nums[right]
                rk = rank[right]
                sub_sum += x
                if x > 0:
                    pos_tree.update(rk, -1)
                    pos_sum -= x
                    pos_cnt -= 1
                elif x < 0:
                    neg_tree.update(rk, 1)
                    neg_cnt += 1

                # 计算通过交换导致的元素和的增量
                delta = 0
                need_swap = min(neg_cnt, pos_cnt, k)
                if need_swap > 0:
                    in_sum = neg_tree.pre_sum(need_swap)
                    out_sum = pos_sum - pos_tree.pre_sum(pos_cnt - need_swap)
                    delta = out_sum - in_sum

                ans = max(ans, sub_sum + delta)

        return ans
```

```java [sol-Java]
class FenwickTree {
    private final int highBit;
    private final int[] sorted;
    private final int[] cnt;
    private final long[] sum;

    public FenwickTree(int[] sorted) {
        int n = sorted.length;
        highBit = Integer.highestOneBit(n);
        this.sorted = sorted;
        cnt = new int[n + 1];
        sum = new long[n + 1];
    }

    // 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    // 如果 num < 0，表示减少 -num 个 val
    // 注意 val = sorted[i - 1]，无需手动传入
    public void update(int i, int num) {
        int val = sorted[i - 1];
        for (; i < cnt.length; i += i & -i) {
            cnt[i] += num;
            sum[i] += (long) num * val;
        }
    }

    // 返回前 k 小的数之和（k 从 1 开始）
    public long preSum(int k) {
        long s = 0;
        int i = 0;
        for (int b = highBit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.length && cnt[nxt] < k) {
                k -= cnt[nxt];
                s += sum[nxt];
                i = nxt;
            }
        }
        // 加上等于第 k 小的数
        return s + (long) sorted[i] * k;
    }

    public FenwickTree copy() {
        FenwickTree f = new FenwickTree(sorted);
        System.arraycopy(cnt, 0, f.cnt, 0, cnt.length);
        System.arraycopy(sum, 0, f.sum, 0, sum.length);
        return f;
    }
}

class Solution {
    public long maxSum(int[] nums, int k) {
        int n = nums.length;
        // O(n) 特判：能否把正数都聚在一起
        long allPosSum = 0;
        int allPosCnt = 0;
        int mx = Integer.MIN_VALUE;
        for (int x : nums) {
            if (x > 0) {
                allPosSum += x;
                allPosCnt++;
            } else {
                mx = Math.max(mx, x);
            }
        }
        if (allPosCnt == 0) { // 没有正数
            return mx;
        }
        // 定长滑动窗口模板，窗口长度为 allPosCnt
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            if (x > 0) {
                cnt++;
            }
            int left = i - allPosCnt + 1;
            if (left < 0) {
                continue;
            }
            if (cnt + k >= allPosCnt) { // 可以把正数都聚在一起
                return allPosSum;
            }
            if (nums[left] > 0) {
                cnt--;
            }
        }

        // 离散化
        int[] sorted = nums.clone();
        Arrays.sort(sorted);
        int[] rank = new int[n]; // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        FenwickTree allPosTree = new FenwickTree(sorted); // 包含所有正数的树状数组
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            rank[i] = Arrays.binarySearch(sorted, x) + 1;
            if (x > 0) {
                allPosTree.update(rank[i], 1);
            }
        }

        long ans = Long.MIN_VALUE;

        // 枚举子数组左端点
        for (int left = 0; left < n; left++) {
            FenwickTree negTree = new FenwickTree(sorted);
            FenwickTree posTree = allPosTree.copy();
            long posSum = allPosSum;
            int posCnt = allPosCnt;
            int negCnt = 0;
            long subSum = 0;

            // 枚举子数组右端点
            for (int right = left; right < n; right++) {
                // x 从子数组外移到子数组内
                int x = nums[right];
                int rk = rank[right];
                subSum += x;
                if (x > 0) {
                    posTree.update(rk, -1);
                    posSum -= x;
                    posCnt--;
                } else if (x < 0) {
                    negTree.update(rk, 1);
                    negCnt++;
                }

                // 计算通过交换导致的元素和的增量
                long delta = 0;
                int needSwap = Math.min(Math.min(negCnt, posCnt), k);
                if (needSwap > 0) {
                    long inSum = negTree.preSum(needSwap);
                    long outSum = posSum - posTree.preSum(posCnt - needSwap);
                    delta = outSum - inSum;
                }

                ans = Math.max(ans, subSum + delta);
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class FenwickTree {
    const vector<int>& sorted;
    const int high_bit;
    vector<int> cnt;
    vector<long long> sum;

public:
    FenwickTree(const vector<int>& sorted) :
        sorted(sorted),
        high_bit(1 << (bit_width(sorted.size()) - 1)),
        cnt(sorted.size() + 1),
        sum(sorted.size() + 1) {}

    // 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
    // 如果 num < 0，表示减少 -num 个 val
    // 注意 val = sorted[i - 1]，无需手动传入
    void update(int i, int num) {
        auto val = sorted[i - 1];
        for (; i < cnt.size(); i += i & -i) {
            cnt[i] += num;
            sum[i] += 1LL * num * val;
        }
    }

    // 返回前 k 小的数之和（k 从 1 开始）
    long long pre_sum(int k) const {
        long long s = 0;
        int i = 0;
        for (int b = high_bit; b > 0; b >>= 1) {
            int nxt = i | b;
            if (nxt < cnt.size() && cnt[nxt] < k) {
                k -= cnt[nxt];
                s += sum[nxt];
                i = nxt;
            }
        }
        // 加上等于第 k 小的数
        return s + 1LL * sorted[i] * k;
    }
};

class Solution {
public:
    long long maxSum(vector<int>& nums, int k) {
        int n = nums.size();
        // O(n) 特判：能否把正数都聚在一起
        int all_pos_sum = 0;
        int all_pos_cnt = 0;
        for (int x : nums) {
            if (x > 0) {
                all_pos_sum += x;
                all_pos_cnt++;
            }
        }
        if (all_pos_cnt == 0) { // 没有正数
            return ranges::max(nums);
        }
        // 定长滑动窗口模板，窗口长度为 all_pos_cnt
        int cnt = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            cnt += x > 0;
            int left = i - all_pos_cnt + 1;
            if (left < 0) {
                continue;
            }
            if (cnt + k >= all_pos_cnt) { // 可以把正数都聚在一起
                return all_pos_sum;
            }
            cnt -= nums[left] > 0;
        }

        // 离散化
        auto sorted = nums;
        ranges::sort(sorted);
        sorted.erase(ranges::unique(sorted).begin(), sorted.end());
        vector<int> rank(n); // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
        FenwickTree all_pos_tree(sorted); // 包含所有正数的树状数组
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            rank[i] = ranges::lower_bound(sorted, x) - sorted.begin() + 1;
            if (x > 0) {
                all_pos_tree.update(rank[i], 1);
            }
        }

        long long ans = LLONG_MIN;

        // 枚举子数组左端点
        for (int left = 0; left < n; left++) {
            FenwickTree neg_tree(sorted);
            FenwickTree pos_tree = all_pos_tree;
            long long pos_sum = all_pos_sum;
            int pos_cnt = all_pos_cnt;
            int neg_cnt = 0;
            long long sub_sum = 0;

            // 枚举子数组右端点
            for (int right = left; right < n; right++) {
                // x 从子数组外移到子数组内
                int x = nums[right];
                int rk = rank[right];
                sub_sum += x;
                if (x > 0) {
                    pos_tree.update(rk, -1);
                    pos_sum -= x;
                    pos_cnt--;
                } else if (x < 0) {
                    neg_tree.update(rk, 1);
                    neg_cnt++;
                }

                long long delta = 0;
                int need_swap = min({neg_cnt, pos_cnt, k});
                if (need_swap > 0) {
                    long long in_sum = neg_tree.pre_sum(need_swap);
                    long long out_sum = pos_sum - pos_tree.pre_sum(pos_cnt - need_swap);
                    delta = out_sum - in_sum;
                }

                ans = max(ans, sub_sum + delta);
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
type pair struct{ cnt, sum int }
type fenwick struct {
	t       []pair
	sorted  []int
	highBit int
}

func newFenwickTree(sorted []int) fenwick {
	n := len(sorted)
	return fenwick{
		t:       make([]pair, n+1),
		sorted:  sorted,
		highBit: 1 << (bits.Len(uint(n)) - 1),
	}
}

// 添加 num 个 val，其中 val 离散化后的值为 i（i 从 1 开始）
// 如果 num < 0，表示减少 -num 个 val
// 注意 val = sorted[i-1]，无需手动传入
func (f *fenwick) update(i, num int) {
	val := f.sorted[i-1]
	for ; i < len(f.t); i += i & -i {
		f.t[i].cnt += num
		f.t[i].sum += num * val
	}
}

// 返回前 k 小的数之和（k 从 1 开始）
func (f *fenwick) preSum(k int) (s int) {
	i := 0
	for b := f.highBit; b > 0; b >>= 1 {
		if nxt := i | b; nxt < len(f.t) && f.t[nxt].cnt < k {
			k -= f.t[nxt].cnt
			s += f.t[nxt].sum
			i = nxt
		}
	}
	// 加上等于第 k 小的数
	s += f.sorted[i] * k
	return
}

func maxSum(nums []int, k int) int64 {
	// O(n) 特判：能否把正数都聚在一起
	allPosSum := 0
	allPosCnt := 0
	for _, x := range nums {
		if x > 0 {
			allPosSum += x
			allPosCnt++
		}
	}
	if allPosCnt == 0 { // 没有正数
		return int64(slices.Max(nums))
	}
	// 定长滑动窗口模板，窗口长度为 allPosCnt
	cnt := 0
	for i, x := range nums {
		if x > 0 {
			cnt++
		}
		left := i - allPosCnt + 1
		if left < 0 {
			continue
		}
		if cnt+k >= allPosCnt { // 可以把正数都聚在一起
			return int64(allPosSum)
		}
		if nums[left] > 0 {
			cnt--
		}
	}

	// 离散化
	n := len(nums)
	sorted := slices.Clone(nums)
	slices.Sort(sorted)
	sorted = slices.Compact(sorted)
	rank := make([]int, n) // rank[i] 是 nums[i] 离散化后的值（从 1 开始）
	allPosTree := newFenwickTree(sorted) // 包含所有正数的树状数组
	for i, x := range nums {
		rank[i] = sort.SearchInts(sorted, x) + 1
		if x > 0 {
			allPosTree.update(rank[i], 1)
		}
	}

	negTree := newFenwickTree(sorted)
	posTree := newFenwickTree(sorted)
	ans := math.MinInt

	// 枚举子数组左端点
	for left := range nums {
		clear(negTree.t)
		copy(posTree.t, allPosTree.t)
		posSum := allPosSum
		posCnt := allPosCnt
		negCnt := 0
		subSum := 0

		// 枚举子数组右端点
		for right := left; right < n; right++ {
			// x 从子数组外移到子数组内
			x := nums[right]
			rk := rank[right]
			subSum += x
			if x > 0 {
				posTree.update(rk, -1)
				posSum -= x
				posCnt--
			} else if x < 0 {
				negTree.update(rk, 1)
				negCnt++
			}

			// 计算通过交换导致的元素和的增量
			delta := 0
			needSwap := min(negCnt, posCnt, k)
			if needSwap > 0 {
				inSum := negTree.preSum(needSwap)
				outSum := posSum - posTree.preSum(posCnt-needSwap)
				delta = outSum - inSum
			}

			ans = max(ans, subSum+delta)
		}
	}

	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数据结构题单的「**§5.7 对顶堆**」和「**§8.1 树状数组**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
