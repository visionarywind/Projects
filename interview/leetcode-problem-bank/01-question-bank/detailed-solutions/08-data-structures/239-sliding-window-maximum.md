# 239. 滑动窗口最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sliding-window-maximum/
- 题目 slug：`sliding-window-maximum`
- 来源专题：常用数据结构
- 来源分类路径：四、队列 / §4.4 单调队列
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sliding-window-maximum/solutions/2499715/shi-pin-yi-ge-shi-pin-miao-dong-dan-diao-ezj6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[降本增笑，秒懂单调队列！附题单！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/sliding-window-maximum/solutions/2499715/shi-pin-yi-ge-shi-pin-miao-dong-dan-diao-ezj6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-yi-ge-shi-pin-miao-dong-dan-diao-ezj6`
- topic id：`2499715`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 比喻

这是一个降本增笑的故事：

1. 如果新员工比老员工强（或者一样强），把老员工裁掉。（元素进入窗口）
2. 如果老员工 35 岁了，也裁掉。（元素离开窗口）

裁员后，资历最老（最左边）的人就是最强的员工了。

## 视频讲解

请看 [单调队列【基础算法精讲 27】](https://www.bilibili.com/video/BV1bM411X72E/)，欢迎点赞关注~

## 单调队列套路

1. 右边入（元素进入**队尾**，同时维护队列**单调性**）
2. 左边出（元素离开**队首**）
3. 记录/维护答案（根据**队首**）

## 答疑

**问**：为什么代码中的「队首离开窗口」写的是 $\texttt{if}$ 而不是 $\texttt{while}$？

**答**：注意窗口在**一步一步**地向右滑动（像只蜗牛），所以元素也是一个一个地离开窗口，不会出现同一轮循环中有多个元素离开窗口的情况。

```py [sol-Python3]
class Solution:
    def maxSlidingWindow(self, nums: List[int], k: int) -> List[int]:
        ans = [0] * (len(nums) - k + 1)  # 窗口个数
        q = deque()  # 双端队列

        for i, x in enumerate(nums):
            # 1. 右边入
            while q and nums[q[-1]] <= x:
                q.pop()  # 维护 q 的单调性
            q.append(i)  # 注意保存的是下标，这样下面可以判断队首是否离开窗口

            # 2. 左边出
            left = i - k + 1  # 窗口左端点
            if q[0] < left:  # 队首离开窗口
                q.popleft()

            # 3. 在窗口左端点处记录答案
            if left >= 0:
                # 由于队首到队尾单调递减，所以窗口最大值就在队首
                ans[left] = nums[q[0]]

        return ans
```

```java [sol-Java]
class Solution {
    public int[] maxSlidingWindow(int[] nums, int k) {
        int n = nums.length;
        int[] ans = new int[n - k + 1]; // 窗口个数
        Deque<Integer> q = new ArrayDeque<>(); // 更快的写法见【Java 数组】

        for (int i = 0; i < n; i++) {
            // 1. 右边入
            while (!q.isEmpty() && nums[q.getLast()] <= nums[i]) {
                q.removeLast(); // 维护 q 的单调性
            }
            q.addLast(i); // 注意保存的是下标，这样下面可以判断队首是否离开窗口

            // 2. 左边出
            int left = i - k + 1; // 窗口左端点
            if (q.getFirst() < left) { // 队首离开窗口
                q.removeFirst();
            }

            // 3. 在窗口左端点处记录答案
            if (left >= 0) {
                // 由于队首到队尾单调递减，所以窗口最大值就在队首
                ans[left] = nums[q.getFirst()];
            }
        }

        return ans;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int[] maxSlidingWindow(int[] nums, int k) {
        int n = nums.length;
        int[] ans = new int[n - k + 1]; // 窗口个数
        int[] q = new int[n];
        int head = 0, tail = -1;

        for (int i = 0; i < n; i++) {
            // 1. 右边入
            while (head <= tail && nums[q[tail]] <= nums[i]) {
                tail--; // 右边出队
            }
            q[++tail] = i; // 右边入队

            // 2. 左边出
            int left = i - k + 1; // 窗口左端点
            if (q[head] < left) { // 队首离开窗口
                head++;
            }

            // 3. 在窗口左端点处记录答案
            if (left >= 0) {
                // 由于队首到队尾单调递减，所以窗口最大值就在队首
                ans[left] = nums[q[head]];
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> ans(n - k + 1); // 窗口个数
        deque<int> q; // 双端队列

        for (int i = 0; i < n; i++) {
            // 1. 右边入
            while (!q.empty() && nums[q.back()] <= nums[i]) {
                q.pop_back(); // 维护 q 的单调性
            }
            q.push_back(i); // 注意保存的是下标，这样下面可以判断队首是否离开窗口

            // 2. 左边出
            int left = i - k + 1; // 窗口左端点
            if (q.front() < left) { // 队首离开窗口
                q.pop_front();
            }

            // 3. 在窗口左端点处记录答案
            if (left >= 0) {
                // 由于队首到队尾单调递减，所以窗口最大值就在队首
                ans[left] = nums[q.front()];
            }
        }

        return ans;
    }
};
```

```c [sol-C]
int* maxSlidingWindow(int* nums, int numsSize, int k, int* returnSize) {
    *returnSize = numsSize - k + 1; // 窗口个数
    int* ans = malloc((*returnSize) * sizeof(int));
    int* q = malloc(numsSize * sizeof(int)); // 用数组模拟双端队列
    int head = 0, tail = -1; // 双端队列的队首和队尾

    for (int i = 0; i < numsSize; i++) {
        // 1. 右边入
        while (head <= tail && nums[q[tail]] <= nums[i]) {
            tail--; // 维护 q 的单调性
        }
        q[++tail] = i; // 注意保存的是下标，这样下面可以判断队首是否离开窗口

        // 2. 左边出
        int left = i - k + 1; // 窗口左端点
        if (q[head] < left) { // 队首离开窗口
            head++;
        }

        // 3. 在窗口左端点处记录答案
        if (left >= 0) {
            // 由于队首到队尾单调递减，所以窗口最大值就在队首
            ans[left] = nums[q[head]];
        }
    }

    free(q);
    return ans;
}
```

```go [sol-Go]
func maxSlidingWindow(nums []int, k int) []int {
    ans := make([]int, len(nums)-k+1) // 窗口个数
    q := []int{}

    for i, x := range nums {
        // 1. 右边入
        for len(q) > 0 && nums[q[len(q)-1]] <= x {
            q = q[:len(q)-1] // 维护 q 的单调性
        }
        q = append(q, i) // 注意保存的是下标，这样下面可以判断队首是否离开窗口

        // 2. 左边出
        left := i - k + 1 // 窗口左端点
        if q[0] < left {  // 队首离开窗口
            q = q[1:] // Go 的切片是 O(1) 的
        }

        // 3. 在窗口左端点处记录答案
        if left >= 0 {
            // 由于队首到队尾单调递减，所以窗口最大值就在队首
            ans[left] = nums[q[0]]
        }
    }

    return ans
}
```

```js [sol-JavaScript]
var maxSlidingWindow = function(nums, k) {
    const ans = [];
    const q = new Deque(); // datastructures-js/deque

    for (let i = 0; i < nums.length; i++) {
        // 1. 右边入
        while (!q.isEmpty() && nums[q.back()] <= nums[i]) {
            q.popBack(); // 维护 q 的单调性
        }
        q.pushBack(i); // 注意保存的是下标，这样下面可以判断队首是否离开窗口

        // 2. 左边出
        const left = i - k + 1; // 窗口左端点
        if (q.front() < left) { // 队首离开窗口
            q.popFront();
        }

        // 3. 在窗口左端点处记录答案
        if (left >= 0) {
            // 由于队首到队尾单调递减，所以窗口最大值就在队首
            ans.push(nums[q.front()]);
        }
    }

    return ans;
};
```

```rust [sol-Rust]
use std::collections::VecDeque;

impl Solution {
    pub fn max_sliding_window(nums: Vec<i32>, k: i32) -> Vec<i32> {
        let k = k as usize;
        let mut ans = Vec::with_capacity(nums.len() - k + 1); // 预分配空间（窗口个数）
        let mut q = VecDeque::new(); // 双端队列

        for (i, &x) in nums.iter().enumerate() {
            // 1. 右边入
            while !q.is_empty() && nums[*q.back().unwrap()] <= x {
                q.pop_back(); // 维护 q 的单调性
            }
            q.push_back(i); // 注意保存的是下标，这样下面可以判断队首是否离开窗口

            // 2. 左边出
            if q[0] + k <= i { // 队首离开窗口
                q.pop_front();
            }

            // 3. 在窗口左端点处记录答案
            if i + 1 >= k {
                // 由于队首到队尾单调递减，所以窗口最大值就在队首
                ans.push(nums[q[0]]);
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。虽然写了个二重循环，但队列入队 $n$ 个元素，所以 `q.pop()` 出队至多执行 $n$ 次，所以二重循环的循环次数只有 $O(n)$。
- 空间复杂度：$\mathcal{O}(\min(k,U))$，其中 $U$ 是 $\textit{nums}$ 中的不同元素个数（本题至多为 $20001$）。双端队列至多有 $k$ 个元素，同时又没有重复元素，所以也至多有 $U$ 个元素，所以空间复杂度为 $\mathcal{O}(\min(k,U))$。返回值的空间不计入。

## 思考题（2026.6.15 添加）

对于 $k=1,2,\ldots,n$，计算本题的答案。

具体地，返回一个长为 $n$ 的数组 $\textit{answers}$，其中 $\textit{answers}[i]$ 表示 $k=i+1$ 时本题的答案。

要求：时间复杂度 $\mathcal{O}(n)$。

欢迎在评论区分享你的思路/代码。

## 专题训练

见下面数据结构题单的「**§4.4 单调队列**」。

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

本题来自 `四、队列 / §4.4 单调队列`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、队列 / §4.4 单调队列`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
