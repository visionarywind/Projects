# 2914. 使二进制字符串变美丽的最少修改次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-changes-to-make-binary-string-beautiful/
- 题目 slug：`minimum-number-of-changes-to-make-binary-string-beautiful`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.3 等价转化
- 难度分：1480
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-changes-to-make-binary-string-beautiful/solutions/2502925/nao-jin-ji-zhuan-wan-pythonjavacgo-by-en-qcm1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[统计 s[i] != s[i+1]（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-number-of-changes-to-make-binary-string-beautiful/solutions/2502925/nao-jin-ji-zhuan-wan-pythonjavacgo-by-en-qcm1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-pythonjavacgo-by-en-qcm1`
- topic id：`2502925`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1Tz4y1N7Wx/) 第二题。

美丽字符串等价于对于所有偶数下标 $i$，有

$$
s[i] = s[i+1]
$$

如果不满足上式，修改其中一个字母即可。

```py [sol-Python3]
class Solution:
    def minChanges(self, s: str) -> int:
        return sum(s[i] != s[i + 1] for i in range(0, len(s), 2))
```

```java [sol-Java]
class Solution {
    public int minChanges(String s) {
        int ans = 0;
        for (int i = 0; i < s.length(); i += 2) {
            if (s.charAt(i) != s.charAt(i + 1)) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minChanges(string s) {
        int ans = 0;
        for (int i = 0; i < s.length(); i += 2) {
            ans += s[i] != s[i + 1];
        }
        return ans;
    }
};
```

```go [sol-Go]
func minChanges(s string) (ans int) {
	for i := 0; i < len(s); i += 2 {
		if s[i] != s[i+1] {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。
