# 位运算

<!-- generated: do not edit by hand; update scripts/generate_leetcode_bank.py instead -->

来源：[分享丨【算法题单】位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
生成时间：2026-09-16 10:17:56 +0800

本页按来源题单中的标题层级组织，只保存题目元数据和链接，不复制题面或题解。

## 一、基础题

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 563 | 3370 | [3370. 仅含置位位的最小整数](https://leetcode.cn/problems/smallest-number-with-all-set-bits/) | 1199 | external-index |
| 564 | 3827 | [3827. 统计单比特整数](https://leetcode.cn/problems/count-monobit-integers/) | Unknown | external-index |
| 565 | 3226 | [3226. 使两个整数相等的位更改次数](https://leetcode.cn/problems/number-of-bit-changes-to-make-two-integers-equal/) | 1247 | external-index |
| 566 | 1356 | [1356. 根据数字二进制下 1 的数目排序](https://leetcode.cn/problems/sort-integers-by-the-number-of-1-bits/) | 1258 | external-index |
| 567 | 461 | [461. 汉明距离](https://leetcode.cn/problems/hamming-distance/) | 1282 | external-index |
| 568 | 2220 | [2220. 转换数字的最少位翻转次数](https://leetcode.cn/problems/minimum-bit-flips-to-convert-number/) | 1282 | external-index |
| 569 | 1342 | [1342. 将数字变成 0 的操作次数](https://leetcode.cn/problems/number-of-steps-to-reduce-a-number-to-zero/) | Unknown | external-index |
| 570 | 476 | [476. 数字的补数](https://leetcode.cn/problems/number-complement/) | Unknown | external-index |
| 571 | 1009 | [1009. 十进制整数的反码](https://leetcode.cn/problems/complement-of-base-10-integer/) | 476 | external-index |
| 572 | 868 | [868. 二进制间距](https://leetcode.cn/problems/binary-gap/) | 1307 | external-index |
| 573 | 2917 | [2917. 找出数组中的 K-or 值](https://leetcode.cn/problems/find-the-k-or-of-an-array/) | 1389 | external-index |
| 574 | 2657 | [2657. 找到两个数组的前缀公共数组](https://leetcode.cn/problems/find-the-prefix-common-array-of-two-arrays/) | Unknown | external-index |
| 575 | 231 | [231. 2 的幂](https://leetcode.cn/problems/power-of-two/) | Unknown | external-index |
| 576 | 342 | [342. 4 的幂](https://leetcode.cn/problems/power-of-four/) | Unknown | external-index |
| 577 | 693 | [693. 交替位二进制数](https://leetcode.cn/problems/binary-number-with-alternating-bits/) | Unknown | external-index |
| 578 | 3950 | [3950. 恰好一对连续置位](https://leetcode.cn/problems/exactly-one-consecutive-set-bits-pair/) | Unknown | external-index |
| 579 | 191 | [191. 位 1 的个数](https://leetcode.cn/problems/number-of-1-bits/) | Unknown | external-index |
| 580 | 338 | [338. 比特位计数](https://leetcode.cn/problems/counting-bits/) | Unknown | external-index |
| 581 | 2595 | [2595. 奇偶位数](https://leetcode.cn/problems/number-of-even-and-odd-bits/) | Unknown | external-index |
| 582 | 2154 | [2154. 将找到的值乘以 2](https://leetcode.cn/problems/keep-multiplying-found-values-by-two/) | Unknown | external-index |
| 583 | 3211 | [3211. 生成不含相邻零的二进制字符串](https://leetcode.cn/problems/generate-binary-strings-without-adjacent-zeros/) | Unknown | external-index |
| 584 | 面试题 05.01 | [面试题 05.01. 插入](https://leetcode.cn/problems/insert-into-bits-lcci/) | Unknown | external-index |
| 585 | 3690 | [3690. 拆分合并数组](https://leetcode.cn/problems/split-and-merge-array-transformation/) | Unknown | external-index |
| 586 | 190 | [190. 颠倒二进制位](https://leetcode.cn/problems/reverse-bits/) | Unknown | external-index |
| 587 | 4030 | [4030. 判断 ASCII 值回文](https://leetcode.cn/problems/check-ascii-palindromic/) | 1165 | external-index |
| 588 | 3750 | [3750. 最少反转次数得到翻转二进制字符串](https://leetcode.cn/problems/minimum-number-of-flips-to-reverse-binary-string/) | 1289 | external-index |
| 589 | 3769 | [3769. 二进制反射排序](https://leetcode.cn/problems/sort-integers-by-binary-reflection/) | 1364 | external-index |

## 二、异或（XOR）的性质

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 590 | 1486 | [1486. 数组异或操作](https://leetcode.cn/problems/xor-operation-in-an-array/) | 1181 | external-index |
| 591 | 1720 | [1720. 解码异或后的数组](https://leetcode.cn/problems/decode-xored-array/) | 1284 | external-index |
| 592 | 2433 | [2433. 找出前缀异或的原始数组](https://leetcode.cn/problems/find-the-original-array-of-prefix-xor/) | 1367 | external-index |
| 593 | 1310 | [1310. 子数组异或查询](https://leetcode.cn/problems/xor-queries-of-a-subarray/) | 1460 | external-index |
| 594 | 3702 | [3702. 按位异或非零的最长子序列](https://leetcode.cn/problems/longest-subsequence-with-non-zero-bitwise-xor/) | 1489 | external-index |
| 595 | 2683 | [2683. 相邻值的按位异或](https://leetcode.cn/problems/neighboring-bitwise-xor/) | 1518 | external-index |
| 596 | 1829 | [1829. 每个查询的最大异或值](https://leetcode.cn/problems/maximum-xor-for-each-query/) | 1523 | external-index |
| 597 | 2997 | [2997. 使数组异或和等于 K 的最少操作次数](https://leetcode.cn/problems/minimum-number-of-operations-to-make-array-xor-equal-to-k/) | 1525 | external-index |
| 598 | 1442 | [1442. 形成两个异或相等数组的三元组数目](https://leetcode.cn/problems/count-triplets-that-can-form-two-arrays-of-equal-xor/) | 1525 | external-index |
| 599 | 2429 | [2429. 最小异或](https://leetcode.cn/problems/minimize-xor/) | 1532 | external-index |
| 600 | 2527 | [2527. 查询数组异或美丽值](https://leetcode.cn/problems/find-xor-beauty-of-array/) | 1550 | external-index |
| 601 | 2317 | [2317. 操作后的最大异或和](https://leetcode.cn/problems/maximum-xor-after-operations/) | 1679 | external-index |
| 602 | 2588 | [2588. 统计美丽子数组数目](https://leetcode.cn/problems/count-the-number-of-beautiful-subarrays/) | 1697 | external-index |
| 603 | 2564 | [2564. 子字符串异或查询](https://leetcode.cn/problems/substring-xor-queries/) | 1959 | external-index |
| 604 | 1734 | [1734. 解码异或后的排列](https://leetcode.cn/problems/decode-xored-permutation/) | 2024 | external-index |
| 605 | 2857 | [2857. 统计距离为 k 的点对](https://leetcode.cn/problems/count-pairs-of-points-with-distance-k/) | 2082 | external-index |
| 606 | 1803 | [1803. 统计异或值在范围内的数对有多少](https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/) | 2479 | external-index |
| 607 | 3215 | [3215. 用偶数异或设置位计数三元组 II](https://leetcode.cn/problems/count-triplets-with-even-xor-set-bits-ii/) | Unknown | external-index |

## 三、与或（AND/OR）的性质

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 608 | 2980 | [2980. 检查按位或是否存在尾随零](https://leetcode.cn/problems/check-if-bitwise-or-has-trailing-zeros/) | 1234 | external-index |
| 609 | 1318 | [1318. 或运算的最小翻转次数](https://leetcode.cn/problems/minimum-flips-to-make-a-or-b-equal-to-c/) | 1383 | external-index |
| 610 | 2419 | [2419. 按位与最大的最长子数组](https://leetcode.cn/problems/longest-subarray-with-maximum-bitwise-and/) | 1496 | external-index |
| 611 | 2871 | [2871. 将数组分割成最多数目的子数组](https://leetcode.cn/problems/split-array-into-maximum-number-of-subarrays/) | 1750 | external-index |
| 612 | 2401 | [2401. 最长优雅子数组](https://leetcode.cn/problems/longest-nice-subarray/) | 1750 | external-index |
| 613 | 2680 | [2680. 最大或值](https://leetcode.cn/problems/maximum-or/) | 1912 | external-index |
| 614 | 3133 | [3133. 数组最后一个元素的最小值](https://leetcode.cn/problems/minimum-array-end/) | 1935 | external-index |
| 615 | 3108 | [3108. 带权图里旅途的最小代价](https://leetcode.cn/problems/minimum-cost-walk-in-weighted-graph/) | 2109 | external-index |
| 616 | 3117 | [3117. 划分数组得到最小的值之和](https://leetcode.cn/problems/minimum-sum-of-values-by-dividing-array/) | 2735 | external-index |
| 617 | 3125 | [3125. 使得按位与结果为 0 的最大数字](https://leetcode.cn/problems/maximum-number-that-makes-result-of-bitwise-and-zero/) | Unknown | external-index |

## 三、与或（AND/OR）的性质 / AND/OR LogTrick

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 618 | 26 | [26. 删除有序数组中的重复项](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/) | Unknown | external-index |
| 619 | 3171 | [3171. 找到按位或最接近 K 的子数组](https://leetcode.cn/problems/find-subarray-with-bitwise-or-closest-to-k/) | Unknown | external-index |
| 620 | 1521 | [1521. 找到最接近目标值的函数值](https://leetcode.cn/problems/find-a-value-of-a-mysterious-function-closest-to-target/) | 3171 | external-index |
| 621 | 3097 | [3097. 或值至少为 K 的最短子数组 II](https://leetcode.cn/problems/shortest-subarray-with-or-at-least-k-ii/) | 1891 | external-index |
| 622 | 2411 | [2411. 按位或最大的最小子数组长度](https://leetcode.cn/problems/smallest-subarrays-with-maximum-bitwise-or/) | 1938 | external-index |
| 623 | 3209 | [3209. 子数组按位与值为 K 的数目](https://leetcode.cn/problems/number-of-subarrays-with-and-value-of-k/) | 2050 | external-index |
| 624 | 898 | [898. 子数组按位或操作](https://leetcode.cn/problems/bitwise-ors-of-subarrays/) | 2133 | external-index |
| 625 | 3878 | [3878. 统计好子数组](https://leetcode.cn/problems/count-good-subarrays/) | 2230 | external-index |

## 三、与或（AND/OR）的性质 / GCD LogTrick

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 626 | 2447 | [2447. 最大公因数等于 K 的子数组数目](https://leetcode.cn/problems/number-of-subarrays-with-gcd-equal-to-k/) | Unknown | external-index |
| 627 | 2654 | [2654. 使数组所有元素变成 1 的最少操作次数](https://leetcode.cn/problems/minimum-number-of-operations-to-make-all-array-elements-equal-to-1/) | Unknown | external-index |
| 628 | 3605 | [3605. 数组的最小稳定性因子](https://leetcode.cn/problems/minimum-stability-factor-of-array/) | 2410 | external-index |
| 629 | 3574 | [3574. 最大子数组 GCD 分数](https://leetcode.cn/problems/maximize-subarray-gcd-score/) | Unknown | external-index |
| 630 | 2941 | [2941. 子数组的最大 GCD-Sum](https://leetcode.cn/problems/maximum-gcd-sum-of-a-subarray/) | Unknown | external-index |

## 四、拆位 / 贡献法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 631 | 477 | [477. 汉明距离总和](https://leetcode.cn/problems/total-hamming-distance/) | Unknown | external-index |
| 632 | 1863 | [1863. 找出所有子集的异或总和再求和](https://leetcode.cn/problems/sum-of-all-subset-xor-totals/) | Unknown | external-index |
| 633 | 2425 | [2425. 所有数对的异或和](https://leetcode.cn/problems/bitwise-xor-of-all-pairings/) | 1622 | external-index |
| 634 | 2275 | [2275. 按位与结果大于零的最长组合](https://leetcode.cn/problems/largest-combination-with-bitwise-and-greater-than-zero/) | 1642 | external-index |
| 635 | 1835 | [1835. 所有数对按位与结果的异或和](https://leetcode.cn/problems/find-xor-sum-of-all-pairs-bitwise-and/) | 1825 | external-index |
| 636 | 3688 | [3688. 偶数的按位或运算](https://leetcode.cn/problems/bitwise-or-of-even-numbers-in-an-array/) | Unknown | external-index |
| 637 | 2505 | [2505. 所有子序列和的按位或](https://leetcode.cn/problems/bitwise-or-of-all-subsequence-sums/) | Unknown | external-index |
| 638 | 3153 | [3153. 所有数对中数位不同之和](https://leetcode.cn/problems/sum-of-digit-differences-of-all-pairs/) | 1645 | external-index |

## 五、试填法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 639 | 421 | [421. 数组中两个数的最大异或值](https://leetcode.cn/problems/maximum-xor-of-two-numbers-in-an-array/) | Unknown | external-index |
| 640 | 3858 | [3858. 按位或的最小值](https://leetcode.cn/problems/minimum-bitwise-or-from-grid/) | 1947 | external-index |
| 641 | 3806 | [3806. 增加操作后最大按位与的结果](https://leetcode.cn/problems/maximum-bitwise-and-after-increment-operations/) | 2259 | external-index |
| 642 | 2935 | [2935. 找出强数对的最大异或值 II](https://leetcode.cn/problems/maximum-strong-pair-xor-ii/) | 2349 | external-index |
| 643 | 3845 | [3845. 最大子数组异或值](https://leetcode.cn/problems/maximum-subarray-xor-with-bounded-range/) | Unknown | external-index |
| 644 | 3007 | [3007. 价值和小于等于 K 的最大数字](https://leetcode.cn/problems/maximum-number-that-sum-of-the-prices-is-less-than-or-equal-to-k/) | Unknown | external-index |
| 645 | 3145 | [3145. 大数组元素的乘积](https://leetcode.cn/problems/find-products-of-elements-of-big-array/) | 2859 | external-index |
| 646 | 3022 | [3022. 给定操作次数内使剩余元素的或值最小](https://leetcode.cn/problems/minimize-or-of-remaining-elements-using-operations/) | 2918 | external-index |
| 647 | 3287 | [3287. 求出数组中最大序列值](https://leetcode.cn/problems/find-the-maximum-sequence-value-of-array/) | Unknown | external-index |
| 648 | 3344 | [3344. 最大尺寸数组](https://leetcode.cn/problems/maximum-sized-array/) | Unknown | external-index |

## 六、恒等式

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 649 | 1835 | [1835. 所有数对按位与结果的异或和](https://leetcode.cn/problems/find-xor-sum-of-all-pairs-bitwise-and/) | 1825 | external-index |
| 650 | 2354 | [2354. 优质数对的数目](https://leetcode.cn/problems/number-of-excellent-pairs/) | 2076 | external-index |

## 七、线性基

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 651 | 3681 | [3681. 子序列最大 XOR 值](https://leetcode.cn/problems/maximum-xor-of-subsequences/) | Unknown | external-index |
| 652 | 3630 | [3630. 划分数组得到最大异或运算和与运算之和](https://leetcode.cn/problems/partition-array-for-maximum-xor-and-and/) | 2744 | external-index |

## 八、思维题

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 653 | 2546 | [2546. 执行逐位运算使字符串相等](https://leetcode.cn/problems/apply-bitwise-operations-to-make-strings-equal/) | 1605 | external-index |
| 654 | 1558 | [1558. 得到目标数组的最少函数调用次数](https://leetcode.cn/problems/minimum-numbers-of-function-calls-to-make-target-array/) | 1637 | external-index |
| 655 | 2571 | [2571. 将整数减少到零需要的最少操作数](https://leetcode.cn/problems/minimum-operations-to-reduce-an-integer-to-0/) | 1649 | external-index |
| 656 | 3315 | [3315. 构造最小位运算数组 II](https://leetcode.cn/problems/construct-the-minimum-bitwise-array-ii/) | 1715 | external-index |
| 657 | 2568 | [2568. 最小无法得到的或值](https://leetcode.cn/problems/minimum-impossible-or/) | 1754 | external-index |
| 658 | 3644 | [3644. 排序排列](https://leetcode.cn/problems/maximum-k-to-sort-a-permutation/) | 1775 | external-index |
| 659 | 2509 | [2509. 查询树中环的长度](https://leetcode.cn/problems/cycle-length-queries-in-a-tree/) | 1948 | external-index |
| 660 | 2939 | [2939. 最大异或乘积](https://leetcode.cn/problems/maximum-xor-product/) | 2128 | external-index |
| 661 | 2749 | [2749. 得到整数零需要执行的最少操作数](https://leetcode.cn/problems/minimum-operations-to-make-the-integer-zero/) | 2132 | external-index |
| 662 | 2835 | [2835. 使子序列的和等于目标的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-form-subsequence-with-target-sum/) | 2207 | external-index |
| 663 | 2897 | [2897. 对数组执行操作使平方和最大](https://leetcode.cn/problems/apply-operations-on-array-to-maximize-sum-of-squares/) | 2301 | external-index |
| 664 | 810 | [810. 黑板异或游戏](https://leetcode.cn/problems/chalkboard-xor-game/) | 2341 | external-index |
| 665 | 3064 | [3064. 使用按位查询猜测数字 I](https://leetcode.cn/problems/guess-the-number-using-bitwise-questions-i/) | Unknown | external-index |
| 666 | 3094 | [3094. 使用按位查询猜测数字 II](https://leetcode.cn/problems/guess-the-number-using-bitwise-questions-ii/) | Unknown | external-index |

## 九、其他

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 667 | 136 | [136. 只出现一次的数字](https://leetcode.cn/problems/single-number/) | Unknown | external-index |
| 668 | 260 | [260. 只出现一次的数字 III](https://leetcode.cn/problems/single-number-iii/) | Unknown | external-index |
| 669 | 2965 | [2965. 找出缺失和重复的数字](https://leetcode.cn/problems/find-missing-and-repeated-values/) | Unknown | external-index |
| 670 | 137 | [137. 只出现一次的数字 II](https://leetcode.cn/problems/single-number-ii/) | Unknown | external-index |
| 671 | 645 | [645. 错误的集合](https://leetcode.cn/problems/set-mismatch/) | Unknown | external-index |
| 672 | 371 | [371. 两整数之和](https://leetcode.cn/problems/sum-of-two-integers/) | Unknown | external-index |
| 673 | 201 | [201. 数字范围按位与](https://leetcode.cn/problems/bitwise-and-of-numbers-range/) | Unknown | external-index |
| 674 | 2438 | [2438. 二的幂数组中查询范围内的乘积](https://leetcode.cn/problems/range-product-queries-of-powers/) | 1610 | external-index |
| 675 | 1680 | [1680. 连接连续二进制数字](https://leetcode.cn/problems/concatenation-of-consecutive-binary-numbers/) | 1630 | external-index |
| 676 | 1415 | [1415. 长度为 n 的开心字符串中字典序第 k 小的字符串](https://leetcode.cn/problems/the-k-th-lexicographical-string-of-all-happy-strings-of-length-n/) | Unknown | external-index |
| 677 | 1261 | [1261. 在受污染的二叉树中查找元素](https://leetcode.cn/problems/find-elements-in-a-contaminated-binary-tree/) | Unknown | external-index |
| 678 | 1545 | [1545. 找出第 N 个二进制字符串中的第 K 位](https://leetcode.cn/problems/find-kth-bit-in-nth-binary-string/) | Unknown | external-index |
| 679 | 89 | [89. 格雷编码](https://leetcode.cn/problems/gray-code/) | Unknown | external-index |
| 680 | 1238 | [1238. 循环码排列](https://leetcode.cn/problems/circular-permutation-in-binary-representation/) | 1775 | external-index |
| 681 | 2126 | [2126. 摧毁小行星](https://leetcode.cn/problems/destroying-asteroids/) | Unknown | external-index |
| 682 | 3766 | [3766. 将数字变成二进制回文数的最少操作](https://leetcode.cn/problems/minimum-operations-to-make-binary-palindrome/) | Unknown | external-index |
| 683 | 3782 | [3782. 交替删除操作后最后剩下的整数](https://leetcode.cn/problems/last-remaining-integer-after-alternating-deletion-operations/) | 2074 | external-index |
| 684 | 390 | [390. 消除游戏](https://leetcode.cn/problems/elimination-game/) | 3782 | external-index |
| 685 | 982 | [982. 按位与为零的三元组](https://leetcode.cn/problems/triples-with-bitwise-and-equal-to-zero/) | 2085 | external-index |
| 686 | 3307 | [3307. 找出第 K 个字符 II](https://leetcode.cn/problems/find-the-k-th-character-in-string-game-ii/) | 2232 | external-index |
| 687 | 1611 | [1611. 使整数变为 0 的最少操作次数](https://leetcode.cn/problems/minimum-one-bit-operations-to-make-integers-zero/) | 2345 | external-index |
| 688 | 3514 | [3514. 不同 XOR 三元组的数目 II](https://leetcode.cn/problems/number-of-unique-xor-triplets-ii/) | Unknown | external-index |
| 689 | LCP 81 | [LCP 81. 与非的谜题](https://leetcode.cn/problems/ryfUiz/) | Unknown | external-index |
| 690 | 751 | [751. IP 到 CIDR](https://leetcode.cn/problems/ip-to-cidr/) | Unknown | external-index |
| 691 | 3595 | [3595. 一次或两次](https://leetcode.cn/problems/once-twice/) | 137 | external-index |
| 692 | 3141 | [3141. 最大汉明距离](https://leetcode.cn/problems/maximum-hamming-distances/) | Unknown | external-index |
