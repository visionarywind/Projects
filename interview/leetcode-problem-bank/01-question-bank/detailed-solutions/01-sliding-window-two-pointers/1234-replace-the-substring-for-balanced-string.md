# 1234. 替换子串得到平衡字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/replace-the-substring-for-balanced-string/
- 题目 slug：`replace-the-substring-for-balanced-string`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.2 越长越合法/求最短/最小
- 难度分：1878
- 外部题解来源：https://leetcode.cn/problems/replace-the-substring-for-balanced-string/solutions/2108358/tong-xiang-shuang-zhi-zhen-hua-dong-chua-z7tu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/replace-the-substring-for-balanced-string/solutions/2108358/tong-xiang-shuang-zhi-zhen-hua-dong-chua-z7tu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-xiang-shuang-zhi-zhen-hua-dong-chua-z7tu`
- topic id：`2108358`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

根据题意，如果在待替换子串**之外**，某种字母的个数超过 $m=\dfrac{n}{4}$，那么无论怎么替换，都无法使这个字母在整个字符串中的个数为 $m$。注意题目保证 $n$ 是 $4$ 的倍数。

比如 $m=2$，待替换子串之外有 $3$ 个 $\texttt{Q}$，由于待替换子串之外的字母我们无法修改，所以 $\texttt{Q}$ 无法减少成 $2$ 个。

反过来说，如果在待替换子串**之外**的任意字母的个数都不超过 $m$，那么可以通过替换子串内的字母，使每种字母的个数均为 $m$。

比如 $m=2$，待替换子串外的字母 $\texttt{QWER}$ 的个数分别为 $1,2,0,2$，由于一共有 $4m=8$ 个字母，所以待替换子串的长度为 $3$。我们把子串内的字母改成 $\texttt{QEE}$，就可以让字母 $\texttt{QWER}$ 的个数都为 $2$ 了。

由于待替换子串越长，越能满足「待替换子串**之外**的任意字母的个数都不超过 $m$」的要求，所以可以用**滑动窗口**解决，请看视频讲解 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

### 答疑

**问**：为什么下面代码的内层循环没有比较 $\textit{left}$ 和 $\textit{right}$ 的大小？这不会导致越界或者其他非法情况吗？

**答**：注意在滑动窗口前，代码有个提前返回 $0$ 的条件判断。如果不满足这个条件，说明存在个数大于 $m$ 的字母，那么后面滑动窗口时，如果窗口是空的，那么必然有字母个数大于 $m$，不会继续跑内层循环。所以不会出现越界或者其他非法情况。

```py [sol-Python3]
class Solution:
    def balancedString(self, s: str) -> int:
        m = len(s) // 4
        cnt = Counter(s)
        if len(cnt) == 4 and min(cnt.values()) == m:  # 已经符合要求啦
            return 0

        ans, left = inf, 0
        for right, c in enumerate(s):  # 枚举子串右端点
            cnt[c] -= 1  # 注意 cnt 统计的是在子串 [left,right] 外面的字母个数
            while max(cnt.values()) <= m:  # 子串 [left,right] 满足要求
                ans = min(ans, right - left + 1)
                cnt[s[left]] += 1  # 缩小子串，把 s[left] 放到外面
                left += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int balancedString(String S) {
        char[] s = S.toCharArray();
        int[] cnt = new int['X']; // 也可以用哈希表，但数组更快
        for (char c : s) {
            cnt[c]++;
        }

        int n = s.length;
        int m = n / 4;
        if (cnt['Q'] == m && cnt['W'] == m && cnt['E'] == m && cnt['R'] == m) {
            return 0; // 已经符合要求啦
        }

        int ans = n;
        int left = 0;
        for (int right = 0; right < n; right++) { // 枚举子串右端点
            cnt[s[right]]--; // 注意 cnt 统计的是在子串 [left,right] 外面的字母个数
            while (cnt['Q'] <= m && cnt['W'] <= m && cnt['E'] <= m && cnt['R'] <= m) {
                // 子串 [left,right] 满足要求
                ans = Math.min(ans, right - left + 1);
                cnt[s[left]]++; // 缩小子串，把 s[left] 放到外面
                left++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int balancedString(string s) {
        int cnt['X']{}; // 也可以用哈希表，不过数组更快一些
        for (char c : s) {
            cnt[c]++;
        }

        int n = s.size(), m = n / 4;
        if (cnt['Q'] == m && cnt['W'] == m && cnt['E'] == m && cnt['R'] == m) {
            return 0; // 已经符合要求啦
        }
        
        int ans = n, left = 0;
        for (int right = 0; right < n; right++) { // 枚举子串右端点
            cnt[s[right]]--; // 注意 cnt 统计的是在子串 [left,right] 外面的字母个数
            while (cnt['Q'] <= m && cnt['W'] <= m && cnt['E'] <= m && cnt['R'] <= m) {
                // 子串 [left,right] 满足要求
                ans = min(ans, right - left + 1);
                cnt[s[left]]++; // 缩小子串，把 s[left] 放到外面
                left++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func balancedString(s string) int {
    cnt := ['X']int{} // 也可以用哈希表，不过数组更快一些
    for _, c := range s {
        cnt[c]++
    }

    m := len(s) / 4
    if cnt['Q'] == m && cnt['W'] == m && cnt['E'] == m && cnt['R'] == m {
        return 0 // 已经符合要求啦
    }

    ans := len(s)
    left := 0
    for right, c := range s { // 枚举子串右端点
        cnt[c]-- // 注意 cnt 统计的是在子串 [left,right] 外面的字母个数
        for cnt['Q'] <= m && cnt['W'] <= m && cnt['E'] <= m && cnt['R'] <= m {
            // 子串 [left,right] 满足要求
            ans = min(ans, right-left+1)
            cnt[s[left]]++ // 缩小子串，把 s[left] 放到外面
            left++
        }
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n|\Sigma|)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|=4$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。如果用哈希表实现，可以做到 $O(|\Sigma|)$。

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
