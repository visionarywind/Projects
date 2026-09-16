# 56. 合并区间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-intervals/
- 题目 slug：`merge-intervals`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/merge-intervals/solutions/2798138/jian-dan-zuo-fa-yi-ji-wei-shi-yao-yao-zh-f2b3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：排序 / 差分数组 / 扫描线（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/merge-intervals/solutions/2798138/jian-dan-zuo-fa-yi-ji-wei-shi-yao-yao-zh-f2b3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-zuo-fa-yi-ji-wei-shi-yao-yao-zh-f2b3`
- topic id：`2798138`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：排序

以示例 1 为例，我们有 $[1,3],[2,6],[8,10],[15,18]$ 这四个区间。

为方便合并，把区间按照左端点从小到大排序（示例 1 已经按照左端点排序了）。排序的理由会在下面的合并过程中说明。

排序后，我们就知道了第一个合并区间的左端点，即 $\textit{intervals}[0][0]=1$。

第一个合并区间的右端点是多少？目前只知道其 $\ge \textit{intervals}[0][1]=3$，但具体是多少现在还不确定，得向右遍历。

具体算法如下：

1. 把 $\textit{intervals}[0]$ 加入答案。注意，答案的最后一个区间表示**当前正在合并的区间**。
2. 遍历到 $\textit{intervals}[1] = [2,6]$，由于左端点 $2$ 不超过当前合并区间的右端点 $3$，可以合并。由于右端点 $6>3$，那么**更新当前合并区间的右端点**为 $6$。由于我们已经按照左端点排序，所以 $\textit{intervals}[1]$ 的左端点 $2$ 必然大于等于合并区间的左端点，所以**无需更新当前合并区间的左端点**。
3. 遍历到 $\textit{intervals}[2] = [8,10]$，由于左端点 $8$ 大于当前合并区间的右端点 $6$，无法合并（两个区间不相交）。再次利用区间按照左端点排序的性质，更后面的区间的左端点也大于 $6$，无法与当前合并区间相交，所以当前合并区间 $[1,6]$ 就固定下来了，把新的合并区间 $[8,10]$ 加入答案。
4. 遍历到 $\textit{intervals}[3] = [15,18]$，由于左端点 $15$ 大于当前合并区间的右端点 $10$，无法合并（两个区间不相交），我们找到了一个新的合并区间 $[15,18]$ 加入答案。

上述算法同时说明，按照左端点排序后，合并的区间一定是 $\textit{intervals}$ 中的连续子数组。

#### 答疑

**问**：能不能按照右端点排序？
 
**答**：可以，但是需要倒着遍历 $\textit{intervals}$ 数组。如果正着遍历，比如 $[1,2],[4,5],[1,6]$ 这三个区间，正确答案是合并成 $[1,6]$，但正着遍历到 $[4,5]$ 这个区间时，无法知道 $[4,5]$ 能否和 $[1,2]$ 彻底断开。但按左端点排序的话，我们就知道这是不会断开的，会和之前的区间合并在一起。

[本题视频讲解](https://www.bilibili.com/video/BV1Qx4y1E7zj/?t=2m56s)，欢迎点赞关注~

#### 写法一

```py [sol-Python3]
class Solution:
    def merge(self, intervals: List[List[int]]) -> List[List[int]]:
        intervals.sort(key=lambda p: p[0])  # 按照左端点从小到大排序

        ans = []
        for p in intervals:
            if ans and p[0] <= ans[-1][1]:  # 左端点在合并区间内，可以合并
                ans[-1][1] = max(ans[-1][1], p[1])  # 更新合并区间的右端点
            else:  # 不相交，无法合并
                ans.append(p)  # 新的合并区间
        return ans
```

```java [sol-Java]
class Solution {
    public int[][] merge(int[][] intervals) {
        Arrays.sort(intervals, (p, q) -> p[0] - q[0]); // 按照左端点从小到大排序

        List<int[]> ans = new ArrayList<>();
        for (int[] p : intervals) {
            int m = ans.size();
            if (m > 0 && p[0] <= ans.get(m - 1)[1]) { // 左端点在合并区间内，可以合并
                ans.get(m - 1)[1] = Math.max(ans.get(m - 1)[1], p[1]); // 更新合并区间的右端点
            } else { // 不相交，无法合并
                ans.add(p); // 新的合并区间
            }
        }
        return ans.toArray(new int[ans.size()][]);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        ranges::sort(intervals); // 按照左端点从小到大排序

        vector<vector<int>> ans;
        for (auto& p : intervals) {
            if (!ans.empty() && p[0] <= ans.back()[1]) { // 左端点在合并区间内，可以合并
                ans.back()[1] = max(ans.back()[1], p[1]); // 更新合并区间的右端点
            } else { // 不相交，无法合并
                ans.emplace_back(p); // 新的合并区间
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func merge(intervals [][]int) (ans [][]int) {
    slices.SortFunc(intervals, func(p, q []int) int { return p[0] - q[0] }) // 按照左端点从小到大排序

    for _, p := range intervals {
        m := len(ans)
        if m > 0 && p[0] <= ans[m-1][1] { // 左端点在合并区间内，可以合并
            ans[m-1][1] = max(ans[m-1][1], p[1]) // 更新合并区间的右端点
        } else { // 不相交，无法合并
            ans = append(ans, p) // 新的合并区间
        }
    }
    return
}
```

```js [sol-JavaScript]
var merge = function(intervals) {
    intervals.sort((p, q) => p[0] - q[0]); // 按照左端点从小到大排序

    const ans = [];
    for (const p of intervals) {
        const m = ans.length;
        if (m && p[0] <= ans[m - 1][1]) { // 左端点在合并区间内，可以合并
            ans[m - 1][1] = Math.max(ans[m - 1][1], p[1]); // 更新合并区间的右端点
        } else { // 不相交，无法合并
            ans.push(p); // 新的合并区间
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn merge(mut intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        intervals.sort_unstable_by_key(|p| p[0]); // 按照左端点从小到大排序

        let mut ans: Vec<Vec<i32>> = vec![];
        for p in intervals {
            if let Some(last) = ans.last_mut() {
                if p[0] <= last[1] { // 左端点在合并区间内，可以合并
                    last[1] = last[1].max(p[1]); // 更新合并区间的右端点
                } else { // 不相交，无法合并
                    ans.push(p); // 新的合并区间
                }
            } else {
                ans.push(p); // 第一个区间
            }
        }
        ans
    }
}
```

#### 写法二

直接生成合并后的区间，不修改 $\textit{ans}$ 中的区间。

```py [sol-Python3]
class Solution:
    def merge(self, intervals: List[List[int]]) -> List[List[int]]:
        intervals.sort(key=lambda p: p[0])  # 按照左端点从小到大排序

        n = len(intervals)
        ans = []
        left, right = inf, -inf

        for i, (l, r) in enumerate(intervals):
            left = min(left, l)
            right = max(right, r)
            # 下一个区间与 [left, right] 不相交
            if i == n - 1 or intervals[i + 1][0] > right:  
                ans.append([left, right])
                left = inf

        return ans
```

```java [sol-Java]
class Solution {
    public int[][] merge(int[][] intervals) {
        Arrays.sort(intervals, (p, q) -> p[0] - q[0]); // 按照左端点从小到大排序

        int n = intervals.length;
        List<int[]> ans = new ArrayList<>();
        int left = Integer.MAX_VALUE;
        int right = Integer.MIN_VALUE;

        for (int i = 0; i < n; i++) {
            left = Math.min(left, intervals[i][0]);
            right = Math.max(right, intervals[i][1]);
            // 下一个区间与 [left, right] 不相交
            if (i == n - 1 || intervals[i + 1][0] > right) {
                ans.add(new int[]{left, right});
                left = Integer.MAX_VALUE;
            }
        }

        return ans.toArray(new int[ans.size()][]);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        ranges::sort(intervals, {}, [](auto& a) { return a[0]; }); // 按照左端点从小到大排序（另一种写法）

        int n = intervals.size();
        vector<vector<int>> ans;
        int left = INT_MAX, right = INT_MIN;

        for (int i = 0; i < n; i++) {
            left = min(left, intervals[i][0]);
            right = max(right, intervals[i][1]);
            // 下一个区间与 [left, right] 不相交
            if (i == n - 1 || intervals[i + 1][0] > right) {
                ans.push_back({left, right});
                left = INT_MAX;
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
func merge(intervals [][]int) (ans [][]int) {
	slices.SortFunc(intervals, func(p, q []int) int { return p[0] - q[0] }) // 按照左端点从小到大排序

	left, right := math.MaxInt, math.MinInt
	for i, p := range intervals {
		left = min(left, p[0])
		right = max(right, p[1])
		// 下一个区间与 [left, right] 不相交
		if i == len(intervals)-1 || intervals[i+1][0] > right {
			ans = append(ans, []int{left, right})
			left = math.MaxInt
		}
	}
	return
}
```

```js [sol-JavaScript]
var merge = function(intervals) {
    intervals.sort((p, q) => p[0] - q[0]); // 按照左端点从小到大排序

    const n = intervals.length;
    const ans = [];
    let left = Infinity;
    let right = -Infinity;

    for (let i = 0; i < n; i++) {
        left = Math.min(left, intervals[i][0]);
        right = Math.max(right, intervals[i][1]);
        // 下一个区间与 [left, right] 不相交
        if (i === n - 1 || intervals[i + 1][0] > right) {
            ans.push([left, right]);
            left = Infinity;
        }
    }

    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn merge(mut intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        intervals.sort_unstable_by_key(|p| p[0]); // 按照左端点从小到大排序

        let n = intervals.len();
        let mut ans = vec![];
        let mut left = i32::MAX;
        let mut right = i32::MIN;

        for (i, p) in intervals.iter().enumerate() {
            left = left.min(p[0]);
            right = right.max(p[1]);
            // 下一个区间与 [left, right] 不相交
            if i == n - 1 || intervals[i + 1][0] > right {
                ans.push(vec![left, right]);
                left = i32::MAX;
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{intervals}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。排序的栈开销和返回值不计入。

## 方法二：差分数组

创建一个计数数组 $\textit{cnt}$，初始值均为 $0$。

对于区间 $[\textit{start},\textit{end}]$，把 $\textit{cnt}[\textit{start}],\textit{cnt}[\textit{start}+1],\ldots,\textit{cnt}[\textit{end}]$ 都增加 $1$。最终满足 $\textit{cnt}[i]>0$ 的连续下标，就是合并后的区间。例如 $\textit{intervals}= [[0,2],[1,3],[5,6]]$，对应的 $\textit{cnt}=[1,2,2,1,0,1,1]$，其中下标 $i=0,1,2,3,5,6$ 的 $\textit{cnt}[i] > 0$，所以合并后的区间为 $[0,3]$ 和 $[5,6]$。

然而，这个做法有一个 bug，例如 $\textit{intervals}= [[1,2],[3,4]]$，这两个区间不能合并，但按照上述做法，由于 $\textit{cnt}=[0,1,1,1,1]$，我们会误认为合并后的区间为 $[1,4]$。

**解决办法**：把区间左右端点乘以 $2$，例如 $[1,2],[3,4]$ 变成 $[2,4],[6,8]$，这样就把相邻的区间用整数 $5$ 隔开了。对应的 $\textit{cnt}=[0,0,1,1,1,0,1,1,1]$，区间为 $[2,4],[6,8]$。最后再把左右端点除以 $2$，得到 $[1,2],[3,4]$。

如何快速实现区间加一？请看 [差分数组原理讲解](https://leetcode.cn/problems/car-pooling/solution/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)。

```py [sol-Python3]
class Solution:
    def merge(self, intervals: List[List[int]]) -> List[List[int]]:
        mx = max(p[1] for p in intervals)

        diff = [0] * (mx * 2 + 2)
        for start, end in intervals:
            # 把区间 [start*2, end*2] 增加 1
            diff[start * 2] += 1
            diff[end * 2 + 1] -= 1

        ans = []
        sum_d = 0
        start = -1  # -1 表示尚未遇到合并后的区间左端点
        for i, d in enumerate(diff):
            sum_d += d  # 计算 diff 的前缀和
            if sum_d > 0:
                if start < 0:
                    start = i  # 合并后的区间左端点
            elif start >= 0:
                # i-1 是合并后的区间右端点
                # 由于乘 2 操作，区间左右端点都是偶数，所以 i-1 是偶数，i 是奇数，(i-1)/2 == floor(i/2)
                ans.append([start // 2, i // 2])
                start = -1
        # 注：最后一轮循环 sum_d == 0，我们不会漏掉最后一个区间
        return ans
```

```java [sol-Java]
class Solution {
    public int[][] merge(int[][] intervals) {
        int mx = 0;
        for (int[] p : intervals) {
            mx = Math.max(mx, p[1]);
        }

        int[] diff = new int[mx * 2 + 2];
        for (int[] p : intervals) {
            // 把区间 [p[0]*2, p[1]*2] 增加 1
            diff[p[0] * 2]++;
            diff[p[1] * 2 + 1]--;
        }

        List<int[]> ans = new ArrayList<>();
        int sumD = 0;
        int start = -1; // -1 表示尚未遇到合并后的区间左端点
        for (int i = 0; i < diff.length; i++) {
            sumD += diff[i]; // 计算 diff 的前缀和
            if (sumD > 0) {
                if (start < 0) {
                    start = i; // 合并后的区间左端点
                }
            } else if (start >= 0) {
                // i-1 是合并后的区间右端点
                // 由于乘 2 操作，区间左右端点都是偶数，所以 i-1 是偶数，i 是奇数，(i-1)/2 == floor(i/2)
                ans.add(new int[]{start / 2, i / 2});
                start = -1;
            }
        }
        // 注：最后一轮循环 sumD == 0，我们不会漏掉最后一个区间
        return ans.toArray(new int[ans.size()][]);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        int mx = 0;
        for (auto& p : intervals) {
            mx = max(mx, p[1]);
        }

        vector<int> diff(mx * 2 + 2);
        for (auto& p : intervals) {
            // 把区间 [p[0]*2, p[1]*2] 增加 1
            diff[p[0] * 2]++;
            diff[p[1] * 2 + 1]--;
        }

        vector<vector<int>> ans;
        int sum_d = 0;
        int start = -1; // -1 表示尚未遇到合并后的区间左端点
        for (int i = 0; i < diff.size(); i++) {
            sum_d += diff[i]; // 计算 diff 的前缀和
            if (sum_d > 0) {
                if (start < 0) {
                    start = i; // 合并后的区间左端点
                }
            } else if (start >= 0) {
                // i-1 是合并后的区间右端点
                // 由于乘 2 操作，区间左右端点都是偶数，所以 i-1 是偶数，i 是奇数，(i-1)/2 == floor(i/2)
                ans.push_back({start / 2, i / 2});
                start = -1;
            }
        }
        // 注：最后一轮循环 sum_d == 0，我们不会漏掉最后一个区间
        return ans;
    }
};
```

```go [sol-Go]
func merge(intervals [][]int) (ans [][]int) {
	mx := 0
	for _, p := range intervals {
		mx = max(mx, p[1])
	}

	diff := make([]int, mx*2+2)
	for _, p := range intervals {
		// 把区间 [p[0]*2, p[1]*2] 增加 1
		diff[p[0]*2]++
		diff[p[1]*2+1]--
	}

	sumD := 0
	start := -1 // -1 表示尚未遇到合并后的区间左端点
	for i, d := range diff {
		sumD += d // 计算 diff 的前缀和
		if sumD > 0 {
			if start < 0 {
				start = i // 合并后的区间左端点
			}
		} else if start >= 0 {
			// i-1 是合并后的区间右端点
			// 由于乘 2 操作，区间左右端点都是偶数，所以 i-1 是偶数，i 是奇数，(i-1)/2 == floor(i/2)
			ans = append(ans, []int{start / 2, i / 2})
			start = -1
		}
	}
	// 注：最后一轮循环 sumD == 0，我们不会漏掉最后一个区间
	return
}
```

```js [sol-JavaScript]
var merge = function(intervals) {
    let mx = 0;
    for (const p of intervals) {
        mx = Math.max(mx, p[1]);
    }

    const diff = Array(mx * 2 + 2).fill(0);
    for (const [start, end] of intervals) {
        // 把区间 [start*2, end*2] 增加 1
        diff[start * 2]++;
        diff[end * 2 + 1]--;
    }

    const ans = [];
    let sumD = 0;
    let start = -1; // -1 表示尚未遇到合并后的区间左端点
    for (let i = 0; i < diff.length; i++) {
        sumD += diff[i]; // 计算 diff 的前缀和
        if (sumD > 0) {
            if (start < 0) {
                start = i; // 合并后的区间左端点
            }
        } else if (start >= 0) {
            ans.push([start / 2, (i - 1) / 2]); // i-1 是合并后的区间右端点
            start = -1;
        }
    }
    // 注：最后一轮循环 sumD == 0，我们不会漏掉最后一个区间
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn merge(intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        let mut mx = intervals.iter().map(|p| p[1]).max().unwrap();

        let mut diff = vec![0; (mx * 2 + 2) as usize];
        for p in intervals {
            // 把区间 [p[0]*2, p[1]*2] 增加 1
            diff[(p[0] * 2) as usize] += 1;
            diff[(p[1] * 2 + 1) as usize] -= 1;
        }

        let mut ans = vec![];
        let mut sum_d = 0;
        let mut start = -1; // -1 表示尚未遇到合并后的区间左端点
        for (i, d) in diff.into_iter().enumerate() {
            sum_d += d; // 计算 diff 的前缀和
            if sum_d > 0 {
                if start < 0 {
                    start = i as i32; // 合并后的区间左端点
                }
            } else if start >= 0 {
                // i-1 是合并后的区间右端点
                // 由于乘 2 操作，区间左右端点都是偶数，所以 i-1 是偶数，i 是奇数，(i-1)/2 == floor(i/2)
                ans.push(vec![start / 2, i as i32 / 2]);
                start = -1;
            }
        }
        // 注：最后一轮循环 sumD == 0，我们不会漏掉最后一个区间
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + U)$，其中 $n$ 是 $\textit{intervals}$ 的长度，$U=\max(\textit{end}_i)$。
- 空间复杂度：$\mathcal{O}(U)$。返回值不计入。

## 方法三：扫描线

想象一根垂线从左到右，缓缓扫过每个区间。在这个过程中，用一个计数器 $\textit{cnt}$ 表示当前垂线与多少个区间相交。

- 如果垂线遇到区间左端点 $\textit{start}$，则垂线开始与该区间相交，把 $\textit{cnt}$ 加一。如果加一前 $\textit{cnt}=0$，则说明我们开始了一段新的合并区间，$\textit{start}$ 是合并后的区间左端点。
- 如果垂线遇到区间右端点 $\textit{end}$，则垂线结束与该区间相交，把 $\textit{cnt}$ 减一。如果减一后 $\textit{cnt}=0$，则说明 $\textit{end}$ 是合并后的区间右端点。

区间 $[1,3]$ 和 $[2,6]$ 的合并过程如下：

1. 初始化 $\textit{cnt} = 0$。
2. 扫描线遇到左端点 $1$，现在 $\textit{cnt} = 1$。由于 $\textit{cnt}$ 增加之前是 $0$，记录合并区间的左端点为 $1$。
3. 扫描线遇到左端点 $2$，现在 $\textit{cnt} = 2$。
4. 扫描线遇到右端点 $3$，现在 $\textit{cnt} = 1$。这说明我们仍然在一个区间内，合并过程没有结束。
5. 扫描线遇到右端点 $6$，现在 $\textit{cnt} = 0$。合并结束，把区间 $[1,6]$ 加入答案。

> 顺带一提，回顾方法二中的例子，对于 $[1,2],[3,4]$ 这样的区间，我们会在 $2$ 这个位置就判断出 $[1,2]$ 是个独立的区间，不会把 $[1,2]$ 和 $[3,4]$ 合并。

```py [sol-Python3]
class Solution:
    def merge(self, intervals: List[List[int]]) -> List[List[int]]:
        events = defaultdict(int)
        for start, end in intervals:
            events[start] += 1  # 垂线遇到左端点则加一
            events[end] -= 1  # 垂线遇到右端点则减一
            # 这样处理后，就可以把 cnt 的更新逻辑统一成 cnt += events[x]，无需区分左右端点

        ans = []
        cnt = 0
        for x, c in sorted(events.items()):
            if cnt == 0:  # 扫描线开始与区间相交
                start = x  # x 是合并后的区间左端点
            cnt += c
            if cnt == 0:  # 扫描线结束与区间相交
                ans.append([start, x])  # x 是合并后的区间右端点
        return ans
```

```java [sol-Java]
class Solution {
    public int[][] merge(int[][] intervals) {
        Map<Integer, Integer> events = new TreeMap<>();
        for (int[] p : intervals) {
            events.merge(p[0], 1, Integer::sum); // 垂线遇到左端点则加一
            events.merge(p[1], -1, Integer::sum); // 垂线遇到右端点则减一
            // 这样处理后，就可以把 cnt 的更新逻辑统一成 cnt += events.get(x)，无需区分左右端点
        }

        List<int[]> ans = new ArrayList<>();
        int cnt = 0;
        int start = 0; // start 的初始值随意，0 可以换成任意值
        for (Map.Entry<Integer, Integer> e : events.entrySet()) {
            int x = e.getKey();
            if (cnt == 0) { // 扫描线开始与区间相交
                start = x; // x 是合并后的区间左端点
            }
            cnt += e.getValue();
            if (cnt == 0) { // 扫描线结束与区间相交
                ans.add(new int[]{start, x}); // x 是合并后的区间右端点
            }
        }
        return ans.toArray(new int[ans.size()][]);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        map<int, int> events;
        for (auto& p : intervals) {
            events[p[0]]++; // 垂线遇到左端点则加一
            events[p[1]]--; // 垂线遇到右端点则减一
            // 这样处理后，就可以把 cnt 的更新逻辑统一成 cnt += events[x]，无需区分左右端点
        }

        vector<vector<int>> ans;
        int cnt = 0;
        int start;
        for (auto& [x, c] : events) {
            if (cnt == 0) { // 扫描线开始与区间相交
                start = x; // x 是合并后的区间左端点
            }
            cnt += c;
            if (cnt == 0) { // 扫描线结束与区间相交
                ans.push_back({start, x}); // x 是合并后的区间右端点
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func merge(intervals [][]int) (ans [][]int) {
	events := map[int]int{}
	for _, p := range intervals {
		events[p[0]]++ // 垂线遇到左端点则加一
		events[p[1]]-- // 垂线遇到右端点则减一
		// 这样处理后，就可以把 cnt 的更新逻辑统一成 cnt += events[x]，无需区分左右端点
	}

	points := slices.Sorted(maps.Keys(events))
	cnt := 0
	start := 0 // start 的初始值随意，0 可以换成任意值
	for _, x := range points {
		if cnt == 0 { // 扫描线开始与区间相交
			start = x // x 是合并后的区间左端点
		}
		cnt += events[x]
		if cnt == 0 { // 扫描线结束与区间相交
			ans = append(ans, []int{start, x}) // x 是合并后的区间右端点
		}
	}
	return
}
```

```js [sol-JavaScript]
var merge = function(intervals) {
    const events = new Map();
    for (const [start, end] of intervals) {
        events.set(start, (events.get(start) ?? 0) + 1); // 垂线遇到左端点则加一
        events.set(end, (events.get(end) ?? 0) - 1); // 垂线遇到右端点则减一
        // 这样处理后，就可以把 cnt 的更新逻辑统一成 cnt += events.get(x)，无需区分左右端点
    }

    const points = [...events.keys()].sort((a, b) => a - b);
    const ans = [];
    let cnt = 0;
    let start;
    for (const x of points) {
        if (cnt === 0) { // 扫描线开始与区间相交
            start = x; // x 是合并后的区间左端点
        }
        cnt += events.get(x);
        if (cnt === 0) { // 扫描线结束与区间相交
            ans.push([start, x]); // x 是合并后的区间右端点
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BTreeMap;

impl Solution {
    pub fn merge(intervals: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        let mut events = BTreeMap::new();
        for p in intervals {
            *events.entry(p[0]).or_insert(0) += 1; // 垂线遇到左端点则加一
            *events.entry(p[1]).or_insert(0) -= 1; // 垂线遇到右端点则减一
            // 这样处理后，就可以把 cnt 的更新逻辑统一成 cnt += events[x]，无需区分左右端点
        }

        let mut ans = vec![];
        let mut cnt = 0;
        let mut start = 0; // start 的初始值随意，0 可以换成任意值
        for (x, v) in events {
            if cnt == 0 { // 扫描线开始与区间相交
                start = x; // x 是合并后的区间左端点
            }
            cnt += v;
            if cnt == 0 { // 扫描线结束与区间相交
                ans.push(vec![start, x]); // x 是合并后的区间右端点
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{intervals}$ 的长度。瓶颈在排序（或者维护有序集合）上。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `二、差分 / §2.1 一维差分 / §2.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、差分 / §2.1 一维差分 / §2.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
