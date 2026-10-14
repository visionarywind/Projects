# 1233. 删除子文件夹

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-sub-folders-from-the-filesystem/
- 题目 slug：`remove-sub-folders-from-the-filesystem`
- 来源专题：常用数据结构
- 来源分类路径：六、字典树（trie） / §6.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-sub-folders-from-the-filesystem/solutions/3727677/pai-xu-pythonjavaccgojsrust-by-endlessch-l0q2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-sub-folders-from-the-filesystem/solutions/3727677/pai-xu-pythonjavaccgojsrust-by-endlessch-l0q2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-pythonjavaccgojsrust-by-endlessch-l0q2`
- topic id：`3727677`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

如果 $B$ 是 $A$ 的子文件夹，那么：

- $A + \texttt{/}$ 是 $B$ 的前缀。

比如 $\texttt{/a/b}$ 是 $\texttt{/a}$ 的子文件夹，其中 $\texttt{/a/}$ 是 $\texttt{/a/b}$ 的前缀。（当然，$\texttt{/a}$ 也是 $\texttt{/a/b}$ 的前缀。）

暴力的想法是，枚举路径 $B$，枚举其他路径 $A$，判断是否满足上述要求，这需要 $\mathcal{O}(\ell\cdot n^2)$ 时间，其中 $\ell\le 100$ 为单个字符串的长度。这太慢了。

可以不枚举 $A$ 吗？

如果 $\textit{folder}$ 列表是有序的（按照字典序从小到大排序），那么当 $B$ 是 $A$ 的子文件夹时，$A$ 是 $B$ 的前缀，所以 $A$ 一定排在 $B$ 的左边。

比如 $\textit{folder} = [\texttt{/a},\texttt{/a/b},\texttt{/c},\texttt{/d}]$：

1. $\texttt{/a}$ 不可能是任何文件夹 $A$ 的子文件夹（否则 $A$ 会排在 $\texttt{/a}$ 前面）。
2. $\texttt{/a/b}$ 是 $\texttt{/a}$ 的子文件夹，删除。
3. $\texttt{/c}$ 不是 $\texttt{/a}$ 的子文件夹。
4. $\texttt{/d}$ 不是 $\texttt{/c}$ 的子文件夹。注意，不需要判断 $\texttt{/d}$ 是否为 $\texttt{/a}$ 的子文件夹。因为一定不是，为什么？如果 $\texttt{/d}$ 是 $\texttt{/a}$ 的子文件夹，由于列表是有序的，$\texttt{/a}$ 的下一个字符串到 $\texttt{/d}$ 之间的字符串都是以 $\texttt{/a/}$ 开头的，所以上一个文件夹 $\texttt{/c}$ 也是以 $\texttt{/a/}$ 开头的，一定是 $\texttt{/a}$ 的子文件夹，一定会被删除，但这与实际矛盾。

因此，排序后，只需比较当前路径与**上一个未被删除的路径**。

代码实现时，为避免 $A + \texttt{/}$ 生成新字符串花费额外时间和空间，判断 $A + \texttt{/}$ 是否为 $B$ 的前缀，可以分解为：

1. 判断 $A$ 是否为 $B$ 的前缀。
2. 判断 $B[m]$ 是否为 $\texttt{/}$，其中 $m$ 是 $A$ 的长度。注意题目保证所有字符串互不相同，所以如果 $A$ 是 $B$ 的前缀，那么 $B$ 的长度一定大于 $A$ 的长度。所以 $B[m]$ 不会下标越界。

```py [sol-Python3]
class Solution:
    def removeSubfolders(self, folder: List[str]) -> List[str]:
        folder.sort()
        ans = [folder[0]]
        for s in folder[1:]:  # 切片可以用 for i in range(1, n) 代替，做到 O(1) 空间
            last = ans[-1]
            if not s.startswith(last) or s[len(last)] != '/':
                ans.append(s)
        return ans
```

```java [sol-Java]
class Solution {
    public List<String> removeSubfolders(String[] folder) {
        Arrays.sort(folder);
        List<String> ans = new ArrayList<>();
        ans.add(folder[0]);
        for (int i = 1; i < folder.length; i++) {
            String s = folder[i];
            String last = ans.getLast();
            if (!s.startsWith(last) || s.charAt(last.length()) != '/') {
                ans.add(s);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<string> removeSubfolders(vector<string>& folder) {
        ranges::sort(folder);
        vector<string> ans = {folder[0]};
        for (int i = 1; i < folder.size(); i++) {
            string& s = folder[i];
            string& last = ans.back();
            if (!s.starts_with(last) || s[last.size()] != '/') {
                ans.emplace_back(s);
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

char** removeSubfolders(char** folder, int folderSize, int* returnSize) {
    qsort(folder, folderSize, sizeof(char*), cmp);

    *returnSize = 1;
    for (int i = 1; i < folderSize; i++) {
        char* s = folder[i];
        char* last = folder[*returnSize - 1];
        int len = strlen(last);
        if (strncmp(last, s, len) != 0 || s[len] != '/') {
            folder[(*returnSize)++] = s; // 原地保存
        }
    }

    return folder;
}
```

```go [sol-Go]
func removeSubfolders(folder []string) []string {
    slices.Sort(folder)
    ans := folder[:1]
    for _, s := range folder[1:] {
        last := ans[len(ans)-1]
        if !strings.HasPrefix(s, last) || s[len(last)] != '/' {
            ans = append(ans, s)
        }
    }
    return ans
}
```

```js [sol-JavaScript]
var removeSubfolders = function(folder) {
    folder.sort();
    const ans = [folder[0]];
    for (let i = 1; i < folder.length; i++) {
        const s = folder[i];
        const last = ans[ans.length - 1];
        if (!s.startsWith(last) || s[last.length] !== '/') {
            ans.push(s);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_subfolders(mut folder: Vec<String>) -> Vec<String> {
        folder.sort_unstable();
        let mut ans = vec![folder[0].clone()];
        for s in folder.into_iter().skip(1) {
            let last = ans.last().unwrap();
            if !s.starts_with(last) || s.as_bytes()[last.len()] != b'/' {
                ans.push(s);
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\ell\cdot n\log n)$ 时间，$n$ 是 $\textit{folder}$ 的长度，$\ell\le 100$ 为单个字符串的长度。排序有 $\mathcal{O}(n\log n)$ 次比较，每次 $\mathcal{O}(\ell)$ 时间。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。忽略排序的栈开销。

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
