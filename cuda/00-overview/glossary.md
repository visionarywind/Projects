# 术语表

| 术语 | 本项目含义 |
|---|---|
| API wrapper | `src/api` 中校验 ABI 和参数并调用 CUI 的公开入口 |
| CUI | CUDA User Interface 内部运行时实现层 |
| TLS | 每线程 current context 等状态 |
| memobj | 记录地址、描述、backing 和映射关系的内存对象 |
| UVA/UVM | 统一虚拟地址与托管/迁移支持管理器 |
| stream | 异步工作序列及其 QMD、marker、semaphore 资源 |
| channel | 面向 GPU engine 的提交通道，后端由 DMAL 初始化 |
| QMD | 架构相关 kernel/dispatch 描述结构 |
| HAL | 架构操作函数表及编码实现 |
| DMAL | OS/RM/driver-model 抽象层 |
| CNP | CUDA Dynamic Parallelism 的 device-side child launch 支持 |
| ICD | OpenCL Installable Client Driver loader/dispatch ABI |
| DVS | 驱动验证套件的测试结果格式与聚合流程 |

若没有源码证据，术语不延伸为标准 CUDA 文档中的额外语义。
