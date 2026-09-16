# 836. 矩形重叠

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/rectangle-overlap/
- 题目 slug：`rectangle-overlap`
- 来源专题：数学算法
- 来源分类路径：五、计算几何 / §5.3 矩形、多边形
- 难度分：1443
- 外部题解来源：https://leetcode.cn/problems/rectangle-overlap/solutions/4021658/zhuan-hua-cheng-yi-wei-wen-ti-pythonjava-osoq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转化成一维问题（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/rectangle-overlap/solutions/4021658/zhuan-hua-cheng-yi-wei-wen-ti-pythonjava-osoq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-hua-cheng-yi-wei-wen-ti-pythonjava-osoq`
- topic id：`4021658`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

两个矩形的交集也是矩形。矩形面积等于长乘宽，如果交集矩形面积为正，意味着交集矩形的长大于 $0$，宽也大于 $0$。

交集矩形的长只与横坐标有关。只看横坐标，问题变成一个一维问题：

- 把两个矩形压扁成线段（闭区间），判断两个闭区间 $[\ell_1,r_1]$ 和 $[\ell_2,r_2]$ 是否严格相交（交集长度大于 $0$）。

## 写法一

交集左端点为 $\max(\ell_1,\ell_2)$，右端点为 $\min(r_1,r_2)$。

如果满足

$$
\max(\ell_1,\ell_2) < \min(r_1,r_2)
$$

那么两个闭区间严格相交。

纵坐标同理。

```py [sol-Python3]
class Solution:
    # 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
    def isIntervalOverlap(self, l1: int, r1: int, l2: int, r2: int) -> bool:
        # 两个闭区间的交集的左端点为 max(l1, l2)，右端点为 min(r1, r2)
        return max(l1, l2) < min(r1, r2)

    def isRectangleOverlap(self, rec1: list[int], rec2: list[int]) -> bool:
        return self.isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) and \
               self.isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3])
```

```java [sol-Java]
class Solution {
    public boolean isRectangleOverlap(int[] rec1, int[] rec2) {
        return isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
               isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3]);
    }

    // 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
    private boolean isIntervalOverlap(int l1, int r1, int l2, int r2) {
        // 两个闭区间的交集的左端点为 max(l1, l2)，右端点为 min(r1, r2)
        return Math.max(l1, l2) < Math.min(r1, r2);
    }
}
```

```cpp [sol-C++]
class Solution {
    // 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
    bool isIntervalOverlap(int l1, int r1, int l2, int r2) {
        // 两个闭区间的交集的左端点为 max(l1, l2)，右端点为 min(r1, r2)
        return max(l1, l2) < min(r1, r2);
    }

public:
    bool isRectangleOverlap(vector<int>& rec1, vector<int>& rec2) {
        return isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
               isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3]);
    }
};
```

```c [sol-C]
// 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
bool is_interval_overlap(int l1, int r1, int l2, int r2) {
    // 两个闭区间的交集的左端点为 MAX(l1, l2)，右端点为 MIN(r1, r2)
    return MAX(l1, l2) < MIN(r1, r2);
}

bool isRectangleOverlap(int* rec1, int rec1Size, int* rec2, int rec2Size) {
    return is_interval_overlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
           is_interval_overlap(rec1[1], rec1[3], rec2[1], rec2[3]);
}
```

```go [sol-Go]
// 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
func isIntervalOverlap(l1, r1, l2, r2 int) bool {
    // 两个闭区间的交集的左端点为 max(l1, l2)，右端点为 min(r1, r2)
    return max(l1, l2) < min(r1, r2)
}

func isRectangleOverlap(rec1, rec2 []int) bool {
    return isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
           isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3])
}
```

```js [sol-JavaScript]
// 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
var isIntervalOverlap = function(l1, r1, l2, r2) {
    // 两个闭区间的交集的左端点为 max(l1, l2)，右端点为 min(r1, r2)
    return Math.max(l1, l2) < Math.min(r1, r2);
};

var isRectangleOverlap = function(rec1, rec2) {
    return isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
           isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3]);
};
```

```rust [sol-Rust]
impl Solution {
    // 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
    fn is_interval_overlap(l1: i32, r1: i32, l2: i32, r2: i32) -> bool {
        // 两个闭区间的交集的左端点为 max(l1, l2)，右端点为 min(r1, r2)
        l1.max(l2) < r1.min(r2)
    }

    pub fn is_rectangle_overlap(rec1: Vec<i32>, rec2: Vec<i32>) -> bool {
        Self::is_interval_overlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
        Self::is_interval_overlap(rec1[1], rec1[3], rec2[1], rec2[3])
    }
}
```

## 写法二

什么情况下区间不相交？

如果一个区间的右端点 $\le$ 另一个区间的左端点，那么两个区间不相交。

如果都不满足，即任意区间的右端点 $>$ 另一个区间的左端点，那么两个区间严格相交。

```py [sol-Python3]
class Solution:
    # 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
    def isIntervalOverlap(self, l1: int, r1: int, l2: int, r2: int) -> bool:
        return r1 > l2 and r2 > l1

    def isRectangleOverlap(self, rec1: list[int], rec2: list[int]) -> bool:
        return self.isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) and \
               self.isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3])
```

```java [sol-Java]
class Solution {
    public boolean isRectangleOverlap(int[] rec1, int[] rec2) {
        return isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
               isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3]);
    }

    // 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
    private boolean isIntervalOverlap(int l1, int r1, int l2, int r2) {
        return r1 > l2 && r2 > l1;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
    bool isIntervalOverlap(int l1, int r1, int l2, int r2) {
        return r1 > l2 && r2 > l1;
    }

public:
    bool isRectangleOverlap(vector<int>& rec1, vector<int>& rec2) {
        return isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
               isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3]);
    }
};
```

```c [sol-C]
// 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
bool is_interval_overlap(int l1, int r1, int l2, int r2) {
    return r1 > l2 && r2 > l1;
}

bool isRectangleOverlap(int* rec1, int rec1Size, int* rec2, int rec2Size) {
    return is_interval_overlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
           is_interval_overlap(rec1[1], rec1[3], rec2[1], rec2[3]);
}
```

```go [sol-Go]
// 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
func isIntervalOverlap(l1, r1, l2, r2 int) bool {
    return r1 > l2 && r2 > l1
}

func isRectangleOverlap(rec1, rec2 []int) bool {
    return isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
           isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3])
}
```

```js [sol-JavaScript]
// 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
var isIntervalOverlap = function(l1, r1, l2, r2) {
    return r1 > l2 && r2 > l1;
};

var isRectangleOverlap = function(rec1, rec2) {
    return isIntervalOverlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
           isIntervalOverlap(rec1[1], rec1[3], rec2[1], rec2[3]);
};
```

```rust [sol-Rust]
impl Solution {
    // 判断闭区间 [l1, r1] 和 [l2, r2] 是否严格相交
    fn is_interval_overlap(l1: i32, r1: i32, l2: i32, r2: i32) -> bool {
        r1 > l2 && r2 > l1
    }

    pub fn is_rectangle_overlap(rec1: Vec<i32>, rec2: Vec<i32>) -> bool {
        Self::is_interval_overlap(rec1[0], rec1[2], rec2[0], rec2[2]) &&
        Self::is_interval_overlap(rec1[1], rec1[3], rec2[1], rec2[3])
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
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

本题来自 `五、计算几何 / §5.3 矩形、多边形`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、计算几何 / §5.3 矩形、多边形`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
