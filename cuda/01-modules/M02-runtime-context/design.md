# M02 设计

M02 把运行时状态分成进程级 globals、线程级 TLS/current context 和 context-owned resources。API 检查通过统一的 `cuiInitCheckEx`/`cuiInitCheckCtx` 收敛 initialized、权限、活跃状态、API 版本和 sticky error（静态确认：[src/cui/cuiinit.c:2910-3039]）。

初始化采用分阶段提交和反向回滚；context 创建先分配对象再初始化，失败时撤销已初始化部分并把输出置空。
