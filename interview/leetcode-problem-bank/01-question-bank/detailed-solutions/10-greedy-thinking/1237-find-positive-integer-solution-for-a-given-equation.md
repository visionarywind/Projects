# 1237. 找出给定方程的正整数解

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-positive-integer-solution-for-a-given-equation/
- 题目 slug：`find-positive-integer-solution-for-a-given-equation`
- 来源专题：贪心与思维
- 来源分类路径：七、交互题
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-positive-integer-solution-for-a-given-equation/solutions/2117698/xiang-xiang-shuang-zhi-zhen-yi-ge-shi-pi-nr4y/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[相向双指针，附题单（Python/Java/C++/Go）](https://leetcode.cn/problems/find-positive-integer-solution-for-a-given-equation/solutions/2117698/xiang-xiang-shuang-zhi-zhen-yi-ge-shi-pi-nr4y/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xiang-xiang-shuang-zhi-zhen-yi-ge-shi-pi-nr4y`
- topic id：`2117698`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

本题其实和 [15. 三数之和](https://leetcode.cn/problems/3sum/) 是类似的，都可以使用**相向双指针**解决，具体请看视频[【基础算法精讲 01】](https://www.bilibili.com/video/BV1bP411c7oJ/)

对于本题，由于 $f$ 是单调递增函数，我们可以从 $x=1,y=1000$ 开始，表示在横坐标为 $[x,1000]$ 以及纵坐标为 $[1,y]$ 的矩形范围内搜索答案。分类讨论：

1. 如果 $f(x,y) < z$，那对于任意 $y'<y$，都有 $f(x,y')<f(x,y)<z$，这说明固定 $x$ 枚举其余 $y$ 无法找到答案，那么将 $x$ 加一，缩小搜索范围。
2. 如果 $f(x,y) > z$，那对于任意 $x'>x$，都有 $f(x',y)>f(x,y)>z$，这说明固定 $y$ 枚举其余 $x$ 无法找到答案，那么将 $y$ 减一，缩小搜索范围。
3. 如果 $f(x,y) = z$，那么记录答案，同情况 1 一样，将 $x$ 加一。由于 $f(x+1,y)>f(x,y)=z$，根据情况 2，可以同时将 $y$ 减一。

不断循环直到 $x>1000$ 或 $y < 1$ 为止，此时搜索范围为空。

```py [sol1-Python3]
class Solution:
    def findSolution(self, customfunction: 'CustomFunction', z: int) -> List[List[int]]:
        ans = []
        x, y = 1, 1000
        while x <= 1000 and y:
            res = customfunction.f(x, y)
            if res < z:
                x += 1
            elif res > z:
                y -= 1
            else:
                ans.append([x, y])
                x += 1
                y -= 1
        return ans
```

```java [sol1-Java]
class Solution {
    public List<List<Integer>> findSolution(CustomFunction customfunction, int z) {
        var ans = new ArrayList<List<Integer>>();
        int x = 1, y = 1000;
        while (x <= 1000 && y > 0) {
            int res = customfunction.f(x, y);
            if (res < z) ++x;
            else if (res > z) --y;
            else ans.add(List.of(x++, y--));
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<vector<int>> findSolution(CustomFunction &customfunction, int z) {
        vector<vector<int>> ans;
        int x = 1, y = 1000;
        while (x <= 1000 && y) {
            int res = customfunction.f(x, y);
            if (res < z) ++x;
            else if (res > z) --y;
            else ans.push_back({x++, y--});
        }
        return ans;
    }
};
```

```go [sol1-Go]
func findSolution(customFunction func(int, int) int, z int) (ans [][]int) {
    x, y := 1, 1000
    for x <= 1000 && y > 0 {
        res := customFunction(x, y)
        if res < z {
            x++
        } else if res > z {
            y--
        } else {
            ans = append(ans, []int{x, y})
            x++
            y--
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$O(C)$，其中 $C=1000$。每次循环 `x++` 和 `y--` 至少会有一个发生，因此循环次数不会超过 $2C$。
- 空间复杂度：$O(1)$。不计入返回值，仅用到若干变量。

## 强化训练：相向双指针

- [167. 两数之和 II - 输入有序数组](https://leetcode.cn/problems/two-sum-ii-input-array-is-sorted/)
- [15. 三数之和](https://leetcode.cn/problems/3sum/)
- [16. 最接近的三数之和](https://leetcode.cn/problems/3sum-closest/)
- [18. 四数之和](https://leetcode.cn/problems/4sum/)
- [611. 有效三角形的个数](https://leetcode.com/problems/valid-triangle-number/)

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
