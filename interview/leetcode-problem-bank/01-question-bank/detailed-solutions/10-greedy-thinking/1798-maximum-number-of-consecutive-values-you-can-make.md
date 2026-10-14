# 1798. 你能构造出连续值的最大数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-consecutive-values-you-can-make/
- 题目 slug：`maximum-number-of-consecutive-values-you-can-make`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.6 归纳法
- 难度分：1931
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-consecutive-values-you-can-make/solutions/2091580/mei-xiang-ming-bai-yi-zhang-tu-miao-dong-7xlx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没想明白？一张图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-number-of-consecutive-values-you-can-make/solutions/2091580/mei-xiang-ming-bai-yi-zhang-tu-miao-dong-7xlx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-zhang-tu-miao-dong-7xlx`
- topic id：`2091580`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

### 提示

**有序**是一个强大的性质，如果对数组排序不影响答案的话，可以尝试将数组排序后，再重新思考，看看能否发现新的思路。

![1798.png](https://pic.leetcode.cn/1675089725-wbrBLD-1798.png)

```py [sol1-Python3]
class Solution:
    def getMaximumConsecutive(self, coins: List[int]) -> int:
        m = 0  # 一开始只能构造出 0
        coins.sort()
        for c in coins:
            if c > m + 1:  # coins 已排序，后面没有比 c 更小的数了
                break  # 无法构造出 m+1，继续循环没有意义
            m += c  # 可以构造出区间 [0,m+c] 中的所有整数
        return m + 1  # [0,m] 中一共有 m+1 个整数
```

```java [sol1-Java]
class Solution {
    public int getMaximumConsecutive(int[] coins) {
        int m = 0; // 一开始只能构造出 0
        Arrays.sort(coins);
        for (int c : coins) {
            if (c > m + 1) // coins 已排序，后面没有比 c 更小的数了
                break; // 无法构造出 m+1，继续循环没有意义
            m += c; // 可以构造出区间 [0,m+c] 中的所有整数
        }
        return m + 1; // [0,m] 中一共有 m+1 个整数
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int getMaximumConsecutive(vector<int> &coins) {
        int m = 0; // 一开始只能构造出 0
        sort(coins.begin(), coins.end());
        for (int c : coins) {
            if (c > m + 1) // coins 已排序，后面没有比 c 更小的数了
                break; // 无法构造出 m+1，继续循环没有意义
            m += c; // 可以构造出区间 [0,m+c] 中的所有整数
        }
        return m + 1; // [0,m] 中一共有 m+1 个整数
    }
};
```

```go [sol1-Go]
func getMaximumConsecutive(coins []int) int {
    m := 0 // 一开始只能构造出 0
    sort.Ints(coins)
    for _, c := range coins {
        if c > m+1 { // coins 已排序，后面没有比 c 更小的数了
            break // 无法构造出 m+1，继续循环没有意义
        }
        m += c // 可以构造出区间 [0,m+c] 中的所有整数
    }
    return m + 1 // [0,m] 中一共有 m+1 个整数
}
```

### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{coins}$ 的长度。瓶颈在排序上。
- 空间复杂度：$O(1)$。忽略排序时的栈空间，仅用到若干额外变量。

---

如果你觉得自己的思维能力有些薄弱，可以做做 [从周赛中学算法 - 2022 年周赛题目总结（下篇）](https://leetcode.cn/circle/discuss/WR1MJP/) 中的「思维题」这节，所有题目我都写了题解。

最后，欢迎关注【bilibili@灵茶山艾府】，每周更新算法教学视频~
