# 1915. 最美子字符串的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-wonderful-substrings/
- 题目 slug：`number-of-wonderful-substrings`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.4 状态压缩前缀和
- 难度分：2235
- 外部题解来源：https://leetcode.cn/problems/number-of-wonderful-substrings/solutions/846871/qian-zhui-he-chang-jian-ji-qiao-by-endle-t57t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和常见技巧（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-wonderful-substrings/solutions/846871/qian-zhui-he-chang-jian-ji-qiao-by-endle-t57t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-chang-jian-ji-qiao-by-endle-t57t`
- topic id：`846871`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于我们只关心每个字母出现次数的奇偶性，因此可以将「字母出现次数」转换成「字母出现次数的奇偶性」，这可以用一个长为 $10$ 的二进制串表示，二进制串的第 $i$ 位为 $0$ 表示第 $i$ 个小写字母出现了偶数次，为 $1$ 表示第 $i$ 个小写字母出现了奇数次。

考虑字母出现次数的前缀和，由于只考虑奇偶性，我们也可以将其视作一个长为 $10$ 的二进制串。此时计算前缀和由加法运算改为异或运算，这是因为异或运算的本质是在模 $2$ 剩余系中进行加法运算，刚好对应奇偶性的变化。

若有两个不同下标的前缀和相同，则这两个前缀和的异或结果为 $0$，意味着这段子串的各个字母的个数均为偶数，符合题目要求。因此，我们可以在求前缀和的同时，用一个长为 $2^{10}=1024$ 的 $\textit{cnt}$ 数组统计每个前缀和二进制串出现的次数，从而得到相同前缀和的对数，即各个字母的个数均为偶数的子串个数。

题目还允许有一个字母出现奇数次，这需要我们寻找两个前缀和，其异或结果的二进制数中恰好有一个 $1$，意味着这段子串的各个字母的个数仅有一个为奇数。对此我们可以枚举当前前缀和的每个比特，将其反转，然后去 $\textit{cnt}$ 中查找该前缀和的出现次数。

将所有统计到的次数累加即为答案。时间复杂度为 $O(10\cdot n)$，$n$ 为字符串 $\textit{word}$ 的长度。

上面所说的技巧在前缀和的题目中经常用到，例如：

- [560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/)
- [930. 和相同的二元子数组](https://leetcode.cn/problems/binary-subarrays-with-sum/)
- [974. 和可被 K 整除的子数组](https://leetcode.cn/problems/subarray-sums-divisible-by-k/)
- [1371. 每个元音包含偶数次的最长子字符串](https://leetcode.cn/problems/find-the-longest-substring-containing-vowels-in-even-counts/)
- [1542. 找出最长的超赞子字符串](https://leetcode.cn/problems/find-longest-awesome-substring/)
- [1590. 使数组和能被 P 整除](https://leetcode.cn/problems/make-sum-divisible-by-p/)



```Python [sol1-Python3]
class Solution:
    def wonderfulSubstrings(self, word: str) -> int:
        cnt = [0] * 1024
        cnt[0] = 1  # 初始前缀和为 0，需将其计入出现次数
        ans = s = 0
        for c in word:
            s ^= 1 << (ord(c) - ord('a'))  # 计算当前前缀和
            ans += cnt[s]  # 所有字母均出现偶数次
            ans += sum(cnt[s ^ (1 << i)] for i in range(10))  # 枚举其中一个字母出现奇数次，反转该字母的出现次数的奇偶性
            cnt[s] += 1  # 更新前缀和出现次数
        return ans
```

```java [sol1-Java]
class Solution {
    public long wonderfulSubstrings(String word) {
        var cnt = new int[1024];
        cnt[0] = 1; // 初始前缀和为 0，需将其计入出现次数
        var ans = 0L;
        for (int i = 0, sum = 0; i < word.length(); ++i) {
            sum ^= 1 << (word.charAt(i) - 'a'); // 计算当前前缀和
            ans += cnt[sum]; // 所有字母均出现偶数次
            for (var j = 1; j < 1024; j <<= 1) // 枚举其中一个字母出现奇数次
                ans += cnt[sum ^ j]; // 反转该字母的出现次数的奇偶性
            ++cnt[sum]; // 更新前缀和出现次数
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long wonderfulSubstrings(string &word) {
        int cnt[1024] = {0};
        cnt[0] = 1; // 初始前缀和为 0，需将其计入出现次数
        long ans = 0L;
        int sum = 0;
        for (char c: word) {
            sum ^= 1 << (c - 'a'); // 计算当前前缀和
            ans += cnt[sum]; // 所有字母均出现偶数次
            for (int j = 1; j < 1024; j <<= 1) // 枚举其中一个字母出现奇数次
                ans += cnt[sum ^ j]; // 反转该字母的出现次数的奇偶性
            ++cnt[sum]; // 更新前缀和出现次数
        }
        return ans;
    }
};
```

```go [sol1-Go]
func wonderfulSubstrings(word string) (ans int64) {
	cnt := [1024]int{1} // 初始前缀和为 0，需将其计入出现次数
	sum := 0
	for _, c := range word {
		sum ^= 1 << (c - 'a') // 计算当前前缀和
		ans += int64(cnt[sum]) // 所有字母均出现偶数次
		for i := 1; i < 1024; i <<= 1 { // 枚举其中一个字母出现奇数次
			ans += int64(cnt[sum^i]) // 反转该字母的出现次数的奇偶性
		}
		cnt[sum]++ // 更新前缀和出现次数
	}
	return
}
```
