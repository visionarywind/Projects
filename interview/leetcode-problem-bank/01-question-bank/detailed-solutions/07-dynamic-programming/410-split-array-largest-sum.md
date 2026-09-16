# 410. 分割数组的最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/split-array-largest-sum/
- 题目 slug：`split-array-largest-sum`
- 来源专题：动态规划
- 来源分类路径：五、划分型 DP / §5.3 约束划分个数
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/split-array-largest-sum/solutions/2613046/er-fen-da-an-fu-ti-dan-pythonjavacgojsru-n5la/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案，附题单！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/split-array-largest-sum/solutions/2613046/er-fen-da-an-fu-ti-dan-pythonjavacgojsru-n5la/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-fu-ti-dan-pythonjavacgojsru-n5la`
- topic id：`2613046`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

「元素和的最大值」越小，需要划分出的段数就越多；「元素和的最大值」越大，需要划分出的段数就越少。例如示例 1 的 $\textit{nums}=[7,2,5,10,8]$，在最大和为 $15$ 时，至少要划分 $3$ 段，比如 $[7,2,5],[10],[8]$。而在最大和为 $18$ 时，只需要划分 $2$ 段，比如 $[7,2,5],[10,8]$。

**套路**：如果发现答案越小，越不能（能）满足要求；答案越大，越能（不能）满足要求，就可以尝试二分答案。关于二分的原理，请看视频讲解：[二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

把二分中点 $\textit{mid}$ 记作 $\textit{mx}$，我们可以贪心地计算要划分出的段数：

1. 初始化段数 $\textit{cnt}=1$（第一段），当前这一段的元素和 $s=0$。
2. 遍历 $\textit{nums}$。
3. 如果 $s+\textit{nums}[i]\le \textit{mx}$，则把 $\textit{nums}[i]$ 加到 $s$ 中。否则我们必须新划分出一段，把 $\textit{cnt}$ 加一，$s$ 替换成 $\textit{nums}[i]$。如果在 $\textit{cnt}$ 加一之前有 $\textit{cnt} = k$，则说明我们划分了超过 $k$ 段，返回 $\texttt{false}$，表示不满足要求。
4. 遍历结束，返回 $\texttt{true}$，表示满足要求。

## 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的。

设 $\textit{nums}$ 的元素和为 $S$。

- 开区间左端点初始值：$\max(\textit{nums})-1$。元素和不可能比数组最大值还小，一定不满足要求。
- 开区间右端点初始值：$S$。无需分割，一定满足要求。

> 注：也可以用平均值 $S/k$ 优化**左端点**初始值，留给感兴趣的读者思考。

## 答疑

**问**：如何保证二分结果一定对应着一个恰好划分成 $k$ 段的方案？

**答**：把二分结果代入 $\text{check}$ 函数，可以得到一个划分成 $\le k$ 段的方案。如果划分的段数小于 $k$，我们可以把其中的一个长度至少为 $2$ 的段分割成两段，这两段的元素和都比划分前的段小（或者相等），也满足题目要求，从而增加了段数。反复分割，最终可以得到划分成 $k$ 段的方案。换句话说，题意其实等价于：把数组划分成**至多** $k$ 段。

**问**：设二分算出来的答案为 $\textit{ans}$，如何保证至少有一个子数组的元素和恰好等于 $\textit{ans}$？

**答**：用反证法证明。假设所有子数组的元素和都小于 $\textit{ans}$，也就是小于等于 $\textit{ans}-1$。这意味着 $\text{check}(\textit{ans}-1)=\texttt{true}$，但是二分结束后必定有 $\text{check}(\textit{ans}-1)=\texttt{false}$，矛盾，所以至少有一个子数组的元素和恰好等于 $\textit{ans}$。

```py [sol-Py3]
class Solution:
    def splitArray(self, nums: List[int], k: int) -> int:
        def check(mx: int) -> bool:
            cnt = 1
            s = 0
            for x in nums:
                if s + x <= mx:
                    s += x
                    continue
                if cnt == k:  # 不能继续划分
                    return False
                cnt += 1  # 新划分一段
                s = x
            return True

        left = max(nums) - 1
        right = sum(nums)
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Py3 库函数]
class Solution:
    def splitArray(self, nums: List[int], k: int) -> int:
        def check(mx: int) -> bool:
            cnt = 1
            s = 0
            for x in nums:
                if s + x <= mx:
                    s += x
                    continue
                if cnt == k:  # 不能继续划分
                    return False
                cnt += 1  # 新划分一段
                s = x
            return True

        # bisect_left 是左闭右开区间
        left = max(nums)
        right = sum(nums)
        return left + bisect_left(range(left, right), True, key=check)
```

```java [sol-Java]
class Solution {
    public int splitArray(int[] nums, int k) {
        int mx = 0;
        int sum = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
            sum += x;
        }

        int left = mx - 1;
        int right = sum;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (check(nums, k, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int[] nums, int k, int mx) {
        int cnt = 1;
        int s = 0;
        for (int x : nums) {
            if (s + x <= mx) {
                s += x;
                continue;
            }
            if (cnt == k) { // 不能继续划分
                return false;
            }
            cnt++; // 新划分一段
            s = x;
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int splitArray(vector<int>& nums, int k) {
        auto check = [&](int mx) -> bool {
            int cnt = 1, s = 0;
            for (int x : nums) {
                if (s + x <= mx) {
                    s += x;
                    continue;
                }
                if (cnt == k) { // 不能继续划分
                    return false;
                }
                cnt++; // 新划分一段
                s = x;
            }
            return true;
        };

        int left = ranges::max(nums) - 1;
        int right = reduce(nums.begin(), nums.end());
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int splitArray(int* nums, int numsSize, int k) {
    int mx = 0;
    int sum = 0;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        mx = MAX(mx, x);
        sum += x;
    }

    int check(int mx) {
        int cnt = 1, s = 0;
        for (int i = 0; i < numsSize; i++) {
            int x = nums[i];
            if (s + x <= mx) {
                s += x;
                continue;
            }
            if (cnt == k) { // 不能继续划分
                return false;
            }
            cnt++; // 新划分一段
            s = x;
        }
        return true;
    }

    int left = mx - 1, right = sum;
    while (left + 1 < right) {
        int mid = left + (right - left) / 2;
        if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
}
```

```go [sol-Go]
func splitArray(nums []int, k int) int {
    check := func(mx int) bool {
        cnt, s := 1, 0
        for _, x := range nums {
            if s+x <= mx {
                s += x
                continue
            }
            if cnt == k { // 不能继续划分
                return false
            }
            cnt++ // 新划分一段
            s = x
        }
        return true
    }

    sum, mx := 0, 0
    for _, x := range nums {
        sum += x
        mx = max(mx, x)
    }
    left := mx - 1
    right := sum
    for left+1 < right {
        mid := left + (right-left)/2
        if check(mid) {
            right = mid
        } else {
            left = mid
        }
    }
    return right
}
```

```go [sol-Go 库函数]
func splitArray(nums []int, k int) int {
    sum, mx := 0, 0
    for _, x := range nums {
        sum += x
        mx = max(mx, x)
    }
    left := mx
    right := sum

    // 必须是左闭右开区间
    ans := left + sort.Search(right-left, func(mx int) bool {
        mx += left
        cnt, s := 1, 0
        for _, x := range nums {
            if s+x <= mx {
                s += x
                continue
            }
            if cnt == k { // 不能继续划分
                return false
            }
            cnt++ // 新划分一段
            s = x
        }
        return true
    })
    return ans
}
```

```js [sol-JS]
var splitArray = function(nums, k) {
    function check(mx) {
        let cnt = 1, s = 0;
        for (const x of nums) {
            if (s + x <= mx) {
                s += x;
                continue;
            }
            if (cnt === k) { // 不能继续划分
                return false;
            }
            cnt++; // 新划分一段
            s = x;
        }
        return true;
    }

    let left = Math.max(...nums) - 1;
    let right = _.sum(nums);
    while (left + 1 < right) {
        const mid = Math.floor((left + right) / 2);
        if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn split_array(nums: Vec<i32>, k: i32) -> i32 {
        let check = |mx: i32| -> bool {
            let mut cnt = 1;
            let mut s = 0;
            for &x in &nums {
                if s + x <= mx {
                    s += x;
                    continue;
                }
                if cnt == k { // 不能继续划分
                    return false;
                }
                cnt += 1; // 新划分一段
                s = x;
            }
            true
        };

        let mut left = *nums.iter().max().unwrap() - 1;
        let mut right = nums.iter().sum::<i32>();
        while left + 1 < right {
            let mid = left + (right - left) / 2;
            if check(mid) {
                right = mid;
            } else {
                left = mid;
            }
        }
        right
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U$ 是 $\textit{nums}$ 的元素和。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、划分型 DP / §5.3 约束划分个数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `五、划分型 DP / §5.3 约束划分个数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
