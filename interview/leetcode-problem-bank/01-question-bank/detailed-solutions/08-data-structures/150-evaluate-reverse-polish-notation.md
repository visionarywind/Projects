# 150. 逆波兰表达式求值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/evaluate-reverse-polish-notation/
- 题目 slug：`evaluate-reverse-polish-notation`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.5 表达式解析
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/evaluate-reverse-polish-notation/solutions/3830661/li-jie-ti-yi-yong-zhan-mo-ni-pythonjavac-tr9x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[理解题意，用栈模拟（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/evaluate-reverse-polish-notation/solutions/3830661/li-jie-ti-yi-yong-zhan-mo-ni-pythonjavac-tr9x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`li-jie-ti-yi-yong-zhan-mo-ni-pythonjavac-tr9x`
- topic id：`3830661`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

关于逆波兰表达式，可以看题目最下面的解释：

> 用栈操作运算：遇到数字则入栈；遇到算符则取出栈顶两个数字计算，把结果压入栈中。

对于示例 1：

1. 把 $2$ 入栈。
2. 把 $1$ 入栈。
3. 弹出栈顶的两个数，执行 $1+2=3$。然后把 $3$ 入栈。
4. 把 $3$ 入栈。
5. 弹出栈顶的两个数，执行 $3\cdot 3=9$。然后把 $9$ 入栈。
6. 最后答案为栈顶数字。

⚠**注意**：执行减法或者除法运算时，栈顶数字在运算符右侧，栈顶下面的数在运算符左侧。

代码实现时，可以只出栈一个数，然后直接修改栈顶，避免出栈再入栈。

```py [sol-Python3]
class Solution:
    def evalRPN(self, tokens: List[str]) -> int:
        st = []
        for token in tokens:
            if len(token) > 1 or token[0].isdigit():  # token 是数字
                st.append(int(token))
                continue

            x = st.pop()
            if token == '+':
                st[-1] += x
            elif token == '-':
                st[-1] -= x
            elif token == '*':
                st[-1] *= x
            else:
                # 题目要求除法向零取整，但 // 是向下取整
                st[-1] = trunc(st[-1] / x)  # 不用浮点数的写法见【Python3 写法二】
        return st[0]
```

```py [sol-Python3 写法二]
class Solution:
    def evalRPN(self, tokens: List[str]) -> int:
        st = []
        for token in tokens:
            if len(token) > 1 or token[0].isdigit():  # token 是数字
                st.append(int(token))
                continue

            x = st.pop()
            if token == '+':
                st[-1] += x
            elif token == '-':
                st[-1] -= x
            elif token == '*':
                st[-1] *= x
            else:
                y = st[-1]
                st[-1] = y // x if y * x >= 0 else -(-y // x)
        return st[0]
```

```java [sol-Java]
class Solution {
    public int evalRPN(String[] tokens) {
        Deque<Integer> st = new ArrayDeque<>(); // 更快的写法见【Java 数组】
        for (String token : tokens) {
            char c = token.charAt(0);
            if (token.length() > 1 || Character.isDigit(c)) { // token 是数字
                st.push(Integer.parseInt(token));
                continue;
            }

            int x = st.pop();
            int y = st.pop(); // 无法直接修改栈顶
            if (c == '+') {
                st.push(y + x);
            } else if (c == '-') {
                st.push(y - x);
            } else if (c == '*') {
                st.push(y * x);
            } else {
                st.push(y / x);
            }
        }
        return st.peek();
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int evalRPN(String[] tokens) {
        int[] st = new int[(tokens.length + 1) / 2]; // 用数组模拟栈
        int top = -1; // 栈顶下标
        for (String token : tokens) {
            char c = token.charAt(0);
            if (token.length() > 1 || Character.isDigit(c)) { // token 是数字
                st[++top] = Integer.parseInt(token); // 入栈
                continue;
            }

            int x = st[top--]; // 出栈
            if (c == '+') {
                st[top] += x;
            } else if (c == '-') {
                st[top] -= x;
            } else if (c == '*') {
                st[top] *= x;
            } else {
                st[top] /= x;
            }
        }
        return st[top];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int evalRPN(vector<string>& tokens) {
        stack<int> st;
        for (auto& token : tokens) {
            char c = token[0];
            if (token.size() > 1 || isdigit(c)) { // token 是数字
                st.push(stoi(token));
                continue;
            }

            int x = st.top();
            st.pop();
            if (c == '+') {
                st.top() += x;
            } else if (c == '-') {
                st.top() -= x;
            } else if (c == '*') {
                st.top() *= x;
            } else {
                st.top() /= x;
            }
        }
        return st.top();
    }
};
```

```c [sol-C]
int evalRPN(char** tokens, int tokensSize) {
    int* st = malloc((tokensSize + 1) / 2 * sizeof(int));
    int top = -1; // 栈顶下标
    for (int i = 0; i < tokensSize; i++) {
        char* token = tokens[i];
        char c = token[0];
        if (token[1] || isdigit(c)) { // token 是数字
            st[++top] = atoi(token); // 入栈
            continue;
        }

        int x = st[top--]; // 出栈
        if (c == '+') {
            st[top] += x;
        } else if (c == '-') {
            st[top] -= x;
        } else if (c == '*') {
            st[top] *= x;
        } else {
            st[top] /= x;
        }
    }

    int res = st[top];
    free(st);
    return res;
}
```

```go [sol-Go]
func evalRPN(tokens []string) int {
    st := []int{}
    for _, token := range tokens {
        x, err := strconv.Atoi(token)
        if err == nil { // token 是数字
            st = append(st, x)
            continue
        }

        x, st = st[len(st)-1], st[:len(st)-1]
        switch token[0] {
        case '+':
            st[len(st)-1] += x
        case '-':
            st[len(st)-1] -= x
        case '*':
            st[len(st)-1] *= x
        default:
            st[len(st)-1] /= x
        }
    }
    return st[0]
}
```

```js [sol-JS]
var evalRPN = function(tokens) {
    const st = [];
    for (const token of tokens) {
        const num = parseInt(token);
        if (!Number.isNaN(num)) { // token 是数字
            st.push(num);
            continue;
        }

        const x = st.pop();
        const topIdx = st.length - 1;
        if (token === '+') {
            st[topIdx] += x;
        } else if (token === '-') {
            st[topIdx] -= x;
        } else if (token === '*') {
            st[topIdx] *= x;
        } else {
            st[topIdx] = Math.trunc(st[topIdx] / x); // 向零取整
        }
    }
    return st[0];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn eval_rpn(tokens: Vec<String>) -> i32 {
        let mut st = vec![];
        for token in tokens {
            if let Ok(x) = token.parse() { // token 是数字
                st.push(x);
                continue;
            }

            let x = st.pop().unwrap();
            let y = st.last_mut().unwrap();
            *y = match token.as_bytes()[0] {
                b'+' => *y + x,
                b'-' => *y - x,
                b'*' => *y * x,
                _ => *y / x,
            };
        }
        st[0]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nL)$，其中 $n$ 是 $\textit{tokens}$ 的长度，$L\le 4$ 是单个字符串的最大长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数据结构题单的「**§3.5 表达式解析**」。

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
