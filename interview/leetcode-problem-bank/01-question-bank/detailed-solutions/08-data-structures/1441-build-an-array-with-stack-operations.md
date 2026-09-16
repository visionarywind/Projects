# 1441. 用栈操作构建数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/build-an-array-with-stack-operations/
- 题目 slug：`build-an-array-with-stack-operations`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.1 基础
- 难度分：1180
- 外部题解来源：https://leetcode.cn/problems/build-an-array-with-stack-operations/solutions/3830582/yue-du-li-jie-jian-ji-xie-fa-pythonjavac-nys0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[阅读理解，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/build-an-array-with-stack-operations/solutions/3830582/yue-du-li-jie-jian-ji-xie-fa-pythonjavac-nys0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yue-du-li-jie-jian-ji-xie-fa-pythonjavac-nys0`
- topic id：`3830582`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

看示例 1，$n=3$ 意味着我们会依次读取 $1,2,3$ 这三个数。

- $1$ 在 $\textit{target}$ 中，入栈。
- $2$ 不在 $\textit{target}$ 中，先入栈，再出栈。注意一定要入栈，这是题目要求。
- $3$ 在 $\textit{target}$ 中，入栈。现在栈等于 $\textit{target}$。

怎么判断当前读取的数是否在 $\textit{target}$ 中？

1. 设 $\textit{target}$ 的最后一个数为 $\textit{mx}$。初始化指针 $i=0$，指向 $\textit{target}$ 的第一个数。
2. 枚举读取的数为 $x = 1,2,\ldots,\textit{mx}$。
3. 先把 $x$ 入栈。
4. 如果 $x = \textit{target}[i]$，那么 $x$ 是我们要的数，把 $i$ 加一，指向 $\textit{target}$ 的下一个数。
5. 否则 $x < \textit{target}[i]$，那么 $x$ 不是我们要的数，把 $x$ 出栈。

由于 $\textit{target}$ 是严格递增的，所以 $x \le \textit{target}[i]$ 始终成立。

```py [sol-Python3]
class Solution:
    def buildArray(self, target: List[int], n: int) -> List[str]:
        ans = []
        i = 0
        for x in range(1, target[-1] + 1):
            ans.append("Push")  # 先把 x 入栈（题目要求）
            if x == target[i]:  # x 是我们要的数
                i += 1
            else:  # x 不是我们要的数，出栈
                ans.append("Pop")
        return ans
```

```java [sol-Java]
class Solution {
    public List<String> buildArray(int[] target, int n) {
        List<String> ans = new ArrayList<>();
        int mx = target[target.length - 1];
        int i = 0;
        for (int x = 1; x <= mx; x++) {
            ans.add("Push"); // 先把 x 入栈（题目要求）
            if (x == target[i]) { // x 是我们要的数
                i++;
            } else { // x 不是我们要的数，出栈
                ans.add("Pop");
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<string> buildArray(vector<int>& target, int) {
        vector<string> ans;
        int mx = target.back();
        int i = 0;
        for (int x = 1; x <= mx; x++) {
            ans.push_back("Push"); // 先把 x 入栈（题目要求）
            if (x == target[i]) { // x 是我们要的数
                i++;
            } else { // x 不是我们要的数，出栈
                ans.push_back("Pop");
            }
        }
        return ans;
    }
};
```

```c [sol-C]
char** buildArray(int* target, int targetSize, int, int* returnSize) {
    int mx = target[targetSize - 1];
    char** ans = malloc(mx * 2 * sizeof(char*));
    *returnSize = 0;
    int i = 0;
    for (int x = 1; x <= mx; x++) {
        ans[(*returnSize)++] = "Push"; // 先把 x 入栈（题目要求）
        if (x == target[i]) { // x 是我们要的数
            i++;
        } else { // x 不是我们要的数，出栈
            ans[(*returnSize)++] = "Pop";
        }
    }
    return ans;
}
```

```go [sol-Go]
func buildArray(target []int, _ int) (ans []string) {
	mx := target[len(target)-1]
	i := 0
	for x := 1; x <= mx; x++ {
		ans = append(ans, "Push") // 先把 x 入栈（题目要求）
		if x == target[i] { // x 是我们要的数
			i++
		} else { // x 不是我们要的数，出栈
			ans = append(ans, "Pop")
		}
	}
	return
}
```

```js [sol-JavaScript]
var buildArray = function(target, _) {
    const ans = [];
    const mx = target[target.length - 1];
    let i = 0;
    for (let x = 1; x <= mx; x++) {
        ans.push("Push"); // 先把 x 入栈（题目要求）
        if (x === target[i]) { // x 是我们要的数
            i++;
        } else { // x 不是我们要的数，出栈
            ans.push("Pop");
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn build_array(target: Vec<i32>, _: i32) -> Vec<String> {
        let mut ans = vec![];
        let mut i = 0;
        for x in 1..=*target.last().unwrap() {
            ans.push("Push".to_string()); // 先把 x 入栈（题目要求）
            if x == target[i] { // x 是我们要的数
                i += 1;
            } else { // x 不是我们要的数，出栈
                ans.push("Pop".to_string());
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\textit{mx})$，其中 $\textit{mx}$ 是 $\textit{target}$ 的最后一个数。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

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

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
