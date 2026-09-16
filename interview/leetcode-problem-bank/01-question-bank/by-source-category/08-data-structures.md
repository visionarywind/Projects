# 常用数据结构

<!-- generated: do not edit by hand; update scripts/generate_leetcode_bank.py instead -->

来源：[分享丨【算法题单】常用数据结构（前缀和/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
生成时间：2026-09-16 10:17:56 +0800

本页按来源题单中的标题层级组织，只保存题目元数据和链接，不复制题面或题解。

## 零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1612 | 1 | [1. 两数之和](https://leetcode.cn/problems/two-sum/) | Unknown | external-index |
| 1613 | 1512 | [1512. 好数对的数目](https://leetcode.cn/problems/number-of-good-pairs/) | 1161 | external-index |
| 1614 | 2441 | [2441. 与对应负数同时存在的最大正整数](https://leetcode.cn/problems/largest-positive-integer-that-exists-with-its-negative/) | 1168 | external-index |
| 1615 | 121 | [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/) | Unknown | external-index |
| 1616 | 2016 | [2016. 增量元素之间的最大差值](https://leetcode.cn/problems/maximum-difference-between-increasing-elements/) | 1246 | external-index |
| 1617 | 624 | [624. 数组列表中的最大距离](https://leetcode.cn/problems/maximum-distance-in-arrays/) | Unknown | external-index |
| 1618 | 3880 | [3880. 两个值之间的最小绝对差值](https://leetcode.cn/problems/minimum-absolute-difference-between-two-values/) | 1257 | external-index |
| 1619 | 2342 | [2342. 数位和相等数对的最大和](https://leetcode.cn/problems/max-sum-of-a-pair-with-equal-sum-of-digits/) | 1309 | external-index |
| 1620 | 3979 | [3979. 最大有效数对和](https://leetcode.cn/problems/maximum-valid-pair-sum/) | 1328 | external-index |
| 1621 | 1128 | [1128. 等价多米诺骨牌对的数量](https://leetcode.cn/problems/number-of-equivalent-domino-pairs/) | 1333 | external-index |
| 1622 | 1679 | [1679. K 和数对的最大数目](https://leetcode.cn/problems/max-number-of-k-sum-pairs/) | 1346 | external-index |
| 1623 | 面试题 16.24 | [面试题 16.24. 数对和](https://leetcode.cn/problems/pairs-with-sum-lcci/) | Unknown | external-index |
| 1624 | 219 | [219. 存在重复元素 II](https://leetcode.cn/problems/contains-duplicate-ii/) | Unknown | external-index |
| 1625 | 2260 | [2260. 必须拿起的最小连续卡牌数](https://leetcode.cn/problems/minimum-consecutive-cards-to-pick-up/) | 1365 | external-index |
| 1626 | 2001 | [2001. 可互换矩形的组数](https://leetcode.cn/problems/number-of-pairs-of-interchangeable-rectangles/) | 1436 | external-index |
| 1627 | 2815 | [2815. 数组中的最大数对和](https://leetcode.cn/problems/max-pair-sum-in-an-array/) | Unknown | external-index |
| 1628 | 3623 | [3623. 统计梯形的数目 I](https://leetcode.cn/problems/count-number-of-trapezoids-i/) | 1580 | external-index |
| 1629 | 2364 | [2364. 统计坏数对的数目](https://leetcode.cn/problems/count-number-of-bad-pairs/) | 1622 | external-index |
| 1630 | 3805 | [3805. 统计凯撒加密对数目](https://leetcode.cn/problems/count-caesar-cipher-pairs/) | 1624 | external-index |
| 1631 | 3371 | [3371. 识别数组中的最大异常值](https://leetcode.cn/problems/identify-the-largest-outlier-in-an-array/) | 1644 | external-index |
| 1632 | 3761 | [3761. 镜像对之间最小绝对距离](https://leetcode.cn/problems/minimum-absolute-distance-between-mirror-pairs/) | 1669 | external-index |
| 1633 | 1014 | [1014. 最佳观光组合](https://leetcode.cn/problems/best-sightseeing-pair/) | 1730 | external-index |
| 1634 | 1814 | [1814. 统计一个数组中好对子的数目](https://leetcode.cn/problems/count-nice-pairs-in-an-array/) | 1738 | external-index |
| 1635 | 3584 | [3584. 子序列首尾元素的最大乘积](https://leetcode.cn/problems/maximum-product-of-first-and-last-elements-of-a-subsequence/) | 1763 | external-index |
| 1636 | 2905 | [2905. 找出满足差值条件的下标 II](https://leetcode.cn/problems/find-indices-with-index-and-value-difference-ii/) | 1764 | external-index |
| 1637 | 3837 | [3837. 相等元素的延迟计数](https://leetcode.cn/problems/delayed-count-of-equal-elements/) | Unknown | external-index |
| 1638 | 3907 | [3907. 统计具有相反奇偶性的较小元素](https://leetcode.cn/problems/count-smaller-elements-with-opposite-parity/) | Unknown | external-index |

## 零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1639 | 1010 | [1010. 总持续时间可被 60 整除的歌曲](https://leetcode.cn/problems/pairs-of-songs-with-total-durations-divisible-by-60/) | Unknown | external-index |
| 1640 | 3185 | [3185. 构成整天的下标对数目 II](https://leetcode.cn/problems/count-pairs-that-form-a-complete-day-ii/) | 1010 | external-index |
| 1641 | 2748 | [2748. 美丽下标对的数目](https://leetcode.cn/problems/number-of-beautiful-pairs/) | Unknown | external-index |
| 1642 | 2506 | [2506. 统计相似字符串对的数目](https://leetcode.cn/problems/count-pairs-of-similar-strings/) | Unknown | external-index |
| 1643 | 2874 | [2874. 有序三元组中的最大值 II](https://leetcode.cn/problems/maximum-value-of-an-ordered-triplet-ii/) | 1583 | external-index |
| 1644 | 1497 | [1497. 检查数组对是否可以被 k 整除](https://leetcode.cn/problems/check-if-array-pairs-are-divisible-by-k/) | 1787 | external-index |
| 1645 | 1031 | [1031. 两个无重叠子数组的最大和](https://leetcode.cn/problems/maximum-sum-of-two-non-overlapping-subarrays/) | 2000 | external-index |
| 1646 | 2555 | [2555. 两个线段获得的最多奖品](https://leetcode.cn/problems/maximize-win-from-two-segments/) | 2081 | external-index |
| 1647 | 1995 | [1995. 统计特殊四元组](https://leetcode.cn/problems/count-special-quadruplets/) | Unknown | external-index |
| 1648 | 3404 | [3404. 统计特殊子序列的数目](https://leetcode.cn/problems/count-special-subsequences/) | 2445 | external-index |
| 1649 | 3267 | [3267. 统计近似相等数对 II](https://leetcode.cn/problems/count-almost-equal-pairs-ii/) | 2545 | external-index |
| 1650 | 3480 | [3480. 删除一个冲突对后最大子数组数目](https://leetcode.cn/problems/maximize-subarrays-after-removing-one-conflicting-pair/) | 2764 | external-index |
| 1651 | 1214 | [1214. 查找两棵二叉搜索树之和](https://leetcode.cn/problems/two-sum-bsts/) | Unknown | external-index |
| 1652 | 2964 | [2964. 可被整除的三元组数量](https://leetcode.cn/problems/number-of-divisible-triplet-sums/) | Unknown | external-index |
| 1653 | 3917 | [3917. 统计下标的相反奇偶性得分](https://leetcode.cn/problems/count-indices-with-opposite-parity/) | Unknown | external-index |
| 1654 | 2078 | [2078. 两栋颜色不同且距离最远的房子](https://leetcode.cn/problems/two-furthest-houses-with-different-colors/) | Unknown | external-index |
| 1655 | 454 | [454. 四数相加 II](https://leetcode.cn/problems/4sum-ii/) | Unknown | external-index |
| 1656 | 220 | [220. 存在重复元素 III](https://leetcode.cn/problems/contains-duplicate-iii/) | Unknown | external-index |
| 1657 | 3027 | [3027. 人员站位的方案数 II](https://leetcode.cn/problems/find-the-number-of-ways-to-place-people-ii/) | 2020 | external-index |
| 1658 | 3548 | [3548. 等和矩阵分割 II](https://leetcode.cn/problems/equal-sum-grid-partition-ii/) | 2245 | external-index |
| 1659 | 3713 | [3713. 最长的平衡子串 I](https://leetcode.cn/problems/longest-balanced-substring-i/) | Unknown | external-index |

## 零、常用枚举技巧 / §0.2 枚举中间 / §0.2.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1660 | 3583 | [3583. 统计特殊三元组](https://leetcode.cn/problems/count-special-triplets/) | 1510 | external-index |
| 1661 | 1930 | [1930. 长度为 3 的不同回文子序列](https://leetcode.cn/problems/unique-length-3-palindromic-subsequences/) | 1533 | external-index |
| 1662 | 3128 | [3128. 直角三角形](https://leetcode.cn/problems/right-triangles/) | 1541 | external-index |

## 零、常用枚举技巧 / §0.2 枚举中间 / §0.2.2 进阶

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1663 | 2909 | [2909. 元素和最小的山形三元组 II](https://leetcode.cn/problems/minimum-sum-of-mountain-triplets-ii/) | 1479 | external-index |
| 1664 | 2874 | [2874. 有序三元组中的最大值 II](https://leetcode.cn/problems/maximum-value-of-an-ordered-triplet-ii/) | 1583 | external-index |
| 1665 | 456 | [456. 132 模式](https://leetcode.cn/problems/132-pattern/) | Unknown | external-index |
| 1666 | 3067 | [3067. 在带权树网络中统计可连接服务器对数目](https://leetcode.cn/problems/count-pairs-of-connectable-servers-in-a-weighted-tree-network/) | 1909 | external-index |
| 1667 | 1534 | [1534. 统计好三元组](https://leetcode.cn/problems/count-good-triplets/) | Unknown | external-index |
| 1668 | 3455 | [3455. 最短匹配子字符串](https://leetcode.cn/problems/shortest-matching-substring/) | 2303 | external-index |
| 1669 | 2242 | [2242. 节点序列的最大得分](https://leetcode.cn/problems/maximum-score-of-a-node-sequence/) | 2304 | external-index |
| 1670 | 2867 | [2867. 统计树中的合法路径数目](https://leetcode.cn/problems/count-valid-paths-in-a-tree/) | 2428 | external-index |
| 1671 | 2552 | [2552. 统计上升四元组](https://leetcode.cn/problems/count-increasing-quadruplets/) | 2433 | external-index |
| 1672 | 3257 | [3257. 放三个车的价值之和最大 II](https://leetcode.cn/problems/maximum-value-sum-by-placing-three-rooks-ii/) | 2553 | external-index |
| 1673 | 3073 | [3073. 最大递增三元组](https://leetcode.cn/problems/maximum-increasing-triplet-value/) | Unknown | external-index |

## 零、常用枚举技巧 / §0.3 遍历对角线

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1674 | 3446 | [3446. 按对角线进行矩阵排序](https://leetcode.cn/problems/sort-matrix-by-diagonals/) | 1373 | external-index |
| 1675 | 2711 | [2711. 对角线上不同值的数量差](https://leetcode.cn/problems/difference-of-number-of-distinct-values-on-diagonals/) | 1429 | external-index |
| 1676 | 1329 | [1329. 将矩阵按对角线排序](https://leetcode.cn/problems/sort-the-matrix-diagonally/) | 1548 | external-index |
| 1677 | 498 | [498. 对角线遍历](https://leetcode.cn/problems/diagonal-traverse/) | Unknown | external-index |
| 1678 | 面试题 17.23 | [面试题 17.23. 最大黑方阵](https://leetcode.cn/problems/max-black-square-lcci/) | 2800 | external-index |
| 1679 | 562 | [562. 矩阵中最长的连续1线段](https://leetcode.cn/problems/longest-line-of-consecutive-one-in-matrix/) | Unknown | external-index |
| 1680 | 1424 | [1424. 对角线遍历 II](https://leetcode.cn/problems/diagonal-traverse-ii/) | 1780 | external-index |

## 一、前缀和 / §1.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1681 | 303 | [303. 区域和检索 - 数组不可变](https://leetcode.cn/problems/range-sum-query-immutable/) | Unknown | external-index |
| 1682 | 3427 | [3427. 变长子数组求和](https://leetcode.cn/problems/sum-of-variable-length-subarrays/) | Unknown | external-index |
| 1683 | 2559 | [2559. 统计范围内的元音字符串数](https://leetcode.cn/problems/count-vowel-strings-in-ranges/) | 1435 | external-index |
| 1684 | 1310 | [1310. 子数组异或查询](https://leetcode.cn/problems/xor-queries-of-a-subarray/) | 1460 | external-index |
| 1685 | 3152 | [3152. 特殊数组 II](https://leetcode.cn/problems/special-array-ii/) | 1523 | external-index |
| 1686 | 1749 | [1749. 任意子数组和的绝对值的最大值](https://leetcode.cn/problems/maximum-absolute-sum-of-any-subarray/) | 1542 | external-index |
| 1687 | 53 | [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/) | Unknown | external-index |
| 1688 | 3652 | [3652. 按策略买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-using-strategy/) | 1557 | external-index |
| 1689 | 3361 | [3361. 两个字符串的切换距离](https://leetcode.cn/problems/shift-distance-between-two-strings/) | Unknown | external-index |
| 1690 | 3511 | [3511. 构造正数组](https://leetcode.cn/problems/make-a-positive-array/) | Unknown | external-index |
| 1691 | 3540 | [3540. 访问所有房屋的最短时间](https://leetcode.cn/problems/minimum-time-to-visit-all-houses/) | Unknown | external-index |
| 1692 | 1523 | [1523. 在区间范围内统计奇数数目](https://leetcode.cn/problems/count-odd-numbers-in-an-interval-range/) | 1209 | external-index |
| 1693 | 848 | [848. 字母移位](https://leetcode.cn/problems/shifting-letters/) | 1353 | external-index |

## 一、前缀和 / §1.2 前缀和与哈希表

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1694 | 560 | [560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/) | Unknown | external-index |
| 1695 | 930 | [930. 和相同的二元子数组](https://leetcode.cn/problems/binary-subarrays-with-sum/) | 1592 | external-index |
| 1696 | 1524 | [1524. 和为奇数的子数组数目](https://leetcode.cn/problems/number-of-sub-arrays-with-odd-sum/) | 1611 | external-index |
| 1697 | 974 | [974. 和可被 K 整除的子数组](https://leetcode.cn/problems/subarray-sums-divisible-by-k/) | 1676 | external-index |
| 1698 | 523 | [523. 连续的子数组和](https://leetcode.cn/problems/continuous-subarray-sum/) | 974 | external-index |
| 1699 | 2588 | [2588. 统计美丽子数组数目](https://leetcode.cn/problems/count-the-number-of-beautiful-subarrays/) | 1697 | external-index |
| 1700 | 525 | [525. 连续数组](https://leetcode.cn/problems/contiguous-array/) | Unknown | external-index |
| 1701 | 面试题 17.05 | [面试题 17.05. 字母与数字](https://leetcode.cn/problems/find-longest-subarray-lcci/) | 525 | external-index |
| 1702 | 3755 | [3755. 最大平衡异或子数组的长度](https://leetcode.cn/problems/find-maximum-balanced-xor-subarray-length/) | Unknown | external-index |
| 1703 | 3026 | [3026. 最大好子数组和](https://leetcode.cn/problems/maximum-good-subarray-sum/) | 1817 | external-index |
| 1704 | 1477 | [1477. 找两个和为目标值且不重叠的子数组](https://leetcode.cn/problems/find-two-non-overlapping-sub-arrays-each-with-target-sum/) | 1851 | external-index |
| 1705 | 1546 | [1546. 和为目标值且不重叠的非空子数组的最大数目](https://leetcode.cn/problems/maximum-number-of-non-overlapping-subarrays-with-sum-equals-target/) | 1855 | external-index |
| 1706 | 1124 | [1124. 表现良好的最长时间段](https://leetcode.cn/problems/longest-well-performing-interval/) | 1908 | external-index |
| 1707 | 3728 | [3728. 边界与内部和相等的稳定子数组](https://leetcode.cn/problems/stable-subarrays-with-equal-boundary-and-interior-sum/) | 1909 | external-index |
| 1708 | 3381 | [3381. 长度可被 K 整除的子数组的最大元素和](https://leetcode.cn/problems/maximum-subarray-sum-with-length-divisible-by-k/) | 1943 | external-index |
| 1709 | 2488 | [2488. 统计中位数为 K 的子数组](https://leetcode.cn/problems/count-subarrays-with-median-k/) | 1999 | external-index |
| 1710 | 1590 | [1590. 使数组和能被 P 整除](https://leetcode.cn/problems/make-sum-divisible-by-p/) | 2039 | external-index |
| 1711 | 2845 | [2845. 统计趣味子数组的数目](https://leetcode.cn/problems/count-of-interesting-subarrays/) | 2073 | external-index |
| 1712 | 3739 | [3739. 统计主要元素子数组数目 II](https://leetcode.cn/problems/count-subarrays-with-majority-element-ii/) | 2090 | external-index |
| 1713 | 3900 | [3900. 一次交换后的最长平衡子串](https://leetcode.cn/problems/longest-balanced-substring-after-one-swap/) | 2135 | external-index |
| 1714 | 1074 | [1074. 元素和为目标值的子矩阵数量](https://leetcode.cn/problems/number-of-submatrices-that-sum-to-target/) | 2189 | external-index |
| 1715 | 1442 | [1442. 形成两个异或相等数组的三元组数目](https://leetcode.cn/problems/count-triplets-that-can-form-two-arrays-of-equal-xor/) | Unknown | external-index |
| 1716 | 3714 | [3714. 最长的平衡子串 II](https://leetcode.cn/problems/longest-balanced-substring-ii/) | 2202 | external-index |
| 1717 | 2025 | [2025. 分割数组的最多方案数](https://leetcode.cn/problems/maximum-number-of-ways-to-partition-an-array/) | 2218 | external-index |
| 1718 | 3729 | [3729. 统计有序数组中可被 K 整除的子数组数量](https://leetcode.cn/problems/count-distinct-subarrays-divisible-by-k-in-sorted-array/) | 2248 | external-index |
| 1719 | 3969 | [3969. 求和后首尾数字相同的有效子数组 I](https://leetcode.cn/problems/valid-subarrays-with-matching-sum-digits-i/) | Unknown | external-index |
| 1720 | 2949 | [2949. 统计美丽子字符串 II](https://leetcode.cn/problems/count-beautiful-substrings-ii/) | 2445 | external-index |
| 1721 | 325 | [325. 和等于 k 的最长子数组长度](https://leetcode.cn/problems/maximum-size-subarray-sum-equals-k/) | Unknown | external-index |
| 1722 | 548 | [548. 将数组分割成和相等的子数组](https://leetcode.cn/problems/split-array-with-equal-sum/) | Unknown | external-index |
| 1723 | 1983 | [1983. 范围和相等的最宽索引对](https://leetcode.cn/problems/widest-pair-of-indices-with-equal-range-sum/) | Unknown | external-index |
| 1724 | 2489 | [2489. 固定比率的子字符串数](https://leetcode.cn/problems/number-of-substrings-with-fixed-ratio/) | Unknown | external-index |
| 1725 | 2031 | [2031. 1 比 0 多的子数组个数](https://leetcode.cn/problems/count-subarrays-with-more-ones-than-zeros/) | Unknown | external-index |
| 1726 | 2950 | [2950. 可整除子串的数量](https://leetcode.cn/problems/number-of-divisible-substrings/) | Unknown | external-index |
| 1727 | 3364 | [3364. 最小正和子数组](https://leetcode.cn/problems/minimum-positive-sum-subarray/) | Unknown | external-index |
| 1728 | 363 | [363. 矩形区域不超过 K 的最大数值和](https://leetcode.cn/problems/max-sum-of-rectangle-no-larger-than-k/) | Unknown | external-index |
| 1729 | 3739 | [3739. 统计主要元素子数组数目 II](https://leetcode.cn/problems/count-subarrays-with-majority-element-ii/) | 2090 | external-index |
| 1730 | 2031 | [2031. 1 比 0 多的子数组个数](https://leetcode.cn/problems/count-subarrays-with-more-ones-than-zeros/) | Unknown | external-index |
| 1731 | 437 | [437. 路径总和 III](https://leetcode.cn/problems/path-sum-iii/) | Unknown | external-index |

## 一、前缀和 / §1.3 距离和

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1732 | 1685 | [1685. 有序数组中差绝对值之和](https://leetcode.cn/problems/sum-of-absolute-differences-in-a-sorted-array/) | 1496 | external-index |
| 1733 | 2615 | [2615. 等值距离和](https://leetcode.cn/problems/sum-of-distances/) | 1793 | external-index |
| 1734 | 2602 | [2602. 使数组元素全部相等的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-make-all-array-elements-equal/) | 1903 | external-index |
| 1735 | 3937 | [3937. 使数组变为模交替数组的最少操作次数 I](https://leetcode.cn/problems/minimum-operations-to-make-array-modulo-alternating-i/) | Unknown | external-index |
| 1736 | 2968 | [2968. 执行操作使频率分数最大](https://leetcode.cn/problems/apply-operations-to-maximize-frequency-score/) | 2444 | external-index |
| 1737 | 1703 | [1703. 得到连续 K 个 1 的最少相邻交换次数](https://leetcode.cn/problems/minimum-adjacent-swaps-for-k-consecutive-ones/) | 2467 | external-index |
| 1738 | 3086 | [3086. 拾起 K 个 1 需要的最少行动次数](https://leetcode.cn/problems/minimum-moves-to-pick-k-ones/) | 2673 | external-index |
| 1739 | 3422 | [3422. 将子数组元素变为相等所需的最小操作数](https://leetcode.cn/problems/minimum-operations-to-make-subarray-elements-equal/) | Unknown | external-index |

## 一、前缀和 / §1.4 状态压缩前缀和

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1740 | 1177 | [1177. 构建回文串检测](https://leetcode.cn/problems/can-make-palindrome-from-substring/) | 1848 | external-index |
| 1741 | 1371 | [1371. 每个元音包含偶数次的最长子字符串](https://leetcode.cn/problems/find-the-longest-substring-containing-vowels-in-even-counts/) | 2041 | external-index |
| 1742 | 1542 | [1542. 找出最长的超赞子字符串](https://leetcode.cn/problems/find-longest-awesome-substring/) | 2222 | external-index |
| 1743 | 1915 | [1915. 最美子字符串的数目](https://leetcode.cn/problems/number-of-wonderful-substrings/) | 2235 | external-index |
| 1744 | 2791 | [2791. 树中可以形成回文的路径数](https://leetcode.cn/problems/count-paths-that-can-form-a-palindrome-in-a-tree/) | 2677 | external-index |

## 一、前缀和 / §1.5 进阶

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1745 | 2389 | [2389. 和有限的最长子序列](https://leetcode.cn/problems/longest-subsequence-with-limited-sum/) | Unknown | external-index |
| 1746 | 3709 | [3709. 设计考试分数记录器](https://leetcode.cn/problems/design-exam-scores-tracker/) | 1648 | external-index |
| 1747 | 3919 | [3919. 在下标间移动的最小代价](https://leetcode.cn/problems/minimum-cost-to-move-between-indices/) | 1777 | external-index |
| 1748 | 1895 | [1895. 最大的幻方](https://leetcode.cn/problems/largest-magic-square/) | 1781 | external-index |
| 1749 | 2055 | [2055. 蜡烛之间的盘子](https://leetcode.cn/problems/plates-between-candles/) | 1819 | external-index |
| 1750 | 1744 | [1744. 你能在你最喜欢的那天吃到你最喜欢的糖果吗？](https://leetcode.cn/problems/can-you-eat-your-favorite-candy-on-your-favorite-day/) | 1859 | external-index |
| 1751 | 1878 | [1878. 矩阵中最大的三个菱形和](https://leetcode.cn/problems/get-biggest-three-rhombus-sums-in-a-grid/) | 1898 | external-index |
| 1752 | 3756 | [3756. 连接非零数字并乘以其数字和 II](https://leetcode.cn/problems/concatenate-non-zero-digits-and-multiply-by-sum-ii/) | 1968 | external-index |
| 1753 | 1031 | [1031. 两个无重叠子数组的最大和](https://leetcode.cn/problems/maximum-sum-of-two-non-overlapping-subarrays/) | Unknown | external-index |
| 1754 | 2245 | [2245. 转角路径的乘积中最多能有几个尾随零](https://leetcode.cn/problems/maximum-trailing-zeros-in-a-cornered-path/) | 2037 | external-index |
| 1755 | 1712 | [1712. 将数组分成三个子数组的方案数](https://leetcode.cn/problems/ways-to-split-array-into-three-subarrays/) | 2079 | external-index |
| 1756 | 1862 | [1862. 向下取整数对和](https://leetcode.cn/problems/sum-of-floored-pairs/) | 2170 | external-index |
| 1757 | 3748 | [3748. 统计稳定子数组的数目](https://leetcode.cn/problems/count-stable-subarrays/) | 2209 | external-index |
| 1758 | 2281 | [2281. 巫师的总力量和](https://leetcode.cn/problems/sum-of-total-strength-of-wizards/) | 2621 | external-index |
| 1759 | 3445 | [3445. 奇偶频次间的最大差值 II](https://leetcode.cn/problems/maximum-difference-between-even-and-odd-frequency-ii/) | 2694 | external-index |
| 1760 | 2983 | [2983. 回文串重新排列查询](https://leetcode.cn/problems/palindrome-rearrangement-queries/) | 2780 | external-index |
| 1761 | 2955 | [2955. 同端子串的数量](https://leetcode.cn/problems/number-of-same-end-substrings/) | Unknown | external-index |
| 1762 | 1788 | [1788. 最大化花园的美观度](https://leetcode.cn/problems/maximize-the-beauty-of-the-garden/) | Unknown | external-index |
| 1763 | 2819 | [2819. 购买巧克力后的最小相对损失](https://leetcode.cn/problems/minimum-relative-loss-after-buying-chocolates/) | Unknown | external-index |
| 1764 | 2300 | [2300. 咒语和药水的成功对数](https://leetcode.cn/problems/successful-pairs-of-spells-and-potions/) | Unknown | external-index |
| 1765 | 1534 | [1534. 统计好三元组](https://leetcode.cn/problems/count-good-triplets/) | Unknown | external-index |

## 一、前缀和 / §1.6 二维前缀和

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1766 | 304 | [304. 二维区域和检索 - 矩阵不可变](https://leetcode.cn/problems/range-sum-query-2d-immutable/) | Unknown | external-index |
| 1767 | 1314 | [1314. 矩阵区域和](https://leetcode.cn/problems/matrix-block-sum/) | 1484 | external-index |
| 1768 | 3070 | [3070. 元素和小于等于 k 的子矩阵的数目](https://leetcode.cn/problems/count-submatrices-with-top-left-element-and-sum-less-than-k/) | 1499 | external-index |
| 1769 | 1738 | [1738. 找出第 K 大的异或坐标值](https://leetcode.cn/problems/find-kth-largest-xor-coordinate-value/) | 1671 | external-index |
| 1770 | 3212 | [3212. 统计 X 和 Y 频数相等的子矩阵数量](https://leetcode.cn/problems/count-submatrices-with-equal-frequency-of-x-and-y/) | 1673 | external-index |
| 1771 | 1292 | [1292. 元素和小于等于阈值的正方形的最大边长](https://leetcode.cn/problems/maximum-side-length-of-a-square-with-sum-less-than-or-equal-to-threshold/) | 1735 | external-index |
| 1772 | 3148 | [3148. 矩阵中的最大得分](https://leetcode.cn/problems/maximum-difference-score-in-a-grid/) | 1820 | external-index |

## 二、差分 / §2.1 一维差分 / §2.1.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1773 | 2848 | [2848. 与车相交的点](https://leetcode.cn/problems/points-that-intersect-with-cars/) | 1230 | external-index |
| 1774 | 1893 | [1893. 检查是否区域内所有整数都被覆盖](https://leetcode.cn/problems/check-if-all-the-integers-in-a-range-are-covered/) | 1307 | external-index |
| 1775 | 1854 | [1854. 人口最多的年份](https://leetcode.cn/problems/maximum-population-year/) | 1370 | external-index |
| 1776 | 面试题 16.10 | [面试题 16.10. 生存人数](https://leetcode.cn/problems/living-people-lcci/) | 1854 | external-index |
| 1777 | 2960 | [2960. 统计已测试设备](https://leetcode.cn/problems/count-tested-devices-after-test-operations/) | Unknown | external-index |
| 1778 | 1094 | [1094. 拼车](https://leetcode.cn/problems/car-pooling/) | 1441 | external-index |
| 1779 | 1109 | [1109. 航班预订统计](https://leetcode.cn/problems/corporate-flight-bookings/) | 1570 | external-index |
| 1780 | 3964 | [3964. 照亮道路的最少灯泡数](https://leetcode.cn/problems/minimum-lights-to-illuminate-a-road/) | 1572 | external-index |
| 1781 | 3355 | [3355. 零数组变换 I](https://leetcode.cn/problems/zero-array-transformation-i/) | 1591 | external-index |
| 1782 | 370 | [370. 区间加法](https://leetcode.cn/problems/range-addition/) | Unknown | external-index |

## 二、差分 / §2.1 一维差分 / §2.1.2 进阶

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1783 | 3914 | [3914. 使数组非递减需要的最小累计值](https://leetcode.cn/problems/minimum-operations-to-make-array-non-decreasing/) | 1662 | external-index |
| 1784 | 3453 | [3453. 分割正方形 I](https://leetcode.cn/problems/separate-squares-i/) | 1735 | external-index |
| 1785 | 2381 | [2381. 字母移位 II](https://leetcode.cn/problems/shifting-letters-ii/) | 1793 | external-index |
| 1786 | 995 | [995. K 连续位的最小翻转次数](https://leetcode.cn/problems/minimum-number-of-k-consecutive-bit-flips/) | 1835 | external-index |
| 1787 | 1589 | [1589. 所有排列中的最大和](https://leetcode.cn/problems/maximum-sum-obtained-of-any-permutation/) | 1871 | external-index |
| 1788 | 1526 | [1526. 形成目标数组的子数组最少增加次数](https://leetcode.cn/problems/minimum-number-of-increments-on-subarrays-to-form-a-target-array/) | 1872 | external-index |
| 1789 | 1871 | [1871. 跳跃游戏 VII](https://leetcode.cn/problems/jump-game-vii/) | 1896 | external-index |
| 1790 | 3356 | [3356. 零数组变换 II](https://leetcode.cn/problems/zero-array-transformation-ii/) | 1913 | external-index |
| 1791 | 1943 | [1943. 描述绘画结果](https://leetcode.cn/problems/describe-the-painting/) | 1969 | external-index |
| 1792 | 3224 | [3224. 使差值相等的最少数组改动次数](https://leetcode.cn/problems/minimum-array-changes-to-make-differences-equal/) | 1996 | external-index |
| 1793 | 2327 | [2327. 知道秘密的人数](https://leetcode.cn/problems/number-of-people-aware-of-a-secret/) | Unknown | external-index |
| 1794 | 2251 | [2251. 花期内花的数目](https://leetcode.cn/problems/number-of-flowers-in-full-bloom/) | 2022 | external-index |
| 1795 | 2772 | [2772. 使数组中的所有元素都等于零](https://leetcode.cn/problems/apply-operations-to-make-all-array-elements-equal-to-zero/) | 2029 | external-index |
| 1796 | 3229 | [3229. 使数组等于目标数组所需的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-make-array-equal-to-target/) | 2067 | external-index |
| 1797 | 3529 | [3529. 统计水平子串和垂直子串重叠格子的数目](https://leetcode.cn/problems/count-cells-in-overlapping-horizontal-and-vertical-substrings/) | 2105 | external-index |
| 1798 | 798 | [798. 得分最高的最小轮调](https://leetcode.cn/problems/smallest-rotation-with-highest-score/) | 2130 | external-index |
| 1799 | 3347 | [3347. 执行操作后元素的最高频率 II](https://leetcode.cn/problems/maximum-frequency-of-an-element-after-performing-operations-ii/) | 2156 | external-index |
| 1800 | 2528 | [2528. 最大化城市的最小电量](https://leetcode.cn/problems/maximize-the-minimum-powered-city/) | 2236 | external-index |
| 1801 | 1674 | [1674. 使数组互补的最少操作次数](https://leetcode.cn/problems/minimum-moves-to-make-array-complementary/) | 2333 | external-index |
| 1802 | 3362 | [3362. 零数组变换 III](https://leetcode.cn/problems/zero-array-transformation-iii/) | 2424 | external-index |
| 1803 | 3655 | [3655. 区间乘法查询后的异或 II](https://leetcode.cn/problems/xor-after-range-multiplication-queries-ii/) | 2454 | external-index |
| 1804 | 3017 | [3017. 按距离统计房屋对数目 II](https://leetcode.cn/problems/count-the-number-of-houses-at-a-certain-distance-ii/) | 2709 | external-index |
| 1805 | 2021 | [2021. 街上最亮的位置](https://leetcode.cn/problems/brightest-position-on-street/) | Unknown | external-index |
| 1806 | 2015 | [2015. 每段建筑物的平均高度](https://leetcode.cn/problems/average-height-of-buildings-in-each-segment/) | Unknown | external-index |
| 1807 | 2237 | [2237. 计算街道上满足所需亮度的位置数量](https://leetcode.cn/problems/count-positions-on-street-with-required-brightness/) | Unknown | external-index |
| 1808 | 3009 | [3009. 折线图上的最大交点数量](https://leetcode.cn/problems/maximum-number-of-intersections-on-the-chart/) | Unknown | external-index |
| 1809 | 3279 | [3279. 活塞占据的最大总面积](https://leetcode.cn/problems/maximum-total-area-occupied-by-pistons/) | Unknown | external-index |
| 1810 | 56 | [56. 合并区间](https://leetcode.cn/problems/merge-intervals/) | Unknown | external-index |
| 1811 | 57 | [57. 插入区间](https://leetcode.cn/problems/insert-interval/) | Unknown | external-index |
| 1812 | 732 | [732. 我的日程安排表 III](https://leetcode.cn/problems/my-calendar-iii/) | Unknown | external-index |
| 1813 | 2406 | [2406. 将区间分为最少组数](https://leetcode.cn/problems/divide-intervals-into-minimum-number-of-groups/) | 1713 | external-index |
| 1814 | 253 | [253. 会议室 II](https://leetcode.cn/problems/meeting-rooms-ii/) | Unknown | external-index |
| 1815 | 759 | [759. 员工空闲时间](https://leetcode.cn/problems/employee-free-time/) | Unknown | external-index |

## 二、差分 / §2.2 二维差分

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1816 | 2536 | [2536. 子矩阵元素加 1](https://leetcode.cn/problems/increment-submatrices-by-one/) | 1583 | external-index |
| 1817 | 850 | [850. 矩形面积 II](https://leetcode.cn/problems/rectangle-area-ii/) | 2236 | external-index |
| 1818 | 2132 | [2132. 用邮票贴满网格图](https://leetcode.cn/problems/stamping-the-grid/) | 2364 | external-index |
| 1819 | LCP 74 | [LCP 74. 最强祝福力场](https://leetcode.cn/problems/xepqZ5/) | Unknown | external-index |
| 1820 | 3888 | [3888. 使所有网格元素相等的最小操作次数](https://leetcode.cn/problems/minimum-operations-to-make-all-grid-elements-equal/) | Unknown | external-index |

## 三、栈 / §3.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1821 | 1441 | [1441. 用栈操作构建数组](https://leetcode.cn/problems/build-an-array-with-stack-operations/) | 1180 | external-index |
| 1822 | 844 | [844. 比较含退格的字符串](https://leetcode.cn/problems/backspace-string-compare/) | 1228 | external-index |
| 1823 | 682 | [682. 棒球比赛](https://leetcode.cn/problems/baseball-game/) | Unknown | external-index |
| 1824 | 2390 | [2390. 从字符串中移除星号](https://leetcode.cn/problems/removing-stars-from-a-string/) | 1348 | external-index |
| 1825 | 1472 | [1472. 设计浏览器历史记录](https://leetcode.cn/problems/design-browser-history/) | 1454 | external-index |
| 1826 | 946 | [946. 验证栈序列](https://leetcode.cn/problems/validate-stack-sequences/) | 1462 | external-index |
| 1827 | 3412 | [3412. 计算字符串的镜像分数](https://leetcode.cn/problems/find-mirror-score-of-a-string/) | 1578 | external-index |
| 1828 | 71 | [71. 简化路径](https://leetcode.cn/problems/simplify-path/) | Unknown | external-index |

## 三、栈 / §3.2 进阶

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1829 | 3170 | [3170. 删除星号以后字典序最小的字符串](https://leetcode.cn/problems/lexicographically-minimum-string-after-removing-stars/) | 1772 | external-index |
| 1830 | 155 | [155. 最小栈](https://leetcode.cn/problems/min-stack/) | Unknown | external-index |
| 1831 | 1381 | [1381. 设计一个支持增量操作的栈](https://leetcode.cn/problems/design-a-stack-with-increment-operation/) | Unknown | external-index |
| 1832 | 636 | [636. 函数的独占时间](https://leetcode.cn/problems/exclusive-time-of-functions/) | Unknown | external-index |
| 1833 | 2434 | [2434. 使用机器人打印字典序最小的字符串](https://leetcode.cn/problems/using-a-robot-to-print-the-lexicographically-smallest-string/) | 1953 | external-index |
| 1834 | 895 | [895. 最大频率栈](https://leetcode.cn/problems/maximum-frequency-stack/) | 2028 | external-index |
| 1835 | 1172 | [1172. 餐盘栈](https://leetcode.cn/problems/dinner-plate-stacks/) | 2110 | external-index |
| 1836 | 2589 | [2589. 完成所有任务的最少时间](https://leetcode.cn/problems/minimum-time-to-complete-all-tasks/) | 2381 | external-index |
| 1837 | 2524 | [2524. 子数组的最大频率分数](https://leetcode.cn/problems/maximum-frequency-score-of-a-subarray/) | Unknown | external-index |
| 1838 | 716 | [716. 最大栈](https://leetcode.cn/problems/max-stack/) | Unknown | external-index |

## 三、栈 / §3.3 邻项消除

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1839 | 2696 | [2696. 删除子串后的字符串最小长度](https://leetcode.cn/problems/minimum-string-length-after-removing-substrings/) | 1282 | external-index |
| 1840 | 1047 | [1047. 删除字符串中的所有相邻重复项](https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string/) | 1286 | external-index |
| 1841 | 1544 | [1544. 整理字符串](https://leetcode.cn/problems/make-the-string-great/) | 1344 | external-index |
| 1842 | 3561 | [3561. 移除相邻字符](https://leetcode.cn/problems/resulting-string-after-adjacent-removals/) | 1397 | external-index |
| 1843 | 1003 | [1003. 检查替换后的词是否有效](https://leetcode.cn/problems/check-if-word-is-valid-after-substitutions/) | 1427 | external-index |
| 1844 | 3834 | [3834. 合并相邻且相等的元素](https://leetcode.cn/problems/merge-adjacent-equal-elements/) | 1429 | external-index |
| 1845 | 3853 | [3853. 合并靠近字符](https://leetcode.cn/problems/merge-close-characters/) | 1472 | external-index |
| 1846 | 2216 | [2216. 美化数组的最少删除数](https://leetcode.cn/problems/minimum-deletions-to-make-array-beautiful/) | 1510 | external-index |
| 1847 | 1209 | [1209. 删除字符串中的所有相邻重复项 II](https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string-ii/) | 1542 | external-index |
| 1848 | 3703 | [3703. 移除K-平衡子字符串](https://leetcode.cn/problems/remove-k-balanced-substrings/) | 1802 | external-index |
| 1849 | 1717 | [1717. 删除子字符串的最大得分](https://leetcode.cn/problems/maximum-score-from-removing-substrings/) | 1868 | external-index |
| 1850 | 2197 | [2197. 替换数组中的非互质数](https://leetcode.cn/problems/replace-non-coprime-numbers-in-array/) | 2057 | external-index |
| 1851 | 735 | [735. 小行星碰撞](https://leetcode.cn/problems/asteroid-collision/) | Unknown | external-index |
| 1852 | 2751 | [2751. 机器人碰撞](https://leetcode.cn/problems/robot-collisions/) | 2092 | external-index |
| 1853 | 853 | [853. 车队](https://leetcode.cn/problems/car-fleet/) | Unknown | external-index |
| 1854 | 1776 | [1776. 车队 II](https://leetcode.cn/problems/car-fleet-ii/) | 2531 | external-index |

## 三、栈 / §3.4 合法括号字符串（RBS）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1855 | 20 | [20. 有效的括号](https://leetcode.cn/problems/valid-parentheses/) | Unknown | external-index |
| 1856 | 921 | [921. 使括号有效的最少添加](https://leetcode.cn/problems/minimum-add-to-make-parentheses-valid/) | 1242 | external-index |
| 1857 | 1021 | [1021. 删除最外层的括号](https://leetcode.cn/problems/remove-outermost-parentheses/) | 1311 | external-index |
| 1858 | 1614 | [1614. 括号的最大嵌套深度](https://leetcode.cn/problems/maximum-nesting-depth-of-the-parentheses/) | 1323 | external-index |
| 1859 | 1190 | [1190. 反转每对括号间的子串](https://leetcode.cn/problems/reverse-substrings-between-each-pair-of-parentheses/) | 1486 | external-index |
| 1860 | 856 | [856. 括号的分数](https://leetcode.cn/problems/score-of-parentheses/) | 1563 | external-index |
| 1861 | 1249 | [1249. 移除无效的括号](https://leetcode.cn/problems/minimum-remove-to-make-valid-parentheses/) | 1657 | external-index |
| 1862 | 1963 | [1963. 使字符串平衡的最小交换次数](https://leetcode.cn/problems/minimum-number-of-swaps-to-make-the-string-balanced/) | 1689 | external-index |
| 1863 | 678 | [678. 有效的括号字符串](https://leetcode.cn/problems/valid-parenthesis-string/) | 1700 | external-index |
| 1864 | 1111 | [1111. 有效括号的嵌套深度](https://leetcode.cn/problems/maximum-nesting-depth-of-two-valid-parentheses-strings/) | 1749 | external-index |
| 1865 | 1541 | [1541. 平衡括号字符串的最少插入次数](https://leetcode.cn/problems/minimum-insertions-to-balance-a-parentheses-string/) | 1759 | external-index |
| 1866 | 2116 | [2116. 判断一个括号字符串是否有效](https://leetcode.cn/problems/check-if-a-parentheses-string-can-be-valid/) | 2038 | external-index |
| 1867 | 32 | [32. 最长有效括号](https://leetcode.cn/problems/longest-valid-parentheses/) | Unknown | external-index |

## 三、栈 / §3.5 表达式解析

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1868 | 1006 | [1006. 笨阶乘](https://leetcode.cn/problems/clumsy-factorial/) | 1408 | external-index |
| 1869 | 150 | [150. 逆波兰表达式求值](https://leetcode.cn/problems/evaluate-reverse-polish-notation/) | Unknown | external-index |
| 1870 | 394 | [394. 字符串解码](https://leetcode.cn/problems/decode-string/) | Unknown | external-index |
| 1871 | 8 | [8. 字符串转换整数 (atoi)](https://leetcode.cn/problems/string-to-integer-atoi/) | Unknown | external-index |
| 1872 | 224 | [224. 基本计算器](https://leetcode.cn/problems/basic-calculator/) | Unknown | external-index |
| 1873 | 227 | [227. 基本计算器 II](https://leetcode.cn/problems/basic-calculator-ii/) | Unknown | external-index |
| 1874 | 726 | [726. 原子的数量](https://leetcode.cn/problems/number-of-atoms/) | Unknown | external-index |
| 1875 | 1106 | [1106. 解析布尔表达式](https://leetcode.cn/problems/parsing-a-boolean-expression/) | 1880 | external-index |
| 1876 | 591 | [591. 标签验证器](https://leetcode.cn/problems/tag-validator/) | Unknown | external-index |
| 1877 | 736 | [736. Lisp 语法解析](https://leetcode.cn/problems/parse-lisp-expression/) | Unknown | external-index |
| 1878 | 1096 | [1096. 花括号展开 II](https://leetcode.cn/problems/brace-expansion-ii/) | 2349 | external-index |
| 1879 | 1896 | [1896. 反转表达式值的最少操作次数](https://leetcode.cn/problems/minimum-cost-to-change-the-final-value-of-expression/) | 2532 | external-index |
| 1880 | 65 | [65. 有效数字](https://leetcode.cn/problems/valid-number/) | Unknown | external-index |
| 1881 | 770 | [770. 基本计算器 IV](https://leetcode.cn/problems/basic-calculator-iv/) | 2863 | external-index |
| 1882 | 439 | [439. 三元表达式解析器](https://leetcode.cn/problems/ternary-expression-parser/) | Unknown | external-index |
| 1883 | 3749 | [3749. 计算有效表达式](https://leetcode.cn/problems/evaluate-valid-expressions/) | Unknown | external-index |
| 1884 | 772 | [772. 基本计算器 III](https://leetcode.cn/problems/basic-calculator-iii/) | Unknown | external-index |
| 1885 | 1087 | [1087. 花括号展开](https://leetcode.cn/problems/brace-expansion/) | Unknown | external-index |
| 1886 | 1597 | [1597. 根据中缀表达式构造二叉表达式树](https://leetcode.cn/problems/build-binary-expression-tree-from-infix-expression/) | Unknown | external-index |
| 1887 | 1628 | [1628. 设计带解析函数的表达式树](https://leetcode.cn/problems/design-an-expression-tree-with-evaluate-function/) | Unknown | external-index |

## 三、栈 / §3.6 对顶栈

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1888 | 2296 | [2296. 设计一个文本编辑器](https://leetcode.cn/problems/design-a-text-editor/) | 1912 | external-index |

## 四、队列 / §4.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1889 | 933 | [933. 最近的请求次数](https://leetcode.cn/problems/number-of-recent-calls/) | 1338 | external-index |
| 1890 | 3829 | [3829. 设计共享出行系统](https://leetcode.cn/problems/design-ride-sharing-system/) | 1594 | external-index |
| 1891 | 950 | [950. 按递增顺序显示卡牌](https://leetcode.cn/problems/reveal-cards-in-increasing-order/) | 1686 | external-index |
| 1892 | 649 | [649. Dota2 参议院](https://leetcode.cn/problems/dota2-senate/) | Unknown | external-index |
| 1893 | 346 | [346. 数据流中的移动平均值](https://leetcode.cn/problems/moving-average-from-data-stream/) | Unknown | external-index |
| 1894 | 362 | [362. 敲击计数器](https://leetcode.cn/problems/design-hit-counter/) | Unknown | external-index |
| 1895 | 3851 | [3851. 不违反限制的最大请求数](https://leetcode.cn/problems/maximum-requests-without-violating-the-limit/) | Unknown | external-index |
| 1896 | 379 | [379. 电话目录管理系统](https://leetcode.cn/problems/design-phone-directory/) | Unknown | external-index |
| 1897 | 1429 | [1429. 第一个唯一数字](https://leetcode.cn/problems/first-unique-number/) | Unknown | external-index |
| 1898 | 2534 | [2534. 通过门的时间](https://leetcode.cn/problems/time-taken-to-cross-the-door/) | Unknown | external-index |

## 四、队列 / §4.2 设计

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1899 | 1670 | [1670. 设计前中后队列](https://leetcode.cn/problems/design-front-middle-back-queue/) | 1610 | external-index |
| 1900 | 3508 | [3508. 设计路由器](https://leetcode.cn/problems/implement-router/) | 1851 | external-index |
| 1901 | 225 | [225. 用队列实现栈](https://leetcode.cn/problems/implement-stack-using-queues/) | Unknown | external-index |
| 1902 | 232 | [232. 用栈实现队列](https://leetcode.cn/problems/implement-queue-using-stacks/) | Unknown | external-index |
| 1903 | 622 | [622. 设计循环队列](https://leetcode.cn/problems/design-circular-queue/) | Unknown | external-index |
| 1904 | 641 | [641. 设计循环双端队列](https://leetcode.cn/problems/design-circular-deque/) | Unknown | external-index |

## 四、队列 / §4.3 双端队列

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1905 | 2810 | [2810. 故障键盘](https://leetcode.cn/problems/faulty-keyboard/) | Unknown | external-index |
| 1906 | 2071 | [2071. 你可以安排的最多任务数目](https://leetcode.cn/problems/maximum-number-of-tasks-you-can-assign/) | 2648 | external-index |

## 四、队列 / §4.4 单调队列

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1907 | 239 | [239. 滑动窗口最大值](https://leetcode.cn/problems/sliding-window-maximum/) | Unknown | external-index |
| 1908 | LCR 184 | [LCR 184. 设计自助结算系统](https://leetcode.cn/problems/dui-lie-de-zui-da-zhi-lcof/) | Unknown | external-index |
| 1909 | 1438 | [1438. 绝对差不超过限制的最长连续子数组](https://leetcode.cn/problems/longest-continuous-subarray-with-absolute-diff-less-than-or-equal-to-limit/) | 1672 | external-index |
| 1910 | 2762 | [2762. 不间断子数组](https://leetcode.cn/problems/continuous-subarrays/) | 1438 | external-index |
| 1911 | 3835 | [3835. 开销小于等于 K 的子数组数目](https://leetcode.cn/problems/count-subarrays-with-cost-less-than-or-equal-to-k/) | 1759 | external-index |
| 1912 | 2398 | [2398. 预算内的最多机器人数目](https://leetcode.cn/problems/maximum-number-of-robots-within-budget/) | 1917 | external-index |
| 1913 | 3589 | [3589. 计数质数间隔平衡子数组](https://leetcode.cn/problems/count-prime-gap-balanced-subarrays/) | 2235 | external-index |
| 1914 | 862 | [862. 和至少为 K 的最短子数组](https://leetcode.cn/problems/shortest-subarray-with-sum-at-least-k/) | 2307 | external-index |
| 1915 | 1499 | [1499. 满足不等式的最大值](https://leetcode.cn/problems/max-value-of-equation/) | 2456 | external-index |

## 五、堆（优先队列） / §5.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1916 | 1046 | [1046. 最后一块石头的重量](https://leetcode.cn/problems/last-stone-weight/) | 1173 | external-index |
| 1917 | 3264 | [3264. K 次乘运算后的最终数组 I](https://leetcode.cn/problems/final-array-state-after-k-multiplication-operations-i/) | 1178 | external-index |
| 1918 | 2558 | [2558. 从数量最多的堆取走礼物](https://leetcode.cn/problems/take-gifts-from-the-richest-pile/) | 1277 | external-index |
| 1919 | 2336 | [2336. 无限集中的最小数字](https://leetcode.cn/problems/smallest-number-in-infinite-set/) | 1375 | external-index |
| 1920 | 2530 | [2530. 执行 K 次操作后的最大分数](https://leetcode.cn/problems/maximal-score-after-applying-k-operations/) | 1386 | external-index |
| 1921 | 3066 | [3066. 超过阈值的最少操作数 II](https://leetcode.cn/problems/minimum-operations-to-exceed-threshold-value-ii/) | 1400 | external-index |
| 1922 | 1962 | [1962. 移除石子使总数最小](https://leetcode.cn/problems/remove-stones-to-minimize-the-total/) | 1419 | external-index |
| 1923 | 703 | [703. 数据流中的第 K 大元素](https://leetcode.cn/problems/kth-largest-element-in-a-stream/) | Unknown | external-index |
| 1924 | 3275 | [3275. 第 K 近障碍物查询](https://leetcode.cn/problems/k-th-nearest-obstacle-queries/) | 1420 | external-index |
| 1925 | 1845 | [1845. 座位预约管理系统](https://leetcode.cn/problems/seat-reservation-manager/) | 1429 | external-index |
| 1926 | 2208 | [2208. 将数组和减半的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-halve-array-sum/) | 1550 | external-index |
| 1927 | 2233 | [2233. K 次增加后的最大乘积](https://leetcode.cn/problems/maximum-product-after-k-increments/) | 1686 | external-index |
| 1928 | 3296 | [3296. 移山所需的最少秒数](https://leetcode.cn/problems/minimum-number-of-seconds-to-make-mountain-height-zero/) | 1695 | external-index |
| 1929 | 1942 | [1942. 最小未被占据椅子的编号](https://leetcode.cn/problems/the-number-of-the-smallest-unoccupied-chair/) | 1695 | external-index |
| 1930 | 1801 | [1801. 积压订单中的订单总数](https://leetcode.cn/problems/number-of-orders-in-the-backlog/) | 1711 | external-index |
| 1931 | 2406 | [2406. 将区间分为最少组数](https://leetcode.cn/problems/divide-intervals-into-minimum-number-of-groups/) | 1713 | external-index |
| 1932 | 3478 | [3478. 选出和最大的 K 个元素](https://leetcode.cn/problems/choose-k-elements-with-maximum-sum/) | 1753 | external-index |
| 1933 | 2462 | [2462. 雇佣 K 位工人的总代价](https://leetcode.cn/problems/total-cost-to-hire-k-workers/) | 1764 | external-index |
| 1934 | 1834 | [1834. 单线程 CPU](https://leetcode.cn/problems/single-threaded-cpu/) | 1798 | external-index |
| 1935 | 1792 | [1792. 最大平均通过率](https://leetcode.cn/problems/maximum-average-pass-ratio/) | 1818 | external-index |
| 1936 | 1167 | [1167. 连接木棍的最低费用](https://leetcode.cn/problems/minimum-cost-to-connect-sticks/) | Unknown | external-index |
| 1937 | 253 | [253. 会议室 II](https://leetcode.cn/problems/meeting-rooms-ii/) | Unknown | external-index |

## 五、堆（优先队列） / §5.2 进阶

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1938 | 23 | [23. 合并 K 个升序链表](https://leetcode.cn/problems/merge-k-sorted-lists/) | Unknown | external-index |
| 1939 | 2931 | [2931. 购买物品的最大开销](https://leetcode.cn/problems/maximum-spending-after-buying-items/) | 1822 | external-index |
| 1940 | 3781 | [3781. 二进制交换后的最大分数](https://leetcode.cn/problems/maximum-score-after-binary-swaps/) | 1823 | external-index |
| 1941 | 502 | [502. IPO](https://leetcode.cn/problems/ipo/) | Unknown | external-index |
| 1942 | 1705 | [1705. 吃苹果的最大数目](https://leetcode.cn/problems/maximum-number-of-eaten-apples/) | 1930 | external-index |
| 1943 | 778 | [778. 水位上升的泳池中游泳](https://leetcode.cn/problems/swim-in-rising-water/) | Unknown | external-index |
| 1944 | 1631 | [1631. 最小体力消耗路径](https://leetcode.cn/problems/path-with-minimum-effort/) | 1948 | external-index |
| 1945 | 1882 | [1882. 使用服务器处理任务](https://leetcode.cn/problems/process-tasks-using-servers/) | 1979 | external-index |
| 1946 | 1354 | [1354. 多次求和构造目标数组](https://leetcode.cn/problems/construct-target-array-with-multiple-sums/) | 2015 | external-index |
| 1947 | 1353 | [1353. 最多可以参加的会议数目](https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended/) | 2016 | external-index |
| 1948 | 1235 | [1235. 规划兼职工作](https://leetcode.cn/problems/maximum-profit-in-job-scheduling/) | 2023 | external-index |
| 1949 | 632 | [632. 最小区间](https://leetcode.cn/problems/smallest-range-covering-elements-from-k-lists/) | Unknown | external-index |
| 1950 | 2542 | [2542. 最大子序列的分数](https://leetcode.cn/problems/maximum-subsequence-score/) | 2056 | external-index |
| 1951 | 1383 | [1383. 最大的团队表现值](https://leetcode.cn/problems/maximum-performance-of-a-team/) | 2091 | external-index |
| 1952 | 2402 | [2402. 会议室 III](https://leetcode.cn/problems/meeting-rooms-iii/) | 2093 | external-index |
| 1953 | 2503 | [2503. 矩阵查询可获得的最大分数](https://leetcode.cn/problems/maximum-number-of-points-from-grid-queries/) | 2196 | external-index |
| 1954 | 2163 | [2163. 删除元素后和的最小差值](https://leetcode.cn/problems/minimum-difference-in-sums-after-removal-of-elements/) | 2225 | external-index |
| 1955 | 857 | [857. 雇佣 K 名工人的最低成本](https://leetcode.cn/problems/minimum-cost-to-hire-k-workers/) | 2260 | external-index |
| 1956 | 1606 | [1606. 找到处理最多请求的服务器](https://leetcode.cn/problems/find-servers-that-handled-most-number-of-requests/) | 2276 | external-index |
| 1957 | 1851 | [1851. 包含每个查询的最小区间](https://leetcode.cn/problems/minimum-interval-to-include-each-query/) | 2286 | external-index |
| 1958 | 407 | [407. 接雨水 II](https://leetcode.cn/problems/trapping-rain-water-ii/) | Unknown | external-index |
| 1959 | 2940 | [2940. 找到 Alice 和 Bob 可以相遇的建筑](https://leetcode.cn/problems/find-building-where-alice-and-bob-can-meet/) | 2327 | external-index |
| 1960 | 3399 | [3399. 字符相同的最短子字符串 II](https://leetcode.cn/problems/smallest-substring-with-identical-characters-ii/) | 2376 | external-index |
| 1961 | 3266 | [3266. K 次乘运算后的最终数组 II](https://leetcode.cn/problems/final-array-state-after-k-multiplication-operations-ii/) | 2509 | external-index |
| 1962 | 1675 | [1675. 数组的最小偏移量](https://leetcode.cn/problems/minimize-deviation-in-array/) | 2533 | external-index |
| 1963 | 2617 | [2617. 网格图中最少访问的格子数](https://leetcode.cn/problems/minimum-number-of-visited-cells-in-a-grid/) | 2582 | external-index |
| 1964 | 2532 | [2532. 过桥的时间](https://leetcode.cn/problems/time-to-cross-a-bridge/) | 2589 | external-index |
| 1965 | LCP 33 | [LCP 33. 蓄水](https://leetcode.cn/problems/o8SXZn/) | Unknown | external-index |
| 1966 | 1500 | [1500. 设计文件分享系统](https://leetcode.cn/problems/design-a-file-sharing-system/) | Unknown | external-index |
| 1967 | 1199 | [1199. 建造街区的最短时间](https://leetcode.cn/problems/minimum-time-to-build-blocks/) | Unknown | external-index |
| 1968 | 3506 | [3506. 查找消除细菌菌株所需时间](https://leetcode.cn/problems/find-time-required-to-eliminate-bacterial-strains/) | Unknown | external-index |
| 1969 | 1348 | [1348. 推文计数](https://leetcode.cn/problems/tweet-counts-per-frequency/) | 2037 | external-index |
| 1970 | 855 | [855. 考场就座](https://leetcode.cn/problems/exam-room/) | 2067 | external-index |
| 1971 | 1912 | [1912. 设计电影租借系统](https://leetcode.cn/problems/design-movie-rental-system/) | 2182 | external-index |

## 五、堆（优先队列） / §5.3 第 K 小/大

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1972 | 264 | [264. 丑数 II](https://leetcode.cn/problems/ugly-number-ii/) | Unknown | external-index |
| 1973 | 378 | [378. 有序矩阵中第 K 小的元素](https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/) | Unknown | external-index |
| 1974 | 23 | [23. 合并 K 个升序链表](https://leetcode.cn/problems/merge-k-sorted-lists/) | Unknown | external-index |
| 1975 | 373 | [373. 查找和最小的 K 对数字](https://leetcode.cn/problems/find-k-pairs-with-smallest-sums/) | Unknown | external-index |
| 1976 | 1439 | [1439. 有序矩阵中的第 k 个最小数组和](https://leetcode.cn/problems/find-the-kth-smallest-sum-of-a-matrix-with-sorted-rows/) | 2134 | external-index |
| 1977 | 786 | [786. 第 K 个最小的质数分数](https://leetcode.cn/problems/k-th-smallest-prime-fraction/) | 2169 | external-index |
| 1978 | 3691 | [3691. 最大子数组总值 II](https://leetcode.cn/problems/maximum-total-subarray-value-ii/) | 2469 | external-index |
| 1979 | 2386 | [2386. 找出数组的第 K 大和](https://leetcode.cn/problems/find-the-k-sum-of-an-array/) | 2648 | external-index |

## 五、堆（优先队列） / §5.4 重排元素

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1980 | 984 | [984. 不含 AAA 或 BBB 的字符串](https://leetcode.cn/problems/string-without-aaa-or-bbb/) | 1474 | external-index |
| 1981 | 767 | [767. 重构字符串](https://leetcode.cn/problems/reorganize-string/) | 1681 | external-index |
| 1982 | 1054 | [1054. 距离相等的条形码](https://leetcode.cn/problems/distant-barcodes/) | 1702 | external-index |
| 1983 | 1405 | [1405. 最长快乐字符串](https://leetcode.cn/problems/longest-happy-string/) | 1821 | external-index |
| 1984 | 3081 | [3081. 替换字符串中的问号使分数最小](https://leetcode.cn/problems/replace-question-marks-in-string-to-minimize-its-value/) | 1905 | external-index |
| 1985 | 621 | [621. 任务调度器](https://leetcode.cn/problems/task-scheduler/) | Unknown | external-index |
| 1986 | 358 | [358. K 距离间隔重排字符串](https://leetcode.cn/problems/rearrange-string-k-distance-apart/) | Unknown | external-index |

## 五、堆（优先队列） / §5.5 反悔堆

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1987 | LCP 30 | [LCP 30. 魔塔游戏](https://leetcode.cn/problems/p0NxJO/) | Unknown | external-index |
| 1988 | 1642 | [1642. 可以到达的最远建筑](https://leetcode.cn/problems/furthest-building-you-can-reach/) | 1962 | external-index |
| 1989 | 630 | [630. 课程表 III](https://leetcode.cn/problems/course-schedule-iii/) | Unknown | external-index |
| 1990 | 871 | [871. 最低加油次数](https://leetcode.cn/problems/minimum-number-of-refueling-stops/) | 2074 | external-index |
| 1991 | 3362 | [3362. 零数组变换 III](https://leetcode.cn/problems/zero-array-transformation-iii/) | 2424 | external-index |
| 1992 | 2813 | [2813. 子序列最大优雅度](https://leetcode.cn/problems/maximum-elegance-of-a-k-length-subsequence/) | 2582 | external-index |
| 1993 | 1388 | [1388. 3n 块披萨](https://leetcode.cn/problems/pizza-with-3n-slices/) | Unknown | external-index |
| 1994 | 3892 | [3892. 产生至少 K 个峰值的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-achieve-at-least-k-peaks/) | Unknown | external-index |
| 1995 | 3049 | [3049. 标记所有下标的最早秒数 II](https://leetcode.cn/problems/earliest-second-to-mark-indices-ii/) | 3111 | external-index |
| 1996 | 3711 | [3711. 不出现负余额的最大交易额](https://leetcode.cn/problems/maximum-transactions-without-negative-balance/) | Unknown | external-index |
| 1997 | 2599 | [2599. 使前缀和数组非负](https://leetcode.cn/problems/make-the-prefix-sum-non-negative/) | Unknown | external-index |

## 五、堆（优先队列） / §5.6 懒删除堆

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 1998 | 2349 | [2349. 设计数字容器系统](https://leetcode.cn/problems/design-a-number-container-system/) | 1540 | external-index |
| 1999 | 3885 | [3885. 设计事件管理器](https://leetcode.cn/problems/design-event-manager/) | 1548 | external-index |
| 2000 | 3607 | [3607. 电网维护](https://leetcode.cn/problems/power-grid-maintenance/) | 1700 | external-index |
| 2001 | 2353 | [2353. 设计食物评分系统](https://leetcode.cn/problems/design-a-food-rating-system/) | 1782 | external-index |
| 2002 | 3092 | [3092. 最高频率的 ID](https://leetcode.cn/problems/most-frequent-ids/) | 1793 | external-index |
| 2003 | 3408 | [3408. 设计任务管理器](https://leetcode.cn/problems/design-task-manager/) | 1807 | external-index |
| 2004 | 2034 | [2034. 股票价格波动](https://leetcode.cn/problems/stock-price-fluctuation/) | 1832 | external-index |
| 2005 | 3815 | [3815. 设计拍卖系统](https://leetcode.cn/problems/design-auction-system/) | 1854 | external-index |
| 2006 | 1172 | [1172. 餐盘栈](https://leetcode.cn/problems/dinner-plate-stacks/) | 2110 | external-index |
| 2007 | 218 | [218. 天际线问题](https://leetcode.cn/problems/the-skyline-problem/) | Unknown | external-index |
| 2008 | 3510 | [3510. 移除最小数对使数组有序 II](https://leetcode.cn/problems/minimum-pair-removal-to-sort-array-ii/) | 2608 | external-index |
| 2009 | 3672 | [3672. 子数组中加权众数的总和](https://leetcode.cn/problems/sum-of-weighted-modes-in-subarrays/) | Unknown | external-index |
| 2010 | 3391 | [3391. 设计一个高效的层跟踪三维二进制矩阵](https://leetcode.cn/problems/design-a-3d-binary-matrix-with-efficient-layer-tracking/) | Unknown | external-index |
| 2011 | 716 | [716. 最大栈](https://leetcode.cn/problems/max-stack/) | Unknown | external-index |

## 五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2012 | 2102 | [2102. 序列顺序查询](https://leetcode.cn/problems/sequentially-ordinal-rank-tracker/) | 2159 | external-index |
| 2013 | 295 | [295. 数据流的中位数](https://leetcode.cn/problems/find-median-from-data-stream/) | Unknown | external-index |
| 2014 | 480 | [480. 滑动窗口中位数](https://leetcode.cn/problems/sliding-window-median/) | Unknown | external-index |
| 2015 | 2653 | [2653. 滑动子数组的美丽值](https://leetcode.cn/problems/sliding-subarray-beauty/) | Unknown | external-index |
| 2016 | 1825 | [1825. 求出 MK 平均值](https://leetcode.cn/problems/finding-mk-average/) | 2396 | external-index |
| 2017 | 3505 | [3505. 使 K 个子数组内元素相等的最少操作数](https://leetcode.cn/problems/minimum-operations-to-make-elements-within-k-subarrays-equal/) | 2539 | external-index |
| 2018 | 3013 | [3013. 将数组分成最小总代价的子数组 II](https://leetcode.cn/problems/divide-an-array-into-subarrays-with-minimum-cost-ii/) | 2540 | external-index |
| 2019 | 3321 | [3321. 计算子数组的 x-sum II](https://leetcode.cn/problems/find-x-sum-of-all-k-long-subarrays-ii/) | 2598 | external-index |
| 2020 | 3962 | [3962. 至多 K 次交换后最大子数组和](https://leetcode.cn/problems/maximum-subarray-sum-after-at-most-k-swaps/) | 2672 | external-index |
| 2021 | LCP 24 | [LCP 24. 数字游戏](https://leetcode.cn/problems/5TxKeK/) | Unknown | external-index |
| 2022 | 3369 | [3369. 设计数组统计跟踪器](https://leetcode.cn/problems/design-an-array-statistics-tracker/) | Unknown | external-index |
| 2023 | 3422 | [3422. 将子数组元素变为相等所需的最小操作数](https://leetcode.cn/problems/minimum-operations-to-make-subarray-elements-equal/) | Unknown | external-index |

## 六、字典树（trie） / §6.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2024 | 208 | [208. 实现 Trie (前缀树)](https://leetcode.cn/problems/implement-trie-prefix-tree/) | Unknown | external-index |
| 2025 | 3597 | [3597. 分割字符串](https://leetcode.cn/problems/partition-string/) | Unknown | external-index |
| 2026 | 3043 | [3043. 最长公共前缀的长度](https://leetcode.cn/problems/find-the-length-of-the-longest-common-prefix/) | Unknown | external-index |
| 2027 | 648 | [648. 单词替换](https://leetcode.cn/problems/replace-words/) | Unknown | external-index |
| 2028 | 720 | [720. 词典中最长的单词](https://leetcode.cn/problems/longest-word-in-dictionary/) | Unknown | external-index |
| 2029 | 2416 | [2416. 字符串的前缀分数和](https://leetcode.cn/problems/sum-of-prefix-scores-of-strings/) | 1725 | external-index |
| 2030 | 677 | [677. 键值映射](https://leetcode.cn/problems/map-sum-pairs/) | Unknown | external-index |
| 2031 | 1268 | [1268. 搜索推荐系统](https://leetcode.cn/problems/search-suggestions-system/) | Unknown | external-index |
| 2032 | 1233 | [1233. 删除子文件夹](https://leetcode.cn/problems/remove-sub-folders-from-the-filesystem/) | Unknown | external-index |
| 2033 | 820 | [820. 单词的压缩编码](https://leetcode.cn/problems/short-encoding-of-words/) | Unknown | external-index |
| 2034 | 2261 | [2261. 含最多 K 个可整除元素的子数组](https://leetcode.cn/problems/k-divisible-elements-subarrays/) | Unknown | external-index |
| 2035 | 1804 | [1804. 实现 Trie （前缀树） II](https://leetcode.cn/problems/implement-trie-ii-prefix-tree/) | Unknown | external-index |
| 2036 | 2168 | [2168. 每个数字的频率都相同的不同子字符串的数量](https://leetcode.cn/problems/unique-substrings-with-equal-digit-frequency/) | 2261 | external-index |

## 六、字典树（trie） / §6.2 进阶

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2037 | 211 | [211. 添加与搜索单词 - 数据结构设计](https://leetcode.cn/problems/design-add-and-search-words-data-structure/) | Unknown | external-index |
| 2038 | 676 | [676. 实现一个魔法字典](https://leetcode.cn/problems/implement-magic-dictionary/) | Unknown | external-index |
| 2039 | 212 | [212. 单词搜索 II](https://leetcode.cn/problems/word-search-ii/) | Unknown | external-index |
| 2040 | 3093 | [3093. 最长公共后缀查询](https://leetcode.cn/problems/longest-common-suffix-queries/) | 2118 | external-index |
| 2041 | 745 | [745. 前缀和后缀搜索](https://leetcode.cn/problems/prefix-and-suffix-search/) | Unknown | external-index |
| 2042 | 3045 | [3045. 统计前后缀下标对 II](https://leetcode.cn/problems/count-prefix-and-suffix-pairs-ii/) | 2328 | external-index |
| 2043 | 336 | [336. 回文对](https://leetcode.cn/problems/palindrome-pairs/) | Unknown | external-index |
| 2044 | 1948 | [1948. 删除系统中的重复文件夹](https://leetcode.cn/problems/delete-duplicate-folders-in-system/) | 2534 | external-index |
| 2045 | 425 | [425. 单词方块](https://leetcode.cn/problems/word-squares/) | Unknown | external-index |
| 2046 | 527 | [527. 单词缩写](https://leetcode.cn/problems/word-abbreviation/) | Unknown | external-index |
| 2047 | 588 | [588. 设计内存文件系统](https://leetcode.cn/problems/design-in-memory-file-system/) | Unknown | external-index |
| 2048 | 616 | [616. 给字符串添加加粗标签](https://leetcode.cn/problems/add-bold-tag-in-string/) | Unknown | external-index |
| 2049 | 758 | [758. 字符串中的加粗单词](https://leetcode.cn/problems/bold-words-in-string/) | Unknown | external-index |
| 2050 | 642 | [642. 设计搜索自动补全系统](https://leetcode.cn/problems/design-search-autocomplete-system/) | Unknown | external-index |
| 2051 | 1065 | [1065. 字符串的索引对](https://leetcode.cn/problems/index-pairs-of-a-string/) | Unknown | external-index |
| 2052 | 1166 | [1166. 设计文件系统](https://leetcode.cn/problems/design-file-system/) | Unknown | external-index |
| 2053 | 1858 | [1858. 包含所有前缀的最长单词](https://leetcode.cn/problems/longest-word-with-all-prefixes/) | Unknown | external-index |
| 2054 | 440 | [440. 字典序的第K小数字](https://leetcode.cn/problems/k-th-smallest-in-lexicographical-order/) | Unknown | external-index |

## 六、字典树（trie） / §6.3 字典树优化 DP

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2055 | 139 | [139. 单词拆分](https://leetcode.cn/problems/word-break/) | Unknown | external-index |
| 2056 | 140 | [140. 单词拆分 II](https://leetcode.cn/problems/word-break-ii/) | Unknown | external-index |
| 2057 | 面试题 17.13 | [面试题 17.13. 恢复空格](https://leetcode.cn/problems/re-space-lcci/) | Unknown | external-index |
| 2058 | 472 | [472. 连接词](https://leetcode.cn/problems/concatenated-words/) | 2300 | external-index |
| 2059 | 2977 | [2977. 转换字符串的最小成本 II](https://leetcode.cn/problems/minimum-cost-to-convert-string-ii/) | 2696 | external-index |

## 六、字典树（trie） / §6.4 0-1 字典树（异或字典树）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2060 | 421 | [421. 数组中两个数的最大异或值](https://leetcode.cn/problems/maximum-xor-of-two-numbers-in-an-array/) | 2000 | external-index |
| 2061 | 2935 | [2935. 找出强数对的最大异或值 II](https://leetcode.cn/problems/maximum-strong-pair-xor-ii/) | 2349 | external-index |
| 2062 | 3845 | [3845. 最大子数组异或值](https://leetcode.cn/problems/maximum-subarray-xor-with-bounded-range/) | Unknown | external-index |
| 2063 | 1707 | [1707. 与数组中元素的最大异或值](https://leetcode.cn/problems/maximum-xor-with-an-element-from-array/) | 2359 | external-index |
| 2064 | 1803 | [1803. 统计异或值在范围内的数对有多少](https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/) | 2479 | external-index |
| 2065 | 1938 | [1938. 查询最大基因差](https://leetcode.cn/problems/maximum-genetic-difference-query/) | 2503 | external-index |
| 2066 | 3632 | [3632. 异或至少为 K 的子数组数目](https://leetcode.cn/problems/subarrays-with-xor-at-least-k/) | Unknown | external-index |
| 2067 | 2479 | [2479. 两个不重叠子树的最大异或值](https://leetcode.cn/problems/maximum-xor-of-two-non-overlapping-subtrees/) | Unknown | external-index |

## 七、并查集 / §7.1 基础

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2068 | 684 | [684. 冗余连接](https://leetcode.cn/problems/redundant-connection/) | Unknown | external-index |
| 2069 | 3493 | [3493. 属性图](https://leetcode.cn/problems/properties-graph/) | 1565 | external-index |
| 2070 | 990 | [990. 等式方程的可满足性](https://leetcode.cn/problems/satisfiability-of-equality-equations/) | 1638 | external-index |
| 2071 | 721 | [721. 账户合并](https://leetcode.cn/problems/accounts-merge/) | Unknown | external-index |
| 2072 | 3532 | [3532. 针对图的路径存在性查询 I](https://leetcode.cn/problems/path-existence-queries-in-a-graph-i/) | Unknown | external-index |
| 2073 | 737 | [737. 句子相似性 II](https://leetcode.cn/problems/sentence-similarity-ii/) | Unknown | external-index |
| 2074 | 1101 | [1101. 彼此熟识的最早时间](https://leetcode.cn/problems/the-earliest-moment-when-everyone-become-friends/) | Unknown | external-index |
| 2075 | 1258 | [1258. 近义词句子](https://leetcode.cn/problems/synonymous-sentences/) | Unknown | external-index |

## 七、并查集 / §7.2 进阶

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2076 | 3551 | [3551. 数位和排序需要的最小交换次数](https://leetcode.cn/problems/minimum-swaps-to-sort-by-digit-sum/) | 1507 | external-index |
| 2077 | 2471 | [2471. 逐层排序二叉树所需的最少操作数目](https://leetcode.cn/problems/minimum-number-of-operations-to-sort-a-binary-tree-by-level/) | 1635 | external-index |
| 2078 | 1202 | [1202. 交换字符串中的元素](https://leetcode.cn/problems/smallest-string-with-swaps/) | 1855 | external-index |
| 2079 | 1061 | [1061. 按字典序排列最小的等效字符串](https://leetcode.cn/problems/lexicographically-smallest-equivalent-string/) | Unknown | external-index |
| 2080 | 1722 | [1722. 执行交换操作后的最小汉明距离](https://leetcode.cn/problems/minimize-hamming-distance-after-swap-operations/) | 1892 | external-index |
| 2081 | 3608 | [3608. 包含 K 个连通分量需要的最小时间](https://leetcode.cn/problems/minimum-time-for-k-connected-components/) | 1893 | external-index |
| 2082 | 3613 | [3613. 最小化连通分量的最大成本](https://leetcode.cn/problems/minimize-maximum-component-cost/) | 3608 | external-index |
| 2083 | 778 | [778. 水位上升的泳池中游泳](https://leetcode.cn/problems/swim-in-rising-water/) | Unknown | external-index |
| 2084 | 3695 | [3695. 交换元素后的最大交替和](https://leetcode.cn/problems/maximize-alternating-sum-using-swaps/) | 1984 | external-index |
| 2085 | 765 | [765. 情侣牵手](https://leetcode.cn/problems/couples-holding-hands/) | 1999 | external-index |
| 2086 | 2092 | [2092. 找出知晓秘密的所有专家](https://leetcode.cn/problems/find-all-people-with-secret/) | 2004 | external-index |
| 2087 | 839 | [839. 相似字符串组](https://leetcode.cn/problems/similar-string-groups/) | 2054 | external-index |
| 2088 | 685 | [685. 冗余连接 II](https://leetcode.cn/problems/redundant-connection-ii/) | Unknown | external-index |
| 2089 | 1970 | [1970. 你能穿过矩阵的最后一天](https://leetcode.cn/problems/last-day-where-you-can-still-cross/) | 2124 | external-index |
| 2090 | 2076 | [2076. 处理含限制条件的好友请求](https://leetcode.cn/problems/process-restricted-friend-requests/) | 2131 | external-index |
| 2091 | 1579 | [1579. 保证图可完全遍历](https://leetcode.cn/problems/remove-max-number-of-edges-to-keep-graph-fully-traversable/) | 2132 | external-index |
| 2092 | 959 | [959. 由斜杠划分区域](https://leetcode.cn/problems/regions-cut-by-slashes/) | 2136 | external-index |
| 2093 | 2812 | [2812. 找出最安全路径](https://leetcode.cn/problems/find-the-safest-path-in-a-grid/) | 2154 | external-index |
| 2094 | 2503 | [2503. 矩阵查询可获得的最大分数](https://leetcode.cn/problems/maximum-number-of-points-from-grid-queries/) | 2196 | external-index |
| 2095 | 3600 | [3600. 升级后最大生成树稳定性](https://leetcode.cn/problems/maximize-spanning-tree-stability-with-upgrades/) | 2301 | external-index |
| 2096 | 2867 | [2867. 统计树中的合法路径数目](https://leetcode.cn/problems/count-valid-paths-in-a-tree/) | 2428 | external-index |
| 2097 | 2421 | [2421. 好路径的数目](https://leetcode.cn/problems/number-of-good-paths/) | 2445 | external-index |
| 2098 | 2157 | [2157. 字符串分组](https://leetcode.cn/problems/groups-of-strings/) | 2499 | external-index |
| 2099 | 803 | [803. 打砖块](https://leetcode.cn/problems/bricks-falling-when-hit/) | 2765 | external-index |
| 2100 | 3235 | [3235. 判断矩形的两个角落是否可达](https://leetcode.cn/problems/check-if-the-rectangle-corner-is-reachable/) | Unknown | external-index |
| 2101 | LCP 71 | [LCP 71. 集水器](https://leetcode.cn/problems/kskhHQ/) | Unknown | external-index |
| 2102 | 2459 | [2459. 通过移动项目到空白区域来排序数组](https://leetcode.cn/problems/sort-array-by-moving-items-to-empty-space/) | Unknown | external-index |

## 七、并查集 / §7.3 中介并查集

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2103 | 947 | [947. 移除最多的同行或同列石头](https://leetcode.cn/problems/most-stones-removed-with-same-row-or-column/) | 2035 | external-index |
| 2104 | 3873 | [3873. 添加一个点后可激活的最大点数](https://leetcode.cn/problems/maximum-points-activated-with-one-addition/) | 947 | external-index |
| 2105 | 2709 | [2709. 最大公约数遍历](https://leetcode.cn/problems/greatest-common-divisor-traversal/) | 2172 | external-index |
| 2106 | 1627 | [1627. 带阈值的图连通性](https://leetcode.cn/problems/graph-connectivity-with-threshold/) | 2221 | external-index |
| 2107 | 952 | [952. 按公因数计算最大组件大小](https://leetcode.cn/problems/largest-component-size-by-common-factor/) | 2272 | external-index |
| 2108 | 1998 | [1998. 数组的最大公因数排序](https://leetcode.cn/problems/gcd-sort-of-an-array/) | 2429 | external-index |
| 2109 | 1632 | [1632. 矩阵转换后的排名](https://leetcode.cn/problems/rank-transform-of-a-matrix/) | 2530 | external-index |
| 2110 | 3378 | [3378. 统计最小公倍数图中的连通块数目](https://leetcode.cn/problems/count-connected-components-in-lcm-graph/) | 2532 | external-index |
| 2111 | 2371 | [2371. 最小化网格中的最大值](https://leetcode.cn/problems/minimize-maximum-value-in-a-grid/) | 1632 | external-index |

## 七、并查集 / §7.4 数组上的并查集

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2112 | 1562 | [1562. 查找大小为 M 的最新分组](https://leetcode.cn/problems/find-latest-group-of-size-m/) | 1928 | external-index |
| 2113 | 1488 | [1488. 避免洪水泛滥](https://leetcode.cn/problems/avoid-flood-in-the-city/) | 1974 | external-index |
| 2114 | 1353 | [1353. 最多可以参加的会议数目](https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended/) | 2016 | external-index |
| 2115 | 2382 | [2382. 删除操作后的最大子段和](https://leetcode.cn/problems/maximum-segment-sum-after-removals/) | 2136 | external-index |
| 2116 | 2334 | [2334. 元素值大于变化阈值的子数组](https://leetcode.cn/problems/subarray-with-elements-greater-than-varying-threshold/) | 2381 | external-index |
| 2117 | 3666 | [3666. 使二进制字符串全为 1 的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-equalize-binary-string/) | 2477 | external-index |
| 2118 | 2612 | [2612. 最少翻转操作数](https://leetcode.cn/problems/minimum-reverse-operations/) | 2824 | external-index |

## 七、并查集 / §7.5 区间并查集

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2119 | 3244 | [3244. 新增道路查询后的最短距离 II](https://leetcode.cn/problems/shortest-distance-after-road-addition-queries-ii/) | 2270 | external-index |
| 2120 | 1851 | [1851. 包含每个查询的最小区间](https://leetcode.cn/problems/minimum-interval-to-include-each-query/) | 2286 | external-index |
| 2121 | LCP 52 | [LCP 52. 二叉搜索树染色](https://leetcode.cn/problems/QO5KpG/) | Unknown | external-index |
| 2122 | 2158 | [2158. 每天绘制新区域的数量](https://leetcode.cn/problems/amount-of-new-area-painted-each-day/) | Unknown | external-index |

## 七、并查集 / §7.6 带权并查集（边权并查集）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2123 | 399 | [399. 除法求值](https://leetcode.cn/problems/evaluate-division/) | Unknown | external-index |
| 2124 | 3887 | [3887. 增量偶权环查询](https://leetcode.cn/problems/incremental-even-weighted-cycle-queries/) | 2128 | external-index |
| 2125 | 3710 | [3710. 最大划分因子](https://leetcode.cn/problems/maximum-partition-factor/) | 2135 | external-index |
| 2126 | 2307 | [2307. 检查方程中的矛盾之处](https://leetcode.cn/problems/check-for-contradictions-in-equations/) | Unknown | external-index |

## 八、树状数组和线段树

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2127 | 1331 | [1331. 数组序号转换](https://leetcode.cn/problems/rank-transform-of-an-array/) | Unknown | external-index |

## 八、树状数组和线段树 / §8.1 树状数组

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2128 | 307 | [307. 区域和检索 - 数组可修改](https://leetcode.cn/problems/range-sum-query-mutable/) | Unknown | external-index |
| 2129 | 3072 | [3072. 将元素分配到两个数组中 II](https://leetcode.cn/problems/distribute-elements-into-two-arrays-ii/) | 2053 | external-index |
| 2130 | 3624 | [3624. 位计数深度为 K 的整数数目 II](https://leetcode.cn/problems/number-of-integers-with-popcount-depth-equal-to-k-ii/) | 2086 | external-index |
| 2131 | 3187 | [3187. 数组中的峰值](https://leetcode.cn/problems/peaks-in-array/) | 2154 | external-index |
| 2132 | 3777 | [3777. 使子字符串变交替的最少删除次数](https://leetcode.cn/problems/minimum-deletions-to-make-alternating-substring/) | 2202 | external-index |
| 2133 | 1649 | [1649. 通过指令创建有序数组](https://leetcode.cn/problems/create-sorted-array-through-instructions/) | 2208 | external-index |
| 2134 | 1626 | [1626. 无矛盾的最佳球队](https://leetcode.cn/problems/best-team-with-no-conflicts/) | Unknown | external-index |
| 2135 | 1409 | [1409. 查询带键的排列](https://leetcode.cn/problems/queries-on-a-permutation-with-key/) | Unknown | external-index |
| 2136 | 2250 | [2250. 统计包含每个点的矩形数目](https://leetcode.cn/problems/count-number-of-rectangles-containing-each-point/) | Unknown | external-index |
| 2137 | 2179 | [2179. 统计数组中好三元组数目](https://leetcode.cn/problems/count-good-triplets-in-an-array/) | 2272 | external-index |
| 2138 | 1395 | [1395. 统计作战单位数](https://leetcode.cn/problems/count-number-of-teams/) | Unknown | external-index |
| 2139 | 2659 | [2659. 将数组清空](https://leetcode.cn/problems/make-array-empty/) | 2282 | external-index |
| 2140 | 3915 | [3915. 距离至少为 K 的交替子序列的最大和](https://leetcode.cn/problems/maximum-sum-of-alternating-subsequence-with-distance-at-least-k/) | 2288 | external-index |
| 2141 | 2653 | [2653. 滑动子数组的美丽值](https://leetcode.cn/problems/sliding-subarray-beauty/) | Unknown | external-index |
| 2142 | 3515 | [3515. 带权树中的最短路径](https://leetcode.cn/problems/shortest-path-in-a-weighted-tree/) | 2312 | external-index |
| 2143 | LCP 05 | [LCP 05. 发 LeetCoin](https://leetcode.cn/problems/coin-bonus/) | 3515 | external-index |
| 2144 | 4033 | [4033. 有效 K 个不同元素子数组 I](https://leetcode.cn/problems/valid-k-unique-subarrays-i/) | 2314 | external-index |
| 2145 | 1505 | [1505. 最多 K 次交换相邻数位后得到的最小整数](https://leetcode.cn/problems/minimum-possible-integer-after-at-most-k-adjacent-swaps-on-digits/) | 2337 | external-index |
| 2146 | 3841 | [3841. 查询树上回文路径](https://leetcode.cn/problems/palindromic-path-queries-in-a-tree/) | 2384 | external-index |
| 2147 | 2926 | [2926. 平衡子序列的最大和](https://leetcode.cn/problems/maximum-balanced-subsequence-sum/) | 2448 | external-index |
| 2148 | 2736 | [2736. 最大和查询](https://leetcode.cn/problems/maximum-sum-queries/) | 2533 | external-index |
| 2149 | 3671 | [3671. 子序列美丽值求和](https://leetcode.cn/problems/sum-of-beautiful-subsequences/) | 2647 | external-index |
| 2150 | 3962 | [3962. 至多 K 次交换后最大子数组和](https://leetcode.cn/problems/maximum-subarray-sum-after-at-most-k-swaps/) | 2672 | external-index |
| 2151 | 3382 | [3382. 用点构造面积最大的矩形 II](https://leetcode.cn/problems/maximum-area-rectangle-with-point-constraints-ii/) | 2723 | external-index |
| 2152 | 3590 | [3590. 第 K 小的路径异或和](https://leetcode.cn/problems/kth-smallest-path-xor-sum/) | Unknown | external-index |
| 2153 | 3245 | [3245. 交替组 III](https://leetcode.cn/problems/alternating-groups-iii/) | 3112 | external-index |
| 2154 | 3027 | [3027. 人员站位的方案数 II](https://leetcode.cn/problems/find-the-number-of-ways-to-place-people-ii/) | Unknown | external-index |
| 2155 | 1756 | [1756. 设计最近使用（MRU）队列](https://leetcode.cn/problems/design-most-recently-used-queue/) | Unknown | external-index |
| 2156 | 60 | [60. 排列序列](https://leetcode.cn/problems/permutation-sequence/) | Unknown | external-index |
| 2157 | 3109 | [3109. 查找排列的下标](https://leetcode.cn/problems/find-the-index-of-permutation/) | Unknown | external-index |
| 2158 | 2519 | [2519. 统计 K-Big 索引的数量](https://leetcode.cn/problems/count-the-number-of-k-big-indices/) | Unknown | external-index |
| 2159 | 2613 | [2613. 美数对](https://leetcode.cn/problems/beautiful-pairs/) | Unknown | external-index |
| 2160 | 2921 | [2921. 价格递增的最大利润三元组 II](https://leetcode.cn/problems/maximum-profitable-triplets-with-increasing-prices-ii/) | Unknown | external-index |
| 2161 | 308 | [308. 二维区域和检索 - 矩阵可修改](https://leetcode.cn/problems/range-sum-query-2d-mutable/) | Unknown | external-index |

## 八、树状数组和线段树 / §8.2 逆序对

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2162 | 3994 | [3994. 划分数组的最少相邻交换次数](https://leetcode.cn/problems/minimum-adjacent-swaps-to-partition-array/) | 1705 | external-index |
| 2163 | LCR 170 | [LCR 170. 交易逆序对的总数](https://leetcode.cn/problems/shu-zu-zhong-de-ni-xu-dui-lcof/) | Unknown | external-index |
| 2164 | 315 | [315. 计算右侧小于当前元素的个数](https://leetcode.cn/problems/count-of-smaller-numbers-after-self/) | Unknown | external-index |
| 2165 | 493 | [493. 翻转对](https://leetcode.cn/problems/reverse-pairs/) | Unknown | external-index |
| 2166 | 327 | [327. 区间和的个数](https://leetcode.cn/problems/count-of-range-sum/) | Unknown | external-index |
| 2167 | 2426 | [2426. 满足不等式的数对数目](https://leetcode.cn/problems/number-of-pairs-satisfying-inequality/) | 2030 | external-index |
| 2168 | 4013 | [4013. 按奇偶比统计子数组 II](https://leetcode.cn/problems/count-subarrays-with-even-odd-ratio-ii/) | 2156 | external-index |
| 2169 | 3768 | [3768. 固定长度子数组中的最小逆序对数目](https://leetcode.cn/problems/minimum-inversion-count-in-subarrays-of-fixed-length/) | 2158 | external-index |
| 2170 | 1850 | [1850. 邻位交换的最小次数](https://leetcode.cn/problems/minimum-adjacent-swaps-to-reach-the-kth-smallest-number/) | Unknown | external-index |
| 2171 | 2193 | [2193. 得到回文串的最少操作次数](https://leetcode.cn/problems/minimum-number-of-moves-to-make-palindrome/) | Unknown | external-index |
| 2172 | 1885 | [1885. 统计数对](https://leetcode.cn/problems/count-pairs-in-two-arrays/) | Unknown | external-index |

## 八、树状数组和线段树 / §8.3 线段树（无区间更新）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2173 | 104 | [104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/) | Unknown | external-index |
| 2174 | 111 | [111. 二叉树的最小深度](https://leetcode.cn/problems/minimum-depth-of-binary-tree/) | Unknown | external-index |
| 2175 | 3479 | [3479. 水果成篮 III](https://leetcode.cn/problems/fruits-into-baskets-iii/) | 2178 | external-index |
| 2176 | 2940 | [2940. 找到 Alice 和 Bob 可以相遇的建筑](https://leetcode.cn/problems/find-building-where-alice-and-bob-can-meet/) | 2327 | external-index |
| 2177 | 2286 | [2286. 以组为单位订音乐会的门票](https://leetcode.cn/problems/booking-concert-tickets-in-groups/) | 2470 | external-index |
| 2178 | 3161 | [3161. 物块放置查询](https://leetcode.cn/problems/block-placement-queries/) | 2513 | external-index |
| 2179 | 3901 | [3901. 好子序列查询](https://leetcode.cn/problems/good-subsequence-queries/) | 2545 | external-index |
| 2180 | 3777 | [3777. 使子字符串变交替的最少删除次数](https://leetcode.cn/problems/minimum-deletions-to-make-alternating-substring/) | Unknown | external-index |
| 2181 | 2213 | [2213. 由单个字符重复的最长子字符串](https://leetcode.cn/problems/longest-substring-of-one-repeating-character/) | 2629 | external-index |
| 2182 | 4017 | [4017. 数组中的峰值 II](https://leetcode.cn/problems/peaks-in-array-ii/) | Unknown | external-index |
| 2183 | 3525 | [3525. 求出数组的 X 值 II](https://leetcode.cn/problems/find-x-value-of-array-ii/) | 2645 | external-index |
| 2184 | 3165 | [3165. 不包含相邻元素的子序列的最大和](https://leetcode.cn/problems/maximum-sum-of-subsequence-with-non-adjacent-elements/) | 2697 | external-index |
| 2185 | 3410 | [3410. 删除所有值为某个元素后的最大子数组和](https://leetcode.cn/problems/maximize-subarray-sum-after-removing-all-occurrences-of-one-element/) | 2844 | external-index |
| 2186 | 3501 | [3501. 操作后最大活跃区段数 II](https://leetcode.cn/problems/maximize-active-section-with-trade-ii/) | 2941 | external-index |
| 2187 | LCP 81 | [LCP 81. 与非的谜题](https://leetcode.cn/problems/ryfUiz/) | Unknown | external-index |
| 2188 | 1157 | [1157. 子数组中占绝大多数的元素](https://leetcode.cn/problems/online-majority-element-in-subarray/) | 2205 | external-index |
| 2189 | 2407 | [2407. 最长递增子序列 II](https://leetcode.cn/problems/longest-increasing-subsequence-ii/) | 2280 | external-index |
| 2190 | 2770 | [2770. 达到末尾下标所需的最大跳跃次数](https://leetcode.cn/problems/maximum-number-of-jumps-to-reach-the-last-index/) | Unknown | external-index |

## 八、树状数组和线段树 / §8.4 Lazy 线段树（有区间更新）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2191 | 2569 | [2569. 更新数组后处理求和查询](https://leetcode.cn/problems/handling-sum-queries-after-update/) | 2398 | external-index |
| 2192 | 1622 | [1622. 奇妙序列](https://leetcode.cn/problems/fancy-sequence/) | 2476 | external-index |
| 2193 | 2502 | [2502. 设计内存分配器](https://leetcode.cn/problems/design-memory-allocator/) | Unknown | external-index |
| 2194 | 2589 | [2589. 完成所有任务的最少时间](https://leetcode.cn/problems/minimum-time-to-complete-all-tasks/) | Unknown | external-index |
| 2195 | 2547 | [2547. 拆分数组的最小代价](https://leetcode.cn/problems/minimum-cost-to-split-an-array/) | Unknown | external-index |
| 2196 | 850 | [850. 矩形面积 II](https://leetcode.cn/problems/rectangle-area-ii/) | Unknown | external-index |
| 2197 | 3454 | [3454. 分割正方形 II](https://leetcode.cn/problems/separate-squares-ii/) | 2671 | external-index |
| 2198 | 3569 | [3569. 分割数组后不同质数的最大数目](https://leetcode.cn/problems/maximize-count-of-distinct-primes-after-split/) | 2697 | external-index |
| 2199 | 3721 | [3721. 最长平衡子数组 II](https://leetcode.cn/problems/longest-balanced-subarray-ii/) | 2724 | external-index |
| 2200 | 2916 | [2916. 子数组不同元素数目的平方和 II](https://leetcode.cn/problems/subarrays-distinct-element-sum-of-squares-ii/) | 2816 | external-index |
| 2201 | LCP 52 | [LCP 52. 二叉搜索树染色](https://leetcode.cn/problems/QO5KpG/) | Unknown | external-index |

## 八、树状数组和线段树 / §8.5 动态开点线段树

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2202 | 699 | [699. 掉落的方块](https://leetcode.cn/problems/falling-squares/) | Unknown | external-index |
| 2203 | 715 | [715. Range 模块](https://leetcode.cn/problems/range-module/) | Unknown | external-index |
| 2204 | 729 | [729. 我的日程安排表 I](https://leetcode.cn/problems/my-calendar-i/) | Unknown | external-index |
| 2205 | 731 | [731. 我的日程安排表 II](https://leetcode.cn/problems/my-calendar-ii/) | Unknown | external-index |
| 2206 | 732 | [732. 我的日程安排表 III](https://leetcode.cn/problems/my-calendar-iii/) | Unknown | external-index |
| 2207 | 2276 | [2276. 统计区间中的整数数目](https://leetcode.cn/problems/count-integers-in-intervals/) | 2222 | external-index |
| 2208 | 3590 | [3590. 第 K 小的路径异或和](https://leetcode.cn/problems/kth-smallest-path-xor-sum/) | 2646 | external-index |

## 八、树状数组和线段树 / §8.6 可持久化线段树

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2209 | 3762 | [3762. 使数组元素相等的最小操作次数](https://leetcode.cn/problems/minimum-operations-to-equalize-subarrays/) | 2497 | external-index |

## 八、树状数组和线段树 / §8.7 ST 表（Sparse Table）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2210 | 3691 | [3691. 最大子数组总值 II](https://leetcode.cn/problems/maximum-total-subarray-value-ii/) | 2469 | external-index |
| 2211 | 3501 | [3501. 操作后最大活跃区段数 II](https://leetcode.cn/problems/maximize-active-section-with-trade-ii/) | 2941 | external-index |
| 2212 | 3933 | [3933. 矩阵中的局部最大值 II](https://leetcode.cn/problems/largest-local-values-in-a-matrix-ii/) | Unknown | external-index |

## 九、伸展树（Splay 树）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2213 | 2296 | [2296. 设计一个文本编辑器](https://leetcode.cn/problems/design-a-text-editor/) | Unknown | external-index |
| 2214 | 3526 | [3526. 范围异或查询与子数组反转](https://leetcode.cn/problems/range-xor-queries-with-subarray-reversals/) | Unknown | external-index |

## 十、根号算法 / §10.1 分块

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2215 | 307 | [307. 区域和检索 - 数组可修改](https://leetcode.cn/problems/range-sum-query-mutable/) | Unknown | external-index |
| 2216 | 3943 | [3943. 递增后的数对数量](https://leetcode.cn/problems/number-of-pairs-after-increment/) | 2410 | external-index |

## 十、根号算法 / §10.2 莫队算法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2217 | 4033 | [4033. 有效 K 个不同元素子数组 I](https://leetcode.cn/problems/valid-k-unique-subarrays-i/) | 2314 | external-index |
| 2218 | 3636 | [3636. 查询超过阈值频率最高元素](https://leetcode.cn/problems/threshold-majority-queries/) | 2451 | external-index |
| 2219 | 3590 | [3590. 第 K 小的路径异或和](https://leetcode.cn/problems/kth-smallest-path-xor-sum/) | 2646 | external-index |
| 2220 | 2846 | [2846. 边权重均等查询](https://leetcode.cn/problems/minimum-edge-weight-equilibrium-queries-in-a-tree/) | Unknown | external-index |

## 十、根号算法 / §10.3 根号分解（Sqrt Decomposition）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2221 | 3655 | [3655. 区间乘法查询后的异或 II](https://leetcode.cn/problems/xor-after-range-multiplication-queries-ii/) | 2454 | external-index |
| 2222 | LCP 16 | [LCP 16. 游乐园的游览计划](https://leetcode.cn/problems/you-le-yuan-de-you-lan-ji-hua/) | Unknown | external-index |
| 2223 | 1714 | [1714. 数组中特殊等间距元素的和](https://leetcode.cn/problems/sum-of-special-evenly-spaced-elements-in-array/) | Unknown | external-index |
| 2224 | 3400 | [3400. 右移后的最大匹配索引数](https://leetcode.cn/problems/maximum-number-of-matching-indices-after-right-shifts/) | Unknown | external-index |

## 十、根号算法 / §10.4 其他

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2225 | 3234 | [3234. 统计 1 显著的字符串的数量](https://leetcode.cn/problems/count-the-number-of-substrings-with-dominant-ones/) | 2557 | external-index |

## 专题：离线算法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2226 | 2343 | [2343. 裁剪数字后查询第 K 小的数字](https://leetcode.cn/problems/query-kth-smallest-trimmed-number/) | 1652 | external-index |
| 2227 | 3607 | [3607. 电网维护](https://leetcode.cn/problems/power-grid-maintenance/) | 1700 | external-index |
| 2228 | 2070 | [2070. 每一个查询的最大美丽值](https://leetcode.cn/problems/most-beautiful-item-for-each-query/) | 1724 | external-index |
| 2229 | 1847 | [1847. 最近的房间](https://leetcode.cn/problems/closest-room/) | 2082 | external-index |
| 2230 | 2503 | [2503. 矩阵查询可获得的最大分数](https://leetcode.cn/problems/maximum-number-of-points-from-grid-queries/) | 2196 | external-index |
| 2231 | 1851 | [1851. 包含每个查询的最小区间](https://leetcode.cn/problems/minimum-interval-to-include-each-query/) | 2286 | external-index |
| 2232 | 1697 | [1697. 检查边长度限制的路径是否存在](https://leetcode.cn/problems/checking-existence-of-edge-length-limited-paths/) | 2300 | external-index |
| 2233 | 4033 | [4033. 有效 K 个不同元素子数组 I](https://leetcode.cn/problems/valid-k-unique-subarrays-i/) | 2314 | external-index |
| 2234 | 2940 | [2940. 找到 Alice 和 Bob 可以相遇的建筑](https://leetcode.cn/problems/find-building-where-alice-and-bob-can-meet/) | 2327 | external-index |
| 2235 | 2747 | [2747. 统计没有收到请求的服务器数目](https://leetcode.cn/problems/count-zero-request-servers/) | 2405 | external-index |
| 2236 | 1938 | [1938. 查询最大基因差](https://leetcode.cn/problems/maximum-genetic-difference-query/) | 2503 | external-index |
| 2237 | 2736 | [2736. 最大和查询](https://leetcode.cn/problems/maximum-sum-queries/) | 2533 | external-index |
| 2238 | 3590 | [3590. 第 K 小的路径异或和](https://leetcode.cn/problems/kth-smallest-path-xor-sum/) | 2646 | external-index |
| 2239 | 3382 | [3382. 用点构造面积最大的矩形 II](https://leetcode.cn/problems/maximum-area-rectangle-with-point-constraints-ii/) | 2723 | external-index |

## 编程能力强化训练 / Part A

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2240 | 13 | [13. 罗马数字转整数](https://leetcode.cn/problems/roman-to-integer/) | Unknown | external-index |
| 2241 | 12 | [12. 整数转罗马数字](https://leetcode.cn/problems/integer-to-roman/) | Unknown | external-index |
| 2242 | 273 | [273. 整数转换英文表示](https://leetcode.cn/problems/integer-to-english-words/) | Unknown | external-index |
| 2243 | 68 | [68. 文本左右对齐](https://leetcode.cn/problems/text-justification/) | Unknown | external-index |
| 2244 | 420 | [420. 强密码检验器](https://leetcode.cn/problems/strong-password-checker/) | Unknown | external-index |
| 2245 | 8 | [8. 字符串转换整数 (atoi)](https://leetcode.cn/problems/string-to-integer-atoi/) | Unknown | external-index |
| 2246 | 65 | [65. 有效数字](https://leetcode.cn/problems/valid-number/) | Unknown | external-index |

## 编程能力强化训练 / Part B

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2247 | 146 | [146. LRU 缓存](https://leetcode.cn/problems/lru-cache/) | Unknown | external-index |
| 2248 | 460 | [460. LFU 缓存](https://leetcode.cn/problems/lfu-cache/) | Unknown | external-index |
| 2249 | 432 | [432. 全 O(1) 的数据结构](https://leetcode.cn/problems/all-oone-data-structure/) | Unknown | external-index |
| 2250 | 1206 | [1206. 设计跳表](https://leetcode.cn/problems/design-skiplist/) | Unknown | external-index |

## 编程能力强化训练 / Part C

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 2251 | 3197 | [3197. 包含所有 1 的最小矩形面积 II](https://leetcode.cn/problems/find-the-minimum-area-to-cover-all-ones-ii/) | 2541 | external-index |
| 2252 | 2532 | [2532. 过桥的时间](https://leetcode.cn/problems/time-to-cross-a-bridge/) | 2589 | external-index |
| 2253 | 2056 | [2056. 棋盘上有效移动组合的数目](https://leetcode.cn/problems/number-of-valid-move-combinations-on-chessboard/) | 2611 | external-index |
| 2254 | LCP 48 | [LCP 48. 无限棋局](https://leetcode.cn/problems/fsa7oZ/) | Unknown | external-index |
| 2255 | LCP 21 | [LCP 21. 追逐游戏](https://leetcode.cn/problems/Za25hA/) | Unknown | external-index |
| 2256 | LCP 58 | [LCP 58. 积木拼接](https://leetcode.cn/problems/De4qBB/) | Unknown | external-index |
| 2257 | LCP 13 | [LCP 13. 寻宝](https://leetcode.cn/problems/xun-bao/) | Unknown | external-index |
| 2258 | LCP 69 | [LCP 69. Hello LeetCode!](https://leetcode.cn/problems/rMeRt2/) | Unknown | external-index |
| 2259 | LCP 76 | [LCP 76. 魔法棋盘](https://leetcode.cn/problems/1ybDKD/) | Unknown | external-index |
| 2260 | LCP 82 | [LCP 82. 万灵之树](https://leetcode.cn/problems/cnHoX6/) | Unknown | external-index |
