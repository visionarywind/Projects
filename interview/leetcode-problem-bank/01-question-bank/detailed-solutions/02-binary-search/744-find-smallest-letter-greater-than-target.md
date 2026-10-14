# 744. 寻找比目标字母大的最小字母

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-smallest-letter-greater-than-target/
- 题目 slug：`find-smallest-letter-greater-than-target`
- 来源专题：二分算法
- 来源分类路径：一、二分查找 / §1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-smallest-letter-greater-than-target/solutions/3892869/mo-ban-er-fen-cha-zhao-pythonjavacgo-by-29zwb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】二分查找（Python/Java/C++/Go）](https://leetcode.cn/problems/find-smallest-letter-greater-than-target/solutions/3892869/mo-ban-er-fen-cha-zhao-pythonjavacgo-by-29zwb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-er-fen-cha-zhao-pythonjavacgo-by-29zwb`
- topic id：`3892869`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**视频讲解**：[二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)，包含闭区间、半闭半开、开区间三种写法。

**相关题目**：[34. 在排序数组中查找元素的第一个和最后一个位置](https://leetcode.cn/problems/find-first-and-last-position-of-element-in-sorted-array/)，推荐阅读 [题解](https://leetcode.cn/problems/find-first-and-last-position-of-element-in-sorted-array/solutions/1980196/er-fen-cha-zhao-zong-shi-xie-bu-dui-yi-g-t9l9/) 中的答疑。

## 手写二分

推荐写开区间二分，简单好记。

```py [sol-Python3]
class Solution:
    def nextGreatestLetter(self, letters: List[str], target: str) -> str:
        n = len(letters)
        left, right = -1, n
        while left + 1 < right:
            mid = (left + right) // 2
            if letters[mid] > target:
                right = mid
            else:
                left = mid
        return letters[right] if right < n else letters[0]
```

```java [sol-Java]
class Solution {
    public char nextGreatestLetter(char[] letters, char target) {
        int n = letters.length;
        int left = -1;
        int right = n;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (letters[mid] > target) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right < n ? letters[right] : letters[0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    char nextGreatestLetter(vector<char>& letters, char target) {
        int n = letters.size();
        int left = -1, right = n;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (letters[mid] > target ? right : left) = mid;
        }
        return right < n ? letters[right] : letters[0];
    }
};
```

```go [sol-Go]
func nextGreatestLetter(letters []byte, target byte) byte {
	n := len(letters)
	left, right := -1, n
	for left+1 < right {
		mid := left + (right-left)/2
		if letters[mid] > target {
			right = mid
		} else {
			left = mid
		}
	}
	if right == n {
		return letters[0]
	}
	return letters[right]
}
```

## 库函数写法

```py [sol-Python3]
class Solution:
    def nextGreatestLetter(self, letters: List[str], target: str) -> str:
        i = bisect_right(letters, target)
        return letters[i] if i < len(letters) else letters[0]
```

```java [sol-Java]
class Solution {
    public char nextGreatestLetter(char[] letters, char target) {
        int i = Arrays.binarySearch(letters, (char) (target + 1));
        if (i < 0) { // letters 中没有 target + 1
            i = ~i; // 根据 Arrays.binarySearch 的文档，~i 是第一个大于 target 的字母的下标
        }
        return i < letters.length ? letters[i] : letters[0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    char nextGreatestLetter(vector<char>& letters, char target) {
        auto it = ranges::upper_bound(letters, target);
        return it < letters.end() ? *it : letters[0];
    }
};
```

```go [sol-Go]
func nextGreatestLetter(letters []byte, target byte) byte {
	i, _ := slices.BinarySearch(letters, target+1)
	if i == len(letters) {
		return letters[0]
	}
	return letters[i]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 是 $\textit{letters}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 附：二分查找常用转化

| **需求**  | **写法**  |  **如果不存在** | 
|---|---|---|
| $\ge x$ 的第一个元素的下标  | $\texttt{lowerBound}(\textit{nums},x)$  | 结果为 $n$ |
| $> x$ 的第一个元素的下标 | $\texttt{lowerBound}(\textit{nums},x+1)$  | 结果为 $n$ |
| $< x$ 的最后一个元素的下标  | $\texttt{lowerBound}(\textit{nums},x)-1$  | 结果为 $-1$ |
| $\le x$ 的最后一个元素的下标  | $\texttt{lowerBound}(\textit{nums},x+1)-1$  | 结果为 $-1$ |

| **需求**  | **写法**  |
|---|---|
| $< x$ 的元素个数  | $\texttt{lowerBound}(\textit{nums},x)$  | 
| $\le x$ 的元素个数 | $\texttt{lowerBound}(\textit{nums},x+1)$  | 
| $\ge x$ 的元素个数  | $n - \texttt{lowerBound}(\textit{nums},x)$  | 
| $> x$ 的元素个数  | $n - \texttt{lowerBound}(\textit{nums},x+1)$  | 

注意 $< x$ 和 $\ge x$ 互为补集，元素个数之和为 $n$。$\le x$ 和 $> x$ 同理。

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
