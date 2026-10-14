# 2591. 将钱分给最多的儿童

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/distribute-money-to-maximum-children/
- 题目 slug：`distribute-money-to-maximum-children`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1531
- 外部题解来源：https://leetcode.cn/problems/distribute-money-to-maximum-children/solutions/2177180/fen-lei-tao-lun-o1-zuo-fa-by-endlesschen-95ef/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论 + O(1) 做法（Python/Java/C++/Go）](https://leetcode.cn/problems/distribute-money-to-maximum-children/solutions/2177180/fen-lei-tao-lun-o1-zuo-fa-by-endlesschen-95ef/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-o1-zuo-fa-by-endlesschen-95ef`
- topic id：`2177180`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

### 本题视频讲解

见[【双周赛 100】](https://www.bilibili.com/video/BV1WM411H7UE/)。

### 思路

首先，每人至少分配 $1$ 美元，把 $\textit{money}$ 减少 $\textit{children}$。

如果 $\textit{money}<0$，返回 $-1$。

然后不断给每个人 $7$ 美元（前面分配了 $1$ 美元），这样可以分给至多

$$
\textit{ans}=\min\left(\left\lfloor\dfrac{\textit{money}}{7}\right\rfloor,\textit{children}\right)
$$

个人。然后更新剩余 $\textit{money}$ 和剩余未分配的人数。

最后，分类讨论：

- 如果剩余 $0$ 人，且 $\textit{money}>0$，那么必须分给一个已经分到 $8$ 美元的人，$\textit{ans}$ 减一。
- 如果剩余 $1$ 人，且 $\textit{money}=3$，为避免分配 $4$ 美元，那么必须分给一个已经分到 $8$ 美元的人，$\textit{ans}$ 减一。（注意输入的 $\textit{children}\ge 2$）
- 其它情况全部给一个人，如果这个人分配到 $4$ 美元，他再给另一个人 $1$ 美元，这样 $\textit{ans}$ 不变。

```py [sol1-Python3]
class Solution:
    def distMoney(self, money: int, children: int) -> int:
        money -= children  # 每人至少 1 美元
        if money < 0: return -1
        ans = min(money // 7, children)  # 初步分配，让尽量多的人分到 8 美元
        money -= ans * 7
        children -= ans
        # children == 0 and money：必须找一个前面分了 8 美元的人，分配完剩余的钱
        # children == 1 and money == 3：不能有人恰好分到 4 美元
        if children == 0 and money or \
           children == 1 and money == 3:
            ans -= 1
        return ans
```

```java [sol1-Java]
class Solution {
    public int distMoney(int money, int children) {
        money -= children; // 每人至少 1 美元
        if (money < 0) return -1;
        int ans = Math.min(money / 7, children); // 初步分配，让尽量多的人分到 8 美元
        money -= ans * 7;
        children -= ans;
        if (children == 0 && money > 0 || // 必须找一个前面分了 8 美元的人，分完剩余的钱
            children == 1 && money == 3) // 不能有人恰好分到 4 美元
            --ans;
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int distMoney(int money, int children) {
        money -= children; // 每人至少 1 美元
        if (money < 0) return -1;
        int ans = min(money / 7, children); // 初步分配，让尽量多的人分到 8 美元
        money -= ans * 7;
        children -= ans;
        if (children == 0 && money || // 必须找一个前面分了 8 美元的人，分完剩余的钱
            children == 1 && money == 3) // 不能有人恰好分到 4 美元
            --ans;
        return ans;
    }
};
```

```go [sol1-Go]
func distMoney(money, children int) int {
	money -= children // 每人至少 1 美元
	if money < 0 {
		return -1
	}
	ans := min(money/7, children) // 初步分配，让尽量多的人分到 8 美元
	money -= ans * 7
	children -= ans
	if children == 0 && money > 0 || // 必须找一个前面分了 8 美元的人，分完剩余的钱
		children == 1 && money == 3 { // 不能有人恰好分到 4 美元
		ans--
	}
	return ans
}

func min(a, b int) int { if a > b { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(1)$。
- 空间复杂度：$O(1)$。仅用到若干额外变量。
