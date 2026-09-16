# 1815. 得到新鲜甜甜圈的最多组数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-groups-getting-fresh-donuts/
- 题目 slug：`maximum-number-of-groups-getting-fresh-donuts`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP
- 难度分：2559
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-groups-getting-fresh-donuts/solutions/2072545/by-endlesscheng-r5ve/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[位运算的魔法（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-number-of-groups-getting-fresh-donuts/solutions/2072545/by-endlesscheng-r5ve/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-r5ve`
- topic id：`2072545`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 $m=\textit{batchSize}$。根据题意，我们只关心上一个顾客购买后「余下」的甜甜圈数量，那么购买 $1$ 个和 $m+1$ 个没有任何区别。所以可以按照 $\textit{groups}[i]\bmod m$ 的结果分成 $m$ 组，统计每一组的元素个数，记到一个 $\textit{cnt}$ 数组中。

对于 $\textit{groups}[i]\bmod m=0$ 的组，我们可以直接把这些顾客安排在最前面，这些顾客都可以感到开心。

然后呢？如果直接尝试回溯，由于 $\textit{groups}$ 的长度至多有 $30$，会超时。

但注意到，对于先 $1$ 后 $2$ 和先 $2$ 后 $1$ 这两种安排顺序，其结果是一样的，都可以看成是前面的人买了 $3$ 个甜甜圈。这样就有重复子问题，可以用记忆化搜索来优化。

我在[【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)中讲了「回溯三问」，也可以用来思考怎么写这个记忆化搜索：

1. 当前操作是什么？枚举 $[1,m-1]$ 中 $\textit{cnt}[x] > 0$ 的 $x$，表示一批 $\textit{groups}[i]\bmod m=x$ 的顾客前来购买甜甜圈。
2. 子问题是什么？想想操作会影响哪些数据？它们是：累计卖出的甜甜圈数量模 $m$ 的结果 $\textit{left}$，以及剩余可以选的元素个数 $\textit{cnt}$。这样就确定了递归函数的两个参数。
3. 下一个子问题是什么？$\textit{left}$ 变成 $(\textit{left}+x)\bmod m$，$\textit{cnt}[x]$ 减少了 $1$。

如果 $\textit{left}=0$，表示上一批顾客购买完后没有剩下甜甜圈，那么这一批顾客会开心。

借助 Python 的 `@cache` 装饰器和 `tuple` 类型，这道难度分高达 $2559$ 的题目就被拿下了。~~不会位运算的人直接逃课~~

```py
class Solution:
    def maxHappyGroups(self, m: int, groups: List[int]) -> int:
        cnt = [0] * m
        for x in groups:
            cnt[x % m] += 1

        @cache  # 记忆化
        def dfs(left: int, cnt: Tuple[int]) -> int:
            res = 0
            cnt = list(cnt)
            for x, c in enumerate(cnt):  # 枚举顾客
                if c:  # cnt[x] > 0
                    cnt[x] -= 1
                    res = max(res, (left == 0) + dfs((left + x + 1) % m, tuple(cnt)))  # x 从 0 开始，这里要 +1
                    cnt[x] += 1
            return res
        return cnt[0] + dfs(0, tuple(cnt[1:]))  # 转成 tuple 这样能记忆化
```

### 进一步优化

把 $0$ 排在前面后，剩下的顾客，尽可能地两两一对安排。例如把 $1$ 和 $m-1$ 排在一起，$2$ 和 $m-2$ 排在一起，……，每一对中都有一批开心的顾客。

这样安排后，剩余没有配对的顾客，就至多有 $4$ 种了，例如 $m=9$，只剩下模 $m$ 结果在 $[1,4]$ 中的，就无法两两一对（这里是举例说明，不是说剩余的一定在 $[1,4]$ 中）。

这样一个小小的优化就可以让上面的 Python 代码跑的飞快。

那么其他语言呢？虽然你也可以想办法把数组当成 key，但是用位运算操作起来会高效一些。

### 位运算魔法

既然剩下的顾客至多有 $4$ 种，而 $\textit{cnt}[x] \le 30$，因此可以用 $5$ 个比特表示顾客 $x$ 的个数，这一共需要 $4\times 5=20$ 个比特，对于 `int` 类型是足够的，用一个变量 $\textit{mask}$ 表示。进一步地，干脆把 $\textit{left}$ 也加到 $\textit{mask}$ 中，由于 $\textit{left}<9$，用 $4$ 个比特足矣，因此一共需要 $20+4=24$ 个比特，同样可以用 `int` 装下。

这 $24$ 个比特的具体分布和操作方法如下：

![1815-2.png](https://pic.leetcode.cn/1674346938-vxqFnV-1815-2.png)

代码实现时，我们还需要用一个数组 $\textit{val}$ 记录这些顾客 $x$。

```Python [sol1-Python3]
# Python 代码只添加优化的逻辑，位运算见其他语言
class Solution:
    def maxHappyGroups(self, m: int, groups: List[int]) -> int:
        ans = 0
        cnt = [0] * m
        for x in groups:
            x %= m
            if x == 0:
                ans += 1  # 直接排在最前面
            elif cnt[-x]:
                cnt[-x] -= 1  # 配对
                ans += 1
            else:
                cnt[x] += 1

        @cache  # 记忆化
        def dfs(left: int, cnt: Tuple[int]) -> int:
            res = 0
            cnt = list(cnt)
            for x, c in enumerate(cnt):  # 枚举顾客
                if c:  # cnt[x] > 0
                    cnt[x] -= 1
                    res = max(res, (left == 0) + dfs((left + x + 1) % m, tuple(cnt)))  # x 从 0 开始，这里要 +1
                    cnt[x] += 1
            return res
        return ans + dfs(0, tuple(cnt[1:]))  # 转成 tuple 这样能记忆化
```

```java [sol1-Java]
class Solution {
    private int m;
    private int[] val;
    private final Map<Integer, Integer> cache = new HashMap<>();

    public int maxHappyGroups(int batchSize, int[] groups) {
        m = batchSize;
        int ans = 0;
        int[] cnt = new int[m];
        for (int x : groups) {
            x %= m;
            if (x == 0) ++ans; // 直接排在最前面
            else if (cnt[m - x] > 0) {
                --cnt[m - x]; // 配对
                ++ans;
            } else ++cnt[x];
        }

        int mask = 0, n = 0;
        for (int c : cnt) if (c > 0) ++n;
        val = new int[n];
        for (int x = 1; x < m; ++x)
            if (cnt[x] > 0) {
                val[--n] = x; // val 越靠后的，在 mask 中的比特位越高
                mask = mask << 5 | cnt[x];
            }

        return ans + dfs(mask);
    }

    private int dfs(int mask) {
        if (cache.containsKey(mask)) return cache.get(mask);
        int res = 0, left = mask >> 20, msk = mask & ((1 << 20) - 1);
        for (int i = 0, j = 0; i < val.length; ++i, j += 5) // 枚举顾客
            if ((msk >> j & 31) > 0) // cnt[val[i]] > 0
                res = Math.max(res, (left == 0 ? 1 : 0) + dfs((left + val[i]) % m << 20 | msk - (1 << j)));
        cache.put(mask, res);
        return res;
    }
}
```

```cpp [sol1-C++]
class Solution {
    int m;
    vector<int> val;
    unordered_map<int, int> cache;

    int dfs(int mask) {
        if (cache.count(mask)) return cache[mask];
        int res = 0, left = mask >> 20, msk = mask & ((1 << 20) - 1);
        for (int i = 0, j = 0; i < val.size(); ++i, j += 5) // 枚举顾客
            if (msk >> j & 31) // cnt[val[i]] > 0
                res = max(res, (left == 0) + dfs((left + val[i]) % m << 20 | msk - (1 << j)));
        return cache[mask] = res;
    }

public:
    int maxHappyGroups(int batchSize, vector<int> &groups) {
        m = batchSize;
        int ans = 0;
        vector<int> cnt(m);
        for (int x: groups) {
            x %= m;
            if (x == 0) ++ans; // 直接排在最前面
            else if (cnt[m - x]) {
                --cnt[m - x]; // 配对
                ++ans;
            } else ++cnt[x];
        }

        int mask = 0;
        for (int x = 1; x < m; ++x)
            if (cnt[x]) {
                val.push_back(x);
                mask = mask << 5 | cnt[x];
            }
        // 上面加入 val 的顺序和拼接 mask 的顺序是相反的，reverse 后就对上了
        reverse(val.begin(), val.end());

        return ans + dfs(mask);
    }
};
```

```go [sol1-Go]
func maxHappyGroups(m int, groups []int) (ans int) {
    cnt := make([]int, m)
    for _, x := range groups {
        x %= m
        if x == 0 {
            ans++ // 直接排在最前面
        } else if cnt[m-x] > 0 {
            cnt[m-x]-- // 配对
            ans++
        } else {
            cnt[x]++
        }
    }

    val, mask := []int{}, 0
    for x, c := range cnt {
        if c > 0 {
            val = append(val, x)
            mask = mask<<5 | c
        }
    }
    // 上面加入 val 的顺序和拼接 mask 的顺序是相反的，reverse 后就对上了
    for i, n := 0, len(val); i < n/2; i++ {
        val[i], val[n-1-i] = val[n-1-i], val[i]
    }

    // 直接用 pair 当作 key，更方便
    type pair struct{ left, mask int }
    cache := map[pair]int{}
    var dfs func(int, int) int
    dfs = func(left, mask int) (res int) {
        p := pair{left, mask}
        if v, ok := cache[p]; ok {
            return v
        }
        for i, x := range val { // 枚举顾客
            i *= 5
            if mask>>i&31 > 0 { // cnt[x] > 0
                r := dfs((left+x)%m, mask-1<<i)
                if left == 0 {
                    r++
                }
                res = max(res, r)
            }
        }
        cache[p] = res
        return
    }
    return ans + dfs(0, mask)
}

func max(a, b int) int { if b > a { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O\left(m^2\left(\dfrac{n}{m}\right)^{m/2}\right)$，其中 $n$ 为 $\textit{groups}$ 的长度，$m=\textit{batchSize}$。最坏情况下没有两个一对的，例如 $\textit{groups}$ 中只有 $[1,4]$ 中的数。根据基本不等式，把 $30$ 平分成 $8+8+7+7$ 是最优的，那么至多有 $9\times(8\times8\times7\times7)=28224$ 个状态，每个状态执行至多 $4$ 次循环，因此记忆化搜索的计算量至多为 $28224\times 4 = 112896$。
- 空间复杂度：$O\left(m\left(\dfrac{n}{m}\right)^{m/2}\right)$。空间复杂度主要取决于状态个数。

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
