# 2745. 构造最长的新字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/construct-the-longest-new-string/
- 题目 slug：`construct-the-longest-new-string`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.1 从特殊到一般
- 难度分：1607
- 外部题解来源：https://leetcode.cn/problems/construct-the-longest-new-string/solutions/2319365/liang-chong-fang-fa-o1-gong-shi-ji-yi-hu-7fdi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：O(1) 公式 / 记忆化搜索（Python/Java/C++/Go）](https://leetcode.cn/problems/construct-the-longest-new-string/solutions/2319365/liang-chong-fang-fa-o1-gong-shi-ji-yi-hu-7fdi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-o1-gong-shi-ji-yi-hu-7fdi`
- topic id：`2319365`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

[视频讲解](https://www.bilibili.com/video/BV1am4y1a7Zi/) 第二题

如果没有 AB，那么 AA 和 BB 只能交替连接，答案为

$$
(\min(x,y)\cdot 2 + [x\ne y])\cdot 2
$$

如果有 AB，它可以与自身连接，且只能插在 BB 和 AA 之间，即 BB + (ABABAB...) + AA。

> 或者接在后缀 BB 之后，或者加到前缀 AA 之前。

所以 AB 不会改变 AA 和 BB 交替连接的上限。

所以答案为

$$
(\min(x,y)\cdot 2 + [x\ne y] + z)\cdot 2
$$

```py [sol-Python3]
class Solution:
    def longestString(self, x: int, y: int, z: int) -> int:
        return (min(x, y) * 2 + (x != y) + z) * 2
```

```java [sol-Java]
class Solution {
    public int longestString(int x, int y, int z) {
        return (Math.min(x, y) * 2 + (x != y ? 1 : 0) + z) * 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestString(int x, int y, int z) {
        return (min(x, y) * 2 + (x != y) + z) * 2;
    }
};
```

```go [sol-Go]
func longestString(x, y, z int) int {
	ans := min(x, y) * 2
	if x != y {
		ans++
	}
	return (ans + z) * 2
}

func min(a, b int) int { if b < a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 附：记忆化搜索

定义 $\textit{dfs}(x,y,z,k)$，其中 $x,y,z$ 为 AA/BB/AB 的剩余数量，$k=0,1,2$ 表示上一个字符串是 AA/BB/AB，此时可以构造出的字符串的最大长度。

类似 [状态机 DP](https://www.bilibili.com/video/BV1ho4y1W7QK/)，分类讨论：

- AA 后面只能接 BB；
- BB 后面可以接 AA 或 AB；
- AB 后面可以接 AA 或 AB。

```py [sol-Python3]
@cache
def dfs(x: int, y: int, z: int, k: int) -> int:
    if k == 0:
        return dfs(x, y - 1, z, 1) + 2 if y else 0
    res1 = dfs(x - 1, y, z, 0) + 2 if x else 0
    res2 = dfs(x, y, z - 1, 2) + 2 if z else 0
    return max(res1, res2)

class Solution:
    def longestString(self, x: int, y: int, z: int) -> int:
        return max(dfs(x, y, z, 0), dfs(x, y, z, 1))
```

```go [sol-Go]
func longestString(x, y, z int) int {
	memo := make([][][][3]int, x+1)
	for i := range memo {
		memo[i] = make([][][3]int, y+1)
		for j := range memo[i] {
			memo[i][j] = make([][3]int, z+1)
			for k := range memo[i][j] {
				memo[i][j][k] = [3]int{-1, -1, -1}
			}
		}
	}
	var dfs func(x, y, z, k int) int
	dfs = func(x, y, z, k int) (res int) {
		p := &memo[x][y][z][k]
		if *p != -1 { // 之前算过
			return *p
		}
		if k == 0 {
			if y > 0 {
				res = dfs(x, y-1, z, 1) + 2
			}
		} else {
			if x > 0 {
				res = dfs(x-1, y, z, 0) + 2
			}
			if z > 0 {
				res = max(res, dfs(x, y, z-1, 2)+2)
			}
		}
		*p = res // 记忆化
		return
	}
	return max(dfs(x, y, z, 0), dfs(x, y, z, 1))
}

func max(a, b int) int { if b > a { return b }; return a }
```
