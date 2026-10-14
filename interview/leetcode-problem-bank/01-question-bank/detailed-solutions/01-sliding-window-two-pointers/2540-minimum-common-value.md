# 2540. 最小公共值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-common-value/
- 题目 slug：`minimum-common-value`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.1 双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/minimum-common-value/solutions/2073029/by-endlesscheng-f6bd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[双指针（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-common-value/solutions/2073029/by-endlesscheng-f6bd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-f6bd`
- topic id：`2073029`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 $n$ 是 $\textit{nums}_1$ 的长度，$m$ 是 $\textit{nums}_2$ 的长度。

找最小公共值，首先关注两个数组的最小值，也就是比较 $x = \textit{nums}_1[0]$ 和 $y = \textit{nums}_2[0]$ 的大小：

- 如果 $x = y$，由于 $\textit{nums}_1$ 和 $\textit{nums}_2$ 都是递增的，所以 $x$ 是最小的公共值。
- 如果 $x < y$，那么 $x$ 也小于 $\textit{nums}_2$ 的其余元素，所以 $x$ 不可能是公共值，排除，问题变成 $\textit{nums}_1$ 的后缀 $[1,n-1]$ 和 $\textit{nums}_2$ 的最小公共值。对于这个子问题，做法是一样的，比较 $\textit{nums}_1[1]$ 和 $\textit{nums}_2[0]$ 的大小。
- 如果 $x > y$，那么 $y$ 也小于 $\textit{nums}_1$ 的其余元素，所以 $y$ 不可能是公共值，排除，问题变成 $\textit{nums}_1$ 和 $\textit{nums}_2$ 的后缀 $[1,m-1]$ 的最小公共值。对于这个子问题，做法是一样的，比较 $\textit{nums}_1[0]$ 和 $\textit{nums}_2[1]$ 的大小。

用两个下标 $i$ 和 $j$ 表示当前要比较 $\textit{nums}_1[i]$ 和 $\textit{nums}_2[j]$ 的大小。

```py [sol-Python3]
class Solution:
    def getCommon(self, nums1: List[int], nums2: List[int]) -> int:
        i, n = 0, len(nums1)
        j, m = 0, len(nums2)
        while i < n and j < m:
            if nums1[i] == nums2[j]:
                return nums1[i]
            if nums1[i] < nums2[j]:
                i += 1
            else:
                j += 1
        return -1
```

```py [sol-Python3 写法二]
class Solution:
    def getCommon(self, nums1: List[int], nums2: List[int]) -> int:
        return min(set(nums1) & set(nums2), default=-1)
```

```java [sol-Java]
class Solution {
    public int getCommon(int[] nums1, int[] nums2) {
        int i = 0, n = nums1.length;
        int j = 0, m = nums2.length;
        while (i < n && j < m) {
            if (nums1[i] == nums2[j]) {
                return nums1[i];
            }
            if (nums1[i] < nums2[j]) {
                i++;
            } else {
                j++;
            }
        }
        return -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int getCommon(vector<int>& nums1, vector<int>& nums2) {
        int i = 0, n = nums1.size();
        int j = 0, m = nums2.size();
        while (i < n && j < m) {
            if (nums1[i] == nums2[j]) {
                return nums1[i];
            }
            if (nums1[i] < nums2[j]) {
                i++;
            } else {
                j++;
            }
        }
        return -1;
    }
};
```

```c [sol-C]
int getCommon(int* nums1, int nums1Size, int* nums2, int nums2Size) {
    int i = 0, j = 0;
    while (i < nums1Size && j < nums2Size) {
        if (nums1[i] == nums2[j]) {
            return nums1[i];
        }
        if (nums1[i] < nums2[j]) {
            i++;
        } else {
            j++;
        }
    }
    return -1;
}
```

```go [sol-Go]
func getCommon(nums1, nums2 []int) int {
	i, n := 0, len(nums1)
	j, m := 0, len(nums2)
	for i < n && j < m {
		if nums1[i] == nums2[j] {
			return nums1[i]
		}
		if nums1[i] < nums2[j] {
			i++
		} else {
			j++
		}
	}
	return -1
}
```

```js [sol-JavaScript]
var getCommon = function(nums1, nums2) {
    let i = 0, n = nums1.length;
    let j = 0, m = nums2.length;
    while (i < n && j < m) {
        if (nums1[i] === nums2[j]) {
            return nums1[i];
        }
        if (nums1[i] < nums2[j]) {
            i++;
        } else {
            j++;
        }
    }
    return -1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn get_common(nums1: Vec<i32>, nums2: Vec<i32>) -> i32 {
        let n = nums1.len();
        let m = nums2.len();
        let mut i = 0;
        let mut j = 0;
        while i < n && j < m {
            if nums1[i] == nums2[j] {
                return nums1[i];
            }
            if nums1[i] < nums2[j] {
                i += 1;
            } else {
                j += 1;
            }
        }
        -1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{nums}_1$ 的长度，$m$ 是 $\textit{nums}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [350. 两个数组的交集 II](https://leetcode.cn/problems/intersection-of-two-arrays-ii/)
- [88. 合并两个有序数组](https://leetcode.cn/problems/merge-sorted-array/)

## 专题训练

见下面双指针题单的「**四、双序列双指针**」。

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
