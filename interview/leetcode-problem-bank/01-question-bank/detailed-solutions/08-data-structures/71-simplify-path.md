# 71. 简化路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/simplify-path/
- 题目 slug：`simplify-path`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/simplify-path/solutions/3042801/zhan-mo-ni-pythonjavacgojsrust-by-endles-y4bc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[栈模拟，简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/simplify-path/solutions/3042801/zhan-mo-ni-pythonjavacgojsrust-by-endles-y4bc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhan-mo-ni-pythonjavacgojsrust-by-endles-y4bc`
- topic id：`3042801`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

给你一组由 `/` 隔开的字符串（忽略空串和 `.`），请你从左到右遍历这些字符串，依次删除每个 `..` 及其左侧的字符串（模拟返回上一级目录）。

## 思路

把 $\textit{path}$ 用 `/` 分割，得到一个字符串列表。

遍历字符串列表的同时，用栈维护遍历过的字符串：

- 如果当前字符串是空串或者 `.`，什么也不做（跳过）。
- 如果当前字符串不是 `..`，那么把字符串入栈。
- 否则弹出栈顶字符串（前提是栈不为空），模拟返回上一级目录。

最后把栈中字符串用 `/` 拼接起来（最前面也要有个 `/`）。

```py [sol-Python3]
class Solution:
    def simplifyPath(self, path: str) -> str:
        stk = []
        for s in path.split('/'):
            if s == "" or s == ".":
                continue
            if s != "..":
                stk.append(s)
            elif stk:
                stk.pop()
        return '/' + '/'.join(stk)
```

```java [sol-Java]
class Solution {
    public String simplifyPath(String path) {
        List<String> stk = new ArrayList<>();
        for (String s : path.split("/")) {
            if (s.isEmpty() || s.equals(".")) {
                continue;
            }
            if (!s.equals("..")) {
                stk.add(s);
            } else if (!stk.isEmpty()) {
                stk.removeLast(); // stk.remove(stk.size() - 1);
            }
        }
        return "/" + String.join("/", stk);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string simplifyPath(string path) {
        vector<string> stk;
        istringstream ss(path); // 使用 views 的代码见【C++ views】
        string s;
        while (getline(ss, s, '/')) {
            if (s.empty() || s == ".") {
                continue;
            }
            if (s != "..") {
                stk.push_back(s);
            } else if (!stk.empty()) {
                stk.pop_back();
            }
        }

        string ans;
        for (string& s : stk) {
            ans += '/';
            ans += s;
        }
        return stk.empty() ? "/" : ans;
    }
};
```

```cpp [sol-C++ views]
class Solution {
public:
    string simplifyPath(string path) {
        vector<string_view> stk;
        for (auto&& part : path | views::split('/')) {
            string_view s(part.begin(), part.end());
            if (s.empty() || s == ".") {
                continue;
            }
            if (s != "..") {
                stk.push_back(s);
            } else if (!stk.empty()) {
                stk.pop_back();
            }
        }
        return "/" + (stk | views::join_with('/') | ranges::to<string>());
    }
};
```

```go [sol-Go]
func simplifyPath(path string) string {
    stk := []string{}
    for _, s := range strings.Split(path, "/") {
        if s == "" || s == "." {
            continue
        }
        if s != ".." {
            stk = append(stk, s)
        } else if len(stk) > 0 {
            stk = stk[:len(stk)-1]
        }
    }
    return "/" + strings.Join(stk, "/")
}
```

```js [sol-JavaScript]
var simplifyPath = function(path) {
    const stk = [];
    for (const s of path.split('/')) {
        if (s === '' || s === '.') {
            continue;
        }
        if (s !== '..') {
            stk.push(s);
        } else if (stk.length > 0) {
            stk.pop();
        }
    }
    return '/' + stk.join('/');
};
```

```rust [sol-Rust]
impl Solution {
    pub fn simplify_path(path: String) -> String {
        let mut stk = vec![];
        for s in path.split('/') {
            match s {
                "" | "." => continue,
                ".." => { stk.pop(); },
                _ => stk.push(s),
            }
        }
        format!("/{}", stk.join("/"))
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{path}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面数据结构题单中的「**三、栈**」。

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
