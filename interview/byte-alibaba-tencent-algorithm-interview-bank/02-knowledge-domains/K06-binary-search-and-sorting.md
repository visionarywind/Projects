# K06 二分、排序、选择与分治

## 核心问题

有序性、答案空间、稳定性、选择算法、归并分治。学习目标是能识别题目中的结构、选择合适算法、证明复杂度并写出边界正确的 C++17 实现。

本域围绕"利用有序性"和"分而治之"展开：**二分查找**要求在有序序列或单调答案空间上排除一半候选；**排序**为后续算法奠定结构（区间按端点排、双指针需有序）；**选择算法**（快速选择）以线性期望求第 k 大；**分治**（归并排序、快速排序）把问题拆成独立子问题再合并。

## 训练顺序

1. 先完成对应入门题，建立术语和模板。
2. 再完成中等级变体，训练多约束组合。
3. 高级和专家题用于训练证明、在线约束、工程边界和追问。

## 核心模式

### 模式 1：二分查找边界（lower/upper bound）

标准二分容易在边界卡死。统一用**左闭右开**区间 `[lo, hi)`，把"查找"抽象为"找第一个满足 `pred` 的位置"，循环结束时 `lo` 即答案。

```cpp
#include <bits/stdc++.h>
using namespace std;

// 第一个 >= x 的位置（lower_bound）
int lowerBound(const vector<int>& a, int x) {
    int lo = 0, hi = a.size();             // [lo, hi)
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;      // 防溢出
        if (a[mid] < x) lo = mid + 1;      // mid 不满足，排除
        else hi = mid;                     // mid 可能满足，保留
    }
    return lo;                             // [0, n]，可能是 n
}
// 第一个 > x 的位置（upper_bound）：把条件改为 a[mid] <= x
int upperBound(const vector<int>& a, int x) {
    int lo = 0, hi = a.size();
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] <= x) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}
```

**不变量**：答案始终在 `[lo, hi]` 内；每轮排除 `mid` 及其一侧，区间严格缩小。时间 O(log n)。`mid = lo + (hi-lo)/2` 避免 `lo+hi` 溢出。C++ 标准库的 `std::lower_bound/upper_bound` 语义与之一致。

### 模式 2：二分答案（对单调的可行性判定）

当"最小化/最大化某个值"且"值越大越可行（或越小越可行）"时，对答案空间二分，用 `check(mid)` 判定可行性。核心是找出**单调性**并写对 `check`。

```cpp
// A003：在 days 天内运完，求最小船载重量
bool canFinish(const vector<int>& w, int cap, int days) {
    int need = 1, cur = 0;
    for (int x : w) {
        if (x > cap) return false;         // 单件超载直接不可行
        if (cur + x > cap) { ++need; cur = 0; }
        cur += x;
    }
    return need <= days;
}
int shipWithinDays(vector<int>& w, int days) {
    int lo = *max_element(w.begin(), w.end());   // 下界：最大单件
    int hi = accumulate(w.begin(), w.end(), 0);  // 上界：一次全运
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (canFinish(w, mid, days)) hi = mid;   // mid 可行，试更小
        else lo = mid + 1;                        // mid 不可行，必须更大
    }
    return lo;                             // 最小可行载重
}
```

**单调性**：载重越大，所需天数越少（不增），故存在最小可行值。时间 O(n log(sum))，空间 O(1)。下界取 `max(w)` 而不是 0 能少一半迭代；`check` 的贪心必须在单趟可判定的前提下才成立。

### 模式 3：快速选择（线性期望求第 k 大）

借鉴快排的 partition：每次划分后枢轴归位，只继续处理包含目标下标的一侧，平均 O(n)。

```cpp
int partition(vector<int>& a, int lo, int hi) {   // 三路可选，这里用 Lomuto
    int pivot = a[hi], i = lo;
    for (int j = lo; j < hi; ++j)
        if (a[j] < pivot) swap(a[i++], a[j]);
    swap(a[i], a[hi]);
    return i;                              // 枢轴最终位置
}
int quickSelect(vector<int>& a, int lo, int hi, int k) {  // 第 k 小（0-based）
    if (lo == hi) return a[lo];
    int p = partition(a, lo, hi);
    if (k == p) return a[p];
    return k < p ? quickSelect(a, lo, p - 1, k)
                 : quickSelect(a, p + 1, hi, k);
}
```

**不变量**：`partition` 后 `a[lo..p-1] < a[p] <= a[p+1..hi]`；目标下标 `k` 只可能在一侧。平均 O(n)，最坏 O(n²)（每次极不平衡）；随机化枢轴或三数取中可把最坏概率压到极低。对比全排序 O(n log n)，快速选择在只求第 k 大时更优。

### 模式 4：归并排序与分治

分治三段式：**分解 → 递归求解 → 合并**。归并排序的合并是 O(n)，递归深度 O(log n)，总时间 O(n log n)，稳定。归并过程天生适合统计"逆序对/右侧更小元素"等跨区间信息。

```cpp
long long mergeCount(vector<int>& a, vector<int>& tmp, int lo, int hi) {
    if (hi - lo <= 1) return 0;
    int mid = lo + (hi - lo) / 2;
    long long inv = mergeCount(a, tmp, lo, mid) + mergeCount(a, tmp, mid, hi);
    int i = lo, j = mid, t = lo;
    while (i < mid && j < hi) {
        if (a[i] <= a[j]) tmp[t++] = a[i++];       // <= 保证稳定
        else { inv += mid - i; tmp[t++] = a[j++]; } // a[i..mid) 都 > a[j]
    }
    while (i < mid) tmp[t++] = a[i++];
    while (j < hi) tmp[t++] = a[j++];
    copy(tmp.begin() + lo, tmp.begin() + hi, a.begin() + lo);
    return inv;
}
```

**逆序对统计**：当 `a[j]` 被取出时，左半段尚未取出的 `mid - i` 个元素都比它大，各贡献一个逆序对。时间 O(n log n)，空间 O(n)。

### 模式 5：快速排序

选枢轴、划分、递归两侧。平均 O(n log n)，最坏 O(n²)（已排序且取端点为枢轴），原地、不稳定。工业实现用随机枢轴 + 三数取中 + 小区间插排 + 三路划分（处理大量重复）。

```cpp
void quickSort(vector<int>& a, int lo, int hi) {   // [lo, hi]
    if (lo >= hi) return;
    int p = partition(a, lo, hi);                  // 复用模式 3 的划分
    quickSort(a, lo, p - 1);
    quickSort(a, p + 1, hi);
}
```

对含大量重复元素的数组，Lomuto 划分会退化；改用三路划分（`<pivot`、`==pivot`、`>pivot`）可把重复段一次归位。

## 常见陷阱

- **`mid = (lo+hi)/2` 溢出**：`lo+hi` 超过 `int` 上限时结果错误，用 `lo + (hi-lo)/2`。
- **二分死循环**：左闭右开 + `hi = mid`（不是 `mid-1`）是安全写法；混用闭区间与 `hi=mid` 会卡死。
- **`check` 非单调**：二分答案的前提是可行性单调，`check` 写错（如贪心跳过可行性）会让二分结果无意义。
- **下界取 0**：`A003` 下界取 `max(w)` 既正确又省迭代；取 0 虽不改变正确性但浪费。
- **快速选择枢轴全等**：全相同元素时 Lomuto 退化 O(n²)，需三路划分或随机化。
- **归并统计逆序对用 `<`**：写成 `a[i] < a[j]` 而非 `<=` 会破坏稳定性，逆序对计数也会偏差。
- **快排最坏退化**：已排序输入 + 端点枢轴 = O(n²)，必须随机化或三数取中。
- **不稳定的误解**：需要稳定时选归并/计数排序；快排、堆排不稳定。
- **比较器不满足严格弱序**：`sort` 的 `cmp` 若对相等元素返回 true，行为未定义（可能崩溃）。

## 复杂度速查

| 模式 | 平均 | 最坏 | 空间 | 稳定性 |
|---|---|---|---|---|
| 二分查找（lower/upper） | O(log n) | O(log n) | O(1) | — |
| 二分答案 | O(n log R) | O(n log R) | O(1) | — |
| 快速选择 | O(n) | O(n²) | O(log n) 递归栈 | — |
| 归并排序 | O(n log n) | O(n log n) | O(n) | 稳定 |
| 快速排序 | O(n log n) | O(n²) | O(log n) | 不稳定 |
| 堆排序 | O(n log n) | O(n log n) | O(1) | 不稳定 |
| 计数/基数排序 | O(n + k) | O(n + k) | O(n + k) | 稳定 |

## 题目索引

- `../01-question-bank/advanced/A003-binary-search-capacity.md`（二分答案：在 D 天内送达的最小船载重量）

跨域相关题：`E002`（快速选择求第 k 高频，见 K05）、`K05`（TopK 的堆解法与快速选择互为替代）、`K01`（二分与双指针都依赖有序性）。

**证据与来源：** `[已确认]` 二分边界模板、二分答案单调性、快速选择与归并/快排复杂度、逆序对归并统计均为公开经典结论；`[推断]` 训练顺序按难度归纳。样例未在目标 OJ 逐例运行，标记 `[待验证]`。
