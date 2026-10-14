# K01 数组、哈希、前缀和、差分、双指针与滑动窗口

## 核心问题

数组下标、频次映射、区间统计、原地更新、窗口不变量。学习目标是能识别题目中的结构、选择合适算法、证明复杂度并写出边界正确的 C++17 实现。

本域的主线是"把区间/配对问题转化为可 O(1) 查询的历史状态"：哈希表存"历史补数"，前缀和把"区间和"变成"两个前缀的差"，差分把"区间更新"变成"两个端点标记"，双指针与滑动窗口把"枚举所有区间"压缩成"每个端点至多移动一次"。

## 训练顺序

1. 先完成对应入门题，建立术语和模板。
2. 再完成中等级变体，训练多约束组合。
3. 高级和专家题用于训练证明、在线约束、工程边界和追问。

## 核心模式

### 模式 1：哈希表（值 → 下标 / 值 → 次数）

哈希表把"查找历史信息"从线性降到平均 O(1)。两种典型语义决定返回值的形态：

- 值 → 下标：回答"是否存在"，返回一条路径（B001 两数之和）。
- 值 → 次数：回答"有多少种方案"，返回计数（A001 和为 K 的子数组）。

```cpp
#include <bits/stdc++.h>
using namespace std;

// 值 -> 下标：两数之和
vector<int> twoSum(vector<int>& a, int target) {
    unordered_map<int, int> pos;
    pos.reserve(a.size() * 2);
    pos.max_load_factor(0.7f);
    for (int i = 0; i < (int)a.size(); ++i) {
        int need = target - a[i];
        auto it = pos.find(need);          // 先查
        if (it != pos.end()) return {it->second, i};
        pos[a[i]] = i;                     // 后插
    }
    return {};
}
```

**不变量**：处理到下标 `i` 时，哈希表恰好保存所有下标 `< i` 的元素（及其一个下标）。"先查后插"保证不会自配对。平均时间 O(n)，最坏 O(n²)（全冲突），空间 O(n)。

### 模式 2：前缀和 `pre[j] - pre[i-1]`

令 `pre[0]=0`，`pre[k]=a[0]+...+a[k-1]`（长度 `n+1`）。则区间和

```
sum(a[l..r]) = pre[r+1] - pre[l]
```

把"子数组和 = k"变成"`pre[j] - pre[i] = k` 的对数"，即对每个 `j` 统计历史 `pre[i] = pre[j] - k` 的个数——与模式 1 的"查找历史补数"同构。

```cpp
long long subarraySumK(vector<int>& a, long long k) {
    unordered_map<long long, long long> cnt;
    cnt.reserve(a.size() * 2);
    cnt[0] = 1;                            // 空前缀，覆盖 l=0
    long long pre = 0, ans = 0;
    for (int x : a) {
        pre += x;                          // 此时 pre = pre[i+1]
        auto it = cnt.find(pre - k);       // 先查
        if (it != cnt.end()) ans += it->second;
        ++cnt[pre];                        // 后插
    }
    return ans;
}
```

**要点**：`cnt[0]=1` 不可省，否则漏掉以 0 开头的区间；"先查后插"避免零长度伪区间（`k=0` 时尤其明显）；前缀和元素含负数时哈希依然成立，滑动窗口则不成立。

### 模式 3：差分数组

差分是前缀和的逆运算，用于**区间批量更新**。令 `diff[i] = a[i] - a[i-1]`，对区间 `[l, r]` 整体加 `v`，只需两处修改，最后做一次前缀和还原。

```cpp
vector<long long> buildDiff(vector<long long>& a) {   // diff[i] += 区间修改
    int n = a.size();
    vector<long long> d(n, 0);
    d[0] = a[0];
    for (int i = 1; i < n; ++i) d[i] = a[i] - a[i - 1];
    return d;
}
void rangeAdd(vector<long long>& d, int l, int r, long long v) {
    int n = d.size();
    d[l] += v;
    if (r + 1 < n) d[r + 1] -= v;           // 越界判断不可省
}
void restore(vector<long long>& d) {        // 还原成原数组值
    for (int i = 1; i < (int)d.size(); ++i) d[i] += d[i - 1];
}
```

`m` 次区间更新 + 一次还原的复杂度为 O(m + n)，优于每次 O(区间长度) 的朴素写法。二维差分同理：矩形加法在四角打标记。

### 模式 4：双指针（相向 / 同向）

**相向双指针**要求有序性，每次移动都排除一整行/列候选，典型是"有序数组两数之和"：

```cpp
pair<int,int> twoSumSorted(vector<int>& a, long long target) {
    int l = 0, r = (int)a.size() - 1;
    while (l < r) {
        long long s = (long long)a[l] + a[r];
        if (s == target) return {l, r};    // 找到
        else if (s < target) ++l;          // 和偏小，左端右移
        else --r;                          // 和偏大，右端左移
    }
    return {-1, -1};
}
```

**不变量**：答案（若存在）始终落在 `[l, r]` 内；`s < target` 时 `a[l]` 与任何更小右端都无解，故可安全 `++l`。时间 O(n)，空间 O(1)。

**同向双指针（快慢指针）**用于原地删除/分区：`slow` 指向下一个写入位置，`fast` 扫描读取。

```cpp
int removeElement(vector<int>& a, int val) {   // 返回新长度
    int slow = 0;
    for (int fast = 0; fast < (int)a.size(); ++fast)
        if (a[fast] != val) a[slow++] = a[fast];
    return slow;
}
```

### 模式 5：滑动窗口不变量

滑动窗口是"同向双指针 + 单调性"的特例。维护窗口 `[l, r]` 与一个可增量更新的状态（和、计数、频次表），每轮先扩张 `r`，再在违反约束时收缩 `l`。正确性依赖**单调性**：`r` 右移后状态只增不减，或约束可被收缩修复。

```cpp
// 最长无重复字符子串：窗口内字符互异
int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> last;         // 字符 -> 最近出现下标
    int l = 0, ans = 0;
    for (int r = 0; r < (int)s.size(); ++r) {
        if (last.count(s[r]))
            l = max(l, last[s[r]] + 1);    // 收缩到重复字符之后
        last[s[r]] = r;
        ans = max(ans, r - l + 1);
    }
    return ans;
}
```

**不变量**：每轮结束时窗口 `[l, r]` 内无重复字符。`l` 只增不减，故总时间 O(n)（每个下标进出窗口各一次）。

## 常见陷阱

- **前缀和下标 off-by-one**：`pre` 长度必须是 `n+1`，区间 `[l, r]` 对应 `pre[r+1]-pre[l]`；写成 `pre[r]-pre[l-1]` 时需保证 `l>=1`，混用两套下标是最高频错误。
- **忘记 `cnt[0]=1`**：`A001` 会漏掉左端为 0 的区间，答案偏小。
- **先插后查**：哈希表解法会把当前元素与自身配对，`k=0` 或 `target=2*x` 时产生零长度伪区间。
- **对含负数的数组套滑动窗口**：窗口和不再随 `r` 单调，收缩判据失效，结果偏小（见 A001 反例）。
- **差分越界**：`rangeAdd` 在 `r+1==n` 时必须跳过，否则写越界。
- **溢出**：前缀和、计数、区间长度常用 `long long`，`int` 在 `n=1e5` 且值大时溢出。
- **双指针无序**：相向双指针的排除论证依赖有序，未排序直接套用会漏解。
- **数组下标与元素值混淆**：哈希键是元素值、值是下标，写反会返回错误结果。

## 复杂度速查

| 模式 | 时间 | 空间 | 关键前提 |
|---|---|---|---|
| 哈希一次遍历 | 平均 O(n)，最坏 O(n²) | O(n) | 元素可哈希 |
| 前缀和 + 哈希计数 | 平均 O(n) | O(n) | 前缀和差值等式 |
| 前缀和数组 | 预处理 O(n)，查询 O(1) | O(n) | 可差分 |
| 差分区间更新 | O(m + n) | O(n) | 只做整体加 |
| 相向双指针 | O(n)（外加排序 O(n log n)） | O(1) | 有序 |
| 同向双指针（原地） | O(n) | O(1) | 相对顺序可保持 |
| 滑动窗口 | O(n) | O(字符集) | 状态单调 / 可收缩修复 |

## 题目索引

- `../01-question-bank/beginner/B001-two-sum.md`（两数之和：哈希值→下标）
- `../01-question-bank/advanced/A001-subarray-sum-k.md`（和为 K 的子数组：前缀和 + 哈希计数）

跨域相关题：`I002`（最长无重复子串，滑窗，见 K02）、`E002`（TopK 高频，哈希计数，见 K05）。

**证据与来源：** `[已确认]` 上述模板与复杂度为公开经典算法结论；`[推断]` 训练顺序按难度归纳。样例未在目标 OJ 逐例运行，标记 `[待验证]`。
