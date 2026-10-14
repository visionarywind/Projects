# K09 回溯、搜索、剪枝与枚举

## 核心问题

- **决策树**：把"每一步做一个选择"建模成一棵搜索树，回溯就是系统地枚举所有从根到叶的路径。
- **回溯模板**：选择 → 递归 → 撤销选择，三者构成一个不可分割的原子操作。
- **子集 / 排列 / 组合**：三种最基本的枚举问题，边界与去重条件各不相同。
- **去重技巧**：先排序，再对"同层重复"剪枝，避免输出重复答案。
- **剪枝**：可行性剪枝（早停非法分支）、最优性剪枝（当前路径不可能更优时早停）、排序剪枝（有序输入便于早停）。
- **状态恢复**：DFS 的回退必须把状态精确还原，否则污染兄弟分支。
- **分支限界**：用上界/下界估计函数引导搜索，优先扩展更有希望的分支。

学习目标是能写出正确的回溯模板、设计有效剪枝并给出边界正确的 C++17 实现。

## 训练顺序

1. 先默写回溯模板（子集、排列、组合三件套），确保"撤销选择"与"选择"完全对称。
2. 再加入去重：排序 + `i > start && nums[i] == nums[i-1]` 或 `!used[i-1]` 两种写法。
3. 然后练剪枝：组合和为定值、N 皇后、括号生成，体会剪枝对搜索空间的实际削减。
4. 最后做分支限界与状态恢复错误排查，理解"可达解全部覆盖"与"无重复"两类正确性证明。

## 核心模式

### 1. 回溯通用模板

```cpp
void backtrack(路径, 选择列表) {
    if (满足结束条件) { 记录答案; return; }
    for (选择 : 选择列表) {
        if (剪枝条件) continue;      // 跳过非法/重复分支
        做选择;                     // 修改状态
        backtrack(新路径, 新选择列表);
        撤销选择;                   // 恢复状态，必须与"做选择"对称
    }
}
```

不变量：进入 `backtrack` 时，`路径` 恰好是根到当前节点的决策序列；返回时状态与进入前完全一致。复杂度：由搜索树节点数决定。

### 2. 子集（每个节点都是答案）

```cpp
vector<vector<int>> subsets(vector<int> nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> res;
    vector<int> path;
    function<void(int)> dfs = [&](int start) {
        res.push_back(path);                       // 走到哪都是合法子集
        for (int i = start; i < (int)nums.size(); ++i) {
            if (i > start && nums[i] == nums[i - 1]) continue;  // 同层去重
            path.push_back(nums[i]);
            dfs(i + 1);                            // 下一层从 i+1 开始，避免重复选取
            path.pop_back();
        }
    };
    dfs(0);
    return res;
}
```

不变量：`path` 是当前已选元素的集合，`start` 之前的元素都不会再被选（保证组合无序性）。复杂度：`O(n·2^n)`（含复制路径）。

### 3. 组合（和为定值，排序 + 剪枝）

```cpp
void combinationSum(vector<int>& c, int target, int start,
                    vector<int>& path, vector<vector<int>>& res) {
    if (target == 0) { res.push_back(path); return; }
    for (int i = start; i < (int)c.size(); ++i) {
        if (c[i] > target) break;                  // 排序后：后续只会更大，直接剪枝
        if (i > start && c[i] == c[i - 1]) continue; // 同层去重
        path.push_back(c[i]);
        combinationSum(c, target - c[i], i + 1, path, res);  // i+1: 每元素用一次
        path.pop_back();
    }
}
```

不变量：剩余需求 `target` 只减不增；剪枝条件 `c[i] > target` 在升序下可 `break` 而非 `continue`。复杂度：受答案规模与剪枝效果支配，最坏 `O(2^n)`。

### 4. 排列（used 数组 + 去重）

```cpp
vector<vector<int>> permuteUnique(vector<int> nums) {
    sort(nums.begin(), nums.end());
    int n = (int)nums.size();
    vector<vector<int>> res;
    vector<int> path;
    vector<char> used(n, 0);
    function<void()> dfs = [&]() {
        if ((int)path.size() == n) { res.push_back(path); return; }
        for (int i = 0; i < n; ++i) {
            if (used[i]) continue;
            if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) continue;  // 同层去重
            used[i] = 1;
            path.push_back(nums[i]);
            dfs();
            path.pop_back();
            used[i] = 0;
        }
    };
    dfs();
    return res;
}
```

去重条件含义：相同值的元素，只允许由"尚未使用的最靠前那个"来承担当前位置，从而消除重复分支。复杂度：`O(n·n!)`，含重复时为 `O(n · n!/Π(c_v!))`。

### 5. N 皇后（状态恢复 + 稀疏剪枝）

```cpp
int n;
vector<char> col, d1, d2;   // 列、主对角线 r+c、副对角线 r-c+n-1
int countNQueens(int r) {
    if (r == n) return 1;
    int cnt = 0;
    for (int c = 0; c < n; ++c) {
        if (col[c] || d1[r + c] || d2[r - c + n - 1]) continue;
        col[c] = d1[r + c] = d2[r - c + n - 1] = 1;
        cnt += countNQueens(r + 1);
        col[c] = d1[r + c] = d2[r - c + n - 1] = 0;   // 精确恢复三个标记
    }
    return cnt;
}
```

不变量：进入第 `r` 行时，三个标记数组准确反映前 `r` 行的冲突占用。复杂度：`O(n!)`，剪枝把实际节点数压到远小于 `n^n`。

### 6. 可行性剪枝与最优性剪枝要点

- **可行性剪枝**：若当前部分选择已不可能满足约束（如组合和超过目标、棋盘已冲突），立即返回，不再深入。
- **最优性剪枝**：维护当前最优值 `best` 和一个乐观估计下界/上界；若"当前代价 + 剩余下界 ≥ best"则剪掉该分支。
- **排序剪枝**：把候选按大小或贡献排序，使超限时能 `break` 而非逐个 `continue`。

## 常见陷阱

- **忘记撤销状态**：只 `push_back` 没有 `pop_back`，或标记置位后未复位，兄弟分支读到脏状态——最常见的错误。
- **去重条件写错**：`if (nums[i] == nums[i-1]) continue;` 缺少 `i > start`（子集/组合）或 `!used[i-1]`（排列）会误杀合法分支，例如 `[1,1]` 得不到任何排列。
- **答案存引用而非拷贝**：`res.push_back(path)` 存的是快照拷贝；若按引用/指针存入，回溯后内容被改写，输出全空或全错。
- **剪枝过头**：在无序输入上用 `break`，或把 `>` 写成 `>=`，导致漏解；剪枝必须以保证"不丢解"为前提。
- **递归深度/栈溢出**：深棋盘或长序列（如 `300×300` 网格 DFS）可能爆栈，需要改为显式栈或 BFS。
- **搜索空间爆炸**：没有剪枝的纯暴力在 `n=8` 以上的排列/`n=15` 以上的子集就会超时，需估算 `n!` 与 `2^n`。
- **重复元素与全局去重混用**：用 `set` 在最后统一去重通常可行但更慢；优化写法是"搜索树上剪枝"，两者选一，不要一半一半。
- **`used` 数组按值传递副本**：传参时拷贝了整个 `used` 数组，虽正确但开销大；应用引用并手动回溯。
- **终止条件位置错误**：先记录答案再判断终止，或反之，会导致多算/少算一层。

## 复杂度速查

| 问题 | 结果规模 | 搜索时间 | 辅助空间 |
|---|---|---|---|
| 子集 | `2^n` | `O(n·2^n)` | `O(n)` |
| 组合（取 k 个） | `C(n,k)` | `O(k·C(n,k))` | `O(k)` |
| 排列（无重复） | `n!` | `O(n·n!)` | `O(n)` |
| 排列（含重复） | `n!/Π(c_v!)` | 同结果规模 | `O(n)` |
| N 皇后 | 解数随 `n` 增长 | 剪枝后约 `O(n!)` | `O(n)` |
| 括号生成 | `Catalan(n)` | `O(4^n / n^{1.5})` | `O(n)` |

说明：输出类问题的时间下界受结果规模约束，剪枝只能降低常数与无效节点，无法低于输出规模。

## 题目索引

本域对应的真实题目文件：

- `../01-question-bank/expert/E003-permutations.md` —— 全排列、含重复元素去重、字典序第 K 个（康托展开）、输出规模下界论证，是 K09 的核心综合题。
- `../01-question-bank/intermediate/I001-valid-parentheses.md` —— 属 K03，但其中的"括号生成/移除无效括号"追问直接落到本域的回溯 + 去重。

延伸方向（题面变体，非独立文件）：组合总和、子集 II（含重复）、N 皇后、单词搜索、括号生成，可在覆盖矩阵 `../00-overview/coverage-matrix.md` 的 `K09` 条目下继续补题。
