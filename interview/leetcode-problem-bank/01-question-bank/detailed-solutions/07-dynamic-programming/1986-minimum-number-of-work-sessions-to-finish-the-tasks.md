# 1986. 完成任务的最少工作时间段

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-work-sessions-to-finish-the-tasks/
- 题目 slug：`minimum-number-of-work-sessions-to-finish-the-tasks`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP
- 难度分：1995
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-work-sessions-to-finish-the-tasks/solutions/964986/zi-ji-dong-tai-gui-hua-by-endlesscheng-wtua/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[子集动态规划](https://leetcode.cn/problems/minimum-number-of-work-sessions-to-finish-the-tasks/solutions/964986/zi-ji-dong-tai-gui-hua-by-endlesscheng-wtua/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zi-ji-dong-tai-gui-hua-by-endlesscheng-wtua`
- topic id：`964986`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

定义状态 $f[s]$ 表示完成任务集合 $s$ 的所有任务所需要的最少数目的工作时间段。

枚举 $s$ 的子集 $\textit{sub}$，若完成 $\textit{sub}$ 的所有任务耗时不超过 $\textit{sessionTime}$，则可以将 $\textit{sub}$ 的所有任务用一个工作时间段来完成，也就是将 $f[s\setminus\textit{sub}]+1$ 转移到 $f[s]$ 上，二者取最小值。这里 $s\setminus\textit{sub}$ 表示从 $s$ 中去掉 $\textit{sub}$ 的剩余集合。

通过预处理 $\textit{tasks}$ 的每个子集的子集和，可以做到 $O(1)$ 判断 $\textit{sub}$ 的所有任务耗时不超过 $\textit{sessionTime}$。

时间复杂度：$O(2^n+3^n)$。预处理子集和耗时 $O(2^n)$；状态转移次数为 $s$ 的每个子集的子集个数之和，由于元素个数为 $k$ 的集合有 $C(n,k)$ 个，其子集有 $2^k$ 个，根据二项式定理，$\sum C(n,k)2^k = (2+1)^n = 3^n$，故状态转移耗时 $O(3^n)$。

相似题目：

- [1494. 并行课程 II](https://leetcode.cn/problems/parallel-courses-ii/)
- [1655. 分配重复整数](https://leetcode.cn/problems/distribute-repeating-integers)

```C++ [sol-C++]
class Solution {
public:
    int minSessions(vector<int> &tasks, int sessionTime) {
        int n = tasks.size();
        int m = 1 << n;
        // 预处理所有子集的子集和，复杂度 O(1+2+4+...+2^(n-1)) = O(2^n)
        vector<int> sum(m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0, k = 1 << i; j < k; ++j) {
                sum[j | k] = sum[j] + tasks[i];
            }
        }
        vector<int> f(m, n);
        f[0] = 0;
        for (int s = 0; s < m; ++s) {
            // 枚举 s 的所有子集 sub，若 sub 耗时不超过 sessionTime，则将 f[s^sub]+1 转移到 f[s] 上
            for (int sub = s; sub; sub = (sub - 1) & s) {
                if (sum[sub] <= sessionTime) {
                    f[s] = min(f[s], f[s ^ sub] + 1);
                }
            }
        }
        return f[m - 1];
    }
};
```

```go [sol-Go]
func minSessions(tasks []int, sessionTime int) (ans int) {
	n := len(tasks)
	m := 1 << n
	// 预处理所有子集的子集和，复杂度 O(1+2+4+...+2^(n-1)) = O(2^n)
	sum := make([]int, m)
	for i, t := range tasks {
		for j, k := 0, 1<<i; j < k; j++ {
			sum[j|k] = sum[j] + t
		}
	}
	f := make([]int, m)
	for i := range f {
		f[i] = n
	}
	f[0] = 0
	for s := range f {
		// 枚举 s 的所有子集 sub，若 sub 耗时不超过 sessionTime，则将 f[s^sub]+1 转移到 f[s] 上
		for sub := s; sub > 0; sub = (sub - 1) & s {
			if sum[sub] <= sessionTime && f[s^sub]+1 < f[s] {
				f[s] = f[s^sub] + 1
			}
		}
	}
	return f[m-1]
}
```
