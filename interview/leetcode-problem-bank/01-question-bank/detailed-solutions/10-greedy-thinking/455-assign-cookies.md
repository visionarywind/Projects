# 455. 分发饼干

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/assign-cookies/
- 题目 slug：`assign-cookies`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.3 双序列配对
- 难度分：1381
- 外部题解来源：https://leetcode.cn/problems/assign-cookies/solutions/2974809/pai-xu-shuang-zhi-zhen-jian-ji-xie-fa-py-ttn8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序+双指针，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/assign-cookies/solutions/2974809/pai-xu-shuang-zhi-zhen-jian-ji-xie-fa-py-ttn8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-shuang-zhi-zhen-jian-ji-xie-fa-py-ttn8`
- topic id：`2974809`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

## 思路

想一想，尺寸最小的饼干，应该给哪个孩子？

- 如果这个饼干的尺寸 $x$ 比最小的 $g[i]$ 还小，那么无法给任何孩子。
- 否则，这个饼干应该给最小的 $g[i]$。如果不给，那万一只有这个孩子胃口小，其他孩子胃口大，我们就浪费了一次分发饼干的机会。或者说，我们相当于在孩子和饼干之间找到一个最优匹配，如果出现 $x$ 匹配胃口大的孩子，另一个比 $x$ 更大的尺寸 $y$ 匹配胃口最小的孩子，那么交换这两个匹配，匹配仍然是成立的，所以总是可以使 $x$ 匹配胃口最小的孩子。

发完这个饼干后，继续考虑尺寸次小的饼干，思考方式同上。

## 算法

1. 把 $g$ 和 $s$ 从小到大排序。
2. 遍历饼干大小 $s$。同时维护另一个指针 $i=0$ 表示胃口。
3. 设 $g$ 的长度为 $n$，如果 $i < n$ 且 $g[i]\le x$，那么把饼干分给这个孩子，$i$ 增加 $1$。
4. $i$ 增加的次数就是得到饼干的孩子个数，所以最后返回 $i$。

```py [sol-Python3]
class Solution:
    def findContentChildren(self, g: List[int], s: List[int]) -> int:
        g.sort()
        s.sort()
        n = len(g)
        i = 0
        for x in s:
            if i < n and g[i] <= x:
                i += 1
        return i
```

```java [sol-Java]
class Solution {
    int findContentChildren(int[] g, int[] s) {
        Arrays.sort(g);
        Arrays.sort(s);
        int i = 0;
        for (int x : s) {
            if (i < g.length && g[i] <= x) {
                i++;
            }
        }
        return i;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findContentChildren(vector<int>& g, vector<int>& s) {
        ranges::sort(g);
        ranges::sort(s);
        int i = 0;
        for (int x : s) {
            if (i < g.size() && g[i] <= x) {
                i++;
            }
        }
        return i;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int findContentChildren(int* g, int gSize, int* s, int sSize) {
    qsort(g, gSize, sizeof(int), cmp);
    qsort(s, sSize, sizeof(int), cmp);
    int i = 0;
    for (int j = 0; j < sSize; j++) {
        if (i < gSize && g[i] <= s[j]) {
            i++;
        }
    }
    return i;
}
```

```go [sol-Go]
func findContentChildren(g, s []int) int {
    slices.Sort(g)
    slices.Sort(s)
    i := 0
    for _, x := range s {
        if i < len(g) && g[i] <= x {
            i++
        }
    }
    return i
}
```

```js [sol-JavaScript]
var findContentChildren = function(g, s) {
    g.sort((a, b) => a - b);
    s.sort((a, b) => a - b);
    let i = 0;
    for (const x of s) {
        if (i < g.length && g[i] <= x) {
            i++;
        }
    }
    return i;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_content_children(mut g: Vec<i32>, mut s: Vec<i32>) -> i32 {
        g.sort_unstable();
        s.sort_unstable();
        let mut i = 0;
        for x in s {
            if i < g.len() && g[i] <= x {
                i += 1;
            }
        }
        i as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + m\log m)$，其中 $n$ 是 $g$ 的长度，$m$ 是 $s$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

更多相似题目，见下面贪心题单中的「**§1.3 双序列配对**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
