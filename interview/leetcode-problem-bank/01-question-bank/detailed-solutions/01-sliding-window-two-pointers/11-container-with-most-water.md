# 11. 盛最多水的容器

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/container-with-most-water/
- 题目 slug：`container-with-most-water`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/container-with-most-water/solutions/1974355/by-endlesscheng-f0xz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频讲解】双指针做法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/container-with-most-water/solutions/1974355/by-endlesscheng-f0xz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-f0xz`
- topic id：`1974355`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看[【基础算法精讲 02】](https://www.bilibili.com/video/BV1Qg411q7ia/)，制作不易，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxArea(self, height: List[int]) -> int:
        ans = left = 0
        right = len(height) - 1
        while left < right:
            area = (right - left) * min(height[left], height[right])
            ans = max(ans, area)
            if height[left] < height[right]:
                # height[left] 与右边的任意垂线都无法组成一个比 ans 更大的面积
                left += 1
            else:
                # height[right] 与左边的任意垂线都无法组成一个比 ans 更大的面积
                right -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int maxArea(int[] height) {
        int ans = 0;
        int left = 0;
        int right = height.length - 1;
        while (left < right) {
            int area = (right - left) * Math.min(height[left], height[right]);
            ans = Math.max(ans, area);
            if (height[left] < height[right]) {
                // height[left] 与右边的任意垂线都无法组成一个比 ans 更大的面积
                left++;
            } else {
                // height[right] 与左边的任意垂线都无法组成一个比 ans 更大的面积
                right--;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxArea(vector<int>& height) {
        int ans = 0, left = 0, right = height.size() - 1;
        while (left < right) {
            int area = (right - left) * min(height[left], height[right]);
            ans = max(ans, area);
            // 如果 height[left] 更矮，则 height[left] 与右边的任意垂线都无法组成一个比 ans 更大的面积
            // 如果 height[right] 更矮，则 height[right] 与左边的任意垂线都无法组成一个比 ans 更大的面积
            height[left] < height[right] ? left++ : right--;
        }
        return ans;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxArea(int* height, int heightSize) {
    int ans = 0, left = 0, right = heightSize - 1;
    while (left < right) {
        int area = (right - left) * MIN(height[left], height[right]);
        ans = MAX(ans, area);
        // 如果 height[left] 更矮，则 height[left] 与右边的任意垂线都无法组成一个比 ans 更大的面积
        // 如果 height[right] 更矮，则 height[right] 与左边的任意垂线都无法组成一个比 ans 更大的面积
        height[left] < height[right] ? left++ : right--;
    }
    return ans;
}
```

```go [sol-Go]
func maxArea(height []int) (ans int) {
    left, right := 0, len(height)-1
    for left < right {
        area := (right - left) * min(height[left], height[right])
        ans = max(ans, area)
        if height[left] < height[right] {
            // height[left] 与右边的任意垂线都无法组成一个比 ans 更大的面积
            left++
        } else {
            // height[right] 与左边的任意垂线都无法组成一个比 ans 更大的面积
            right--
        }
    }
    return
}
```

```js [sol-JavaScript]
var maxArea = function(height) {
    let ans = 0, left = 0, right = height.length - 1;
    while (left < right) {
        const area = (right - left) * Math.min(height[left], height[right]);
        ans = Math.max(ans, area);
        if (height[left] < height[right]) {
            // height[left] 与右边的任意垂线都无法组成一个比 ans 更大的面积
            left++;
        } else {
            // height[right] 与左边的任意垂线都无法组成一个比 ans 更大的面积
            right--;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_area(height: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut left = 0;
        let mut right = height.len() - 1;
        while left < right {
            let area = (right - left) as i32 * height[left].min(height[right]);
            ans = ans.max(area);
            if height[left] < height[right] {
                // height[left] 与右边的任意垂线都无法组成一个比 ans 更大的面积
                left += 1;
            } else {
                // height[right] 与左边的任意垂线都无法组成一个比 ans 更大的面积
                right -= 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{height}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

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
