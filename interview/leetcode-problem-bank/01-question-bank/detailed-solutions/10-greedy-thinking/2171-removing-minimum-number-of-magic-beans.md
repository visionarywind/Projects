# 2171. 拿出最少数目的魔法豆

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/removing-minimum-number-of-magic-beans/
- 题目 slug：`removing-minimum-number-of-magic-beans`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.6 先枚举，再贪心
- 难度分：1748
- 外部题解来源：https://leetcode.cn/problems/removing-minimum-number-of-magic-beans/solutions/1262419/pai-xu-hou-yi-ci-bian-li-by-endlesscheng-36g8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举最后袋子中魔法豆的数目（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/removing-minimum-number-of-magic-beans/solutions/1262419/pai-xu-hou-yi-ci-bian-li-by-endlesscheng-36g8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-hou-yi-ci-bian-li-by-endlesscheng-36g8`
- topic id：`1262419`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

我们可以将 $\textit{beans}$ 从小到大排序后，枚举最终非空袋子中魔法豆的数目 $v$，将小于 $v$ 的魔法豆全部清空，大于 $v$ 的魔法豆减少至 $v$，这样所有非空袋子中的魔法豆就都相等了。

由于拿出魔法豆 + 剩余魔法豆 = 初始魔法豆之和，我们可以考虑最多能剩下多少个魔法豆，从而计算出最少能拿出多少个魔法豆。

![](https://pic.leetcode.cn/1644881496-veNnxl-2171.drawio%20\(2\).png)

如上图所示，可以保留蓝色矩形区域内的魔法豆。设 $\textit{beans}$ 的长度为 $n$，以 $n-i$ 为矩形底边长，$v=\textit{beans}[i]$ 为矩形高，则矩形面积为

$$
(n-i) \cdot v
$$

用 $\sum\textit{beans}[i]$ 减去矩形面积的最大值，即为拿出魔法豆的最小值。

```py [sol-Python3]
class Solution:
    def minimumRemoval(self, beans: List[int]) -> int:
        beans.sort()
        n = len(beans)
        max_save = max((n - i) * v for i, v in enumerate(beans))
        return sum(beans) - max_save
```

```java [sol-Java]
class Solution {
    public long minimumRemoval(int[] beans) {
        Arrays.sort(beans);
        int n = beans.length;
        long sum = 0;
        long maxSave = 0;
        for (int i = 0; i < n; i++) {
            sum += beans[i];
            maxSave = Math.max(maxSave, (long) (n - i) * beans[i]);
        }
        return sum - maxSave;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minimumRemoval(vector<int>& beans) {
        ranges::sort(beans);
        long long sum = 0, max_save = 0;
        int n = beans.size();
        for (int i = 0; i < n; i++) {
            sum += beans[i];
            max_save = max(max_save, 1LL * (n - i) * beans[i]);
        }
        return sum - max_save;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int cmp(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

long long minimumRemoval(int* beans, int beansSize) {
    qsort(beans, beansSize, sizeof(int), cmp);
    long long sum = 0;
    long long max_save = 0;
    for (int i = 0; i < beansSize; i++) {
        sum += beans[i];
        max_save = MAX(max_save, 1LL * (beansSize - i) * beans[i]);
    }
    return sum - max_save;
}
```

```go [sol-Go]
func minimumRemoval(beans []int) int64 {
	slices.Sort(beans)
	sum, maxSave := 0, 0
	for i, v := range beans {
		sum += v
		maxSave = max(maxSave, (len(beans)-i)*v)
	}
	return int64(sum - maxSave)
}
```

```js [sol-JavaScript]
var minimumRemoval = function(beans) {
    beans.sort((a, b) => a - b);
    const n = beans.length;
    let sum = 0, maxSave = 0;
    for (let i = 0; i < n; i++) {
        sum += beans[i];
        maxSave = Math.max(maxSave, (n - i) * beans[i]);
    }
    return sum - maxSave;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_removal(mut beans: Vec<i32>) -> i64 {
        beans.sort_unstable();
        let mut sum = 0;
        let mut max_save = 0;
        let n = beans.len();
        for (i, v) in beans.into_iter().enumerate() {
            sum += v as i64;
            max_save = max_save.max((n - i) as i64 * v as i64);
        }
        sum - max_save
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{beans}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
