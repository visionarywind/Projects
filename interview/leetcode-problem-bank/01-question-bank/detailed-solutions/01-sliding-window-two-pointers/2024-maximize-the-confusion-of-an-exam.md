# 2024. 考试的最大困扰度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-the-confusion-of-an-exam/
- 题目 slug：`maximize-the-confusion-of-an-exam`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础
- 难度分：1643
- 外部题解来源：https://leetcode.cn/problems/maximize-the-confusion-of-an-exam/solutions/1028668/zhi-jie-zhao-ban-1004-de-dai-ma-by-endle-42x3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最大滑动窗口（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximize-the-confusion-of-an-exam/solutions/1028668/zhi-jie-zhao-ban-1004-de-dai-ma-by-endle-42x3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-jie-zhao-ban-1004-de-dai-ma-by-endle-42x3`
- topic id：`1028668`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 题意

求 $\textit{answerKey}$ 的一个最长子串，包含至多 $k$ 个 $\texttt{T}$ **或者**至多 $k$ 个 $\texttt{F}$。

## 思路

由于子串越长，越无法满足要求，有单调性，可以用**滑动窗口**解决，原理请看 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

1. 遍历 $\textit{answerKey}$，枚举子串右端点 $\textit{right}$，同时维护最小左端点 $\textit{left}$ 以及子串中的字符个数 $\textit{cnt}$。
2. 把 $\textit{answerKey}[\textit{right}]$ 的出现次数加一。
3. 如果 $\texttt{T}$ 和 $\texttt{F}$ 的出现次数**都**超过 $k$，那么必须不断移动左端点 $\textit{left}$，同时减少 $\textit{answerKey}[\textit{left}]$ 的出现次数，直到 $\texttt{T}$ 和 $\texttt{F}$ 的出现次数至少有一个 $\le k$。
4. 循环结束后，说明子串右端点在 $\textit{right}$ 时，对应的最小左端点为 $\textit{left}$，用子串长度 $\textit{right}-\textit{left}+1$ 更新答案的最大值。
5. 遍历 $\textit{answerKey}$ 结束后，返回答案。

代码实现时，由于 $\texttt{T}$ 和 $\texttt{F}$ 的 ASCII 值除以 $2$ 后的奇偶性不同，也就是它们二进制的次低位不同，可以改为统计二进制次低位。

```py [sol-Python3]
class Solution:
    def maxConsecutiveAnswers(self, answerKey: str, k: int) -> int:
        ans = left = 0
        cnt = defaultdict(int)
        for right, ch in enumerate(answerKey):
            cnt[ch] += 1
            while cnt['T'] > k and cnt['F'] > k:
                cnt[answerKey[left]] -= 1
                left += 1
            ans = max(ans, right - left + 1)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxConsecutiveAnswers(String answerKey, int k) {
        char[] s = answerKey.toCharArray();
        int ans = 0;
        int left = 0;
        int[] cnt = new int[2];
        for (int right = 0; right < s.length; right++) {
            cnt[s[right] >> 1 & 1]++;
            while (cnt[0] > k && cnt[1] > k) {
                cnt[s[left] >> 1 & 1]--;
                left++;
            }
            ans = Math.max(ans, right - left + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxConsecutiveAnswers(string answerKey, int k) {
        int ans = 0, left = 0, cnt[2]{};
        for (int right = 0; right < answerKey.length(); right++) {
            cnt[answerKey[right] >> 1 & 1]++;
            while (cnt[0] > k && cnt[1] > k) {
                cnt[answerKey[left] >> 1 & 1]--;
                left++;
            }
            ans = max(ans, right - left + 1);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxConsecutiveAnswers(char* answerKey, int k) {
    int ans = 0, left = 0, cnt[2] = {};
    for (int right = 0; answerKey[right]; right++) {
        cnt[answerKey[right] >> 1 & 1]++;
        while (cnt[0] > k && cnt[1] > k) {
            cnt[answerKey[left] >> 1 & 1]--;
            left++;
        }
        ans = MAX(ans, right - left + 1);
    }
    return ans;
}
```

```go [sol-Go]
func maxConsecutiveAnswers(answerKey string, k int) (ans int) {
    cnt := [2]int{}
    left := 0
    for right, ch := range answerKey {
        cnt[ch>>1&1]++
        for cnt[0] > k && cnt[1] > k {
            cnt[answerKey[left]>>1&1]--
            left++
        }
        ans = max(ans, right-left+1)
    }
    return
}
```

```js [sol-JS]
var maxConsecutiveAnswers = function(answerKey, k) {
    let ans = 0, left = 0;
    const cnt = {'T': 0, 'F': 0};
    for (let right = 0; right < answerKey.length; right++) {
        cnt[answerKey[right]]++;
        while (cnt['T'] > k && cnt['F'] > k) {
            cnt[answerKey[left]]--;
            left++
        }
        ans = Math.max(ans, right - left + 1);
    }
    return ans;
};
```

```js [sol-JS 写法二]
var maxConsecutiveAnswers = function(answerKey, k) {
    let ans = 0, left = 0;
    const cnt = [0, 0];
    for (let right = 0; right < answerKey.length; right++) {
        cnt[answerKey[right].charCodeAt(0) >> 1 & 1]++;
        while (cnt[0] > k && cnt[1] > k) {
            cnt[answerKey[left].charCodeAt(0) >> 1 & 1]--;
            left++;
        }
        ans = Math.max(ans, right - left + 1);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_consecutive_answers(answer_key: String, k: i32) -> i32 {
        let s = answer_key.as_bytes();
        let mut ans = 0;
        let mut left = 0;
        let mut cnt = [0, 0];
        for (right, &ch) in s.iter().enumerate() {
            cnt[(ch >> 1 & 1) as usize] += 1;
            while cnt[0] > k && cnt[1] > k {
                cnt[(s[left] >> 1 & 1) as usize] -= 1;
                left += 1;
            }
            ans = ans.max(right - left + 1);
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{answerKey}$ 的长度。虽然写了个二重循环，但是内层循环中对 $\textit{left}$ 加一的**总**执行次数不会超过 $n$ 次，所以总的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

把子串改成**子序列**要怎么做？

欢迎在评论区分享你的思路/代码。

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
