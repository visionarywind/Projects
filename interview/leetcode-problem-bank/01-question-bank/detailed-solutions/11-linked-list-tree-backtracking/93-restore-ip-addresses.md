# 93. 复原 IP 地址

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/restore-ip-addresses/
- 题目 slug：`restore-ip-addresses`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.3 划分型回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/restore-ip-addresses/solutions/3727037/liang-chong-fang-fa-san-zhong-xun-huan-h-hxak/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：三重循环 / 回溯+剪枝（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/restore-ip-addresses/solutions/3727037/liang-chong-fang-fa-san-zhong-xun-huan-h-hxak/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-san-zhong-xun-huan-h-hxak`
- topic id：`3727037`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 方法一：三重循环，枚举分割位置

问题相当于把 $s$ 切三刀，分割成四段。

写个三重循环，枚举这三刀（三个点号）的位置 $i,j,k$：

- 第一段从 $0$ 到 $i-1$。
- 第二段从 $i$ 到 $j-1$。
- 第三段从 $j$ 到 $k-1$。
- 第四段从 $k$ 到 $n-1$。其中 $n$ 是 $s$ 的长度。

对于每一段（子串 $t$），还需要判断子串 $t$ 是不是合法的，也就是：

- 如果 $t$ 的长度大于 $1$ 且 $t[0] = \texttt{`0'}$，那么 $t$ 有前导零，不符合要求。
- 否则，$t$ 对应的整数必须 $\le 255$，否则不符合要求。可以先判断 $t$ 的长度是否超过 $3$，超过就直接判定为不合法。

```py [sol-Python3]
class Solution:
    def restoreIpAddresses(self, s: str) -> List[str]:
        def is_valid(t: str) -> bool:
            # 长度不能超过 3，不能有前导零
            if len(t) > 3 or len(t) > 1 and t[0] == '0':
                return False
            return int(t) <= 255

        n = len(s)
        ans = []
        for i in range(1, n):
            if not is_valid(s[:i]):
                break
            for j in range(i + 1, n):
                if not is_valid(s[i:j]):
                    break
                for k in range(j + 1, n):
                    if not is_valid(s[j:k]):
                        break
                    if is_valid(s[k:]):
                        ans.append(f"{s[:i]}.{s[i:j]}.{s[j:k]}.{s[k:]}")
        return ans
```

```java [sol-Java]
class Solution {
    public List<String> restoreIpAddresses(String s) {
        int n = s.length();
        List<String> ans = new ArrayList<>();
        for (int i = 1; i < n && isValid(s, 0, i); i++) {
            for (int j = i + 1; j < n && isValid(s, i, j); j++) {
                for (int k = j + 1; k < n && isValid(s, j, k); k++) {
                    if (isValid(s, k, n)) {
                        ans.add(String.format("%s.%s.%s.%s",
                            s.substring(0, i),
                            s.substring(i, j),
                            s.substring(j, k),
                            s.substring(k)));
                    }
                }
            }
        }
        return ans;
    }

    // 判断子串 [i,j) 是否合法，注意这里是左闭右开区间
    private boolean isValid(String s, int i, int j) {
        // 长度不能超过 3，不能有前导零
        if (j - i > 3 || j - i > 1 && s.charAt(i) == '0') {
            return false;
        }
        return Integer.parseInt(s.substring(i, j)) <= 255;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<string> restoreIpAddresses(string s) {
        // 判断子串 [i,j) 是否合法，注意这里是左闭右开区间
        auto is_valid = [&](int i, int j) -> bool {
            // 长度不能超过 3，不能有前导零
            if (j - i > 3 || j - i > 1 && s[i] == '0') {
                return false;
            }
            return stoi(s.substr(i, j - i)) <= 255;
        };

        int n = s.size();
        vector<string> ans;
        for (int i = 1; i < n && is_valid(0, i); i++) {
            for (int j = i + 1; j < n && is_valid(i, j); j++) {
                for (int k = j + 1; k < n && is_valid(j, k); k++) {
                    if (is_valid(k, n)) {
                        ans.push_back(s.substr(0, i) + "." +
                                      s.substr(i, j - i) + "." +
                                      s.substr(j, k - j) + "." +
                                      s.substr(k));
                    }
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func isValid(t string) bool {
    // 长度不能超过 3，不能有前导零
    if len(t) > 3 || len(t) > 1 && t[0] == '0' {
        return false
    }
    x, _ := strconv.Atoi(t)
    return x <= 255
}

func restoreIpAddresses(s string) (ans []string) {
    n := len(s)
    for i := 1; i < n && isValid(s[:i]); i++ {
        for j := i + 1; j < n && isValid(s[i:j]); j++ {
            for k := j + 1; k < n && isValid(s[j:k]); k++ {
                if isValid(s[k:]) {
                    ans = append(ans, fmt.Sprintf("%s.%s.%s.%s", s[:i], s[i:j], s[j:k], s[k:]))
                }
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var restoreIpAddresses = function(s) {
    // 判断子串 [i,j) 是否合法，注意这里是左闭右开区间
    function isValid(i, j) {
        // 长度不能超过 3，不能有前导零
        if (j - i > 3 || j - i > 1 && s[i] === '0') {
            return false;
        }
        return parseInt(s.slice(i, j)) <= 255;
    }

    const n = s.length;
    const ans = [];
    for (let i = 1; i < n && isValid(0, i); i++) {
        for (let j = i + 1; j < n && isValid(i, j); j++) {
            for (let k = j + 1; k < n && isValid(j, k); k++) {
                if (isValid(k, n)) {
                    ans.push(`${s.slice(0, i)}.${s.slice(i, j)}.${s.slice(j, k)}.${s.slice(k)}`);
                }
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn restore_ip_addresses(s: String) -> Vec<String> {
        fn is_valid(t: &str) -> bool {
            // 长度不能超过 3，不能有前导零
            if t.len() > 3 || t.len() > 1 && t.starts_with('0') {
                return false;
            }
            t.parse::<u32>().unwrap() <= 255
        }

        let n = s.len();
        let mut ans = vec![];
        for i in 1..n {
            let a = &s[..i];
            if !is_valid(a) {
                break;
            }
            for j in i + 1..n {
                let b = &s[i..j];
                if !is_valid(b) {
                    break;
                }
                for k in j + 1..n {
                    let c = &s[j..k];
                    if !is_valid(c) {
                        break;
                    }
                    let d = &s[k..];
                    if is_valid(d) {
                        ans.push(format!("{}.{}.{}.{}", a, b, c, d));
                    }
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^4)$，其中 $n$ 是 $s$ 的长度。写了一个 $\mathcal{O}(n^3)$ 的三重循环，再算上生成 IP 地址字符串的 $\mathcal{O}(n)$ 时间，一共是 $\mathcal{O}(n^4)$。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 方法二：回溯

推荐先完成 [131. 分割回文串](https://leetcode.cn/problems/palindrome-partitioning/)，因为 131 题没有分割段数的要求，而本题要求分成恰好 $4$ 段，所以本题相对来说更复杂一些。 

### 写法一：选或不选（是否分割）

在 131 题的基础上，我们需要额外知道：

- 当前在第几段。
- 当前分割的子串，对应的数值是多少，便于我们判断子串是否合法。

定义 $\textit{dfs}(i,j,\textit{ipVal})$，表示：

- 剩余字符从 $s[i]$ 到 $s[n-1]$。
- 现在在第 $j$ 段（$j$ 从 $0$ 开始）。
- 第 $j$ 段的数值目前为 $\textit{ipVal}$。

递归过程中，首先把 $s[i]$ 加到当前这一段的末尾，即更新 $\textit{ipVal}$ 为 $\textit{ipVal}\cdot 10 + \texttt{int}(s[i])$。例如在 $12$ 的末尾添加 $3$，数值更新为 $12\cdot 10 + 3 = 123$。

分类讨论：

- 不分割，前提是不能有前导零，即此时 $\textit{ipVal} > 0$。往下递归到 $\textit{dfs}(i+1,j,\textit{ipVal})$。注意，如果有前导零的话，会在前导零那个字符处发现 $\textit{ipVal} = 0$，不会往下递归。
- 分割，$s[i]$ 作为当前这段子串的右端点。把 $j$ 加一，$\textit{ipVal}$ 重置为 $0$。往下递归到 $\textit{dfs}(i+1,j+1,0)$。

递归边界：

- $i=n$ 时，$s$ 分割完毕，如果此时 $j=4$，把分割结果加入答案。
- 否则，如果 $j=4$，由于此时已经分出 $4$ 段，不能再分割，所以不再递归。

递归入口：$\textit{dfs}(0,0,0)$。

⚠**注意**：下面代码中，$\textit{path}$ 的长度是固定的 $4$，可以直接覆盖 $\textit{path}[j]$，无需恢复现场。如果是 $\textit{path}$ 初始化为空列表的那种写法，就需要恢复现场。

```py [sol-Python3]
class Solution:
    def restoreIpAddresses(self, s: str) -> List[str]:
        n = len(s)
        ans = []
        path = [0] * 4  # path[i] 表示第 i 段（i 从 0 开始）的结束位置 + 1（右开区间，方便切片）

        # 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始），数值为 ip_val
        def dfs(i: int, j: int, ip_val: int) -> None:
            if i == n:  # s 分割完毕
                if j == 4:  # 必须有 4 段
                    a, b, c, _ = path
                    ans.append(f"{s[:a]}.{s[a:b]}.{s[b:c]}.{s[c:]}")
                return

            if j == 4:  # j=4 的时候必须分割完毕，不能有剩余字符
                return

            # 手动把字符串转成整数，这样字符串转整数是严格 O(1) 的
            ip_val = ip_val * 10 + int(s[i])
            if ip_val > 255:  # 不合法
                return

            # 不分割，不以 s[i] 为这一段的结尾
            if ip_val > 0:  # 无前导零
                dfs(i + 1, j, ip_val)

            # 分割，以 s[i] 为这一段的结尾
            path[j] = i + 1  # 记录这一段的结束位置 + 1
            dfs(i + 1, j + 1, 0)

        dfs(0, 0, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<String> restoreIpAddresses(String s) {
        List<String> ans = new ArrayList<>();
        // path[i] 表示第 i 段（i 从 0 开始）的结束位置 + 1（右开区间，方便求 substring）
        int[] path = new int[4];
        dfs(0, 0, 0, s, s.length(), path, ans);
        return ans;
    }

    // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始），数值为 ipVal
    private void dfs(int i, int j, int ipVal, String s, int n, int[] path, List<String> ans) {
        if (i == n) { // s 分割完毕
            if (j == 4) { // 必须有 4 段
                int a = path[0], b = path[1], c = path[2];
                ans.add(s.substring(0, a) + "." + s.substring(a, b) + "." + s.substring(b, c) + "." + s.substring(c));
            }
            return;
        }

        if (j == 4) { // j=4 的时候必须分割完毕，不能有剩余字符
            return;
        }

        // 手动把字符串转成整数，这样字符串转整数是严格 O(1) 的
        ipVal = ipVal * 10 + (s.charAt(i) - '0');
        if (ipVal > 255) { // 不合法
            return;
        }

        // 不分割，不以 s[i] 为这一段的结尾
        if (ipVal > 0) { // 无前导零
            dfs(i + 1, j, ipVal, s, n, path, ans);
        }

        // 分割，以 s[i] 为这一段的结尾
        path[j] = i + 1; // 记录这一段的结束位置 + 1
        dfs(i + 1, j + 1, 0, s, n, path, ans);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<string> restoreIpAddresses(string s) {
        int n = s.size();
        vector<string> ans;
        int path[4]; // path[i] 表示第 i 段（i 从 0 开始）的结束位置 + 1（右开区间，方便求 substr）

        // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始），数值为 ip_val
        auto dfs = [&](this auto&& dfs, int i, int j, int ip_val) -> void {
            if (i == n) { // s 分割完毕
                if (j == 4) { // 必须有 4 段
                    auto [a, b, c, _] = path;
                    ans.emplace_back(s.substr(0, a) + "." + s.substr(a, b - a) + "." + s.substr(b, c - b) + "." + s.substr(c));
                }
                return;
            }

            if (j == 4) { // j=4 的时候必须分割完毕，不能有剩余字符
                return;
            }

            // 手动把字符串转成整数，这样字符串转整数是严格 O(1) 的
            ip_val = ip_val * 10 + (s[i] - '0');
            if (ip_val > 255) { // 不合法
                return;
            }

            // 不分割，不以 s[i] 为这一段的结尾
            if (ip_val > 0) { // 无前导零
                dfs(i + 1, j, ip_val);
            }

            // 分割，以 s[i] 为这一段的结尾
            path[j] = i + 1; // 记录这一段的结束位置 + 1
            dfs(i + 1, j + 1, 0);
        };

        dfs(0, 0, 0);
        return ans;
    }
};
```

```go [sol-Go]
func restoreIpAddresses(s string) (ans []string) {
    n := len(s)
    path := [4]int{} // path[i] 表示第 i 段（i 从 0 开始）的结束位置 + 1（右开区间，方便切片）

    // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始），数值为 ipVal
    var dfs func(int, int, int)
    dfs = func(i, j, ipVal int) {
        if i == n { // s 分割完毕
            if j == 4 { // 必须有 4 段
                a, b, c := path[0], path[1], path[2]
                ans = append(ans, fmt.Sprintf("%s.%s.%s.%s", s[:a], s[a:b], s[b:c], s[c:]))
            }
            return
        }

        if j == 4 { // j=4 的时候必须分割完毕，不能有剩余字符
            return
        }

        // 手动把字符串转成整数，这样字符串转整数是严格 O(1) 的
        ipVal = ipVal*10 + int(s[i]-'0')
        if ipVal > 255 { // 不合法
            return
        }

        // 不分割，不以 s[i] 为这一段的结尾
        if ipVal > 0 { // 无前导零
            dfs(i+1, j, ipVal)
        }

        // 分割，以 s[i] 为这一段的结尾
        path[j] = i + 1 // 记录这一段的结束位置 + 1
        dfs(i+1, j+1, 0)
    }

    dfs(0, 0, 0)
    return
}
```

```js [sol-JavaScript]
var restoreIpAddresses = function(s) {
    const n = s.length;
    const ans = [];
    const path = Array(4); // path[i] 表示第 i 段（i 从 0 开始）的结束位置 + 1（右开区间，方便 slice）

    // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始），数值为 ipVal
    function dfs(i, j, ipVal) {
        if (i === n) { // s 分割完毕
            if (j === 4) { // 必须有 4 段
                const [a, b, c, _] = path;
                ans.push(`${s.slice(0, a)}.${s.slice(a, b)}.${s.slice(b, c)}.${s.slice(c)}`);
            }
            return;
        }

        if (j === 4) { // j=4 的时候必须分割完毕，不能有剩余字符
            return;
        }

        // 手动把字符串转成整数，这样字符串转整数是严格 O(1) 的
        ipVal = ipVal * 10 + parseInt(s[i]);
        if (ipVal > 255) { // 不合法
            return;
        }

        // 不分割，不以 s[i] 为这一段的结尾
        if (ipVal > 0) { // 无前导零
            dfs(i + 1, j, ipVal);
        }

        // 分割，以 s[i] 为这一段的结尾
        path[j] = i + 1; // 记录这一段的结束位置 + 1
        dfs(i + 1, j + 1, 0);
    }

    dfs(0, 0, 0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn restore_ip_addresses(s: String) -> Vec<String> {
        // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始），数值为 ip_val
        fn dfs(i: usize, j: usize, ip_val: i32, s: &str, path: &mut [usize; 4], ans: &mut Vec<String>) {
            if i == s.len() { // s 分割完毕
                if j == 4 { // 必须有 4 段
                    let a = path[0];
                    let b = path[1];
                    let c = path[2];
                    ans.push(format!("{}.{}.{}.{}", &s[..a], &s[a..b], &s[b..c], &s[c..]));
                }
                return;
            }

            if j == 4 {
                return;
            }

            // 手动把字符串转成整数，这样字符串转整数是严格 O(1) 的
            let ip_val = ip_val * 10 + (s.as_bytes()[i] - b'0') as i32;
            if ip_val > 255 { // 不合法
                return;
            }

            // 不分割，不以 s[i] 为这一段的结尾
            if ip_val > 0 { // 无前导零
                dfs(i + 1, j, ip_val, s, path, ans);
            }

            // 分割，以 s[i] 为这一段的结尾
            path[j] = i + 1; // 记录这一段的结束位置 + 1
            dfs(i + 1, j + 1, 0, s, path, ans);
        }

        let mut ans = vec![];
        let mut path = [0; 4]; // path[i] 表示第 i 段（i 从 0 开始）的结束位置 + 1（右开区间，方便切片）
        dfs(0, 0, 0, &s, &mut path, &mut ans);
        ans
    }
}
```

### 写法二：枚举选哪个（枚举子串右端点）

在 131 题的基础上，我们需要额外知道当前在第几段。

定义 $\textit{dfs}(i,j)$，表示：

- 剩余字符从 $s[i]$ 到 $s[n-1]$。
- 现在在第 $j$ 段（$j$ 从 $0$ 开始）。

递归过程中，当前子串左端点为 $i$，我们枚举右端点 $\textit{right}=i,i+1,i+2,\ldots,n-1$，同时像写法一那样维护 $\textit{ipVal}$。如果 $\textit{ipVal} > 255$ 则退出循环。否则下一段的左端点为 $\textit{right}+1$，$j$ 增加 $1$，递归到 $\textit{dfs}(\textit{right}+1,j+1)$。循环末尾如果发现 $\textit{ipVal}=0$，那么对于后续循环来说有前导零，不合法，也退出循环。

递归边界：

- **剪枝**：还剩下 $n-i$ 个字符，需要分成 $4-j$ 段，每段至少 $1$ 个字符，至多 $3$ 个字符，所以必须满足 $4-j \le n-i \le (4-j)\cdot 3$，若不满足则返回。
- $i=n$ 时，$s$ 分割完毕，把分割结果加入答案。注意我们上面剪枝了，此时 $j=4$ 一定成立。

递归入口：$\textit{dfs}(0,0)$。

⚠**注意**：下面代码中，$\textit{path}$ 的长度是固定的 $4$，可以直接覆盖 $\textit{path}[j]$，无需恢复现场。如果是 $\textit{path}$ 初始化为空列表的那种写法，就需要恢复现场。

> 下面的代码其实和方法一的三重循环是类似的，相当于三重循环的递归形式。递归越深，就位于越内层的循环。

```py [sol-Python3]
class Solution:
    def restoreIpAddresses(self, s: str) -> List[str]:
        n = len(s)
        ans = []
        path = [''] * 4

        # 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始）
        def dfs(i: int, j: int) -> None:
            # 剪枝：还剩下 n-i 个字符，需要分成 4-j 段，每段至少 1 个字符，至多 3 个字符，所以 4-j <= n-i <= (4-j)*3
            if not 4 - j <= n - i <= (4 - j) * 3:
                return

            if i == n:  # s 分割完毕
                ans.append('.'.join(path))
                return

            # 子串左端点为 i
            # 枚举子串右端点 right
            ip_val = 0
            for right in range(i, n):
                ip_val = ip_val * 10 + int(s[right])
                if ip_val > 255:  # 不合法
                    break
                path[j] = s[i: right + 1]  # 直接覆盖 path[j]，无需恢复现场
                dfs(right + 1, j + 1)
                if ip_val == 0:  # 前导零，对于后续循环不合法
                    break

        dfs(0, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<String> restoreIpAddresses(String s) {
        int n = s.length();
        List<String> ans = new ArrayList<>();
        String[] path = new String[4];
        dfs(0, 0, s, s.length(), path, ans);
        return ans;
    }

    // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始）
    private void dfs(int i, int j, String s, int n, String[] path, List<String> ans) {
        // 剪枝：还剩下 n-i 个字符，需要分成 4-j 段，每段至少 1 个字符，至多 3 个字符，所以 4-j <= n-i <= (4-j)*3
        if (n - i < 4 - j || n - i > (4 - j) * 3) {
            return;
        }

        if (i == n) { // s 分割完毕
            ans.add(String.join(".", path));
            return;
        }

        // 子串左端点为 i
        // 枚举子串右端点 right
        int ipVal = 0;
        for (int right = i; right < n; right++) {
            ipVal = ipVal * 10 + (s.charAt(right) - '0');
            if (ipVal > 255) { // 不合法
                break;
            }
            path[j] = s.substring(i, right + 1); // 直接覆盖 path[j]，无需恢复现场
            dfs(right + 1, j + 1, s, n, path, ans);
            if (ipVal == 0) { // 前导零，对于后续循环不合法
                break;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<string> restoreIpAddresses(string s) {
        int n = s.size();
        vector<string> ans;
        string path[4];

        // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始）
        auto dfs = [&](this auto&& dfs, int i, int j) -> void {
            // 剪枝：还剩下 n-i 个字符，需要分成 4-j 段，每段至少 1 个字符，至多 3 个字符，所以 4-j <= n-i <= (4-j)*3
            if (n - i < 4 - j || n - i > (4 - j) * 3) {
                return;
            }

            if (i == n) { // s 分割完毕
                ans.emplace_back(path[0] + "." + path[1] + "." + path[2] + "." + path[3]);
                return;
            }

            // 子串左端点为 i
            // 枚举子串右端点 right
            int ip_val = 0;
            for (int right = i; right < n; ++right) {
                ip_val = ip_val * 10 + (s[right] - '0');
                if (ip_val > 255) { // 不合法
                    break;
                }
                path[j] = s.substr(i, right - i + 1); // 直接覆盖 path[j]，无需恢复现场
                dfs(right + 1, j + 1);
                if (ip_val == 0) { // 前导零，对于后续循环不合法
                    break;
                }
            }
        };

        dfs(0, 0);
        return ans;
    }
};
```

```go [sol-Go]
func restoreIpAddresses(s string) (ans []string) {
    n := len(s)
    path := make([]string, 4)

    // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始）
    var dfs func(int, int)
    dfs = func(i, j int) {
        // 剪枝：还剩下 n-i 个字符，需要分成 4-j 段，每段至少 1 个字符，至多 3 个字符，所以 4-j <= n-i <= (4-j)*3 
        if n-i < 4-j || n-i > (4-j)*3 {
            return
        }

        if i == n { // s 分割完毕
            ans = append(ans, strings.Join(path, "."))
            return
        }

        // 子串左端点为 i
        // 枚举子串右端点 right
        ipVal := 0
        for right := i; right < n; right++ {
            ipVal = ipVal*10 + int(s[right]-'0')
            if ipVal > 255 { // 不合法
                break
            }
            path[j] = s[i : right+1] // 直接覆盖 path[j]，无需恢复现场
            dfs(right+1, j+1)
            if ipVal == 0 { // 前导零，对于后续循环不合法
                break
            }
        }
    }

    dfs(0, 0)
    return
}
```

```js [sol-JavaScript]
var restoreIpAddresses = function(s) {
    const n = s.length;
    const ans = [];
    const path = Array(4);

    // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始）
    function dfs(i, j) {
        // 剪枝：还剩下 n-i 个字符，需要分成 4-j 段，每段至少 1 个字符，至多 3 个字符，所以 4-j <= n-i <= (4-j)*3
        if (n - i < 4 - j || n - i > (4 - j) * 3) {
            return;
        }

        if (i === n) { // s 分割完毕
            ans.push(path.join("."));
            return;
        }

        // 子串左端点为 i
        // 枚举子串右端点 right
        let ipVal = 0;
        for (let right = i; right < n; right++) {
            ipVal = ipVal * 10 + parseInt(s[right]);
            if (ipVal > 255) { // 不合法
                break;
            }
            path[j] = s.slice(i, right + 1); // 直接覆盖 path[j]，无需恢复现场
            dfs(right + 1, j + 1);
            if (ipVal === 0) { // 前导零，对于后续循环不合法
                break;
            }
        }
    }

    dfs(0, 0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn restore_ip_addresses(s: String) -> Vec<String> {
        // 分割 s[i] 到 s[n-1]，现在在第 j 段（j 从 0 开始）
        fn dfs<'a>(i: usize, j: usize, s: &'a str, path: &mut [&'a str; 4], ans: &mut Vec<String>) {
            let n = s.len();
            // 剪枝：还剩下 n-i 个字符，需要分成 4-j 段，每段至少 1 个字符，至多 3 个字符，所以 4-j <= n-i <= (4-j)*3
            if n - i < 4 - j || n - i > (4 - j) * 3 {
                return;
            }

            if i == n { // s 分割完毕
                ans.push(path.join("."));
                return;
            }

            // 子串左端点为 i
            // 枚举子串右端点 right
            let mut ip_val = 0;
            for right in i..n {
                ip_val = ip_val * 10 + (s.as_bytes()[right] - b'0') as i32;
                if ip_val > 255 { // 不合法
                    break;
                }
                path[j] = &s[i..=right]; // 直接覆盖 path[j]，无需恢复现场
                dfs(right + 1, j + 1, s, path, ans);
                if ip_val == 0 { // 前导零，对于后续循环不合法
                    break;
                }
            }
        }

        let mut ans = vec![];
        let mut path = [""; 4];
        dfs(0, 0, &s, &mut path, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m^4)$，其中 $m = \min(n, 12)$，$n$ 是 $s$ 的长度。$s$ 要分成 $4$ 段，每段长度至多为 $3$，所以 $n > 12$ 的 $s$ 一定无解，我们只需考虑 $n\le 12$ 的 $s$，这个优化已经包含在剪枝当中了。从 [组合型回溯【基础算法精讲 15】](https://www.bilibili.com/video/BV1xG4y1F7nC/)的角度理解，我们在从 $m-1$ 个空隙中选 $3$ 个位置放点号，$C_{m-1}^3 = \dfrac{(m-1)(m-2)(m-3)}{3!} = \mathcal{O}(m^3)$，所以递归过程是 $\mathcal{O}(m^3)$ 的，再算上生成 IP 地址字符串的 $\mathcal{O}(m)$ 时间，一共是 $\mathcal{O}(m^4)$。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 专题训练

见下面回溯题单的「**§4.3 划分型回溯**」。

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

本题来自 `四、回溯 / §4.3 划分型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.3 划分型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
