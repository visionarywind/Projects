# 3044. 出现频率最高的质数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/most-frequent-prime/
- 题目 slug：`most-frequent-prime`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.1 判断质数
- 难度分：1737
- 外部题解来源：https://leetcode.cn/problems/most-frequent-prime/solutions/2644161/mei-ju-pythonjavacgo-by-endlesscheng-enj0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举（Python/Java/C++/Go）](https://leetcode.cn/problems/most-frequent-prime/solutions/2644161/mei-ju-pythonjavacgo-by-endlesscheng-enj0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-pythonjavacgo-by-endlesscheng-enj0`
- topic id：`2644161`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

对于每个单元格，枚举八个方向，生成数字，统计其中质数个数。

最后返回出现次数最多的质数，如果有多个这样的质数，返回最大的那个。

```py [sol-Python3]
class Solution:
    def mostFrequentPrime(self, mat: List[List[int]]) -> int:
        m, n = len(mat), len(mat[0])
        cnt = Counter()
        for i, row in enumerate(mat):
            for j, v in enumerate(row):
                for dx, dy in (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1), (0, -1), (1, -1):
                    x, y, val = i + dx, j + dy, v
                    while 0 <= x < m and 0 <= y < n:
                        val = val * 10 + mat[x][y]
                        # 如果 val 在 cnt 中，那么 val 一定是质数
                        if val in cnt or self.is_prime(val):
                            cnt[val] += 1
                        x += dx
                        y += dy

        ans, max_cnt = -1, 0
        for v, c in cnt.items():
            if c > max_cnt:
                ans, max_cnt = v, c
            elif c == max_cnt:
                ans = max(ans, v)
        return ans

    def is_prime(self, n: int) -> bool:
        return all(n % i for i in range(2, isqrt(n) + 1))
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

    public int mostFrequentPrime(int[][] mat) {
        int m = mat.length;
        int n = mat[0].length;
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                for (int[] d : DIRS) {
                    int x = i + d[0];
                    int y = j + d[1];
                    int v = mat[i][j];
                    while (x >= 0 && x < m && y >= 0 && y < n) {
                        v = v * 10 + mat[x][y];
                        if (isPrime(v)) {
                            cnt.merge(v, 1, Integer::sum);
                        }
                        x += d[0];
                        y += d[1];
                    }
                }
            }
        }

        int ans = -1;
        int maxCnt = 0;
        for (Map.Entry<Integer, Integer> e : cnt.entrySet()) {
            int v = e.getKey();
            int c = e.getValue();
            if (c > maxCnt) {
                ans = v;
                maxCnt = c;
            } else if (c == maxCnt) {
                ans = Math.max(ans, v);
            }
        }
        return ans;
    }

    private boolean isPrime(int n) {
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int dirs[8][2] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

    bool is_prime(int n) {
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        return true;
    }

public:
    int mostFrequentPrime(vector<vector<int>> &mat) {
        int m = mat.size(), n = mat[0].size();
        unordered_map<int, int> cnt;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                for (auto &d: dirs) {
                    int x = i + d[0], y = j + d[1], v = mat[i][j];
                    while (x >= 0 && x < m && y >= 0 && y < n) {
                        v = v * 10 + mat[x][y];
                        // 如果 v 在 cnt 中，那么 v 一定是质数
                        if (cnt.contains(v) || is_prime(v)) {
                            cnt[v]++;
                        }
                        x += d[0];
                        y += d[1];
                    }
                }
            }
        }

        int ans = -1, max_cnt = 0;
        for (auto &[v, c]: cnt) {
            if (c > max_cnt) {
                ans = v;
                max_cnt = c;
            } else if (c == max_cnt) {
                ans = max(ans, v);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func isPrime(n int) bool {
	for i := 2; i*i <= n; i++ {
		if n%i == 0 {
			return false
		}
	}
	return true
}

func mostFrequentPrime(mat [][]int) int {
	dirs := []struct{ x, y int }{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}}
	m, n := len(mat), len(mat[0])
	cnt := map[int]int{}
	for i, row := range mat {
		for j, v := range row {
			for _, d := range dirs {
				x, y, v := i+d.x, j+d.y, v
				for 0 <= x && x < m && 0 <= y && y < n {
					v = v*10 + mat[x][y]
					// 如果 v 在 cnt 中，那么 v 一定是质数
					if cnt[v] > 0 || isPrime(v) {
						cnt[v]++
					}
					x += d.x
					y += d.y
				}
			}
		}
	}

	ans, maxCnt := -1, 0
	for v, c := range cnt {
		if c > maxCnt {
			ans, maxCnt = v, c
		} else if c == maxCnt {
			ans = max(ans, v)
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mnk\cdot 10^{k/2})$，其中 $m$ 和 $n$ 分别为 $\textit{mat}$ 的行数和列数，$k=\max(m,n)$。总共有 $\mathcal{O}(mnk)$ 个数，判断质数需要 $\mathcal{O}(10^{k/2})$ 的时间。
- 空间复杂度：$\mathcal{O}(mnk)$。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.1 判断质数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.1 判断质数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
