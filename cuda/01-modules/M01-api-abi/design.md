# M01 设计原则

M01 将公开 ABI 稳定性与内部实现解耦：公开旧入口通过 v2/common wrapper 归一到 `CUdeviceptr_v2`、内部 context 和 CUI 对象；wrapper 负责参数和输出初始化，CUI 负责状态与资源（静态确认：[inc/cuda.h:62-170]、[src/api/apimem.c:51-118]）。

设计约束：API 入口必须保留旧符号兼容；context/stream/function 句柄必须校验来源与所属 context；异步对象的释放不能由 wrapper 直接决定。导出名、生成 API table 和 `cuda_master.def` 是同一 ABI 契约的不同构建表现。
