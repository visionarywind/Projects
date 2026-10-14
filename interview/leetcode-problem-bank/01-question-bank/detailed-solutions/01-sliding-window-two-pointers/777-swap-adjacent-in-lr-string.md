# 777. 在 LR 字符串中交换相邻字符

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/swap-adjacent-in-lr-string/
- 题目 slug：`swap-adjacent-in-lr-string`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.1 双指针
- 难度分：2337
- 外部题解来源：https://leetcode.cn/problems/swap-adjacent-in-lr-string/solutions/1659769/by-endlesscheng-fcgc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯（Python/Java/C++/Go）](https://leetcode.cn/problems/swap-adjacent-in-lr-string/solutions/1659769/by-endlesscheng-fcgc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-fcgc`
- topic id：`1659769`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1aU4y1q7BA)（和本题是一样的）已出炉，欢迎点赞关注~

替换操作可以转换成，把 `L` 往左移动（需要左边是 `X`），把 `R` 往右移动（需要右边是 `X`）。

那么无论怎么移动，由于 `L` 和 `R` 无法互相穿过对方，那么去掉 `X` 后的剩余字符应该是相同的，否则返回 `false`。

然后用双指针遍历 $\textit{start}[i]$ 和 $\textit{target}[j]$，分类讨论：

- 如果当前字符为 `L` 且 $i<j$，那么这个 `L` 由于无法向右移动，返回 `false`；
- 如果当前字符为 `R` 且 $i>j$，那么这个 `R` 由于无法向左移动，返回 `false`。

遍历完，若中途没有返回 `false` 就返回 `true`。

```py [sol-Python3]
class Solution:
    def canTransform(self, start: str, target: str) -> bool:
        if start.replace('X', '') != target.replace('X', ''):
            return False
        j = 0
        for i, c in enumerate(start):
            if c == 'X':
                continue
            while target[j] == 'X':
                j += 1
            if i != j and (c == 'L') != (i > j):
                return False
            j += 1
        return True
```

```java [sol-Java]
class Solution {
    public boolean canTransform(String start, String target) {
        if (!start.replace("X", "").equals(target.replace("X", ""))) {
            return false;
        }
        for (int i = 0, j = 0; i < start.length(); i++) {
            if (start.charAt(i) == 'X') {
                continue;
            }
            while (target.charAt(j) == 'X') {
                j++;
            }
            if (i != j && (start.charAt(i) == 'L') != (i > j)) {
                return false;
            }
            j++;
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canTransform(string start, string target) {
        string s = start, t = target;
        s.erase(remove(s.begin(), s.end(), 'X'), s.end());
        t.erase(remove(t.begin(), t.end(), 'X'), t.end());
        if (s != t) {
            return false;
        }
        for (int i = 0, j = 0; i < start.length(); i++) {
            if (start[i] == 'X') {
                continue;
            }
            while (target[j] == 'X') {
                j++;
            }
            if (i != j && (start[i] == 'L') != (i > j)) {
                return false;
            }
            j++;
        }
        return true;
    }
};
```

```go [sol-Go]
func canTransform(start, target string) bool {
	if strings.ReplaceAll(start, "X", "") != strings.ReplaceAll(target, "X", "") {
		return false
	}
	j := 0
	for i, c := range start {
		if c != 'X' {
			for target[j] == 'X' {
				j++
			}
			if i != j && c == 'L' != (i > j) {
				return false
			}
			j++
		}
	}
	return true
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{start}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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
