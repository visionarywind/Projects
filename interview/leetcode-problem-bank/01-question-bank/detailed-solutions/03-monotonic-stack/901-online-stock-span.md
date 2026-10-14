# 901. 股票价格跨度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/online-stock-span/
- 题目 slug：`online-stock-span`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.1 基础
- 难度分：1709
- 外部题解来源：https://leetcode.cn/problems/online-stock-span/solutions/2470527/shi-pin-yi-ge-shi-pin-jiang-tou-dan-diao-cuk7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】一个视频讲透单调栈！附题单（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/online-stock-span/solutions/2470527/shi-pin-yi-ge-shi-pin-jiang-tou-dan-diao-cuk7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-yi-ge-shi-pin-jiang-tou-dan-diao-cuk7`
- topic id：`2470527`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)，欢迎点赞关注~

记住十六字真言：

$$
\begin{aligned}
&及时去掉无用数据，\\
&保证栈中元素有序。
\end{aligned}
$$

## 思路

这题让我们求出每个 $\textit{price}$ 的**上一个更大元素**距离当前有多远。比如示例中 $70$ 的上一个更大元素是 $80$，距离为 $2$。

特别地，如果 $\textit{price}$ 比前面所有数都要大，那么返回当前是第几天（第几次调用 $\text{next}$）。

由于数据是流式输入的，除了记录 $\textit{price}$，还需要记录它是第几天输入的（相当于 $\textit{price}$ 在数组中的下标）。

暴力做法是从当前位置往回找，直到找到一个大于 $\textit{price}$ 的数为止，即 $\textit{price}$ 的**上一个更大元素**。但实际上，对于小于等于 $\textit{price}$ 的数 $x$，不可能作为后续 $\text{next}$ 输入的数的上一个更大元素（因为 $x\le \textit{price}$ 且更远）。所以一旦发现小于等于 $\textit{price}$ 的数，就直接移除。

代码实现时，可以在初始化时往栈底添加一个 $(-1,\infty)$，这样栈一定不会为空，无需单独处理 $\textit{price}$ 大于等于之前所有输入的情况。

```py [sol-Python3]
class StockSpanner:
    def __init__(self):
        self.stack = [(-1, inf)]  # 这样无需判断栈为空的情况
        self.cur_day = -1  # 第一个 next 调用算作第 0 天

    def next(self, price: int) -> int:
        while price >= self.stack[-1][1]:
            self.stack.pop()  # 栈顶数据后面不会再用到了，因为 price 更大
        self.cur_day += 1
        self.stack.append((self.cur_day, price))
        return self.cur_day - self.stack[-2][0]
```

```java [sol-Java]
class StockSpanner {
    private final Deque<int[]> stack = new ArrayDeque<>();
    private int curDay = -1; // 第一个 next 调用算作第 0 天

    public StockSpanner() {
        stack.push(new int[]{-1, Integer.MAX_VALUE}); // 这样无需判断栈为空的情况
    }

    public int next(int price) {
        while (price >= stack.peek()[1]) {
            stack.pop(); // 栈顶数据后面不会再用到了，因为 price 更大
        }
        curDay++;
        int ans = curDay - stack.peek()[0];
        stack.push(new int[]{curDay, price});
        return ans;
    }
}
```

```cpp [sol-C++]
class StockSpanner {
    stack<pair<int, int>> st;
    int cur_day = -1; // 第一个 next 调用算作第 0 天

public:
    StockSpanner() {
        st.emplace(-1, INT_MAX); // 这样无需判断栈为空的情况
    }

    int next(int price) {
        while (price >= st.top().second) {
            st.pop(); // 栈顶数据后面不会再用到了，因为 price 更大
        }
        cur_day++;
        int ans = cur_day - st.top().first;
        st.emplace(cur_day, price);
        return ans;
    }
};
```

```go [sol-Go]
type pair struct {
    day   int
    price int
}

type StockSpanner struct {
    stack  []pair
    curDay int // 第一个 next 调用算作第 0 天
}

func Constructor() StockSpanner {
    return StockSpanner{[]pair{{-1, math.MaxInt}}, -1} // 这样无需判断栈为空的情况
}

func (s *StockSpanner) Next(price int) int {
    for price >= s.stack[len(s.stack)-1].price {
        s.stack = s.stack[:len(s.stack)-1] // 栈顶数据后面不会再用到了，因为 price 更大
    }
    s.curDay++
    s.stack = append(s.stack, pair{s.curDay, price})
    return s.curDay - s.stack[len(s.stack)-2].day
}
```

```js [sol-JavaScript]
var StockSpanner = function() {
    this.stack = [[-1, Infinity]]; // 这样无需判断栈为空的情况
    this.curDay = -1; // 第一个 next 调用算作第 0 天
};

StockSpanner.prototype.next = function(price) {
    while (price >= this.stack[this.stack.length - 1][1]) {
        this.stack.pop(); // 栈顶数据后面不会再用到了，因为 price 更大
    }
    this.curDay++;
    this.stack.push([this.curDay, price]);
    return this.curDay - this.stack[this.stack.length - 2][0];
};
```

```rust [sol-Rust]
struct StockSpanner {
    stack: Vec<(i32, i32)>,
    cur_day: i32,
}

impl StockSpanner {
    fn new() -> Self {
        StockSpanner {
            stack: vec![(-1, i32::MAX)], // 这样无需判断栈为空的情况
            cur_day: -1, // 第一个 next 调用算作第 0 天
        }
    }

    fn next(&mut self, price: i32) -> i32 {
        while price >= self.stack.last().unwrap().1 {
            self.stack.pop(); // 栈顶数据后面不会再用到了，因为 price 更大
        }
        self.cur_day += 1;
        let ans = self.cur_day - self.stack.last().unwrap().0;
        self.stack.push((self.cur_day, price));
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：均摊 $\mathcal{O}(1)$。因为每个元素至多入栈出栈各一次。
- 空间复杂度：$\mathcal{O}(\min(q,U))$。其中 $q$ 为 $\text{next}$ 的调用次数，$U$ 为 $\textit{price}$ 的范围。注意栈中没有重复元素，在 $\textit{price}$ 值域很小的情况下，空间复杂度主要取决于 $\textit{price}$ 的值域范围。

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
