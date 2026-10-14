# 4038. 统计特殊整数个数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-integers-appearing-in-a-single-block/
- 题目 slug：`count-integers-appearing-in-a-single-block`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-integers-appearing-in-a-single-block/solutions/4019841/jian-dan-ti-jian-dan-zuo-on-zuo-fa-pytho-q0ng/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题，简单做，两种 O(n) 方法（Python/Java/C++/Go）](https://leetcode.cn/problems/count-integers-appearing-in-a-single-block/solutions/4019841/jian-dan-ti-jian-dan-zuo-on-zuo-fa-pytho-q0ng/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-on-zuo-fa-pytho-q0ng`
- topic id：`4019841`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：记录元素出现位置

设元素 $x$ 的出现位置列表为 $p$，长度为 $m$。

如果 $p[m-1] - p[0] + 1 = m$，说明所有 $x$ 都在一个连续区间内。

否则 $p[m-1] - p[0] + 1 > m$，说明第一个 $x$ 到最后一个 $x$ 之间夹杂着其他数。

[本题视频讲解](https://www.bilibili.com/video/BV1NV4X6bEhr/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countSpecialIntegers(self, nums: list[int]) -> int:
        pos = defaultdict(list)  # x -> [x 的所有出现位置]
        for i, x in enumerate(nums):
            pos[x].append(i)

        ans = 0
        for p in pos.values():
            if p[-1] - p[0] + 1 == len(p):
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int countSpecialIntegers(int[] nums) {
        Map<Integer, List<Integer>> pos = new HashMap<>(); // x -> [x 的所有出现位置]
        for (int i = 0; i < nums.length; i++) {
            pos.computeIfAbsent(nums[i], _ -> new ArrayList<>()).add(i);
        }

        int ans = 0;
        for (List<Integer> p : pos.values()) {
            if (p.getLast() - p.getFirst() + 1 == p.size()) {
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
    int countSpecialIntegers(vector<int>& nums) {
        unordered_map<int, vector<int>> pos; // x -> [x 的所有出现位置]
        for (int i = 0; i < nums.size(); i++) {
            pos[nums[i]].push_back(i);
        }

        int ans = 0;
        for (auto& [_, p] : pos) {
            if (p.back() - p[0] + 1 == p.size()) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countSpecialIntegers(nums []int) (ans int) {
	pos := map[int][]int{} // x -> [x 的所有出现位置]
	for i, x := range nums {
		pos[x] = append(pos[x], i)
	}

	for _, p := range pos {
		if p[len(p)-1]-p[0]+1 == len(p) {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：一次遍历

只考虑每个连续相同段的第一个数 $x$。

- 如果首次遇到 $x$，那么暂时认为 $x$ 是特殊整数，把答案加一。
- 如果第二次遇到 $x$，那么 $x$ 不是特殊整数，撤销之前的加一。

```py [sol-Python3]
class Solution:
    def countSpecialIntegers(self, nums: list[int]) -> int:
        cnt = defaultdict(int)
        ans = 0
        for i, x in enumerate(nums):
            if i == 0 or x != nums[i - 1]:  # x 是这一段的第一个数
                cnt[x] += 1
                if cnt[x] == 1:  # 首次遇到 x，暂时认为 x 是特殊整数
                    ans += 1
                elif cnt[x] == 2:  # x 不是特殊整数，撤销之前的 ans += 1
                    ans -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int countSpecialIntegers(int[] nums) {
        Map<Integer, Integer> cnt = new HashMap<>();
        int ans = 0;
        for (int i = 0; i < nums.length; i++) {
            int x = nums[i];
            if (i == 0 || x != nums[i - 1]) { // x 是这一段的第一个数
                int c = cnt.merge(x, 1, Integer::sum); // c = ++cnt[x]
                if (c == 1) { // 首次遇到 x，暂时认为 x 是特殊整数
                    ans++;
                } else if (c == 2) { // x 不是特殊整数，撤销之前的 ans++
                    ans--;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countSpecialIntegers(vector<int>& nums) {
        unordered_map<int, int> cnt;
        int ans = 0;
        for (int i = 0; i < nums.size(); i++) {
            int x = nums[i];
            if (i == 0 || x != nums[i - 1]) { // x 是这一段的第一个数
                cnt[x]++;
                if (cnt[x] == 1) { // 首次遇到 x，暂时认为 x 是特殊整数
                    ans++;
                } else if (cnt[x] == 2) { // x 不是特殊整数，撤销之前的 ans++
                    ans--;
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countSpecialIntegers(nums []int) (ans int) {
	cnt := map[int]int{}
	for i, x := range nums {
		if i == 0 || x != nums[i-1] { // x 是这一段的第一个数
			cnt[x]++
			if cnt[x] == 1 { // 首次遇到 x，暂时认为 x 是特殊整数
				ans++
			} else if cnt[x] == 2 { // x 不是特殊整数，撤销之前的 ans++
				ans--
			}
		}
	}
	return
}
```

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
