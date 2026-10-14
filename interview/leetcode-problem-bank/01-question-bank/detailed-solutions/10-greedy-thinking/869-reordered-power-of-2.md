# 869. 重新排序得到 2 的幂

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reordered-power-of-2/
- 题目 slug：`reordered-power-of-2`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.4 逆向思维
- 难度分：1505
- 外部题解来源：https://leetcode.cn/problems/reordered-power-of-2/solutions/3748346/ni-xiang-si-wei-pythonjavacgo-by-endless-ehey/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维（Python/Java/C++/Go）](https://leetcode.cn/problems/reordered-power-of-2/solutions/3748346/ni-xiang-si-wei-pythonjavacgo-by-endless-ehey/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ni-xiang-si-wei-pythonjavacgo-by-endless-ehey`
- topic id：`3748346`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

**题意**：把 $n$ 视作一个十进制字符串，判断 $n$ 能否重排为 $1,2,4,8,16,32,\dots$ 这种 $2$ 的幂。不能有前导零。

换句话说，$n$ 和某个 $2^k$ 是**字母异位词**，见 [49. 字母异位词分组](https://leetcode.cn/problems/group-anagrams/)。如果 $n$ 的十进制字符串排序后的字符串，等于某个 $2^k$ 的十进制字符串排序后的字符串，那么返回 $\texttt{true}$。否则返回 $\texttt{false}$。

对于本题的数据范围，我们可以预处理 $[2^0,2^{29}]$ 中的所有 $2$ 的幂的十进制字符串排序后的字符串，保存到一个哈希集合中。预处理后，只需判断 $n$ 的十进制字符串排序后的字符串是否在哈希集合中。

> **注 1**：也可以用长为 $10$ 的数组统计每个数位的出现次数，避免排序。
> 
> **注 2**：也可以做到与 $10^9$ 无关。由于十进制长度与 $n$ 相同的 $2$ 的幂至多只有 $4$ 种（比如最高位为 $1,2,4,8$），所以至多枚举 $4$ 个 $2$ 的幂，看其排序后能否与 $n$ 排序后的结果相同（或者统计数位出现次数）。

```py [sol-Python3]
pow_two_sorted_str_set = {''.join(sorted(str(1 << i))) for i in range(30)}

class Solution:
    def reorderedPowerOf2(self, n: int) -> bool:
        s = ''.join(sorted(str(n)))
        return s in pow_two_sorted_str_set
```

```java [sol-Java]
class Solution {
    private static final Set<String> powTwoSortedStrSet = new HashSet<>();

    static {
        final int MAX_N = 1_000_000_000;
        for (int i = 1; i < MAX_N; i <<= 1) {
            String s = intToSortedStr(i);
            powTwoSortedStrSet.add(s);
        }
    }

    private static String intToSortedStr(int n) {
        char[] s = String.valueOf(n).toCharArray();
        Arrays.sort(s);
        return new String(s);
    }

    public boolean reorderedPowerOf2(int n) {
        String s = intToSortedStr(n);
        return powTwoSortedStrSet.contains(s);
    }
}
```

```cpp [sol-C++]
string int_to_sorted_str(int n) {
    string s = to_string(n);
    ranges::sort(s);
    return s;
}

unordered_set<string> pow_two_sorted_str_set;

int init = []() {
    const int MAX_N = 1'000'000'000;
    for (int i = 1; i < MAX_N; i <<= 1) {
        string s = int_to_sorted_str(i);
        pow_two_sorted_str_set.insert(s);
    }
    return 0;
}();

class Solution {
public:
    bool reorderedPowerOf2(int n) {
        string s = int_to_sorted_str(n);
        return pow_two_sorted_str_set.contains(s);
    }
};
```

```go [sol-Go]
var powTwoSortedStrSet = map[string]bool{}

func init() {
    const maxN = 1_000_000_000
    for i := 1; i < maxN; i <<= 1 {
        s := intToSortedStr(i)
        powTwoSortedStrSet[s] = true
    }
}

func intToSortedStr(n int) string {
    s := []byte(strconv.Itoa(n))
    slices.Sort(s)
    return string(s)
}

func reorderedPowerOf2(n int) bool {
    s := intToSortedStr(n)
    return powTwoSortedStrSet[s]
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(M\log M)$，其中 $M=\log n$ 是 $n$ 的十进制长度。也可以用长为 $C=10$ 的数组统计每个数位的出现次数，避免排序。
- 空间复杂度：$\mathcal{O}(M)$。

## 专题训练

见下面贪心与思维题单的「**§5.3 逆向思维**」。

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
