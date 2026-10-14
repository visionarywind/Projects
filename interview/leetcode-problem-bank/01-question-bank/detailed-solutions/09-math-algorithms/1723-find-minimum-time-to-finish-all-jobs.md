# 1723. 完成所有工作的最短时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-minimum-time-to-finish-all-jobs/
- 题目 slug：`find-minimum-time-to-finish-all-jobs`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.5 快速沃尔什变换（FWT）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-minimum-time-to-finish-all-jobs/solutions/1606001/by-endlesscheng-d2oa/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[如何用 Python 的状压 DP 递推写法通过此题](https://leetcode.cn/problems/find-minimum-time-to-finish-all-jobs/solutions/1606001/by-endlesscheng-d2oa/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-d2oa`
- topic id：`1606001`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

算法讲解见 [我的另一篇题解](https://leetcode.cn/problems/fair-distribution-of-cookies/solution/by-endlesscheng-80ao/)，这里只说如何用 Python 写状压 DP 递推并顺利通过此题。

如果直接把上面题解中的代码搬过来提交是会超时的，需要加上一些优化手段才能 AC。

优化 1：把 `min` 和 `max` 拆开，改为手写，避免额外的函数调用开销。

优化 2：预处理每个二进制数的子集。

```py
subsets = [[] for _ in range(1 << 12)]
for i in range(1 << 12):
    s = i
    while s:
        subsets[i].append(s)
        s = (s - 1) & i

class Solution:
    def minimumTimeRequired(self, cookies: List[int], k: int) -> int:
        m = 1 << len(cookies)
        SUM = [0] * m
        for i, v in enumerate(cookies):
            bit = 1 << i
            for j in range(bit):
                SUM[bit | j] = SUM[j] + v

        f = SUM.copy()
        for _ in range(1, k):
            for j in range(m - 1, 0, -1):
                for s in subsets[j]:
                    v = f[j ^ s]
                    if SUM[s] > v: v = SUM[s]  # 不要用 max 和 min，那样会有额外的函数调用开销
                    if v < f[j]: f[j] = v
        return f[-1]
```
