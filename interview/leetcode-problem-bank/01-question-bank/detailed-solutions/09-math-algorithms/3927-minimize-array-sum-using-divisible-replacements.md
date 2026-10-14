# 3927. 可整除替换后的数组最小元素和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-array-sum-using-divisible-replacements/
- 题目 slug：`minimize-array-sum-using-divisible-replacements`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.5 因子
- 难度分：1652
- 外部题解来源：https://leetcode.cn/problems/minimize-array-sum-using-divisible-replacements/solutions/3966168/ti-huan-cheng-zui-xiao-de-yin-zi-pythonj-ltih/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[替换成最小的因子（Python/Java/C++/Go）](https://leetcode.cn/problems/minimize-array-sum-using-divisible-replacements/solutions/3966168/ti-huan-cheng-zui-xiao-de-yin-zi-pythonj-ltih/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ti-huan-cheng-zui-xiao-de-yin-zi-pythonj-ltih`
- topic id：`3966168`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

每次操作，可以把 $x$ 替换成 $x$ 的因子 $d$，要求 $d$ 在 $\textit{nums}$ 中。

为了让总和尽量小，把 $x$ 替换成 $x$ 的最小因子（因子必须在 $\textit{nums}$ 中）是最优的。

> **注**：替换成 $x$ 的最小因子 $d$ 后，不可能再把 $d$ 替换成更小的因子，这是因为 $x$ 的因子的因子也是 $x$ 的因子。

提前预处理 $[1,10^5]$ 每个整数的因子，即可快速枚举因子。

[本题视频讲解](https://www.bilibili.com/video/BV1Vb5L6zEgm/?t=10m20s)，欢迎点赞关注~

```py [sol-Python3]
# 预处理每个数的因子
MX = 100_001
divisors = [[] for _ in range(MX)]
for i in range(1, MX):
    for j in range(i, MX, i):  # 枚举 i 的倍数 j
        divisors[j].append(i)  # i 是 j 的因子

class Solution:
    def minArraySum(self, nums: list[int]) -> int:
        cnt = Counter(nums)
        ans = 0

        for x, c in cnt.items():  # 遍历 cnt 而不是 nums，这样重复元素只会计算一次
            for d in divisors[x]:  # 从小到大枚举 x 的因子 d
                if d in cnt:
                    ans += d * c  # 把 x 变成 d 是最优的
                    break

        return ans
```

```java [sol-Java]
class Solution {
    private static final int MX = 100_001;
    private static final List<Integer>[] divisors = new ArrayList[MX];
    private static boolean initialized = false;

    // 这样写比 static block 快
    public Solution() {
        if (initialized) {
            return;
        }
        initialized = true;

        Arrays.setAll(divisors, _ -> new ArrayList<>());
        for (int i = 1; i < MX; i++) {
            for (int j = i; j < MX; j += i) { // 枚举 i 的倍数 j
                divisors[j].add(i); // i 是 j 的因子
            }
        }
    }

    public long minArraySum(int[] nums) {
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            cnt.merge(x, 1, Integer::sum); // cnt[x]++
        }

        long ans = 0;
        // 遍历 cnt 而不是 nums，这样重复元素只会计算一次
        for (Map.Entry<Integer, Integer> e : cnt.entrySet()) {
            int x = e.getKey();
            int c = e.getValue();
            for (int d : divisors[x]) { // 从小到大枚举 x 的因子 d
                if (cnt.containsKey(d)) {
                    ans += (long) d * c; // 把 x 变成 d 是最优的
                    break;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
constexpr int MX = 100'001;
vector<int> divisors[MX];

int init = [] {
    for (int i = 1; i < MX; i++) {
        for (int j = i; j < MX; j += i) { // 枚举 i 的倍数 j
            divisors[j].push_back(i); // i 是 j 的因子
        }
    }
    return 0;
}();

class Solution {
public:
    long long minArraySum(vector<int>& nums) {
        unordered_map<int, int> cnt;
        for (int x : nums) {
            cnt[x]++;
        }

        long long ans = 0;
        for (auto& [x, c] : cnt) { // 遍历 cnt 而不是 nums，这样重复元素只会计算一次
            for (int d : divisors[x]) { // 从小到大枚举 x 的因子 d
                if (cnt.contains(d)) {
                    ans += 1LL * d * c; // 把 x 变成 d 是最优的
                    break;
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
const mx = 100_001
var divisors [mx][]int

func init() {
	for i := 1; i < mx; i++ {
		for j := i; j < mx; j += i { // 枚举 i 的倍数 j
			divisors[j] = append(divisors[j], i) // i 是 j 的因子
		}
	}
}

func minArraySum(nums []int) (ans int64) {
	cnt := map[int]int{}
	for _, x := range nums {
		cnt[x]++
	}

	for x, c := range cnt { // 遍历 cnt 而不是 nums，这样重复元素只会计算一次
		for _, d := range divisors[x] { // 从小到大枚举 x 的因子 d
			if cnt[d] > 0 {
				ans += int64(d) * int64(c) // 把 x 变成 d 是最优的
				break
			}
		}
	}
	return
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(nA)$，其中 $n$ 是 $\textit{nums}$ 的长度，$A\le 128$ 是 $\textit{nums}[i]$ 的最大因子个数。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数学题单的「**§1.5 因子**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
