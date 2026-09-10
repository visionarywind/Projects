# M01 风险

公共头文件的风险是影响半径而不是单一输出：架构条件错误、PTX 指令不支持、shared-memory size 错误、barrier/mbarrier 生命周期错误，都可能在多个 phase 出现。当前完整 helper-to-consumer 矩阵未知。
