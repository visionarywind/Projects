# 图论算法

<!-- generated: do not edit by hand; update scripts/generate_leetcode_bank.py instead -->

来源：[分享丨【算法题单】图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
生成时间：2026-09-16 10:17:56 +0800

本页按来源题单中的标题层级组织，只保存题目元数据和链接，不复制题面或题解。

## 一、图的遍历 / §1.1 深度优先搜索（DFS）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 693 | 547 | [547. 省份数量](https://leetcode.cn/problems/number-of-provinces/) | Unknown | external-index |
| 694 | 1971 | [1971. 寻找图中是否存在路径](https://leetcode.cn/problems/find-if-path-exists-in-graph/) | Unknown | external-index |
| 695 | 797 | [797. 所有可能的路径](https://leetcode.cn/problems/all-paths-from-source-to-target/) | 1383 | external-index |
| 696 | 1306 | [1306. 跳跃游戏 III](https://leetcode.cn/problems/jump-game-iii/) | 1397 | external-index |
| 697 | 841 | [841. 钥匙和房间](https://leetcode.cn/problems/keys-and-rooms/) | 1412 | external-index |
| 698 | 2316 | [2316. 统计无向图中无法互相到达点对数](https://leetcode.cn/problems/count-unreachable-pairs-of-nodes-in-an-undirected-graph/) | 1604 | external-index |
| 699 | 1319 | [1319. 连通网络的操作次数](https://leetcode.cn/problems/number-of-operations-to-make-network-connected/) | 1633 | external-index |
| 700 | 2492 | [2492. 两个城市间路径的最小分数](https://leetcode.cn/problems/minimum-score-of-a-path-between-two-cities/) | 1680 | external-index |
| 701 | 3310 | [3310. 移除可疑的方法](https://leetcode.cn/problems/remove-methods-from-project/) | 1711 | external-index |
| 702 | 2685 | [2685. 统计完全连通分量的数量](https://leetcode.cn/problems/count-the-number-of-complete-components/) | 1769 | external-index |
| 703 | 2192 | [2192. 有向无环图中一个节点的所有祖先](https://leetcode.cn/problems/all-ancestors-of-a-node-in-a-directed-acyclic-graph/) | 1788 | external-index |
| 704 | 3387 | [3387. 两天自由外汇交易后的最大货币数](https://leetcode.cn/problems/maximize-amount-after-two-days-of-conversions/) | 1788 | external-index |
| 705 | 924 | [924. 尽量减少恶意软件的传播](https://leetcode.cn/problems/minimize-malware-spread/) | 1869 | external-index |
| 706 | 2101 | [2101. 引爆最多的炸弹](https://leetcode.cn/problems/detonate-the-maximum-bombs/) | 1880 | external-index |
| 707 | 721 | [721. 账户合并](https://leetcode.cn/problems/accounts-merge/) | Unknown | external-index |
| 708 | 207 | [207. 课程表](https://leetcode.cn/problems/course-schedule/) | Unknown | external-index |
| 709 | 802 | [802. 找到最终的安全状态](https://leetcode.cn/problems/find-eventual-safe-states/) | 1962 | external-index |
| 710 | 3695 | [3695. 交换元素后的最大交替和](https://leetcode.cn/problems/maximize-alternating-sum-using-swaps/) | 1984 | external-index |
| 711 | 928 | [928. 尽量减少恶意软件的传播 II](https://leetcode.cn/problems/minimize-malware-spread-ii/) | 1985 | external-index |
| 712 | 2092 | [2092. 找出知晓秘密的所有专家](https://leetcode.cn/problems/find-all-people-with-secret/) | 2004 | external-index |
| 713 | 3108 | [3108. 带权图里旅途的最小代价](https://leetcode.cn/problems/minimum-cost-walk-in-weighted-graph/) | 2109 | external-index |
| 714 | LCP 07 | [LCP 07. 传递信息](https://leetcode.cn/problems/chuan-di-xin-xi/) | Unknown | external-index |
| 715 | 261 | [261. 以图判树](https://leetcode.cn/problems/graph-valid-tree/) | Unknown | external-index |
| 716 | 323 | [323. 无向图中连通分量的数目](https://leetcode.cn/problems/number-of-connected-components-in-an-undirected-graph/) | Unknown | external-index |
| 717 | 1298 | [1298. 你能从盒子里获得的最大糖果数](https://leetcode.cn/problems/maximum-candies-you-can-get-from-boxes/) | 1825 | external-index |

## 一、图的遍历 / §1.2 广度优先搜索（BFS）

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 718 | 3243 | [3243. 新增道路查询后的最短距离 I](https://leetcode.cn/problems/shortest-distance-after-road-addition-queries-i/) | 1568 | external-index |
| 719 | 1311 | [1311. 获取你好友已观看的视频](https://leetcode.cn/problems/get-watched-videos-by-your-friends/) | 1653 | external-index |
| 720 | 3015 | [3015. 按距离统计房屋对数目 I](https://leetcode.cn/problems/count-the-number-of-houses-at-a-certain-distance-i/) | 1658 | external-index |
| 721 | 1129 | [1129. 颜色交替的最短路径](https://leetcode.cn/problems/shortest-path-with-alternating-colors/) | 1780 | external-index |
| 722 | 2039 | [2039. 网络空闲的时刻](https://leetcode.cn/problems/the-time-when-the-network-becomes-idle/) | 1865 | external-index |
| 723 | 2608 | [2608. 图中的最短环](https://leetcode.cn/problems/shortest-cycle-in-a-graph/) | 1904 | external-index |
| 724 | 815 | [815. 公交路线](https://leetcode.cn/problems/bus-routes/) | 1964 | external-index |
| 725 | 3807 | [3807. 修复边以遍历图的最小成本](https://leetcode.cn/problems/minimum-cost-to-repair-edges-to-traverse-a-graph/) | Unknown | external-index |

## 一、图的遍历 / §1.3 图论建模 + BFS 最短路

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 726 | 322 | [322. 零钱兑换](https://leetcode.cn/problems/coin-change/) | Unknown | external-index |
| 727 | 279 | [279. 完全平方数](https://leetcode.cn/problems/perfect-squares/) | Unknown | external-index |
| 728 | 3877 | [3877. 达到目标异或值的最少删除次数](https://leetcode.cn/problems/minimum-removals-to-achieve-target-xor/) | 1745 | external-index |
| 729 | 433 | [433. 最小基因变化](https://leetcode.cn/problems/minimum-genetic-mutation/) | Unknown | external-index |
| 730 | 1284 | [1284. 转化为全零矩阵的最少反转次数](https://leetcode.cn/problems/minimum-number-of-flips-to-convert-binary-matrix-to-zero-matrix/) | 1811 | external-index |
| 731 | 773 | [773. 滑动谜题](https://leetcode.cn/problems/sliding-puzzle/) | 1815 | external-index |
| 732 | 752 | [752. 打开转盘锁](https://leetcode.cn/problems/open-the-lock/) | 1878 | external-index |
| 733 | 3690 | [3690. 拆分合并数组](https://leetcode.cn/problems/split-and-merge-array-transformation/) | 1982 | external-index |
| 734 | 301 | [301. 删除无效的括号](https://leetcode.cn/problems/remove-invalid-parentheses/) | Unknown | external-index |
| 735 | 514 | [514. 自由之路](https://leetcode.cn/problems/freedom-trail/) | Unknown | external-index |
| 736 | 847 | [847. 访问所有节点的最短路径](https://leetcode.cn/problems/shortest-path-visiting-all-nodes/) | 2201 | external-index |
| 737 | 854 | [854. 相似度为 K 的字符串](https://leetcode.cn/problems/k-similar-strings/) | 2377 | external-index |
| 738 | 127 | [127. 单词接龙](https://leetcode.cn/problems/word-ladder/) | Unknown | external-index |
| 739 | 488 | [488. 祖玛游戏](https://leetcode.cn/problems/zuma-game/) | Unknown | external-index |
| 740 | 3991 | [3991. 使用前缀反转对数组进行排序](https://leetcode.cn/problems/sort-array-using-prefix-reversals/) | Unknown | external-index |
| 741 | 3141 | [3141. 最大汉明距离](https://leetcode.cn/problems/maximum-hamming-distances/) | Unknown | external-index |
| 742 | 2998 | [2998. 使 X 和 Y 相等的最少操作次数](https://leetcode.cn/problems/minimum-number-of-operations-to-make-x-and-y-equal/) | 1795 | external-index |
| 743 | 1345 | [1345. 跳跃游戏 IV](https://leetcode.cn/problems/jump-game-iv/) | 1810 | external-index |
| 744 | 2059 | [2059. 转化数字的最小运算数](https://leetcode.cn/problems/minimum-operations-to-convert-number/) | 1850 | external-index |
| 745 | 1654 | [1654. 到家的最少跳跃次数](https://leetcode.cn/problems/minimum-jumps-to-reach-home/) | 2124 | external-index |
| 746 | 3629 | [3629. 通过质数传送到达终点的最少跳跃次数](https://leetcode.cn/problems/minimum-jumps-to-reach-end-via-prime-teleportation/) | 2139 | external-index |
| 747 | LCP 09 | [LCP 09. 最小跳跃次数](https://leetcode.cn/problems/zui-xiao-tiao-yue-ci-shu/) | Unknown | external-index |

## 二、拓扑排序 / §2.1 拓扑排序

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 748 | 1557 | [1557. 可以到达所有点的最少点数目](https://leetcode.cn/problems/minimum-number-of-vertices-to-reach-all-nodes/) | Unknown | external-index |
| 749 | 210 | [210. 课程表 II](https://leetcode.cn/problems/course-schedule-ii/) | Unknown | external-index |
| 750 | 2115 | [2115. 从给定原材料中找到所有可以做出的菜](https://leetcode.cn/problems/find-all-possible-recipes-from-given-supplies/) | 1679 | external-index |
| 751 | 2392 | [2392. 给定条件下构造矩阵](https://leetcode.cn/problems/build-a-matrix-with-conditions/) | 1961 | external-index |
| 752 | 802 | [802. 找到最终的安全状态](https://leetcode.cn/problems/find-eventual-safe-states/) | 1962 | external-index |
| 753 | 1591 | [1591. 奇怪的打印机 II](https://leetcode.cn/problems/strange-printer-ii/) | 2291 | external-index |
| 754 | 1203 | [1203. 项目管理](https://leetcode.cn/problems/sort-items-by-groups-respecting-dependencies/) | 2419 | external-index |
| 755 | 1632 | [1632. 矩阵转换后的排名](https://leetcode.cn/problems/rank-transform-of-a-matrix/) | 2530 | external-index |
| 756 | 2603 | [2603. 收集树中金币](https://leetcode.cn/problems/collect-coins-in-a-tree/) | 2712 | external-index |
| 757 | LCR 114 | [LCR 114. 火星词典](https://leetcode.cn/problems/Jf1JuT/) | Unknown | external-index |
| 758 | 444 | [444. 序列重建](https://leetcode.cn/problems/sequence-reconstruction/) | Unknown | external-index |
| 759 | 3481 | [3481. 应用替换](https://leetcode.cn/problems/apply-substitutions/) | Unknown | external-index |
| 760 | 269 | [269. 火星词典](https://leetcode.cn/problems/alien-dictionary/) | Unknown | external-index |
| 761 | 2371 | [2371. 最小化网格中的最大值](https://leetcode.cn/problems/minimize-maximum-value-in-a-grid/) | 1632 | external-index |
| 762 | 310 | [310. 最小高度树](https://leetcode.cn/problems/minimum-height-trees/) | Unknown | external-index |
| 763 | 1361 | [1361. 验证二叉树](https://leetcode.cn/problems/validate-binary-tree-nodes/) | Unknown | external-index |

## 二、拓扑排序 / §2.2 在拓扑序上 DP

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 764 | 851 | [851. 喧闹和富有](https://leetcode.cn/problems/loud-and-rich/) | 1783 | external-index |
| 765 | 2050 | [2050. 并行课程 III](https://leetcode.cn/problems/parallel-courses-iii/) | 2084 | external-index |
| 766 | 3620 | [3620. 恢复网络路径](https://leetcode.cn/problems/network-recovery-pathways/) | Unknown | external-index |
| 767 | 1857 | [1857. 有向图中最大颜色值](https://leetcode.cn/problems/largest-color-value-in-a-directed-graph/) | 2313 | external-index |
| 768 | 1136 | [1136. 并行课程](https://leetcode.cn/problems/parallel-courses/) | Unknown | external-index |

## 二、拓扑排序 / §2.3 基环树

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 769 | 2359 | [2359. 找到离给定两个节点最近的节点](https://leetcode.cn/problems/find-closest-node-to-given-two-nodes/) | 1715 | external-index |
| 770 | 2360 | [2360. 图中的最长环](https://leetcode.cn/problems/longest-cycle-in-a-graph/) | 1897 | external-index |
| 771 | 684 | [684. 冗余连接](https://leetcode.cn/problems/redundant-connection/) | Unknown | external-index |
| 772 | 685 | [685. 冗余连接 II](https://leetcode.cn/problems/redundant-connection-ii/) | Unknown | external-index |
| 773 | 2876 | [2876. 有向图访问计数](https://leetcode.cn/problems/count-visited-nodes-in-a-directed-graph/) | 2210 | external-index |
| 774 | 2127 | [2127. 参加会议的最多员工数](https://leetcode.cn/problems/maximum-employees-to-be-invited-to-a-meeting/) | 2449 | external-index |
| 775 | 2836 | [2836. 在传球游戏中最大化函数值](https://leetcode.cn/problems/maximize-value-of-function-in-a-ball-passing-game/) | 2769 | external-index |
| 776 | LCP 21 | [LCP 21. 追逐游戏](https://leetcode.cn/problems/Za25hA/) | Unknown | external-index |
| 777 | 2204 | [2204. 无向图中到环的距离](https://leetcode.cn/problems/distance-to-a-cycle-in-undirected-graph/) | Unknown | external-index |
| 778 | 287 | [287. 寻找重复数](https://leetcode.cn/problems/find-the-duplicate-number/) | Unknown | external-index |

## 三、最短路 / §3.1 单源最短路：Dijkstra 算法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 779 | 743 | [743. 网络延迟时间](https://leetcode.cn/problems/network-delay-time/) | Unknown | external-index |
| 780 | 3341 | [3341. 到达最后一个房间的最少时间 I](https://leetcode.cn/problems/find-minimum-time-to-reach-last-room-i/) | 1721 | external-index |
| 781 | 3112 | [3112. 访问消失节点的最少时间](https://leetcode.cn/problems/minimum-time-to-visit-disappearing-nodes/) | 1757 | external-index |
| 782 | 2642 | [2642. 设计可以求最短路径的图类](https://leetcode.cn/problems/design-graph-with-shortest-path-calculator/) | 1811 | external-index |
| 783 | 3604 | [3604. 有向图中到达终点的最少时间](https://leetcode.cn/problems/minimum-time-to-reach-destination-in-directed-graph/) | 1845 | external-index |
| 784 | 1514 | [1514. 概率最大的路径](https://leetcode.cn/problems/path-with-maximum-probability/) | 1846 | external-index |
| 785 | 3650 | [3650. 边反转的最小路径总成本](https://leetcode.cn/problems/minimum-cost-path-with-edge-reversals/) | 1854 | external-index |
| 786 | 3342 | [3342. 到达最后一个房间的最少时间 II](https://leetcode.cn/problems/find-minimum-time-to-reach-last-room-ii/) | 1862 | external-index |
| 787 | 1631 | [1631. 最小体力消耗路径](https://leetcode.cn/problems/path-with-minimum-effort/) | 1948 | external-index |
| 788 | 1786 | [1786. 从第一个节点出发到最后一个节点的受限路径数](https://leetcode.cn/problems/number-of-restricted-paths-from-first-to-last-node/) | 2079 | external-index |
| 789 | 3924 | [3924. 有限重边的最小阈值路径](https://leetcode.cn/problems/minimum-threshold-path-with-limited-heavy-edges/) | 2080 | external-index |
| 790 | 3123 | [3123. 最短路径中的边](https://leetcode.cn/problems/find-edges-in-shortest-paths/) | 2093 | external-index |
| 791 | 1976 | [1976. 到达目的地的方案数](https://leetcode.cn/problems/number-of-ways-to-arrive-at-destination/) | 2095 | external-index |
| 792 | 778 | [778. 水位上升的泳池中游泳](https://leetcode.cn/problems/swim-in-rising-water/) | 2097 | external-index |
| 793 | 2662 | [2662. 前往目标的最小代价](https://leetcode.cn/problems/minimum-cost-of-a-path-with-special-roads/) | 2154 | external-index |
| 794 | 3377 | [3377. 使两个整数相等的数位操作](https://leetcode.cn/problems/digit-operations-to-make-two-integers-equal/) | 2186 | external-index |
| 795 | 3928 | [3928. 购买苹果的最低成本 II](https://leetcode.cn/problems/minimum-cost-to-buy-apples-ii/) | 2187 | external-index |
| 796 | 2045 | [2045. 到达目的地的第二短时间](https://leetcode.cn/problems/second-minimum-time-to-reach-destination/) | 2202 | external-index |
| 797 | 3419 | [3419. 图的最大边权的最小值](https://leetcode.cn/problems/minimize-the-maximum-edge-weight-of-graph/) | 2243 | external-index |
| 798 | 882 | [882. 细分图中的可到达节点](https://leetcode.cn/problems/reachable-nodes-in-subdivided-graph/) | 2328 | external-index |
| 799 | 2203 | [2203. 得到要求路径的最小带权子图](https://leetcode.cn/problems/minimum-weighted-subgraph-with-the-required-paths/) | 2364 | external-index |
| 800 | 2577 | [2577. 在网格图中访问一个格子的最少时间](https://leetcode.cn/problems/minimum-time-to-visit-a-cell-in-a-grid/) | 2382 | external-index |
| 801 | 818 | [818. 赛车](https://leetcode.cn/problems/race-car/) | 2392 | external-index |
| 802 | 1928 | [1928. 规定时间内到达终点的最小花费](https://leetcode.cn/problems/minimum-cost-to-reach-destination-in-time/) | 2413 | external-index |
| 803 | 787 | [787. K 站中转内最便宜的航班](https://leetcode.cn/problems/cheapest-flights-within-k-stops/) | 1928 | external-index |
| 804 | 2699 | [2699. 修改图中的边权](https://leetcode.cn/problems/modify-graph-edge-weights/) | 2874 | external-index |
| 805 | 1810 | [1810. 隐藏网格下的最小消耗路径](https://leetcode.cn/problems/minimum-path-cost-in-a-hidden-grid/) | Unknown | external-index |
| 806 | 2093 | [2093. 前往目标城市的最小费用](https://leetcode.cn/problems/minimum-cost-to-reach-city-with-discounts/) | Unknown | external-index |
| 807 | 2473 | [2473. 购买苹果的最低成本](https://leetcode.cn/problems/minimum-cost-to-buy-apples/) | Unknown | external-index |
| 808 | 2737 | [2737. 找到最近的标记节点](https://leetcode.cn/problems/find-the-closest-marked-node/) | Unknown | external-index |
| 809 | 3970 | [3970. 最多 K 个连续相同字符的最短路径](https://leetcode.cn/problems/shortest-path-with-at-most-k-consecutive-identical-characters/) | 1840 | external-index |
| 810 | 3977 | [3977. 有限电量到达目标节点的最少时间](https://leetcode.cn/problems/minimum-time-to-reach-target-with-limited-power/) | 2103 | external-index |
| 811 | LCP 35 | [LCP 35. 电动车游城市](https://leetcode.cn/problems/DFPeFJ/) | Unknown | external-index |
| 812 | 3599 | [3599. 划分数组得到最小 XOR](https://leetcode.cn/problems/partition-array-to-minimize-xor/) | Unknown | external-index |
| 813 | 3594 | [3594. 所有人渡河所需的最短时间](https://leetcode.cn/problems/minimum-time-to-transport-all-individuals/) | 2604 | external-index |
| 814 | 3778 | [3778. 排除一个最大权重边的最小距离](https://leetcode.cn/problems/minimum-distance-excluding-one-maximum-weighted-edge/) | Unknown | external-index |
| 815 | 2714 | [2714. 找到 K 次跨越的最短路径](https://leetcode.cn/problems/find-shortest-path-with-k-hops/) | Unknown | external-index |
| 816 | 2589 | [2589. 完成所有任务的最少时间](https://leetcode.cn/problems/minimum-time-to-complete-all-tasks/) | 2381 | external-index |

## 三、最短路 / §3.2 全源最短路：Floyd 算法

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 817 | 2642 | [2642. 设计可以求最短路径的图类](https://leetcode.cn/problems/design-graph-with-shortest-path-calculator/) | 1811 | external-index |
| 818 | 1334 | [1334. 阈值距离内邻居最少的城市](https://leetcode.cn/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/) | 1855 | external-index |
| 819 | 2976 | [2976. 转换字符串的最小成本 I](https://leetcode.cn/problems/minimum-cost-to-convert-string-i/) | 1882 | external-index |
| 820 | 2959 | [2959. 关闭分部的可行集合数目](https://leetcode.cn/problems/number-of-possible-sets-of-closing-branches/) | 2077 | external-index |
| 821 | 2977 | [2977. 转换字符串的最小成本 II](https://leetcode.cn/problems/minimum-cost-to-convert-string-ii/) | 2696 | external-index |
| 822 | 1462 | [1462. 课程表 IV](https://leetcode.cn/problems/course-schedule-iv/) | 1693 | external-index |
| 823 | 2101 | [2101. 引爆最多的炸弹](https://leetcode.cn/problems/detonate-the-maximum-bombs/) | Unknown | external-index |

## 四、最小生成树

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 824 | 1584 | [1584. 连接所有点的最小费用](https://leetcode.cn/problems/min-cost-to-connect-all-points/) | 1858 | external-index |
| 825 | 3600 | [3600. 升级后最大生成树稳定性](https://leetcode.cn/problems/maximize-spanning-tree-stability-with-upgrades/) | 2301 | external-index |
| 826 | 1489 | [1489. 找到最小生成树里的关键边和伪关键边](https://leetcode.cn/problems/find-critical-and-pseudo-critical-edges-in-minimum-spanning-tree/) | 2572 | external-index |
| 827 | 1135 | [1135. 最低成本连通所有城市](https://leetcode.cn/problems/connecting-cities-with-minimum-cost/) | Unknown | external-index |
| 828 | 1168 | [1168. 水资源分配优化](https://leetcode.cn/problems/optimize-water-distribution-in-a-village/) | Unknown | external-index |
| 829 | 3219 | [3219. 切蛋糕的最小总开销 II](https://leetcode.cn/problems/minimum-cost-for-cutting-cake-ii/) | Unknown | external-index |

## 五、欧拉路径/欧拉回路

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 830 | 332 | [332. 重新安排行程](https://leetcode.cn/problems/reconstruct-itinerary/) | Unknown | external-index |
| 831 | 753 | [753. 破解保险箱](https://leetcode.cn/problems/cracking-the-safe/) | 2274 | external-index |
| 832 | 2097 | [2097. 合法重新排列数对](https://leetcode.cn/problems/valid-arrangement-of-pairs/) | 2651 | external-index |

## 六、强连通分量/双连通分量

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 833 | 1192 | [1192. 查找集群内的关键连接](https://leetcode.cn/problems/critical-connections-in-a-network/) | 2085 | external-index |
| 834 | 1568 | [1568. 使陆地分离的最少天数](https://leetcode.cn/problems/minimum-number-of-days-to-disconnect-island/) | 2209 | external-index |
| 835 | LCP 54 | [LCP 54. 夺回据点](https://leetcode.cn/problems/s5kipK/) | Unknown | external-index |
| 836 | 3383 | [3383. 施法所需最低符文数量](https://leetcode.cn/problems/minimum-runes-to-add-to-cast-spell/) | Unknown | external-index |

## 七、二分图染色

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 837 | 785 | [785. 判断二分图](https://leetcode.cn/problems/is-graph-bipartite/) | 1625 | external-index |
| 838 | 886 | [886. 可能的二分法](https://leetcode.cn/problems/possible-bipartition/) | 1795 | external-index |
| 839 | 3710 | [3710. 最大划分因子](https://leetcode.cn/problems/maximum-partition-factor/) | 2135 | external-index |

## 八、网络流

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 840 | 1947 | [1947. 最大兼容性评分和](https://leetcode.cn/problems/maximum-compatibility-score-sum/) | 1704 | external-index |
| 841 | 3376 | [3376. 破解锁的最少时间 I](https://leetcode.cn/problems/minimum-time-to-break-locks-i/) | 1793 | external-index |
| 842 | 2850 | [2850. 将石头分散到网格图的最少移动次数](https://leetcode.cn/problems/minimum-moves-to-spread-stones-over-grid/) | 2001 | external-index |
| 843 | 1879 | [1879. 两个数组最小的异或值之和](https://leetcode.cn/problems/minimum-xor-sum-of-two-arrays/) | 2145 | external-index |
| 844 | 1349 | [1349. 参加考试的最大学生数](https://leetcode.cn/problems/maximum-students-taking-exam/) | 2386 | external-index |
| 845 | 2172 | [2172. 数组的最大与和](https://leetcode.cn/problems/maximum-and-sum-of-array/) | 2392 | external-index |
| 846 | 3276 | [3276. 选择矩阵中单元格的最大得分](https://leetcode.cn/problems/select-cells-in-grid-with-maximum-score/) | 2403 | external-index |
| 847 | 1595 | [1595. 连通两组点的最小成本](https://leetcode.cn/problems/minimum-cost-to-connect-two-groups-of-points/) | 2538 | external-index |
| 848 | 3257 | [3257. 放三个车的价值之和最大 II](https://leetcode.cn/problems/maximum-value-sum-by-placing-three-rooks-ii/) | 2553 | external-index |
| 849 | LCP 04 | [LCP 04. 覆盖](https://leetcode.cn/problems/broken-board-dominoes/) | Unknown | external-index |
| 850 | LCP 38 | [LCP 38. 守卫城堡](https://leetcode.cn/problems/7rLGCR/) | Unknown | external-index |
| 851 | 1820 | [1820. 最多邀请的个数](https://leetcode.cn/problems/maximum-number-of-accepted-invitations/) | Unknown | external-index |
| 852 | 2403 | [2403. 杀死所有怪物的最短时间](https://leetcode.cn/problems/minimum-time-to-kill-all-monsters/) | 3376 | external-index |
| 853 | 3385 | [3385. 破解锁的最少时间 II](https://leetcode.cn/problems/minimum-time-to-break-locks-ii/) | 3376 | external-index |
| 854 | 1066 | [1066. 校园自行车分配 II](https://leetcode.cn/problems/campus-bikes-ii/) | Unknown | external-index |
| 855 | 4004 | [4004. 使循环数组余额非负的最少移动次数 II](https://leetcode.cn/problems/minimum-moves-to-balance-circular-array-ii/) | Unknown | external-index |
| 856 | 2123 | [2123. 使矩阵中的 1 互不相邻的最小操作数](https://leetcode.cn/problems/minimum-operations-to-remove-adjacent-ones-in-matrix/) | Unknown | external-index |
| 857 | 2463 | [2463. 最小移动总距离](https://leetcode.cn/problems/minimum-total-distance-traveled/) | Unknown | external-index |

## 九、其他

| 顺序 | 题号 | 题目 | 难度分 | 证据状态 |
|---:|---:|---|---:|---|
| 858 | 1042 | [1042. 不邻接植花](https://leetcode.cn/problems/flower-planting-with-no-adjacent/) | 1712 | external-index |
| 859 | 1761 | [1761. 一个图中连通三元组的最小度数](https://leetcode.cn/problems/minimum-degree-of-a-connected-trio-in-a-graph/) | 2005 | external-index |
| 860 | 2508 | [2508. 添加边使所有节点度数都为偶数](https://leetcode.cn/problems/add-edges-to-make-degrees-of-all-nodes-even/) | 2060 | external-index |
| 861 | 1579 | [1579. 保证图可完全遍历](https://leetcode.cn/problems/remove-max-number-of-edges-to-keep-graph-fully-traversable/) | 2132 | external-index |
| 862 | 2065 | [2065. 最大化一张图中的路径价值](https://leetcode.cn/problems/maximum-path-quality-of-a-graph/) | 2178 | external-index |
| 863 | 1697 | [1697. 检查边长度限制的路径是否存在](https://leetcode.cn/problems/checking-existence-of-edge-length-limited-paths/) | 2300 | external-index |
| 864 | 2242 | [2242. 节点序列的最大得分](https://leetcode.cn/problems/maximum-score-of-a-node-sequence/) | 2304 | external-index |
| 865 | 2493 | [2493. 将节点分成尽可能多的组](https://leetcode.cn/problems/divide-nodes-into-the-maximum-number-of-groups/) | 2415 | external-index |
| 866 | 1782 | [1782. 统计点对的数目](https://leetcode.cn/problems/count-pairs-of-nodes/) | 2457 | external-index |
| 867 | 3666 | [3666. 使二进制字符串全为 1 的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-equalize-binary-string/) | 2477 | external-index |
| 868 | 2612 | [2612. 最少翻转操作数](https://leetcode.cn/problems/minimum-reverse-operations/) | 2824 | external-index |
| 869 | 3435 | [3435. 最短公共超序列的字母出现频率](https://leetcode.cn/problems/frequencies-of-shortest-supersequences/) | 3028 | external-index |
| 870 | 466 | [466. 统计重复个数](https://leetcode.cn/problems/count-the-repetitions/) | Unknown | external-index |
| 871 | LCP 16 | [LCP 16. 游乐园的游览计划](https://leetcode.cn/problems/you-le-yuan-de-you-lan-ji-hua/) | Unknown | external-index |
| 872 | 277 | [277. 搜寻名人](https://leetcode.cn/problems/find-the-celebrity/) | Unknown | external-index |
| 873 | 1724 | [1724. 检查边长度限制的路径是否存在 II](https://leetcode.cn/problems/checking-existence-of-edge-length-limited-paths-ii/) | Unknown | external-index |
| 874 | 2077 | [2077. 殊途同归](https://leetcode.cn/problems/paths-in-maze-that-lead-to-same-room/) | Unknown | external-index |
| 875 | 3656 | [3656. 判断是否存在简单图](https://leetcode.cn/problems/determine-if-a-simple-graph-exists/) | Unknown | external-index |
