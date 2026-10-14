# 682. 棒球比赛

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/baseball-game/
- 题目 slug：`baseball-game`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/baseball-game/solutions/2861078/yong-lie-biao-mo-ni-pythonjavaccgojsrust-pj1t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[用列表模拟（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/baseball-game/solutions/2861078/yong-lie-biao-mo-ni-pythonjavaccgojsrust-pj1t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-lie-biao-mo-ni-pythonjavaccgojsrust-pj1t`
- topic id：`2861078`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

遍历 $\textit{operations}$，用列表（或者栈）模拟题目的四个操作：

1. 操作一，把 $x$ 加到列表末尾。
2. 操作二，把列表的后两项之和加到列表末尾。
3. 操作三，把列表最后一项的两倍加到列表末尾。
4. 操作四，去掉列表最后一项。

遍历结束后，返回列表中的元素之和。

```py [sol-Python3]
class Solution:
    def calPoints(self, operations: List[str]) -> int:
        st = []
        for op in operations:
            if op == '+':
                st.append(st[-2] + st[-1])
            elif op == 'D':
                st.append(st[-1] * 2)
            elif op == 'C':
                st.pop()
            else:
                st.append(int(op))
        return sum(st)
```

```java [sol-Java]
class Solution {
    public int calPoints(String[] operations) {
        List<Integer> st = new ArrayList<>();
        for (String op : operations) {
            switch (op.charAt(0)) {
                case '+':
                    st.add(st.get(st.size() - 2) + st.get(st.size() - 1));
                    break;
                case 'D':
                    st.add(st.get(st.size() - 1) * 2);
                    break;
                case 'C':
                    st.remove(st.size() - 1);
                    break;
                default:
                    st.add(Integer.parseInt(op));
            }
        }
        int sum = 0;
        for (int x : st) {
            sum += x;
        }
        return sum;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int calPoints(String[] operations) {
        int[] st = new int[operations.length];
        int m = 0;
        for (String op : operations) {
            switch (op.charAt(0)) {
                case '+':
                    st[m] = st[m - 2] + st[m - 1];
                    m++;
                    break;
                case 'D':
                    st[m] = st[m - 1] * 2;
                    m++;
                    break;
                case 'C':
                    m--;
                    break;
                default:
                    st[m++] = Integer.parseInt(op);
            }
        }
        int sum = 0;
        for (int i = 0; i < m; i++) {
            sum += st[i];
        }
        return sum;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int calPoints(vector<string>& operations) {
        vector<int> st;
        for (auto& op : operations) {
            switch (op[0]) {
                case '+':
                    st.push_back(st[st.size() - 2] + st.back());
                    break;
                case 'D':
                    st.push_back(st.back() * 2);
                    break;
                case 'C':
                    st.pop_back();
                    break;
                default:
                    st.push_back(stoi(op));
            }
        }
        return reduce(st.begin(), st.end());
    }
};
```

```c [sol-C]
int calPoints(char** operations, int operationsSize) {
    int* st = malloc(operationsSize * sizeof(int));
    int m = 0;
    for (int i = 0; i < operationsSize; i++) {
        switch (operations[i][0]) {
            case '+':
                st[m] = st[m - 2] + st[m - 1];
                m++;
                break;
            case 'D':
                st[m] = st[m - 1] * 2;
                m++;
                break;
            case 'C':
                m--;
                break;
            default:
                st[m++] = atoi(operations[i]);
        }
    }
    int sum = 0;
    for (int i = 0; i < m; i++) {
        sum += st[i];
    }
    free(st);
    return sum;
}
```

```go [sol-Go]
func calPoints(operations []string) (sum int) {
    st := []int{}
    for _, op := range operations {
        switch op[0] {
        case '+':
            st = append(st, st[len(st)-2]+st[len(st)-1])
        case 'D':
            st = append(st, st[len(st)-1]*2)
        case 'C':
            st = st[:len(st)-1]
        default:
            x, _ := strconv.Atoi(op)
            st = append(st, x)
        }
    }
    for _, x := range st {
        sum += x
    }
    return
}
```

```js [sol-JavaScript]
var calPoints = function(operations) {
    const st = [];
    for (const op of operations) {
        switch (op) {
            case "+":
                st.push(st[st.length - 2] + st[st.length - 1]);
                break;
            case "D":
                st.push(st[st.length - 1] * 2);
                break;
            case "C":
                st.pop();
                break;
            default:
                st.push(parseInt(op));
        }
    }
    return _.sum(st);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn cal_points(operations: Vec<String>) -> i32 {
        let mut st = vec![];
        for op in operations {
            match op.as_bytes()[0] {
                b'+' => st.push(st[st.len() - 2] + st[st.len() - 1]),
                b'D' => st.push(st[st.len() - 1] * 2),
                b'C' => { st.pop(); }
                _ => st.push(op.parse::<i32>().unwrap())
            }
        }
        st.iter().sum()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{operations}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面数据结构题单中的「**栈**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
