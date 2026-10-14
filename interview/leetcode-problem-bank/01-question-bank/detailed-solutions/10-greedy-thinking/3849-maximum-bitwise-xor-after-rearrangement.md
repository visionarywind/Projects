# 3849. 重新排列后的最大按位异或值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-bitwise-xor-after-rearrangement/
- 题目 slug：`maximum-bitwise-xor-after-rearrangement`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1556
- 外部题解来源：https://leetcode.cn/problems/maximum-bitwise-xor-after-rearrangement/solutions/3906204/cong-zuo-dao-you-tan-xin-jian-ji-xie-fa-ced9g/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从左到右贪心，简洁写法（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-bitwise-xor-after-rearrangement/solutions/3906204/cong-zuo-dao-you-tan-xin-jian-ji-xie-fa-ced9g/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-zuo-dao-you-tan-xin-jian-ji-xie-fa-ced9g`
- topic id：`3906204`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

为了让异或结果尽量大，我们要优先满足异或结果的**高位**是 $1$。

从左到右遍历 $s$：

- 如果 $s[i] = 0$，那么需要把 $t$ 中的一个 $1$ 换到 $t[i]$，这样 $i$ 处的异或结果是 $1$。否则，如果 $t$ 没有剩下的 $1$，那么只能 $0$ 和 $0$ 异或，结果是 $0$。
- 如果 $s[i] = 1$，那么需要把 $t$ 中的一个 $0$ 换到 $t[i]$，这样 $i$ 处的异或结果是 $1$。否则，如果 $t$ 没有剩下的 $0$，那么只能 $1$ 和 $1$ 异或，结果是 $0$。

这两种情况可以合二为一，请看代码。

[本题视频讲解](https://www.bilibili.com/video/BV1HeZfB7EBt/?t=16m41s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maximumXor(self, s: str, t: str) -> str:
        cnt0 = t.count('0')
        left = [cnt0, len(t) - cnt0]  # t 中剩余的 0 和 1 的个数

        ans = list(s)
        for i, ch in enumerate(ans):
            x = int(ch)
            # 如果 x 是 0，那就看还有没有剩下的 1
            # 如果 x 是 1，那就看还有没有剩下的 0
            if left[x ^ 1] > 0:
                left[x ^ 1] -= 1
                ans[i] = '1'  # x ^ (x ^ 1) = 1
            else:  # 只能让两个相同的数异或
                left[x] -= 1
                ans[i] = '0'  # x ^ x = 0
        return ''.join(ans)
```

```java [sol-Java]
class Solution {
    public String maximumXor(String s, String t) {
        int cnt1 = 0;
        for (char ch : t.toCharArray()) {
            cnt1 += ch - '0';
        }
        int[] left = new int[]{t.length() - cnt1, cnt1}; // t 中剩余的 0 和 1 的个数

        char[] ans = s.toCharArray();
        for (int i = 0; i < ans.length; i++) {
            int x = ans[i] - '0';
            // 如果 x 是 0，那就看还有没有剩下的 1
            // 如果 x 是 1，那就看还有没有剩下的 0
            if (left[x ^ 1] > 0) {
                left[x ^ 1]--;
                ans[i] = '1'; // x ^ (x ^ 1) = 1
            } else { // 只能让两个相同的数异或
                left[x]--;
                ans[i] = '0'; // x ^ x = 0
            }
        }
        return new String(ans);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string maximumXor(string s, string t) {
        int n = t.size();
        int cnt1 = 0;
        for (char ch : t) {
            cnt1 += ch - '0';
        }
        int left[2] = {n - cnt1, cnt1}; // t 中剩余的 0 和 1 的个数

        for (char& ch : s) {
            int x = ch - '0';
            // 如果 x 是 0，那就看还有没有剩下的 1
            // 如果 x 是 1，那就看还有没有剩下的 0
            if (left[x ^ 1] > 0) {
                left[x ^ 1]--;
                ch = '1'; // x ^ (x ^ 1) = 1
            } else { // 只能让两个相同的数异或
                left[x]--;
                ch = '0'; // x ^ x = 0
            }
        }
        return s;
    }
};
```

```go [sol-Go]
func maximumXor(s, t string) string {
	cnt0 := strings.Count(t, "0")
	left := [2]int{cnt0, len(t) - cnt0} // t 中剩余的 0 和 1 的个数

	ans := []byte(s)
	for i, ch := range ans {
		x := int(ch - '0')
		// 如果 x 是 0，那就看还有没有剩下的 1
		// 如果 x 是 1，那就看还有没有剩下的 0
		if left[x^1] > 0 {
			left[x^1]--
			ans[i] = '1' // x ^ (x^1) = 1
		} else { // 只能让两个相同的数异或
			left[x]--
			ans[i] = '0' // x ^ x = 0
		}
	}
	return string(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$，取决于能否原地修改 $s$。

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
