# 169. 多数元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/majority-element/
- 题目 slug：`majority-element`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.8 摩尔投票法
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/majority-element/solutions/3744717/on-mo-er-tou-piao-fa-yan-jin-zheng-ming-ww1zv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 武林大会打擂台，严谨证明（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/majority-element/solutions/3744717/on-mo-er-tou-piao-fa-yan-jin-zheng-ming-ww1zv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-mo-er-tou-piao-fa-yan-jin-zheng-ming-ww1zv`
- topic id：`3744717`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

**题意**：计算 $\textit{nums}$ 的**绝对众数**。绝对众数的出现次数，比其余所有元素的出现次数加起来还多。题目保证 $\textit{nums}$ 一定存在绝对众数。

尝试设计一个 $\mathcal{O}(n)$ 一次遍历，同时只用到 $\mathcal{O}(1)$ 额外空间的算法。

想象一众武林高手比武，谁会笑到最后？

我用「擂台赛」打比方：

1. **擂主登场**：$\textit{nums}[0]$ 成为初始擂主，生命值为 $1$。
2. **挑战者出现**：遍历后续元素，作为挑战者。
3. **比武**：如果挑战者与擂主属于同一门派（值相同），那么擂主生命值加 $1$，否则擂主生命值减 $1$。
4. **擂主更迭**：如果比武后，擂主生命值降为 $0$（同归于尽），那么下一个挑战者成为新的擂主，生命值为 $1$。
5. 最后在擂台上的那人，便是武林盟主（绝对众数）。

为什么这样做是对的？

设出现次数最多的元素的出现次数为 $a$，其余元素的出现次数之和为 $b=n-a$。题目保证 $a>b$。

**证明**：上述过程中，每次擂主的生命值降为 $0$ 时，相当于开了一个新的擂台赛，在 $\textit{nums}[i-1]$ 和 $\textit{nums}[i]$ 之间切一刀。这会把 $\textit{nums}$ 分成若干段。依次考察这些段：

- 对于除了最后一段的每一段（注意这些段的擂主不一定是绝对众数，比如绝对众数是 $9$，这一段是 $[1,1,2,9]$），设绝对众数在其中出现了 $x$ 次，其余元素的出现次数之和为 $y$，则必然有 $x\le y$。这可以用反证法证明，如果 $x>y$，那么绝对众数血多，不可能被其余元素同归于尽，绝对众数的生命值在这段结束时必然大于 $0$，矛盾。由此可得 $a-x > b-y$，意思是，把 $a$ 减去 $x$，$b$ 减去 $y$，所得到的 $a'$ 和 $b'$ 仍然满足 $a'>b'$。依此类推，每一段结束时，在剩余元素（未遍历到的元素）中，设出现次数最多的元素的出现次数为 $a'$，其余元素的出现次数之和为 $b'$，那么 $a'>b'$ 始终成立。
- 对于最后一段，由于 $a'>b'$，绝对众数血多，不可能被其余元素同归于尽，绝对众数的生命值最终必然大于 $0$，所以最后在擂台上的是绝对众数。

```py [sol-Python3]
class Solution:
    def majorityElement(self, nums: List[int]) -> int:
        ans = hp = 0
        for x in nums:
            if hp == 0:  # x 是初始擂主，生命值为 1
                ans, hp = x, 1
            else:  # 比武，同门加血，否则扣血
                hp += 1 if x == ans else -1
        return ans
```

```java [sol-Java]
class Solution {
    public int majorityElement(int[] nums) {
        int ans = 0;
        int hp = 0;
        for (int x : nums) {
            if (hp == 0) { // x 是初始擂主，生命值为 1
                ans = x;
                hp = 1;
            } else { // 比武，同门加血，否则扣血
                hp += x == ans ? 1 : -1;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int majorityElement(vector<int>& nums) {
        int ans = 0, hp = 0;
        for (int x : nums) {
            if (hp == 0) { // x 是初始擂主，生命值为 1
                ans = x;
                hp = 1;
            } else { // 比武，同门加血，否则扣血
                hp += x == ans ? 1 : -1;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int majorityElement(int* nums, int numsSize) {
    int ans = 0, hp = 0;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        if (hp == 0) { // x 是初始擂主，生命值为 1
            ans = x;
            hp = 1;
        } else { // 比武，同门加血，否则扣血
            hp += x == ans ? 1 : -1;
        }
    }
    return ans;
}
```

```go [sol-Go]
func majorityElement(nums []int) (ans int) {
    hp := 0
    for _, x := range nums {
        if hp == 0 { // x 是初始擂主，生命值为 1
            ans, hp = x, 1
        } else if x == ans { // 比武，同门加血，否则扣血
            hp++
        } else {
            hp--
        }
    }
    return
}
```

```js [sol-JavaScript]
var majorityElement = function(nums) {
    let ans = 0, hp = 0;
    for (const x of nums) {
        if (hp === 0) { // x 是初始擂主，生命值为 1
            ans = x;
            hp = 1;
        } else { // 比武，同门加血，否则扣血
            hp += x === ans ? 1 : -1;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn majority_element(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut hp = 0;
        for x in nums {
            if hp == 0 { // x 是初始擂主，生命值为 1
                ans = x;
                hp = 1;
            } else if x == ans { // 比武，同门加血，否则扣血
                hp += 1;
            } else {
                hp -= 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

给定数组 $\textit{nums}$，判断 $\textit{nums}$ 是否存在绝对众数。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
