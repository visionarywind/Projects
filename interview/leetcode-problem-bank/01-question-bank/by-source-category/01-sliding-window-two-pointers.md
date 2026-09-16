# 滑动窗口与双指针

<!-- generated: do not edit by hand; update scripts/generate_leetcode_bank.py instead -->

来源：[分享丨【算法题单】滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
生成时间：2026-09-16 10:17:56 +0800

本页按来源题单中的标题层级组织，只保存题目元数据和链接，不复制题面或题解。

## 一、定长滑动窗口 / §1.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1 | 1456 | [1456. 定长子串中元音的最大数目](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/) | 1263 | external-index |
| 2 | 643 | [643. 子数组最大平均数 I](https://leetcode.cn/problems/maximum-average-subarray-i/) | Unknown | external-index |
| 3 | 1343 | [1343. 大小为 K 且平均值大于等于阈值的子数组数目](https://leetcode.cn/problems/number-of-sub-arrays-of-size-k-and-average-greater-than-or-equal-to-threshold/) | 1317 | external-index |
| 4 | 2090 | [2090. 半径为 k 的子数组平均值](https://leetcode.cn/problems/k-radius-subarray-averages/) | 1358 | external-index |
| 5 | 2379 | [2379. 得到 K 个黑块的最少涂色次数](https://leetcode.cn/problems/minimum-recolors-to-get-k-consecutive-black-blocks/) | 1360 | external-index |
| 6 | 2841 | [2841. 几乎唯一子数组的最大和](https://leetcode.cn/problems/maximum-sum-of-almost-unique-subarray/) | 1546 | external-index |
| 7 | 2461 | [2461. 长度为 K 子数组中的最大和](https://leetcode.cn/problems/maximum-sum-of-distinct-subarrays-with-length-k/) | 1553 | external-index |
| 8 | 1423 | [1423. 可获得的最大点数](https://leetcode.cn/problems/maximum-points-you-can-obtain-from-cards/) | 1574 | external-index |
| 9 | 1176 | [1176. 健身计划评估](https://leetcode.cn/problems/diet-plan-performance/) | Unknown | external-index |
| 10 | 1100 | [1100. 长度为 K 的无重复字符子串](https://leetcode.cn/problems/find-k-length-substrings-with-no-repeated-characters/) | Unknown | external-index |
| 11 | 1852 | [1852. 每个子数组的数字种类数](https://leetcode.cn/problems/distinct-numbers-in-each-subarray/) | Unknown | external-index |
| 12 | 1151 | [1151. 最少交换次数来组合所有的 1](https://leetcode.cn/problems/minimum-swaps-to-group-all-1s-together/) | Unknown | external-index |
| 13 | 2107 | [2107. 分享 K 个糖果后独特口味的数量](https://leetcode.cn/problems/number-of-unique-flavors-after-sharing-k-candies/) | Unknown | external-index |

## 一、定长滑动窗口 / §1.2 进阶（选做）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 14 | 1052 | [1052. 爱生气的书店老板](https://leetcode.cn/problems/grumpy-bookstore-owner/) | 1418 | external-index |
| 15 | 3679 | [3679. 使库存平衡的最少丢弃次数](https://leetcode.cn/problems/minimum-discards-to-balance-inventory/) | 1639 | external-index |
| 16 | 3439 | [3439. 重新安排会议得到最多空余时间 I](https://leetcode.cn/problems/reschedule-meetings-for-maximum-free-time-i/) | 1729 | external-index |
| 17 | 3694 | [3694. 删除子字符串后不同的终点](https://leetcode.cn/problems/distinct-points-reachable-after-substring-removal/) | 1739 | external-index |
| 18 | 1297 | [1297. 子串的最大出现次数](https://leetcode.cn/problems/maximum-number-of-occurrences-of-a-substring/) | 1748 | external-index |
| 19 | 2134 | [2134. 最少交换次数来组合所有的 1 II](https://leetcode.cn/problems/minimum-swaps-to-group-all-1s-together-ii/) | 1748 | external-index |
| 20 | 1652 | [1652. 拆炸弹](https://leetcode.cn/problems/defuse-the-bomb/) | Unknown | external-index |
| 21 | 4043 | [4043. 恰好有 K 对相等相邻字符的循环移位数量](https://leetcode.cn/problems/count-rotations-with-exactly-k-equal-adjacent-pairs/) | Unknown | external-index |
| 22 | 4044 | [4044. 统计好循环移位的数量](https://leetcode.cn/problems/count-good-cyclic-rotations/) | Unknown | external-index |
| 23 | 3652 | [3652. 按策略买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-using-strategy/) | Unknown | external-index |
| 24 | 567 | [567. 字符串的排列](https://leetcode.cn/problems/permutation-in-string/) | Unknown | external-index |
| 25 | 438 | [438. 找到字符串中所有字母异位词](https://leetcode.cn/problems/find-all-anagrams-in-a-string/) | Unknown | external-index |
| 26 | 30 | [30. 串联所有单词的子串](https://leetcode.cn/problems/substring-with-concatenation-of-all-words/) | Unknown | external-index |
| 27 | 1888 | [1888. 使二进制字符串字符交替的最少反转次数](https://leetcode.cn/problems/minimum-number-of-flips-to-make-the-binary-string-alternating/) | 2006 | external-index |
| 28 | 2156 | [2156. 查找给定哈希值的子串](https://leetcode.cn/problems/find-substring-with-given-hash-value/) | 2063 | external-index |
| 29 | 2953 | [2953. 统计完全子字符串](https://leetcode.cn/problems/count-complete-substrings/) | 2449 | external-index |
| 30 | 3672 | [3672. 子数组中加权众数的总和](https://leetcode.cn/problems/sum-of-weighted-modes-in-subarrays/) | Unknown | external-index |
| 31 | 2067 | [2067. 等计数子串的数量](https://leetcode.cn/problems/number-of-equal-count-substrings/) | Unknown | external-index |
| 32 | 2524 | [2524. 子数组的最大频率分数](https://leetcode.cn/problems/maximum-frequency-score-of-a-subarray/) | Unknown | external-index |
| 33 | 2200 | [2200. 找出数组中的所有 K 近邻下标](https://leetcode.cn/problems/find-all-k-distant-indices-in-an-array/) | Unknown | external-index |
| 34 | 1461 | [1461. 检查一个字符串是否包含所有长度为 K 的二进制子串](https://leetcode.cn/problems/check-if-a-string-contains-all-binary-codes-of-size-k/) | 1504 | external-index |
| 35 | 1016 | [1016. 子串能表示从 1 到 N 数字的二进制串](https://leetcode.cn/problems/binary-string-with-substrings-representing-1-to-n/) | Unknown | external-index |
| 36 | 2653 | [2653. 滑动子数组的美丽值](https://leetcode.cn/problems/sliding-subarray-beauty/) | 1786 | external-index |

## 二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 37 | 3 | [3. 无重复字符的最长子串](https://leetcode.cn/problems/longest-substring-without-repeating-characters/) | Unknown | external-index |
| 38 | 3090 | [3090. 每个字符最多出现两次的最长子字符串](https://leetcode.cn/problems/maximum-length-substring-with-two-occurrences/) | 1329 | external-index |
| 39 | 1493 | [1493. 删掉一个元素以后全为 1 的最长子数组](https://leetcode.cn/problems/longest-subarray-of-1s-after-deleting-one-element/) | 1423 | external-index |
| 40 | 3634 | [3634. 使数组平衡的最少移除数目](https://leetcode.cn/problems/minimum-removals-to-balance-array/) | 1453 | external-index |
| 41 | 1208 | [1208. 尽可能使字符串相等](https://leetcode.cn/problems/get-equal-substrings-within-budget/) | 1497 | external-index |
| 42 | 904 | [904. 水果成篮](https://leetcode.cn/problems/fruit-into-baskets/) | 1516 | external-index |
| 43 | 1695 | [1695. 删除子数组的最大得分](https://leetcode.cn/problems/maximum-erasure-value/) | 1529 | external-index |
| 44 | 2958 | [2958. 最多 K 个重复元素的最长子数组](https://leetcode.cn/problems/length-of-longest-subarray-with-at-most-k-frequency/) | 1535 | external-index |
| 45 | 2024 | [2024. 考试的最大困扰度](https://leetcode.cn/problems/maximize-the-confusion-of-an-exam/) | 1643 | external-index |
| 46 | 1004 | [1004. 最大连续 1 的个数 III](https://leetcode.cn/problems/max-consecutive-ones-iii/) | 1656 | external-index |
| 47 | 3641 | [3641. 最长半重复子数组](https://leetcode.cn/problems/longest-semi-repeating-subarray/) | Unknown | external-index |

## 二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 48 | 2730 | [2730. 找到最长的半重复子字符串](https://leetcode.cn/problems/find-the-longest-semi-repetitive-substring/) | Unknown | external-index |
| 49 | 2779 | [2779. 数组的最大美丽值](https://leetcode.cn/problems/maximum-beauty-of-an-array-after-applying-operation/) | 1638 | external-index |
| 50 | 4032 | [4032. 至多 K 个不同质因数集合的最长子数组](https://leetcode.cn/problems/longest-subarray-with-at-most-k-distinct-prime-factors/) | 1759 | external-index |
| 51 | 1658 | [1658. 将 x 减到 0 的最小操作数](https://leetcode.cn/problems/minimum-operations-to-reduce-x-to-zero/) | 1817 | external-index |
| 52 | 1838 | [1838. 最高频元素的频数](https://leetcode.cn/problems/frequency-of-the-most-frequent-element/) | 1876 | external-index |
| 53 | 2516 | [2516. 每种字符至少取 K 个](https://leetcode.cn/problems/take-k-of-each-character-from-left-and-right/) | 1948 | external-index |
| 54 | 2831 | [2831. 找出最长等值子数组](https://leetcode.cn/problems/find-the-longest-equal-subarray/) | 1976 | external-index |
| 55 | 2271 | [2271. 毯子覆盖的最多白色砖块数](https://leetcode.cn/problems/maximum-white-tiles-covered-by-a-carpet/) | 2022 | external-index |
| 56 | 2106 | [2106. 摘水果](https://leetcode.cn/problems/maximum-fruits-harvested-after-at-most-k-steps/) | 2062 | external-index |
| 57 | 2555 | [2555. 两个线段获得的最多奖品](https://leetcode.cn/problems/maximize-win-from-two-segments/) | 2081 | external-index |
| 58 | 2009 | [2009. 使数组连续的最少操作数](https://leetcode.cn/problems/minimum-number-of-operations-to-make-array-continuous/) | 2084 | external-index |
| 59 | 1610 | [1610. 可见点的最大数目](https://leetcode.cn/problems/maximum-number-of-visible-points/) | 2147 | external-index |
| 60 | 2781 | [2781. 最长合法子字符串的长度](https://leetcode.cn/problems/length-of-the-longest-valid-substring/) | 2204 | external-index |
| 61 | 3411 | [3411. 最长乘积等价子数组](https://leetcode.cn/problems/maximum-subarray-with-equal-products/) | 2300 | external-index |
| 62 | 3413 | [3413. 收集连续 K 个袋子可以获得的最多硬币数量](https://leetcode.cn/problems/maximum-coins-from-k-consecutive-bags/) | 2374 | external-index |
| 63 | 395 | [395. 至少有 K 个重复字符的最长子串](https://leetcode.cn/problems/longest-substring-with-at-least-k-repeating-characters/) | Unknown | external-index |
| 64 | 1763 | [1763. 最长的美好子字符串](https://leetcode.cn/problems/longest-nice-substring/) | Unknown | external-index |
| 65 | 2968 | [2968. 执行操作使频率分数最大](https://leetcode.cn/problems/apply-operations-to-maximize-frequency-score/) | 2444 | external-index |
| 66 | 1040 | [1040. 移动石子直到连续 II](https://leetcode.cn/problems/moving-stones-until-consecutive-ii/) | 2456 | external-index |
| 67 | 487 | [487. 最大连续 1 的个数 II](https://leetcode.cn/problems/max-consecutive-ones-ii/) | Unknown | external-index |
| 68 | 159 | [159. 至多包含两个不同字符的最长子串](https://leetcode.cn/problems/longest-substring-with-at-most-two-distinct-characters/) | Unknown | external-index |
| 69 | 340 | [340. 至多包含 K 个不同字符的最长子串](https://leetcode.cn/problems/longest-substring-with-at-most-k-distinct-characters/) | Unknown | external-index |

## 二、不定长滑动窗口 / §2.2 越长越合法/求最短/最小

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 70 | 209 | [209. 长度最小的子数组](https://leetcode.cn/problems/minimum-size-subarray-sum/) | Unknown | external-index |
| 71 | 3795 | [3795. 不同元素和至少为 K 的最短子数组长度](https://leetcode.cn/problems/minimum-subarray-length-with-distinct-sum-at-least-k/) | 1505 | external-index |
| 72 | 2904 | [2904. 最短且字典序最小的美丽子字符串](https://leetcode.cn/problems/shortest-and-lexicographically-smallest-beautiful-string/) | Unknown | external-index |
| 73 | 1234 | [1234. 替换子串得到平衡字符串](https://leetcode.cn/problems/replace-the-substring-for-balanced-string/) | 1878 | external-index |
| 74 | 2875 | [2875. 无限数组的最短子数组](https://leetcode.cn/problems/minimum-size-subarray-in-infinite-array/) | 1914 | external-index |
| 75 | 76 | [76. 最小覆盖子串](https://leetcode.cn/problems/minimum-window-substring/) | Unknown | external-index |
| 76 | 632 | [632. 最小区间](https://leetcode.cn/problems/smallest-range-covering-elements-from-k-lists/) | Unknown | external-index |

## 二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.1 越短越合法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 77 | 713 | [713. 乘积小于 K 的子数组](https://leetcode.cn/problems/subarray-product-less-than-k/) | Unknown | external-index |
| 78 | 3258 | [3258. 统计满足 K 约束的子字符串数量 I](https://leetcode.cn/problems/count-substrings-that-satisfy-k-constraint-i/) | Unknown | external-index |
| 79 | 2302 | [2302. 统计得分小于 K 的子数组数目](https://leetcode.cn/problems/count-subarrays-with-score-less-than-k/) | 1808 | external-index |
| 80 | 2762 | [2762. 不间断子数组](https://leetcode.cn/problems/continuous-subarrays/) | 1940 | external-index |
| 81 | LCP 68 | [LCP 68. 美观的花束](https://leetcode.cn/problems/1GxJYY/) | Unknown | external-index |
| 82 | 2743 | [2743. 计算没有重复字符的子字符串数量](https://leetcode.cn/problems/count-substrings-without-repeating-character/) | Unknown | external-index |
| 83 | 3134 | [3134. 找出唯一性数组的中位数](https://leetcode.cn/problems/find-the-median-of-the-uniqueness-array/) | 2451 | external-index |
| 84 | 3261 | [3261. 统计满足 K 约束的子字符串数量 II](https://leetcode.cn/problems/count-substrings-that-satisfy-k-constraint-ii/) | 2659 | external-index |

## 二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.2 越长越合法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 85 | 1358 | [1358. 包含所有三种字符的子字符串数目](https://leetcode.cn/problems/number-of-substrings-containing-all-three-characters/) | 1646 | external-index |
| 86 | 2962 | [2962. 统计最大元素出现至少 K 次的子数组](https://leetcode.cn/problems/count-subarrays-where-max-element-appears-at-least-k-times/) | 1701 | external-index |
| 87 | 3325 | [3325. 字符至少出现 K 次的子字符串 I](https://leetcode.cn/problems/count-substrings-with-k-frequency-characters-i/) | Unknown | external-index |
| 88 | 2062 | [2062. 统计字符串中的元音子字符串](https://leetcode.cn/problems/count-vowel-substrings-of-a-string/) | Unknown | external-index |
| 89 | 2799 | [2799. 统计完全子数组的数目](https://leetcode.cn/problems/count-complete-subarrays-in-an-array/) | Unknown | external-index |
| 90 | 2537 | [2537. 统计好子数组的数目](https://leetcode.cn/problems/count-the-number-of-good-subarrays/) | 1892 | external-index |
| 91 | 3298 | [3298. 统计重新排列后包含另一个字符串的子字符串数目 II](https://leetcode.cn/problems/count-substrings-that-can-be-rearranged-to-contain-a-string-ii/) | 1909 | external-index |
| 92 | 2495 | [2495. 乘积为偶数的子数组数](https://leetcode.cn/problems/number-of-subarrays-having-even-product/) | Unknown | external-index |

## 二、不定长滑动窗口 / §2.3 求子数组个数 / §2.3.3 恰好型滑动窗口

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 93 | 930 | [930. 和相同的二元子数组](https://leetcode.cn/problems/binary-subarrays-with-sum/) | 1592 | external-index |
| 94 | 1248 | [1248. 统计「优美子数组」](https://leetcode.cn/problems/count-number-of-nice-subarrays/) | 1624 | external-index |
| 95 | 3306 | [3306. 元音辅音字符串计数 II](https://leetcode.cn/problems/count-of-substrings-containing-every-vowel-and-k-consonants-ii/) | 2200 | external-index |
| 96 | 992 | [992. K 个不同整数的子数组](https://leetcode.cn/problems/subarrays-with-k-different-integers/) | 2210 | external-index |
| 97 | 3859 | [3859. 统计包含 K 个不同整数的子数组](https://leetcode.cn/problems/count-subarrays-with-k-distinct-integers/) | 2302 | external-index |

## 二、不定长滑动窗口 / §2.4 其他（选做）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 98 | 825 | [825. 适龄的朋友](https://leetcode.cn/problems/friends-of-appropriate-ages/) | 1697 | external-index |
| 99 | 2401 | [2401. 最长优雅子数组](https://leetcode.cn/problems/longest-nice-subarray/) | 1750 | external-index |
| 100 | 1156 | [1156. 单字符重复子串的最大长度](https://leetcode.cn/problems/swap-for-longest-repeated-character-substring/) | 1787 | external-index |
| 101 | 424 | [424. 替换后的最长重复字符](https://leetcode.cn/problems/longest-repeating-character-replacement/) | Unknown | external-index |
| 102 | 438 | [438. 找到字符串中所有字母异位词](https://leetcode.cn/problems/find-all-anagrams-in-a-string/) | Unknown | external-index |
| 103 | 1712 | [1712. 将数组分成三个子数组的方案数](https://leetcode.cn/problems/ways-to-split-array-into-three-subarrays/) | 2079 | external-index |
| 104 | 4033 | [4033. 有效 K 个不同元素子数组 I](https://leetcode.cn/problems/valid-k-unique-subarrays-i/) | 2314 | external-index |
| 105 | LCR 180 | [LCR 180. 文件组合](https://leetcode.cn/problems/he-wei-sde-lian-xu-zheng-shu-xu-lie-lcof/) | Unknown | external-index |
| 106 | 1918 | [1918. 第 K 小的子数组和](https://leetcode.cn/problems/kth-smallest-subarray-sum/) | Unknown | external-index |

## 三、单序列双指针 / §3.1 反转字符串

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 107 | 344 | [344. 反转字符串](https://leetcode.cn/problems/reverse-string/) | Unknown | external-index |
| 108 | 3794 | [3794. 反转字符串前缀](https://leetcode.cn/problems/reverse-string-prefix/) | Unknown | external-index |
| 109 | 2000 | [2000. 反转单词前缀](https://leetcode.cn/problems/reverse-prefix-of-word/) | 1199 | external-index |
| 110 | 3643 | [3643. 垂直翻转子矩阵](https://leetcode.cn/problems/flip-square-submatrix-vertically/) | 1235 | external-index |
| 111 | 832 | [832. 翻转图像](https://leetcode.cn/problems/flipping-an-image/) | 1243 | external-index |
| 112 | 3823 | [3823. 反转一个字符串里的字母后反转特殊字符](https://leetcode.cn/problems/reverse-letters-then-special-characters-in-a-string/) | 1250 | external-index |
| 113 | 541 | [541. 反转字符串 II](https://leetcode.cn/problems/reverse-string-ii/) | Unknown | external-index |
| 114 | 557 | [557. 反转字符串中的单词 III](https://leetcode.cn/problems/reverse-words-in-a-string-iii/) | Unknown | external-index |
| 115 | 151 | [151. 反转字符串中的单词](https://leetcode.cn/problems/reverse-words-in-a-string/) | Unknown | external-index |
| 116 | 3775 | [3775. 反转元音数相同的单词](https://leetcode.cn/problems/reverse-words-with-same-vowel-count/) | 1392 | external-index |
| 117 | 917 | [917. 仅仅反转字母](https://leetcode.cn/problems/reverse-only-letters/) | Unknown | external-index |
| 118 | 345 | [345. 反转字符串中的元音字母](https://leetcode.cn/problems/reverse-vowels-of-a-string/) | Unknown | external-index |
| 119 | 3865 | [3865. 反转 K 个子数组](https://leetcode.cn/problems/reverse-k-subarrays/) | Unknown | external-index |
| 120 | 186 | [186. 反转字符串中的单词 II](https://leetcode.cn/problems/reverse-words-in-a-string-ii/) | Unknown | external-index |

## 三、单序列双指针 / §3.2 相向双指针

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 121 | 2697 | [2697. 字典序最小回文串](https://leetcode.cn/problems/lexicographically-smallest-palindrome/) | 1304 | external-index |
| 122 | 125 | [125. 验证回文串](https://leetcode.cn/problems/valid-palindrome/) | Unknown | external-index |
| 123 | 3936 | [3936. 将 0 移到末尾的最少交换次数](https://leetcode.cn/problems/minimum-swaps-to-move-zeros-to-end/) | 1346 | external-index |
| 124 | 1750 | [1750. 删除字符串两端相同字符后的最短长度](https://leetcode.cn/problems/minimum-length-of-string-after-deleting-similar-ends/) | 1502 | external-index |
| 125 | 2105 | [2105. 给植物浇水 II](https://leetcode.cn/problems/watering-plants-ii/) | 1507 | external-index |
| 126 | 977 | [977. 有序数组的平方](https://leetcode.cn/problems/squares-of-a-sorted-array/) | Unknown | external-index |
| 127 | 658 | [658. 找到 K 个最接近的元素](https://leetcode.cn/problems/find-k-closest-elements/) | Unknown | external-index |
| 128 | 1471 | [1471. 数组中的 K 个最强值](https://leetcode.cn/problems/the-k-strongest-values-in-an-array/) | Unknown | external-index |
| 129 | 167 | [167. 两数之和 II - 输入有序数组](https://leetcode.cn/problems/two-sum-ii-input-array-is-sorted/) | Unknown | external-index |
| 130 | 633 | [633. 平方数之和](https://leetcode.cn/problems/sum-of-square-numbers/) | Unknown | external-index |
| 131 | 2824 | [2824. 统计和小于目标的下标对数目](https://leetcode.cn/problems/count-pairs-whose-sum-is-less-than-target/) | Unknown | external-index |
| 132 | LCP 28 | [LCP 28. 采购方案](https://leetcode.cn/problems/4xy4Wx/) | 2824 | external-index |
| 133 | 16 | [16. 最接近的三数之和](https://leetcode.cn/problems/3sum-closest/) | Unknown | external-index |
| 134 | 15 | [15. 三数之和](https://leetcode.cn/problems/3sum/) | Unknown | external-index |
| 135 | 18 | [18. 四数之和](https://leetcode.cn/problems/4sum/) | Unknown | external-index |
| 136 | 1577 | [1577. 数的平方等于两数乘积的方法数](https://leetcode.cn/problems/number-of-ways-where-square-of-number-is-equal-to-product-of-two-numbers/) | 1594 | external-index |
| 137 | 3862 | [3862. 找出最小平衡下标](https://leetcode.cn/problems/find-the-smallest-balanced-index/) | 1697 | external-index |
| 138 | 611 | [611. 有效三角形的个数](https://leetcode.cn/problems/valid-triangle-number/) | Unknown | external-index |
| 139 | 923 | [923. 三数之和的多种可能](https://leetcode.cn/problems/3sum-with-multiplicity/) | 1711 | external-index |
| 140 | 2563 | [2563. 统计公平数对的数目](https://leetcode.cn/problems/count-the-number-of-fair-pairs/) | 1721 | external-index |
| 141 | 948 | [948. 令牌放置](https://leetcode.cn/problems/bag-of-tokens/) | 1762 | external-index |
| 142 | 11 | [11. 盛最多水的容器](https://leetcode.cn/problems/container-with-most-water/) | Unknown | external-index |
| 143 | 42 | [42. 接雨水](https://leetcode.cn/problems/trapping-rain-water/) | Unknown | external-index |
| 144 | 1616 | [1616. 分割两个字符串得到回文串](https://leetcode.cn/problems/split-two-strings-to-make-palindrome/) | 1868 | external-index |
| 145 | 1498 | [1498. 满足条件的子序列数目](https://leetcode.cn/problems/number-of-subsequences-that-satisfy-the-given-sum-condition/) | 2276 | external-index |
| 146 | 1782 | [1782. 统计点对的数目](https://leetcode.cn/problems/count-pairs-of-nodes/) | 2457 | external-index |
| 147 | 1099 | [1099. 小于 K 的两数之和](https://leetcode.cn/problems/two-sum-less-than-k/) | Unknown | external-index |
| 148 | 360 | [360. 有序转化数组](https://leetcode.cn/problems/sort-transformed-array/) | Unknown | external-index |
| 149 | 2422 | [2422. 使用合并操作将数组转换为回文序列](https://leetcode.cn/problems/merge-operations-to-turn-array-into-a-palindrome/) | Unknown | external-index |
| 150 | 259 | [259. 较小的三数之和](https://leetcode.cn/problems/3sum-smaller/) | Unknown | external-index |
| 151 | 3802 | [3802. 给纸张涂色的方式数量](https://leetcode.cn/problems/number-of-ways-to-paint-sheets/) | Unknown | external-index |
| 152 | 1861 | [1861. 旋转盒子](https://leetcode.cn/problems/rotating-the-box/) | 1537 | external-index |
| 153 | 3814 | [3814. 预算下的最大总容量](https://leetcode.cn/problems/maximum-capacity-within-budget/) | 1796 | external-index |

## 三、单序列双指针 / §3.3 同向双指针

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 154 | 2200 | [2200. 找出数组中的所有 K 近邻下标](https://leetcode.cn/problems/find-all-k-distant-indices-in-an-array/) | Unknown | external-index |
| 155 | 611 | [611. 有效三角形的个数](https://leetcode.cn/problems/valid-triangle-number/) | Unknown | external-index |
| 156 | 3649 | [3649. 完美对的数目](https://leetcode.cn/problems/number-of-perfect-pairs/) | 1716 | external-index |
| 157 | 1871 | [1871. 跳跃游戏 VII](https://leetcode.cn/problems/jump-game-vii/) | 1896 | external-index |
| 158 | 1574 | [1574. 删除最短的子数组使剩余数组有序](https://leetcode.cn/problems/shortest-subarray-to-be-removed-to-make-array-sorted/) | 1932 | external-index |
| 159 | 2972 | [2972. 统计移除递增子数组的数目 II](https://leetcode.cn/problems/count-the-number-of-incremovable-subarrays-ii/) | 2153 | external-index |
| 160 | 2122 | [2122. 还原原数组](https://leetcode.cn/problems/recover-the-original-array/) | 2159 | external-index |
| 161 | 2234 | [2234. 花园的最大总美丽值](https://leetcode.cn/problems/maximum-total-beauty-of-the-gardens/) | 2562 | external-index |
| 162 | 1989 | [1989. 捉迷藏中可捕获的最大人数](https://leetcode.cn/problems/maximum-number-of-people-that-can-be-caught-in-tag/) | Unknown | external-index |
| 163 | 3323 | [3323. 通过插入区间最小化连通组](https://leetcode.cn/problems/minimize-connected-groups-by-inserting-interval/) | Unknown | external-index |
| 164 | 581 | [581. 最短无序连续子数组](https://leetcode.cn/problems/shortest-unsorted-continuous-subarray/) | Unknown | external-index |
| 165 | 3555 | [3555. 排序每个滑动窗口中最小的子数组](https://leetcode.cn/problems/smallest-subarray-to-sort-in-every-sliding-window/) | Unknown | external-index |

## 三、单序列双指针 / §3.4 背向双指针

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 166 | 1793 | [1793. 好子数组的最大分数](https://leetcode.cn/problems/maximum-score-of-a-good-subarray/) | 1946 | external-index |
| 167 | 976 | [976. 三角形的最大周长](https://leetcode.cn/problems/largest-perimeter-triangle/) | Unknown | external-index |

## 三、单序列双指针 / §3.5 原地修改

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 168 | 27 | [27. 移除元素](https://leetcode.cn/problems/remove-element/) | Unknown | external-index |
| 169 | 26 | [26. 删除有序数组中的重复项](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/) | Unknown | external-index |
| 170 | 80 | [80. 删除有序数组中的重复项 II](https://leetcode.cn/problems/remove-duplicates-from-sorted-array-ii/) | Unknown | external-index |
| 171 | 3940 | [3940. 限制有序数组中的元素出现次数](https://leetcode.cn/problems/limit-occurrences-in-sorted-array/) | Unknown | external-index |
| 172 | 2273 | [2273. 移除字母异位词后的结果数组](https://leetcode.cn/problems/find-resultant-array-after-removing-anagrams/) | Unknown | external-index |
| 173 | 3684 | [3684. 至多 K 个不同元素的最大和](https://leetcode.cn/problems/maximize-sum-of-at-most-k-distinct-elements/) | Unknown | external-index |
| 174 | 283 | [283. 移动零](https://leetcode.cn/problems/move-zeroes/) | Unknown | external-index |
| 175 | 905 | [905. 按奇偶排序数组](https://leetcode.cn/problems/sort-array-by-parity/) | Unknown | external-index |
| 176 | 922 | [922. 按奇偶排序数组 II](https://leetcode.cn/problems/sort-array-by-parity-ii/) | Unknown | external-index |
| 177 | 3467 | [3467. 将数组按照奇偶性转化](https://leetcode.cn/problems/transform-array-by-parity/) | Unknown | external-index |
| 178 | 3992 | [3992. 重新排列字符串以避免字符对](https://leetcode.cn/problems/rearrange-string-to-avoid-character-pair/) | Unknown | external-index |
| 179 | 2460 | [2460. 对数组执行操作](https://leetcode.cn/problems/apply-operations-to-an-array/) | Unknown | external-index |
| 180 | 1089 | [1089. 复写零](https://leetcode.cn/problems/duplicate-zeros/) | Unknown | external-index |
| 181 | 75 | [75. 颜色分类](https://leetcode.cn/problems/sort-colors/) | Unknown | external-index |
| 182 | 2784 | [2784. 检查数组是否是好的](https://leetcode.cn/problems/check-if-array-is-good/) | Unknown | external-index |
| 183 | 442 | [442. 数组中重复的数据](https://leetcode.cn/problems/find-all-duplicates-in-an-array/) | Unknown | external-index |
| 184 | 448 | [448. 找到所有数组中消失的数字](https://leetcode.cn/problems/find-all-numbers-disappeared-in-an-array/) | Unknown | external-index |
| 185 | 1470 | [1470. 重新排列数组](https://leetcode.cn/problems/shuffle-the-array/) | Unknown | external-index |
| 186 | 1920 | [1920. 基于排列构建数组](https://leetcode.cn/problems/build-array-from-permutation/) | Unknown | external-index |
| 187 | 41 | [41. 缺失的第一个正数](https://leetcode.cn/problems/first-missing-positive/) | Unknown | external-index |

## 三、单序列双指针 / §3.6 矩阵上的双指针

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 188 | 240 | [240. 搜索二维矩阵 II](https://leetcode.cn/problems/search-a-2d-matrix-ii/) | Unknown | external-index |
| 189 | 1351 | [1351. 统计有序矩阵中的负数](https://leetcode.cn/problems/count-negative-numbers-in-a-sorted-matrix/) | Unknown | external-index |

## 四、双序列双指针 / §4.1 双指针

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 190 | 2109 | [2109. 向字符串添加空格](https://leetcode.cn/problems/adding-spaces-to-a-string/) | 1315 | external-index |
| 191 | 2540 | [2540. 最小公共值](https://leetcode.cn/problems/minimum-common-value/) | Unknown | external-index |
| 192 | 350 | [350. 两个数组的交集 II](https://leetcode.cn/problems/intersection-of-two-arrays-ii/) | Unknown | external-index |
| 193 | 88 | [88. 合并两个有序数组](https://leetcode.cn/problems/merge-sorted-array/) | Unknown | external-index |
| 194 | 2570 | [2570. 合并两个二维数组 - 求和法](https://leetcode.cn/problems/merge-two-2d-arrays-by-summing-values/) | Unknown | external-index |
| 195 | 4001 | [4001. 聚合两个时间序列](https://leetcode.cn/problems/aggregate-two-time-series/) | 1506 | external-index |
| 196 | 1855 | [1855. 下标对中的最大距离](https://leetcode.cn/problems/maximum-distance-between-a-pair-of-values/) | 1515 | external-index |
| 197 | LCP 18 | [LCP 18. 早餐组合](https://leetcode.cn/problems/2vYnGI/) | Unknown | external-index |
| 198 | 1385 | [1385. 两个数组间的距离值](https://leetcode.cn/problems/find-the-distance-value-between-two-arrays/) | Unknown | external-index |
| 199 | 925 | [925. 长按键入](https://leetcode.cn/problems/long-pressed-name/) | Unknown | external-index |
| 200 | 809 | [809. 情感丰富的文字](https://leetcode.cn/problems/expressive-words/) | 1605 | external-index |
| 201 | 2337 | [2337. 移动片段得到字符串](https://leetcode.cn/problems/move-pieces-to-obtain-a-string/) | 1693 | external-index |
| 202 | 777 | [777. 在 LR 字符串中交换相邻字符](https://leetcode.cn/problems/swap-adjacent-in-lr-string/) | 2337 | external-index |
| 203 | 844 | [844. 比较含退格的字符串](https://leetcode.cn/problems/backspace-string-compare/) | Unknown | external-index |
| 204 | 986 | [986. 区间列表的交集](https://leetcode.cn/problems/interval-list-intersections/) | Unknown | external-index |
| 205 | 面试题 16.06 | [面试题 16.06. 最小差](https://leetcode.cn/problems/smallest-difference-lcci/) | Unknown | external-index |
| 206 | 475 | [475. 供暖器](https://leetcode.cn/problems/heaters/) | Unknown | external-index |
| 207 | 3998 | [3998. 使用子序列排序转换二进制字符串](https://leetcode.cn/problems/transform-binary-string-using-subsequence-sort/) | 1862 | external-index |
| 208 | 1537 | [1537. 最大得分](https://leetcode.cn/problems/get-the-maximum-score/) | 1961 | external-index |
| 209 | 244 | [244. 最短单词距离 II](https://leetcode.cn/problems/shortest-word-distance-ii/) | Unknown | external-index |
| 210 | 2838 | [2838. 英雄可以获得的最大金币数](https://leetcode.cn/problems/maximum-coins-heroes-can-collect/) | Unknown | external-index |
| 211 | 1229 | [1229. 安排会议日程](https://leetcode.cn/problems/meeting-scheduler/) | Unknown | external-index |
| 212 | 1570 | [1570. 两个稀疏向量的点积](https://leetcode.cn/problems/dot-product-of-two-sparse-vectors/) | Unknown | external-index |
| 213 | 1868 | [1868. 两个行程编码数组的积](https://leetcode.cn/problems/product-of-two-run-length-encoded-arrays/) | Unknown | external-index |

## 四、双序列双指针 / §4.2 判断子序列

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 214 | 392 | [392. 判断子序列](https://leetcode.cn/problems/is-subsequence/) | Unknown | external-index |
| 215 | 524 | [524. 通过删除字母匹配到字典里最长单词](https://leetcode.cn/problems/longest-word-in-dictionary-through-deleting/) | Unknown | external-index |
| 216 | 2486 | [2486. 追加字符以获得子序列](https://leetcode.cn/problems/append-characters-to-string-to-make-subsequence/) | 1363 | external-index |
| 217 | 2825 | [2825. 循环增长使字符串子序列等于另一个字符串](https://leetcode.cn/problems/make-string-a-subsequence-using-cyclic-increments/) | 1415 | external-index |
| 218 | 1023 | [1023. 驼峰式匹配](https://leetcode.cn/problems/camelcase-matching/) | 1537 | external-index |
| 219 | 3132 | [3132. 找出与数组相加的整数 II](https://leetcode.cn/problems/find-the-integer-added-to-array-ii/) | 1620 | external-index |
| 220 | 522 | [522. 最长特殊序列 II](https://leetcode.cn/problems/longest-uncommon-subsequence-ii/) | 1700 | external-index |
| 221 | 1826 | [1826. 有缺陷的传感器](https://leetcode.cn/problems/faulty-sensor/) | Unknown | external-index |
| 222 | 4026 | [4026. 工位的最大间隔](https://leetcode.cn/problems/maximum-gap-between-stations/) | 1675 | external-index |
| 223 | 3983 | [3983. 一次替换后的子序列](https://leetcode.cn/problems/subsequence-after-one-replacement/) | 1755 | external-index |
| 224 | 1898 | [1898. 可移除字符的最大数目](https://leetcode.cn/problems/maximum-number-of-removable-characters/) | 1913 | external-index |
| 225 | 2565 | [2565. 最少得分子序列](https://leetcode.cn/problems/subsequence-with-the-minimum-score/) | 2432 | external-index |
| 226 | 3302 | [3302. 字典序最小的合法序列](https://leetcode.cn/problems/find-the-lexicographically-smallest-valid-sequence/) | 2474 | external-index |

## 五、三指针

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 227 | 2367 | [2367. 等差三元组的数目](https://leetcode.cn/problems/number-of-arithmetic-triplets/) | Unknown | external-index |
| 228 | 2563 | [2563. 统计公平数对的数目](https://leetcode.cn/problems/count-the-number-of-fair-pairs/) | 1721 | external-index |
| 229 | 795 | [795. 区间子数组个数](https://leetcode.cn/problems/number-of-subarrays-with-bounded-maximum/) | 1817 | external-index |
| 230 | 2444 | [2444. 统计定界子数组的数目](https://leetcode.cn/problems/count-subarrays-with-fixed-bounds/) | 2093 | external-index |
| 231 | 3347 | [3347. 执行操作后元素的最高频率 II](https://leetcode.cn/problems/maximum-frequency-of-an-element-after-performing-operations-ii/) | 2156 | external-index |
| 232 | 1213 | [1213. 三个有序数组的交集](https://leetcode.cn/problems/intersection-of-three-sorted-arrays/) | Unknown | external-index |
| 233 | 3464 | [3464. 正方形上的点之间的最大距离](https://leetcode.cn/problems/maximize-the-distance-between-points-on-a-square/) | 2806 | external-index |

## 六、分组循环

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 234 | 485 | [485. 最大连续 1 的个数](https://leetcode.cn/problems/max-consecutive-ones/) | Unknown | external-index |
| 235 | 1446 | [1446. 连续字符](https://leetcode.cn/problems/consecutive-characters/) | 1165 | external-index |
| 236 | 1869 | [1869. 哪种连续子字符串更长](https://leetcode.cn/problems/longer-contiguous-segments-of-ones-than-zeros/) | 1205 | external-index |
| 237 | 4038 | [4038. 统计特殊整数个数](https://leetcode.cn/problems/count-integers-appearing-in-a-single-block/) | Unknown | external-index |
| 238 | 2414 | [2414. 最长的字母序连续子字符串的长度](https://leetcode.cn/problems/length-of-the-longest-alphabetical-continuous-substring/) | 1222 | external-index |
| 239 | 3456 | [3456. 找出长度为 K 的特殊子字符串](https://leetcode.cn/problems/find-special-substring-of-length-k/) | 1244 | external-index |
| 240 | 830 | [830. 较大分组的位置](https://leetcode.cn/problems/positions-of-large-groups/) | 1252 | external-index |
| 241 | 2273 | [2273. 移除字母异位词后的结果数组](https://leetcode.cn/problems/find-resultant-array-after-removing-anagrams/) | 1295 | external-index |
| 242 | 2264 | [2264. 字符串中最大的 3 位相同数字](https://leetcode.cn/problems/largest-3-same-digit-number-in-string/) | 1309 | external-index |
| 243 | 2348 | [2348. 全 0 子数组的数目](https://leetcode.cn/problems/number-of-zero-filled-subarrays/) | 1316 | external-index |
| 244 | 1513 | [1513. 仅含 1 的子串数](https://leetcode.cn/problems/number-of-substrings-with-only-1s/) | 1351 | external-index |
| 245 | 1957 | [1957. 删除字符使字符串变好](https://leetcode.cn/problems/delete-characters-to-make-fancy-string/) | 1358 | external-index |
| 246 | 674 | [674. 最长连续递增序列](https://leetcode.cn/problems/longest-continuous-increasing-subsequence/) | Unknown | external-index |
| 247 | 3708 | [3708. 最长斐波那契子数组](https://leetcode.cn/problems/longest-fibonacci-subarray/) | 1381 | external-index |
| 248 | 696 | [696. 计数二进制子串](https://leetcode.cn/problems/count-binary-substrings/) | Unknown | external-index |
| 249 | 978 | [978. 最长湍流子数组](https://leetcode.cn/problems/longest-turbulent-subarray/) | 1393 | external-index |
| 250 | 2110 | [2110. 股票平滑下跌阶段的数目](https://leetcode.cn/problems/number-of-smooth-descent-periods-of-a-stock/) | 1408 | external-index |
| 251 | 228 | [228. 汇总区间](https://leetcode.cn/problems/summary-ranges/) | Unknown | external-index |
| 252 | 2760 | [2760. 最长奇偶子数组](https://leetcode.cn/problems/longest-even-odd-subarray-with-threshold/) | 1420 | external-index |
| 253 | 1887 | [1887. 使数组元素相等的减少操作次数](https://leetcode.cn/problems/reduction-operations-to-make-the-array-elements-equal/) | 1428 | external-index |
| 254 | 845 | [845. 数组中的最长山脉](https://leetcode.cn/problems/longest-mountain-in-array/) | 1437 | external-index |
| 255 | 2038 | [2038. 如果相邻两个颜色均相同则删除当前颜色](https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/) | 1468 | external-index |
| 256 | 2900 | [2900. 最长相邻不相等子序列 I](https://leetcode.cn/problems/longest-unequal-adjacent-groups-subsequence-i/) | 1469 | external-index |
| 257 | 1759 | [1759. 统计同质子字符串的数目](https://leetcode.cn/problems/count-number-of-homogenous-substrings/) | 1491 | external-index |
| 258 | 3011 | [3011. 判断一个数组是否可以变为有序](https://leetcode.cn/problems/find-if-array-can-be-sorted/) | 1497 | external-index |
| 259 | 1861 | [1861. 旋转盒子](https://leetcode.cn/problems/rotating-the-box/) | 1537 | external-index |
| 260 | 1578 | [1578. 使绳子变成彩色的最短时间](https://leetcode.cn/problems/minimum-time-to-make-rope-colorful/) | 1574 | external-index |
| 261 | 1839 | [1839. 所有元音按顺序排布的最长子字符串](https://leetcode.cn/problems/longest-substring-of-all-vowels-in-order/) | 1580 | external-index |
| 262 | 2765 | [2765. 最长交替子数组](https://leetcode.cn/problems/longest-alternating-subarray/) | 1581 | external-index |
| 263 | 3255 | [3255. 长度为 K 的子数组的能量值 II](https://leetcode.cn/problems/find-the-power-of-k-size-subarrays-ii/) | 1595 | external-index |
| 264 | 3350 | [3350. 检测相邻递增子数组 II](https://leetcode.cn/problems/adjacent-increasing-subarrays-detection-ii/) | 1600 | external-index |
| 265 | 3105 | [3105. 最长的严格递增或递减子数组](https://leetcode.cn/problems/longest-strictly-increasing-or-strictly-decreasing-subarray/) | Unknown | external-index |
| 266 | 3926 | [3926. 有效单词计数](https://leetcode.cn/problems/count-valid-word-occurrences/) | 1608 | external-index |
| 267 | 838 | [838. 推多米诺](https://leetcode.cn/problems/push-dominoes/) | 1638 | external-index |
| 268 | 467 | [467. 环绕字符串中唯一的子字符串](https://leetcode.cn/problems/unique-substrings-in-wraparound-string/) | 1700 | external-index |
| 269 | 3499 | [3499. 操作后最大活跃区段数 I](https://leetcode.cn/problems/maximize-active-section-with-trade-i/) | 1729 | external-index |
| 270 | 3952 | [3952. 下标覆盖处的最大总和](https://leetcode.cn/problems/maximum-total-value-of-covered-indices/) | 1763 | external-index |
| 271 | 413 | [413. 等差数列划分](https://leetcode.cn/problems/arithmetic-slices/) | Unknown | external-index |
| 272 | 3738 | [3738. 替换至多一个元素后最长非递减子数组](https://leetcode.cn/problems/longest-non-decreasing-subarray-after-replacing-at-most-one-element/) | 1811 | external-index |
| 273 | 2147 | [2147. 分隔长廊的方案数](https://leetcode.cn/problems/number-of-ways-to-divide-a-long-corridor/) | 1915 | external-index |
| 274 | 2593 | [2593. 标记所有元素后数组的分数](https://leetcode.cn/problems/find-score-of-an-array-after-marking-all-elements/) | Unknown | external-index |
| 275 | 68 | [68. 文本左右对齐](https://leetcode.cn/problems/text-justification/) | Unknown | external-index |
| 276 | 135 | [135. 分发糖果](https://leetcode.cn/problems/candy/) | Unknown | external-index |
| 277 | 3872 | [3872. 替换最多一个元素后的最长等差子数组](https://leetcode.cn/problems/longest-arithmetic-sequence-after-changing-at-most-one-element/) | 2042 | external-index |
| 278 | 2948 | [2948. 交换得到字典序最小的数组](https://leetcode.cn/problems/make-lexicographically-smallest-array-by-swapping-elements/) | 2047 | external-index |
| 279 | 3948 | [3948. 字典序最大的 MEX 数组](https://leetcode.cn/problems/lexicographically-maximum-mex-array/) | 2122 | external-index |
| 280 | 3830 | [3830. 移除至多一个元素后的最长交替子数组](https://leetcode.cn/problems/longest-alternating-subarray-after-removing-at-most-one-element/) | 2162 | external-index |
| 281 | 3640 | [3640. 三段式数组 II](https://leetcode.cn/problems/trionic-array-ii/) | 2278 | external-index |
| 282 | 2393 | [2393. 严格递增的子数组个数](https://leetcode.cn/problems/count-strictly-increasing-subarrays/) | Unknown | external-index |
| 283 | 3773 | [3773. 最大等长连续字符组](https://leetcode.cn/problems/maximum-number-of-equal-length-runs/) | Unknown | external-index |
| 284 | 2436 | [2436. 使子数组最大公约数大于一的最小分割数](https://leetcode.cn/problems/minimum-split-into-subarrays-with-gcd-greater-than-one/) | Unknown | external-index |
| 285 | 2495 | [2495. 乘积为偶数的子数组数](https://leetcode.cn/problems/number-of-subarrays-having-even-product/) | Unknown | external-index |
| 286 | 3063 | [3063. 链表频率](https://leetcode.cn/problems/linked-list-frequency/) | Unknown | external-index |
