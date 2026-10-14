# 2957. 消除相邻近似相等字符

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-adjacent-almost-equal-characters/
- 题目 slug：`remove-adjacent-almost-equal-characters`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1430
- 外部题解来源：https://leetcode.cn/problems/remove-adjacent-almost-equal-characters/solutions/2560730/tan-xin-pythonjavacgo-by-endlesscheng-2ub2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/remove-adjacent-almost-equal-characters/solutions/2560730/tan-xin-pythonjavacgo-by-endlesscheng-2ub2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavacgo-by-endlesscheng-2ub2`
- topic id：`2560730`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

[本题视频讲解](https://www.bilibili.com/video/BV1dC4y1X7PE/)

下文把 $\textit{word}$ 简记为 $s$。

从左到右遍历 $s$，如果发现 $s[i-1]$ 和 $s[i]$ 近似相等，应当改 $s[i-1]$ 还是 $s[i]$？

如果改 $s[i-1]$，那么 $s[i]$ 和 $s[i+1]$ 是可能近似相等的，但如果改 $s[i]$，就可以避免 $s[i]$ 和 $s[i+1]$ 近似相等。

所以每次发现两个相邻字母近似相等，就改右边那个。

```py [sol-Python3]
class Solution:
    def removeAlmostEqualCharacters(self, s: str) -> int:
        ans = 0
        i, n = 1, len(s)
        while i < n:
            if abs(ord(s[i - 1]) - ord(s[i])) <= 1:
                ans += 1
                i += 2
            else:
                i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int removeAlmostEqualCharacters(String s) {
        int ans = 0;
        for (int i = 1; i < s.length(); i++) {
            if (Math.abs(s.charAt(i - 1) - s.charAt(i)) <= 1) {
                ans++;
                i++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int removeAlmostEqualCharacters(string s) {
        int ans = 0;
        for (int i = 1; i < s.length(); i++) {
            if (abs(s[i - 1] - s[i]) <= 1) {
                ans++;
                i++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func removeAlmostEqualCharacters(s string) (ans int) {
	for i := 1; i < len(s); i++ {
		if abs(int(s[i-1])-int(s[i])) <= 1 {
			ans++
			i++
		}
	}
	return
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [2560. 打家劫舍 IV](https://leetcode.cn/problems/house-robber-iv/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
