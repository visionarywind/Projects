# 3430. 最多 K 个元素的子数组的最值之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-and-minimum-sums-of-at-most-size-k-subarrays/
- 题目 slug：`maximum-and-minimum-sums-of-at-most-size-k-subarrays`
- 来源专题：单调栈
- 来源分类路径：三、贡献法
- 难度分：2645
- 外部题解来源：https://leetcode.cn/problems/maximum-and-minimum-sums-of-at-most-size-k-subarrays/solutions/3051552/gong-xian-fa-dan-diao-zhan-pythonjavacgo-9gz3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贡献法+单调栈，两种计算子数组的方法（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-and-minimum-sums-of-at-most-size-k-subarrays/solutions/3051552/gong-xian-fa-dan-diao-zhan-pythonjavacgo-9gz3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gong-xian-fa-dan-diao-zhan-pythonjavacgo-9gz3`
- topic id：`3051552`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：请先完成本题的简单版本 [907. 子数组的最小值之和](https://leetcode.cn/problems/sum-of-subarray-minimums/)，并阅读 [我的题解](https://leetcode.cn/problems/sum-of-subarray-minimums/solutions/1930857/gong-xian-fa-dan-diao-zhan-san-chong-shi-gxa5/)。

本题有长度 $\le k$ 的限制，怎么做？

下面介绍两种计算方法：第一种直截了当，但有一些计算量；第二种简洁明了，但不容易想到。

## 第一种计算方法

设 $x=\textit{nums}[i]$ 是子数组的最小值，设 $x$ 对应的边界为开区间 $(L,R)$（见 907 题我题解中的定义）。

分类讨论：

- 如果 $R-L-1\le k$，那么没有 $k$ 的限制，做法和 907 题一样，有 $(i - L) * (R - i)$ 个最小值是 $x$ 的子数组。
- 如果 $R-L-1 > k$：
  - 首先，子数组左端点不能低于 $i-k+1$，所以更新 $L$ 为 $\max(L,i-k)$；同理，更新 $R$ 为 $\min(R,i+k)$，
  - 如果子数组左端点 $> R-k$，那么右端点可以在 $[i,R)$ 中任意取，这样的左端点有 $i-(R-k)$ 个，所以子数组个数为 $(R-i)(i-(R-k))$。
  - 如果子数组左端点 $\le R-k$，那么右端点会受到左端点的约束：
    - 如果左端点在 $L+1$，那么右端点可以在区间 $[i,L+k]$ 中，子数组个数为 $L+k-i+1$。
    - 如果左端点在 $L+2$，那么右端点可以在区间 $[i,L+k+1]$ 中，子数组个数为 $L+k-i+2$。
    - 如果左端点在 $L+3$，那么右端点可以在区间 $[i,L+k+2]$ 中，子数组个数为 $L+k-i+3$。
    - ……
    - 如果左端点在 $R-k$，那么右端点可以在区间 $[i,R-1]$ 中，子数组个数为 $R-i$。
    - 累加，根据等差数列求和公式，得子数组个数为 $\dfrac{(L + R + k - 2i + 1) (R - L - k)}{2}$
  - 所以一共有
    $$
    (R-i)(i-(R-k)) + \dfrac{(L + R + k - 2i + 1) (R - L - k)}{2}
    $$
    个最小值是 $x$ 的子数组。

子数组个数乘以 $x$，加到答案中。

**技巧**：把所有 $\textit{nums}[i]$ 取反变成 $-\textit{nums}[i]$，就可以复用同一份代码求最大值的贡献了。

具体请看 [视频讲解](https://www.bilibili.com/video/BV17RwBeqErJ/?t=37m10s)，欢迎点赞关注~

### 优化前

```py [sol-Python3]
class Solution:
    # 计算最小值的贡献
    def sumSubarrayMins(self, nums: List[int], k: int) -> int:
        n = len(nums)
        # 左边界 left[i] 为左侧严格小于 nums[i] 的最近元素位置（不存在时为 -1）
        left = [-1] * n
        # 右边界 right[i] 为右侧小于等于 nums[i] 的最近元素位置（不存在时为 n）
        right = [n] * n
        st = []
        for i, x in enumerate(nums):
            while st and x <= nums[st[-1]]:
                right[st.pop()] = i  # i 是栈顶的右边界
            if st:
                left[i] = st[-1]
            st.append(i)

        ans = 0
        for i, (x, l, r) in enumerate(zip(nums, left, right)):
            if r - l - 1 <= k:
                cnt = (i - l) * (r - i)
                ans += x * cnt  # 累加贡献
            else:
                l = max(l, i - k)
                r = min(r, i + k)
                # 左端点 > r-k 的子数组个数
                cnt = (r - i) * (i - (r - k))
                # 左端点 <= r-k 的子数组个数
                cnt2 = (l + r + k - i * 2 + 1) * (r - l - k) // 2
                ans += x * (cnt + cnt2)  # 累加贡献
        return ans

    def minMaxSubarraySum(self, nums: List[int], k: int) -> int:
        ans = self.sumSubarrayMins(nums, k)
        # 所有元素取反，就可以复用同一份代码求最大值的贡献了
        ans -= self.sumSubarrayMins([-x for x in nums], k)
        return ans
```

```java [sol-Java]
class Solution {
    public long minMaxSubarraySum(int[] nums, int k) {
        long ans = sumSubarrayMins(nums, k);
        // 所有元素取反，就可以复用同一份代码求最大值的贡献了
        for (int i = 0; i < nums.length; i++) {
            nums[i] = -nums[i];
        }
        ans -= sumSubarrayMins(nums, k);
        return ans;
    }

    // 计算最小值的贡献
    private long sumSubarrayMins(int[] nums, int k) {
        int n = nums.length;
        // 左边界 left[i] 为左侧严格小于 nums[i] 的最近元素位置（不存在时为 -1）
        int[] left = new int[n];
        Arrays.fill(left, -1);
        // 右边界 right[i] 为右侧小于等于 nums[i] 的最近元素位置（不存在时为 n）
        int[] right = new int[n];
        Arrays.fill(right, n);

        Deque<Integer> st = new ArrayDeque<>();
        st.push(-1); // 哨兵，方便赋值 left
        for (int i = 0; i < n; i++) {
            while (st.size() > 1 && nums[i] <= nums[st.peek()]) {
                right[st.pop()] = i; // i 是栈顶的右边界
            }
            left[i] = st.peek();
            st.push(i);
        }

        long res = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            int l = left[i];
            int r = right[i];
            if (r - l - 1 <= k) {
                long cnt = (long) (i - l) * (r - i);
                res += x * cnt; // 累加贡献
            } else {
                l = Math.max(l, i - k);
                r = Math.min(r, i + k);
                // 左端点 > r-k 的子数组个数
                long cnt = (long) (r - i) * (i - (r - k));
                // 左端点 <= r-k 的子数组个数
                long cnt2 = (long) (l + r + k - i * 2 + 1) * (r - l - k) / 2;
                res += x * (cnt + cnt2); // 累加贡献
            }
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minMaxSubarraySum(vector<int>& nums, int k) {
        // 计算最小值的贡献
        auto sumSubarrayMins = [&]() -> long long {
            int n = nums.size();
            // 左边界 left[i] 为左侧严格小于 nums[i] 的最近元素位置（不存在时为 -1）
            vector<int> left(n, -1);
            // 右边界 right[i] 为右侧小于等于 nums[i] 的最近元素位置（不存在时为 n）
            vector<int> right(n, n);
            stack<int> st;
            st.push(-1); // 哨兵，方便赋值 left
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                while (st.size() > 1 && x <= nums[st.top()]) {
                    right[st.top()] = i; // i 是栈顶的右边界
                    st.pop();
                }
                left[i] = st.top();
                st.push(i);
            }

            long long res = 0;
            for (int i = 0; i < n; i++) {
                int x = nums[i], l = left[i], r = right[i];
                if (r - l - 1 <= k) {
                    long long cnt = 1LL * (i - l) * (r - i);
                    res += x * cnt; // 累加贡献
                } else {
                    l = max(l, i - k);
                    r = min(r, i + k);
                    // 左端点 > r-k 的子数组个数
                    long long cnt = 1LL * (r - i) * (i - (r - k));
                    // 左端点 <= r-k 的子数组个数
                    long long cnt2 = 1LL * (l + r + k - i * 2 + 1) * (r - l - k) / 2;
                    res += x * (cnt + cnt2); // 累加贡献
                }
            }
            return res;
        };

        long long ans = sumSubarrayMins();
        // 所有元素取反，就可以复用同一份代码求最大值的贡献了
        for (int& x : nums) {
            x = -x;
        }
        ans -= sumSubarrayMins();
        return ans;
    }
};
```

```go [sol-Go]
func minMaxSubarraySum(nums []int, k int) int64 {
	// 计算最小值的贡献
	sumSubarrayMins := func() (res int) {
		n := len(nums)
		// 左边界 left[i] 为左侧严格小于 nums[i] 的最近元素位置（不存在时为 -1）
		left := make([]int, n)
		// 右边界 right[i] 为右侧小于等于 nums[i] 的最近元素位置（不存在时为 n）
		right := make([]int, n)
		st := []int{-1} // 哨兵，方便赋值 left
		for i, x := range nums {
			for len(st) > 1 && x <= nums[st[len(st)-1]] {
				right[st[len(st)-1]] = i // i 是栈顶的右边界
				st = st[:len(st)-1]
			}
			left[i] = st[len(st)-1]
			st = append(st, i)
		}
		for _, i := range st[1:] {
			right[i] = n
		}

		for i, x := range nums {
			l, r := left[i], right[i]
			if r-l-1 <= k {
				cnt := (i - l) * (r - i)
				res += x * cnt // 累加贡献
			} else {
				l = max(l, i-k)
				r = min(r, i+k)
				// 左端点 > r-k 的子数组个数
				cnt := (r - i) * (i - (r - k))
				// 左端点 <= r-k 的子数组个数
				cnt2 := (l + r + k - i*2 + 1) * (r - l - k) / 2
				res += x * (cnt + cnt2) // 累加贡献
			}
		}
		return
	}
	ans := sumSubarrayMins()
	// 所有元素取反，就可以复用同一份代码求最大值的贡献了
	for i := range nums {
		nums[i] = -nums[i]
	}
	ans -= sumSubarrayMins()
	return int64(ans)
}
```

### 优化：减少遍历次数

原理见 [907 题解](https://leetcode.cn/problems/sum-of-subarray-minimums/solutions/1930857/gong-xian-fa-dan-diao-zhan-san-chong-shi-gxa5/) 的方法三。

```py [sol-Python3]
class Solution:
    # 计算最小值的贡献
    def sumSubarrayMins(self, nums: List[int], k: int) -> int:
        ans = 0
        st = [-1]
        for r, x in enumerate(nums + [-inf]):
            r0 = r
            while len(st) > 1 and nums[st[-1]] >= x:
                i = st.pop()
                l = st[-1]
                if r - l - 1 <= k:
                    cnt = (i - l) * (r - i)
                    ans += nums[i] * cnt  # 累加贡献
                else:
                    l = max(l, i - k)
                    r = min(r, i + k)
                    # 左端点 > r-k 的子数组个数
                    cnt = (r - i) * (i - (r - k))
                    # 左端点 <= r-k 的子数组个数
                    cnt2 = (l + r + k - i * 2 + 1) * (r - l - k) // 2
                    ans += nums[i] * (cnt + cnt2)  # 累加贡献
            st.append(r0)
        return ans

    def minMaxSubarraySum(self, nums: List[int], k: int) -> int:
        ans = self.sumSubarrayMins(nums, k)
        # 所有元素取反，就可以复用同一份代码求最大值的贡献了
        ans -= self.sumSubarrayMins([-x for x in nums], k)
        return ans
```

```java [sol-Java]
class Solution {
    public long minMaxSubarraySum(int[] nums, int k) {
        long ans = sumSubarrayMins(nums, k);
        // 所有元素取反，就可以复用同一份代码求最大值的贡献了
        for (int i = 0; i < nums.length; i++) {
            nums[i] = -nums[i];
        }
        ans -= sumSubarrayMins(nums, k);
        return ans;
    }

    // 计算最小值的贡献
    private long sumSubarrayMins(int[] nums, int k) {
        int n = nums.length;
        Deque<Integer> st = new ArrayDeque<>();
        st.push(-1);
        long res = 0;
        for (int r = 0; r <= n; r++) {
            int v = r < n ? nums[r] : Integer.MIN_VALUE; // 假设 nums 末尾有个 Integer.MIN_VALUE
            while (st.size() > 1 && nums[st.peek()] >= v) {
                int i = st.pop();
                int l = st.peek();
                int x = nums[i];
                if (r - l - 1 <= k) {
                    long cnt = (long) (i - l) * (r - i);
                    res += x * cnt; // 累加贡献
                } else {
                    l = Math.max(l, i - k);
                    int R = Math.min(r, i + k);
                    // 左端点 > R-k 的子数组个数
                    long cnt = (long) (R - i) * (i - (R - k));
                    // 左端点 <= r-k 的子数组个数
                    long cnt2 = (long) (l + R + k - i * 2 + 1) * (R - l - k) / 2;
                    res += x * (cnt + cnt2); // 累加贡献
                }
            }
            st.push(r);
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minMaxSubarraySum(vector<int>& nums, int k) {
        // 计算最小值的贡献
        auto sumSubarrayMins = [&]() -> long long {
            long long res = 0;
            stack<int> st;
            st.push(-1); // 哨兵
            for (int r = 0; r < nums.size(); r++) {
                while (st.size() > 1 && nums[st.top()] >= nums[r]) {
                    int i = st.top();
                    st.pop();
                    int l = st.top();
                    int x = nums[i];
                    if (r - l - 1 <= k) {
                        long long cnt = 1LL * (i - l) * (r - i);
                        res += x * cnt; // 累加贡献
                    } else {
                        l = max(l, i - k);
                        int R = min(r, i + k);
                        // 左端点 > r-k 的子数组个数
                        long long cnt = 1LL * (R - i) * (i - (R - k));
                        // 左端点 <= r-k 的子数组个数
                        long long cnt2 = 1LL * (l + R + k - i * 2 + 1) * (R - l - k) / 2;
                        res += x * (cnt + cnt2); // 累加贡献
                    }
                }
                st.push(r);
            }
            return res;
        };

        nums.push_back(INT_MIN / 2);
        long long ans = sumSubarrayMins();
        // 所有元素取反，就可以复用同一份代码求最大值的贡献了
        for (int& x : nums) {
            x = -x;
        }
        nums.back() *= -1;
        ans -= sumSubarrayMins();
        return ans;
    }
};
```

```go [sol-Go]
func minMaxSubarraySum(nums []int, k int) int64 {
	// 计算最小值的贡献
	sumSubarrayMins := func() (res int) {
		st := []int{-1} // 哨兵
		for r, x := range nums {
			r0 := r
			for len(st) > 1 && nums[st[len(st)-1]] >= x {
				i := st[len(st)-1]
				st = st[:len(st)-1]
				l := st[len(st)-1]
				if r-l-1 <= k {
					cnt := (i - l) * (r - i)
					res += nums[i] * cnt // 累加贡献
				} else {
					l = max(l, i-k)
					r = min(r, i+k)
					// 左端点 > r-k 的子数组个数
					cnt := (r - i) * (i - (r - k))
					// 左端点 <= r-k 的子数组个数
					cnt2 := (l + r + k - i*2 + 1) * (r - l - k) / 2
					res += nums[i] * (cnt + cnt2) // 累加贡献
				}
			}
			st = append(st, r0)
		}
		return
	}
	nums = append(nums, math.MinInt)
	ans := sumSubarrayMins()
	// 所有元素取反（求最大值），就可以复用同一份代码了
	for i := range nums {
		nums[i] = -nums[i]
	}
	ans -= sumSubarrayMins()
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 第二种计算方法

设 $x=\textit{nums}[i]$ 是子数组的最小值，设 $x$ 对应的边界为开区间 $(L,R)$（见 907 题我题解中的定义）。

包含 $x$ 的长度至多为 $k$ 的子数组个数，等于：

- 开区间 $(L,R)$ 中的所有长度至多为 $k$ 的子数组个数，
- 减去开区间 $(L,i)$ 中的长度至多为 $k$ 的子数组个数，
- 减去开区间 $(i,R)$ 中的长度至多为 $k$ 的子数组个数。

设开区间长度为 $m$，分类讨论：

- 如果 $m\le k$，那么长为 $1$ 的子数组有 $m$ 个，长为 $2$ 的子数组有 $m-1$ 个，……，长为 $m$ 的子数组有 $1$ 个，一共有 $m+(m-1)+\cdots+1=\dfrac{(m+1)m}{2}$ 个子数组。
- 如果 $m> k$，那么长为 $1$ 的子数组有 $m$ 个，长为 $2$ 的子数组有 $m-1$ 个，……，长为 $k$ 的子数组有 $m-k+1$ 个，一共有 $m+(m-1)+\cdots+(m-k+1)=\dfrac{(2m-k+1)k}{2}$ 个子数组。

```py [sol-Python3]
class Solution:
    def sumSubarrayMins(self, nums: List[int], k: int) -> int:
        count = lambda m: (m * 2 - k + 1) * k // 2 if m > k else (m + 1) * m // 2
        ans = 0
        st = [-1]
        for r, x in enumerate(nums + [-inf]):
            while len(st) > 1 and nums[st[-1]] >= x:
                i = st.pop()
                l = st[-1]
                cnt = count(r - l - 1) - count(i - l - 1) - count(r - i - 1)
                ans += nums[i] * cnt
            st.append(r)
        return ans

    def minMaxSubarraySum(self, nums: List[int], k: int) -> int:
        ans = self.sumSubarrayMins(nums, k)
        ans -= self.sumSubarrayMins([-x for x in nums], k)
        return ans
```

```java [sol-Java]
class Solution {
    public long minMaxSubarraySum(int[] nums, int k) {
        long ans = sumSubarrayMins(nums, k);
        // 所有元素取反，就可以复用同一份代码求最大值的贡献了
        for (int i = 0; i < nums.length; i++) {
            nums[i] = -nums[i];
        }
        ans -= sumSubarrayMins(nums, k);
        return ans;
    }

    // 计算最小值的贡献
    private long sumSubarrayMins(int[] nums, int k) {
        int n = nums.length;
        Deque<Integer> st = new ArrayDeque<>();
        st.push(-1);
        long res = 0;
        for (int r = 0; r <= n; r++) {
            int v = r < n ? nums[r] : Integer.MIN_VALUE; // 假设 nums 末尾有个 Integer.MIN_VALUE
            while (st.size() > 1 && nums[st.peek()] >= v) {
                int i = st.pop();
                int l = st.peek();
                long cnt = count(r - l - 1, k) - count(i - l - 1, k) - count(r - i - 1, k);
                res += nums[i] * cnt; // 累加贡献
            }
            st.push(r);
        }
        return res;
    }

    private long count(int m, int k) {
        return m > k ? ((long) m * 2 - k + 1) * k / 2 : ((long) m + 1) * m / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minMaxSubarraySum(vector<int>& nums, int k) {
        auto count = [&](int m) -> long long {
            return m > k ? 1LL * (m * 2 - k + 1) * k / 2 : 1LL * (m + 1) * m / 2;
        };

        // 计算最小值的贡献
        auto sumSubarrayMins = [&]() -> long long {
            long long res = 0;
            stack<int> st;
            st.push(-1); // 哨兵
            for (int r = 0; r < nums.size(); r++) {
                while (st.size() > 1 && nums[st.top()] >= nums[r]) {
                    int i = st.top();
                    st.pop();
                    int l = st.top();
                    long long cnt = count(r - l - 1) - count(i - l - 1) - count(r - i - 1);
                    res += nums[i] * cnt; // 累加贡献
                }
                st.push(r);
            }
            return res;
        };

        nums.push_back(INT_MIN / 2);
        long long ans = sumSubarrayMins();
        // 所有元素取反，就可以复用同一份代码求最大值的贡献了
        for (int& x : nums) {
            x = -x;
        }
        nums.back() *= -1;
        ans -= sumSubarrayMins();
        return ans;
    }
};
```

```go [sol-Go]
func minMaxSubarraySum(nums []int, k int) int64 {
	count := func(m int) int {
		if m <= k {
			return (m + 1) * m / 2
		}
		return (m*2 - k + 1) * k / 2
	}

	// 计算最小值的贡献
	sumSubarrayMins := func() (res int) {
		st := []int{-1} // 哨兵
		for r, x := range nums {
			for len(st) > 1 && nums[st[len(st)-1]] >= x {
				i := st[len(st)-1]
				st = st[:len(st)-1]
				l := st[len(st)-1]
				cnt := count(r-l-1) - count(i-l-1) - count(r-i-1)
				res += nums[i] * cnt // 累加贡献
			}
			st = append(st, r)
		}
		return
	}

	nums = append(nums, math.MinInt)
	ans := sumSubarrayMins()
	// 所有元素取反（求最大值），就可以复用同一份代码了
	for i := range nums {
		nums[i] = -nums[i]
	}
	ans -= sumSubarrayMins()
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面单调栈题单中的「**贡献法**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. 【本题相关】[单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
