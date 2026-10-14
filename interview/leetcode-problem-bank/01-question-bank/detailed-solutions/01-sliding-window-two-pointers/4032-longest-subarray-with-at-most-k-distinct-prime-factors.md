# 4032. 至多 K 个不同质因数集合的最长子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-subarray-with-at-most-k-distinct-prime-factors/
- 题目 slug：`longest-subarray-with-at-most-k-distinct-prime-factors`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）
- 难度分：1759
- 外部题解来源：https://leetcode.cn/problems/longest-subarray-with-at-most-k-distinct-prime-factors/solutions/4016363/yu-chu-li-hua-dong-chuang-kou-pythonjava-4hfj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理 + 滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-subarray-with-at-most-k-distinct-prime-factors/solutions/4016363/yu-chu-li-hua-dong-chuang-kou-pythonjava-4hfj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-hua-dong-chuang-kou-pythonjava-4hfj`
- topic id：`4016363`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

子数组越短，越能满足要求；子数组越长，越不能满足要求。有这样的性质，可以用**滑动窗口**解决，原理请看视频[【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

用哈希表统计每个质因子及其出现次数，那么哈希表的大小即为子数组内的不同质因子的个数。

质因子可以提前预处理，原理同埃式筛，见 [本题视频讲解](https://www.bilibili.com/video/BV18p846TEwX/?t=52m47s)，欢迎点赞关注~

```py [sol-Python3]
MX = 100_001
prime_factors = [[] for _ in range(MX)]
for i in range(2, MX):
    if not prime_factors[i]:  # i 是质数
        for j in range(i, MX, i):  # i 的倍数 j 有质因子 i
            prime_factors[j].append(i)

class Solution:
    def longestSubarray(self, nums: list[int], k: int) -> int:
        cnt = defaultdict(int)
        ans = left = 0
        for i, x in enumerate(nums):
            for p in prime_factors[x]:
                cnt[p] += 1
            while len(cnt) > k:
                for p in prime_factors[nums[left]]:
                    if cnt[p] > 1:
                        cnt[p] -= 1
                    else:
                        del cnt[p]  # 保证 len(cnt) 是窗口内的不同质因子个数
                left += 1
            ans = max(ans, i - left + 1)
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MX = 100_001;
    private static final List<Integer>[] primeFactors = new ArrayList[MX];
    private static boolean initialized = false;

    // 这样写比 static block 快
    public Solution() {
        if (initialized) {
            return;
        }
        initialized = true;

        Arrays.setAll(primeFactors, _ -> new ArrayList<>());
        for (int i = 2; i < MX; i++) {
            if (primeFactors[i].isEmpty()) { // i 是质数
                for (int j = i; j < MX; j += i) { // i 的倍数 j 有质因子 i
                    primeFactors[j].add(i);
                }
            }
        }
    }

    public int longestSubarray(int[] nums, int k) {
        HashMap<Integer, Integer> cnt = new HashMap<>();
        int left = 0;
        int ans = 0;
        for (int i = 0; i < nums.length; i++) {
            int x = nums[i];
            for (int p : primeFactors[x]) {
                cnt.merge(p, 1, Integer::sum); // ++cnt[p]
            }
            while (cnt.size() > k) {
                for (int p : primeFactors[nums[left]]) {
                    int c = cnt.merge(p, -1, Integer::sum); // c = --cnt[p]
                    if (c == 0) {
                        cnt.remove(p); // 保证 cnt.size() 是窗口内的不同质因子个数
                    }
                }
                left++;
            }
            ans = Math.max(ans, i - left + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
constexpr int MX = 100'001;
vector<int> prime_factors[MX];

int init = [] {
    for (int i = 2; i < MX; i++) {
        if (prime_factors[i].empty()) { // i 是质数
            for (int j = i; j < MX; j += i) { // i 的倍数 j 有质因子 i
                prime_factors[j].push_back(i);
            }
        }
    }
    return 0;
}();

class Solution {
public:
    int longestSubarray(vector<int>& nums, int k) {
        unordered_map<int, int> cnt;
        int ans = 0, left = 0;
        for (int i = 0; i < nums.size(); i++) {
            int x = nums[i];
            for (int p : prime_factors[x]) {
                cnt[p]++;
            }
            while (cnt.size() > k) {
                for (int p : prime_factors[nums[left]]) {
                    auto it = cnt.find(p);
                    if (--it->second == 0) {
                        cnt.erase(it); // 保证 cnt.size() 是窗口内的不同质因子个数
                    }
                }
                left++;
            }
            ans = max(ans, i - left + 1);
        }
        return ans;
    }
};
```

```go [sol-Go]
const mx = 100_001
var primeFactors = [mx][]int{}

func init() {
	for i := 2; i < mx; i++ {
		if primeFactors[i] == nil { // i 是质数
			for j := i; j < mx; j += i { // i 的倍数 j 有质因子 i
				primeFactors[j] = append(primeFactors[j], i)
			}
		}
	}
}

func longestSubarray(nums []int, k int) (ans int) {
	cnt := map[int]int{}
	left := 0
	for i, x := range nums {
		for _, p := range primeFactors[x] {
			cnt[p]++
		}
		for len(cnt) > k {
			for _, p := range primeFactors[nums[left]] {
				if cnt[p] > 1 {
					cnt[p]--
				} else {
					delete(cnt, p) // 保证 len(cnt) 是窗口内的不同质因子个数
				}
			}
			left++
		}
		ans = max(ans, i-left+1)
	}
	return
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(k)$。哈希表的大小为 $\mathcal{O}(k)$。

## 专题训练

1. 滑动窗口题单的「**二、不定长滑动窗口**」。
2. 数学题单的「**§1.3 质因数分解**」。

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
