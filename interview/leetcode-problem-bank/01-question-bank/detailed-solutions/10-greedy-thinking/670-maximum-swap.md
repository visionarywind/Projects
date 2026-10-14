# 670. 最大交换

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-swap/
- 题目 slug：`maximum-swap`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-swap/solutions/2614470/yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-c9b1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次遍历，简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-swap/solutions/2614470/yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-c9b1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-c9b1`
- topic id：`2614470`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

举例说明，假设 $\textit{num}=9952767$。为了得到最大值，我们来看看怎么贪心：

1. 从左往右考察 $\textit{num}$ 的每个数字，如果一个数字右边没有比它大的，那么肯定无需交换它。比如这里的 $9$，无论它和谁交换都不能让 $\textit{num}$ 更大。
2. 反之，如果一个数字右边有比它大的数，那么肯定要交换它，比如这里的 $5$，右边有比它大的 $7$ 和 $6$，为了让 $\textit{num}$ 尽量大，和 $7$ 交换最优。
3. 但是，如果有多个 $7$ 呢？我们应该和哪个 $7$ 交换呢？
   - 如果和第一个 $7$ 交换，我们得到的是 $9972567$。
   - 如果和第二个 $7$ 交换，我们得到的是 $9972765$。
   - 和**最后一个** $7$ 交换是最优的。
    
设 $\textit{num}$ 的十进制字符串为 $s$。算法如下：

1. 倒序遍历 $s$，同时维护最大数的下标 $\textit{maxIdx}$。它只在遇到更大的数字才更新，遇到相同数字不会更新，从而满足上面讨论的「最后一个」。
2. 如果发现 $s[i] < s[\textit{maxIdx}]$，满足交换要求，我们先把这两个下标保存在变量 $p$ 和 $q$ 中。注：$p$ 在遍历前的初始值为 $-1$。
3. 继续向左遍历，如果又遇到 $s[i] < s[\textit{maxIdx}]$，就更新 $p=i,\ q=\textit{maxIdx}$，因为 $s[i]$ 越靠左越好，我们要交换的是从左到右第一个右边有比它大的数字。
4. 遍历结束，如果无需交换，即 $p=-1$，那么直接返回 $\textit{num}$。否则交换 $s[p]$ 和 $s[q]$，然后把 $s$ 转换成数字返回。

```py [sol-Python3]
class Solution:
    def maximumSwap(self, num: int) -> int:
        s = str(num)
        max_idx = len(s) - 1
        p = q = -1
        for i in range(len(s) - 2, -1, -1):
            if s[i] > s[max_idx]:  # s[i] 是目前最大数字
                max_idx = i
            elif s[i] < s[max_idx]:  # s[i] 右边有比它大的
                p, q = i, max_idx  # 更新 p 和 q
        if p == -1:  # 这意味着 s 是降序的
            return num
        s = list(s)
        s[p], s[q] = s[q], s[p]  # 交换 s[p] 和 s[q]
        return int(''.join(s))
```

```java [sol-Java]
class Solution {
    public int maximumSwap(int num) {
        char[] s = Integer.toString(num).toCharArray();
        int maxIdx = s.length - 1;
        int p = -1, q = 0;
        for (int i = s.length - 2; i >= 0; i--) {
            if (s[i] > s[maxIdx]) { // s[i] 是目前最大数字
                maxIdx = i;
            } else if (s[i] < s[maxIdx]) { // s[i] 右边有比它大的
                p = i;
                q = maxIdx; // 更新 p 和 q
            }
        }
        if (p == -1) { // 这意味着 s 是降序的
            return num;
        }
        char temp = s[p];
        s[p] = s[q];
        s[q] = temp; // 交换 s[p] 和 s[q]
        return Integer.parseInt(new String(s));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSwap(int num) {
        string s = to_string(num);
        int n = s.length();
        int max_idx = n - 1;
        int p = -1, q;
        for (int i = n - 2; i >= 0; i--) {
            if (s[i] > s[max_idx]) { // s[i] 是目前最大数字
                max_idx = i;
            } else if (s[i] < s[max_idx]) { // s[i] 右边有比它大的
                p = i;
                q = max_idx; // 更新 p 和 q
            }
        }
        if (p == -1) { // 这意味着 s 是降序的
            return num;
        }
        swap(s[p], s[q]); // 交换 s[p] 和 s[q]
        return stoi(s);
    }
};
```

```go [sol-Go]
func maximumSwap(num int) int {
    s := strconv.Itoa(num)
    maxIdx := len(s) - 1
    p, q := -1, 0
    for i := len(s) - 2; i >= 0; i-- {
        if s[i] > s[maxIdx] { // s[i] 是目前最大数字
            maxIdx = i
        } else if s[i] < s[maxIdx] { // s[i] 右边有比它大的
            p, q = i, maxIdx // 更新 p 和 q
        }
    }
    if p == -1 { // 这意味着 s 是降序的
        return num
    }
    t := []byte(s)
    t[p], t[q] = t[q], t[p]
    ans, _ := strconv.Atoi(string(t))
    return ans
}
```

```js [sol-JavaScript]
var maximumSwap = function (num) {
    const s = num.toString();
    let maxIdx = s.length - 1;
    let p = -1, q = 0;
    for (let i = s.length - 2; i >= 0; i--) {
        if (s[i] > s[maxIdx]) { // s[i] 是目前最大数字
            maxIdx = i;
        } else if (s[i] < s[maxIdx]) { // s[i] 右边有比它大的
            p = i;
            q = maxIdx; // 更新 p 和 q
        }
    }
    if (p === -1) { // 这意味着 s 是降序的
        return num;
    }
    let t = s.split('');
    [t[p], t[q]] = [t[q], t[p]];
    return parseInt(t.join(''));
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_swap(num: i32) -> i32 {
        let mut s = num.to_string().into_bytes();
        let n = s.len();
        let mut max_idx = n - 1;
        let mut p = n;
        let mut q = 0;
        for i in (0..n - 1).rev() {
            if s[i] > s[max_idx] { // s[i] 是目前最大数字
                max_idx = i;
            } else if s[i] < s[max_idx] { // s[i] 右边有比它大的
                p = i;
                q = max_idx; // 更新 p 和 q
            }
        }
        if p == n { // 这意味着 s 是降序的
            return num;
        }
        s.swap(p, q); // 交换 s[p] 和 s[q]
        unsafe { String::from_utf8_unchecked(s).parse().unwrap() }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \textit{num})$。$\textit{num}$ 的十进制长度为 $\mathcal{O}(\log \textit{num})$。
- 空间复杂度：$\mathcal{O}(\log \textit{num})$。

## 思考题

1. 如果要交换的两个数，下标相差不能超过 $k$ 呢？
2. 如果可以交换两次呢？

欢迎在评论区发表你的思路。

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
