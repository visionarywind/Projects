# 166. 分数到小数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/fraction-to-recurring-decimal/
- 题目 slug：`fraction-to-recurring-decimal`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/fraction-to-recurring-decimal/solutions/3790535/mo-ni-chang-chu-fa-pythonjavacgojsrust-b-di8h/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[模拟长除法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/fraction-to-recurring-decimal/solutions/3790535/mo-ni-chang-chu-fa-pythonjavacgojsrust-b-di8h/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ni-chang-chu-fa-pythonjavacgojsrust-b-di8h`
- topic id：`3790535`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

我们可以用长除法计算小数。来看两个例子。

## 例一：9/8 = 1.125

读者可以先在纸上算算 $9/8$，方便理解下述流程。

整数部分为 $\left\lfloor\dfrac{9}{8}\right\rfloor = 1$，初始余数为 $r = 9\bmod 8 = 1$。

1. $r=1$。计算商 $\left\lfloor\dfrac{r\cdot 10}{8}\right\rfloor = 1$，得到小数点后第一位，更新 $r$ 为 $(r\cdot 10)\bmod 8 = 2$。
2. $r=2$。计算商 $\left\lfloor\dfrac{r\cdot 10}{8}\right\rfloor = 2$，得到小数点后第二位，更新 $r$ 为 $(r\cdot 10)\bmod 8 = 4$。
3. $r=4$。计算商 $\left\lfloor\dfrac{r\cdot 10}{8}\right\rfloor = 5$，得到小数点后第三位，更新 $r$ 为 $(r\cdot 10)\bmod 8 = 0$。
4. $r=0$，说明 $9/8$ 是有限小数。

## 例二：3/14 = 0.2(142857)

整数部分为 $\left\lfloor\dfrac{3}{14}\right\rfloor = 0$，初始余数为 $r = 3\bmod 14 = 3$。

1. $r=3$。计算商 $\left\lfloor\dfrac{r\cdot 10}{14}\right\rfloor = 2$，得到小数点后第一位，更新 $r$ 为 $(r\cdot 10)\bmod 14 = 2$。
2. $r=2$。计算商 $\left\lfloor\dfrac{r\cdot 10}{14}\right\rfloor = 1$，得到小数点后第二位，更新 $r$ 为 $(r\cdot 10)\bmod 14 = 6$。
3. $r=6$。计算商 $\left\lfloor\dfrac{r\cdot 10}{14}\right\rfloor = 4$，得到小数点后第三位，更新 $r$ 为 $(r\cdot 10)\bmod 14 = 4$。
4. $r=4$。计算商 $\left\lfloor\dfrac{r\cdot 10}{14}\right\rfloor = 2$，得到小数点后第四位，更新 $r$ 为 $(r\cdot 10)\bmod 14 = 12$。
5. $r=12$。计算商 $\left\lfloor\dfrac{r\cdot 10}{14}\right\rfloor = 8$，得到小数点后第五位，更新 $r$ 为 $(r\cdot 10)\bmod 14 = 8$。
6. $r=8$。计算商 $\left\lfloor\dfrac{r\cdot 10}{14}\right\rfloor = 5$，得到小数点后第六位，更新 $r$ 为 $(r\cdot 10)\bmod 14 = 10$。
7. $r=10$。计算商 $\left\lfloor\dfrac{r\cdot 10}{14}\right\rfloor = 7$，得到小数点后第七位，更新 $r$ 为 $(r\cdot 10)\bmod 14 = 2$。
8. $r=2$，等于第 2 步开始时的余数。如果继续计算，我们会重复上面的第 2~7 步。这意味着我们找到了循环节。

根据 $r=2$ 首次出现的位置，可以知道循环节之前的小数为 $2$，循环节为 $142857$。

## 怎么知道进入循环了？

用一个哈希表记录，哈希表的 key 是余数 $r$，value 是这个余数对应着第几位小数。

计算商（添加到答案），更新 $r$ 后：

- 如果 $r$ 在哈希表中，说明有循环节，根据哈希表中记录的小数位置，可以得到循环节之前的小数，以及循环节的内容。
- 如果 $r$ 不在哈希表中，往哈希表中插入 $r$ 以及此时我们在算第几位小数。
- 特别地，如果 $r=0$，说明没有循环节，退出循环。

```py [sol-Python3]
class Solution:
    def fractionToDecimal(self, numerator: int, denominator: int) -> str:
        sign = '-' if numerator * denominator < 0 else ''
        numerator = abs(numerator)  # 保证下面的计算过程不产生负数
        denominator = abs(denominator)

        # 计算整数部分 q 和初始余数 r
        q, r = divmod(numerator, denominator)
        if r == 0:  # 没有小数部分
            return sign + str(q)

        ans = [sign + str(q) + '.']
        r_to_pos = {r: 1}  # 初始余数对应小数点后第一位
        while r:
            # 计算小数点后的数字 q，更新 r
            q, r = divmod(r * 10, denominator)
            ans.append(str(q))
            if r in r_to_pos:  # 有循环节
                pos = r_to_pos[r]  # 循环节的开始位置
                return f"{''.join(ans[:pos])}({''.join(ans[pos:])})"
            r_to_pos[r] = len(ans)  # 记录余数对应位置
        return ''.join(ans)  # 有限小数
```

```java [sol-Java]
class Solution {
    public String fractionToDecimal(int numerator, int denominator) {
        long a = numerator;
        long b = denominator;
        String sign = a * b < 0 ? "-" : "";
        a = Math.abs(a); // 保证下面的计算过程不产生负数
        b = Math.abs(b);

        // 计算整数部分 q 和初始余数 r
        long q = a / b;
        long r = a % b;
        if (r == 0) { // 没有小数部分
            return sign + q;
        }

        StringBuilder ans = new StringBuilder(sign).append(q).append('.');
        Map<Long, Integer> rToPos = new HashMap<>();
        rToPos.put(r, ans.length()); // 记录初始余数对应位置
        while (r > 0) {
            // 计算小数点后的数字 q，更新 r
            r *= 10;
            q = r / b;
            r %= b;
            ans.append(q);
            if (rToPos.containsKey(r)) { // 有循环节
                int pos = rToPos.get(r); // 循环节的开始位置
                return ans.substring(0, pos) + "(" + ans.substring(pos) + ")";
            }
            rToPos.put(r, ans.length()); // 记录余数对应位置
        }
        return ans.toString(); // 有限小数
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string fractionToDecimal(int numerator, int denominator) {
        long long a = numerator, b = denominator;
        string sign = a * b < 0 ? "-" : "";
        a = abs(a); // 保证下面的计算过程不产生负数
        b = abs(b);

        // 计算整数部分 q 和初始余数 r
        long long q = a / b, r = a % b;
        if (r == 0) { // 没有小数部分
            return sign + to_string(q);
        }

        string ans = sign + to_string(q) + ".";
        unordered_map<long long, int> r_to_pos = {{r, ans.size()}}; // 记录初始余数对应位置
        while (r) {
            // 计算小数点后的数字 q，更新 r
            r *= 10;
            q = r / b;
            r %= b;
            ans += '0' + q;
            if (r_to_pos.contains(r)) { // 有循环节
                int pos = r_to_pos[r]; // 循环节的开始位置
                return ans.substr(0, pos) + "(" + ans.substr(pos) + ")";
            }
            r_to_pos[r] = ans.size(); // 记录余数对应位置
        }
        return ans; // 有限小数
    }
};
```

```go [sol-Go]
func fractionToDecimal(numerator, denominator int) string {
    sign := ""
    if numerator*denominator < 0 {
        sign = "-"
    }
    numerator = abs(numerator) // 保证下面的计算过程不产生负数
    denominator = abs(denominator)

    // 计算整数部分 q 和初始余数 r
    q, r := numerator/denominator, numerator%denominator
    if r == 0 { // 没有小数部分
        return sign + strconv.Itoa(q)
    }

    ans := []byte(sign + strconv.Itoa(q) + ".")
    rToPos := map[int]int{r: len(ans)} // 记录初始余数对应位置
    for r != 0 {
        // 计算小数点后的数字 q，更新 r
        r *= 10
        q = r / denominator
        r %= denominator
        ans = append(ans, '0'+byte(q))
        if pos, ok := rToPos[r]; ok { // 有循环节，pos 为循环节的开始位置
            return string(ans[:pos]) + "(" + string(ans[pos:]) + ")"
        }
        rToPos[r] = len(ans) // 记录余数对应位置
    }
    return string(ans) // 有限小数
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

```js [sol-JavaScript]
var fractionToDecimal = function(numerator, denominator) {
    const sign = numerator * denominator < 0 ? "-" : "";
    numerator = Math.abs(numerator); // 保证下面的计算过程不产生负数
    denominator = Math.abs(denominator);

    // 计算整数部分 q 和初始余数 r
    let q = Math.floor(numerator / denominator);
    let r = numerator % denominator;
    if (r === 0) { // 没有小数部分
        return sign + String(q);
    }

    const ans = [sign + String(q) + "."];
    const r_to_pos = new Map();
    r_to_pos.set(r, 1); // 初始余数对应小数点后第一位
    while (r) {
        // 计算小数点后的数字 q，更新 r
        r *= 10;
        q = Math.floor(r / denominator);
        r = r % denominator;
        ans.push(String(q));
        if (r_to_pos.has(r)) { // 有循环节
            const pos = r_to_pos.get(r); // 循环节的开始位置
            return ans.slice(0, pos).join("") + "(" + ans.slice(pos).join("") + ")";
        }
        r_to_pos.set(r, ans.length); // 记录余数对应位置
    }
    return ans.join(""); // 有限小数
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn fraction_to_decimal(numerator: i32, denominator: i32) -> String {
        let mut a = numerator as i64;
        let mut b = denominator as i64;
        let sign = if a * b < 0 { "-" } else { "" };
        a = a.abs(); // 保证下面的计算过程不产生负数
        b = b.abs();

        // 计算整数部分 q 和初始余数 r
        let mut q = a / b;
        let mut r = a % b;
        if r == 0 { // 没有小数部分
            return format!("{}{}", sign, q);
        }

        let mut ans = format!("{}{}.", sign, q);
        let mut r_to_pos = HashMap::new();
        r_to_pos.insert(r, ans.len()); // 记录初始余数对应位置
        while r != 0 {
            // 计算小数点后的数字 q，更新 r
            r *= 10;
            q = r / b;
            r %= b;
            ans.push((b'0' + q as u8) as char);
            if let Some(&pos) = r_to_pos.get(&r) { // 有循环节，pos 为循环节的开始位置
                return format!("{}({})", &ans[..pos], &ans[pos..]);
            }
            r_to_pos.insert(r, ans.len()); // 记录余数对应位置
        }
        ans // 有限小数
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(N)$。其中 $N = \min(|\textit{denominator}|, 10^4)$。至多有 $|\textit{denominator}|$ 个不同的余数，最多循环 $\mathcal{O}(|\textit{denominator}|)$ 次。不过，本题保证答案长度小于 $10^4$。
- 空间复杂度：$\mathcal{O}(N)$。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、杂项 / §7.10 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.10 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
