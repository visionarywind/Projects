# 4001. 聚合两个时间序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/aggregate-two-time-series/
- 题目 slug：`aggregate-two-time-series`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.1 双指针
- 难度分：1506
- 外部题解来源：https://leetcode.cn/problems/aggregate-two-time-series/solutions/4001980/shuang-zhi-zhen-he-bing-liang-ge-you-xu-4ur12/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[双指针，合并两个有序列表（Python/Java/C++/Go）](https://leetcode.cn/problems/aggregate-two-time-series/solutions/4001980/shuang-zhi-zhen-he-bing-liang-ge-you-xu-4ur12/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shuang-zhi-zhen-he-bing-liang-ge-you-xu-4ur12`
- topic id：`4001980`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

思路类似 [21. 合并两个有序链表](https://leetcode.cn/problems/merge-two-sorted-lists/)。双指针，谁小就移动谁。~~难度全在阅读理解~~

[本题视频讲解](https://www.bilibili.com/video/BV1Ps3j6nE3D/?t=33m31s)（包含题意解读），欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def aggregateTimeSeries(self, series1: list[list[int]], series2: list[list[int]]) -> list[list[int]]:
        ans = []
        n, m = len(series1), len(series2)
        i = j = 0

        while i < n and j < m:
            t1, t2 = series1[i][0], series2[j][0]
            s = series1[i][1] + series2[j][1]
            if t1 < t2:
                ans.append([t1, s])
                i += 1
            elif t1 > t2:
                ans.append([t2, s])
                j += 1
            else:  # 相等
                ans.append([t1, s])
                i += 1
                j += 1

        ans += series1[i:]
        ans += series2[j:]
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> aggregateTimeSeries(int[][] series1, int[][] series2) {
        List<List<Integer>> ans = new ArrayList<>();
        int n = series1.length, m = series2.length;
        int i = 0, j = 0;

        while (i < n && j < m) {
            int t1 = series1[i][0], t2 = series2[j][0];
            int sum = series1[i][1] + series2[j][1];
            if (t1 < t2) {
                ans.add(List.of(t1, sum));
                i++;
            } else if (t1 > t2) {
                ans.add(List.of(t2, sum));
                j++;
            } else { // 相等
                ans.add(List.of(t1, sum));
                i++;
                j++;
            }
        }

        while (i < n) {
            ans.add(List.of(series1[i][0], series1[i][1]));
            i++;
        }
        while (j < m) {
            ans.add(List.of(series2[j][0], series2[j][1]));
            j++;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> aggregateTimeSeries(vector<vector<int>>& series1, vector<vector<int>>& series2) {
        vector<vector<int>> ans;
        int n = series1.size(), m = series2.size();
        int i = 0, j = 0;

        while (i < n && j < m) {
            int t1 = series1[i][0], t2 = series2[j][0];
            int sum = series1[i][1] + series2[j][1];
            if (t1 < t2) {
                ans.push_back({t1, sum});
                i++;
            } else if (t1 > t2) {
                ans.push_back({t2, sum});
                j++;
            } else { // 相等
                ans.push_back({t1, sum});
                i++;
                j++;
            }
        }

        ans.insert(ans.end(), series1.begin() + i, series1.end());
        ans.insert(ans.end(), series2.begin() + j, series2.end());
        return ans;
    }
};
```

```go [sol-Go]
func aggregateTimeSeries(series1, series2 [][]int) (ans [][]int) {
	n, m := len(series1), len(series2)
	i, j := 0, 0

	for i < n && j < m {
		t1, t2 := series1[i][0], series2[j][0]
		sum := series1[i][1] + series2[j][1]
		if t1 < t2 {
			ans = append(ans, []int{t1, sum})
			i++
		} else if t1 > t2 {
			ans = append(ans, []int{t2, sum})
			j++
		} else { // 相等
			ans = append(ans, []int{t1, sum})
			i++
			j++
		}
	}

	ans = append(ans, series1[i:]...)
	ans = append(ans, series2[j:]...)
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{series}_1$ 的长度，$m$ 是 $\textit{series}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 专题训练

见下面双指针题单的「**§4.1 双指针**」。

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
