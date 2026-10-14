# 3926. 有效单词计数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-valid-word-occurrences/
- 题目 slug：`count-valid-word-occurrences`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1608
- 外部题解来源：https://leetcode.cn/problems/count-valid-word-occurrences/solutions/3966164/fen-zu-xun-huan-pythonjavacgo-by-endless-ii4f/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分组循环（Python/Java/C++/Go）](https://leetcode.cn/problems/count-valid-word-occurrences/solutions/3966164/fen-zu-xun-huan-pythonjavacgo-by-endless-ii4f/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-zu-xun-huan-pythonjavacgo-by-endless-ii4f`
- topic id：`3966164`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**适用场景**：按照题目要求，字符串会被分割成若干组，每一组的判断/处理逻辑是相同的。

**核心思想**：

- 外层循环负责遍历组之前的准备工作（记录开始位置），和遍历组之后的统计工作。
- 内层循环负责遍历组，找出这一组最远在哪结束。

这个写法的好处是，各个逻辑块分工明确，也不需要特判最后一组（易错点）。以我的经验，这个写法是所有写法中最不容易出 bug 的，推荐大家记住。

对于本题，单词的第一个字母必须是小写英文字母，作为一组的开始。如果遍历到 `' '` 或者 `"--"` 或者 `"- "`，跳出内层循环。

用哈希表统计每个单词的个数，即可快速回答询问。

[本题视频讲解](https://www.bilibili.com/video/BV1Vb5L6zEgm/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countWordOccurrences(self, chunks: list[str], queries: list[str]) -> list[int]:
        s = ''.join(chunks)
        cnt = defaultdict(int)

        for t in s.split():  # 不用 split 的写法见另一份代码
            n = len(t)
            i = 0
            while i < n:
                if t[i] == '-':
                    i += 1
                    continue
                start = i
                # 遇到 "--"（连续两个 '-'）就跳出循环
                while i < n and (t[i] != '-' or i < n - 1 and t[i + 1] != '-'):
                    i += 1
                cnt[t[start: i]] += 1

        return [cnt[q] for q in queries]
```

```py [sol-Python3 不用 split]
class Solution:
    def countWordOccurrences(self, chunks: list[str], queries: list[str]) -> list[int]:
        s = ''.join(chunks)
        n = len(s)
        cnt = defaultdict(int)

        i = 0
        while i < n:
            if s[i] == ' ' or s[i] == '-':
                i += 1
                continue
            start = i
            # 遇到 ' ' 或者 "--" 或者 "- " 时，跳出循环
            while i < n and s[i] != ' ' and (s[i] != '-' or i < n - 1 and s[i + 1] != '-' and s[i + 1] != ' '):
                i += 1
            cnt[s[start: i]] += 1

        return [cnt[q] for q in queries]
```

```java [sol-Java]
class Solution {
    public int[] countWordOccurrences(String[] chunks, String[] queries) {
        char[] s = String.join("", chunks).toCharArray();
        int n = s.length;
        Map<String, Integer> cnt = new HashMap<>();

        for (int i = 0; i < n; i++) {
            if (s[i] == ' ' || s[i] == '-') {
                continue;
            }
            int start = i;
            // 遇到 ' ' 或者 "--" 或者 "- " 时，跳出循环
            while (i < n && s[i] != ' ' && (s[i] != '-' || i < n - 1 && s[i + 1] != '-' && s[i + 1] != ' ')) {
                i++;
            }
            String word = new String(s, start, i - start);
            cnt.merge(word, 1, Integer::sum); // cnt[word]++
        }

        int[] ans = new int[queries.length];
        for (int i = 0; i < queries.length; i++) {
            ans[i] = cnt.getOrDefault(queries[i], 0);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> countWordOccurrences(vector<string>& chunks, vector<string>& queries) {
        string s = chunks | views::join | ranges::to<string>();
        int n = s.size();
        unordered_map<string, int> cnt;

        for (int i = 0; i < n; i++) {
            if (s[i] == ' ' || s[i] == '-') {
                continue;
            }
            int start = i;
            // 遇到 ' ' 或者 "--" 或者 "- " 时，跳出循环
            while (i < n && s[i] != ' ' && (s[i] != '-' || i < n - 1 && s[i + 1] != '-' && s[i + 1] != ' ')) {
                i++;
            }
            cnt[s.substr(start, i - start)]++;
        }

        vector<int> ans(queries.size());
        for (int i = 0; i < queries.size(); i++) {
            ans[i] = cnt[queries[i]];
        }
        return ans;
    }
};
```

```go [sol-Go]
func countWordOccurrences(chunks []string, queries []string) []int {
	s := strings.Join(chunks, "")
	n := len(s)
	cnt := map[string]int{}

	for i := 0; i < n; i++ {
		if s[i] == ' ' || s[i] == '-' {
			continue
		}
		start := i
		// 遇到 ' ' 或者 "--" 或者 "- " 时，跳出循环
		for i < n && s[i] != ' ' && (s[i] != '-' || i < n-1 && s[i+1] != '-' && s[i+1] != ' ') {
			i++
		}
		cnt[s[start:i]]++
	}

	ans := make([]int, len(queries))
	for i, q := range queries {
		ans[i] = cnt[q]
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(C+Q)$，其中 $C$ 是 $\textit{chunks}$ 的字符串长度之和，$Q$ 是 $\textit{queries}$ 的字符串长度之和。
- 空间复杂度：$\mathcal{O}(C)$。返回值不计入。

## 专题训练

见下面双指针题单的「**六、分组循环**」。

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
