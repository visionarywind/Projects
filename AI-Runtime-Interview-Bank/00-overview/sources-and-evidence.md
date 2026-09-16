# 资料与证据

## 1. 个人证据（P）

以下只来自用户本轮自述，尚未拆分到具体项目：

| 编号 | 自述事实 | 当前可支持的表达 | 需要补充 |
|---|---|---|---|
| P01 | 13 年工作经验 | 具备长期系统软件/服务端工程经验 | 时间线和代表项目 |
| P02 | 最近约 1.5 年 MindSpore 开发 | 参与深度学习框架相关开发 | 模块、版本、职责、代码边界 |
| P03 | 约半年 UMD 软件开发 | 接触设备用户态软件链路 | UMD 含义、接口和实际工作 |
| P04 | 了解 PyTorch 显存池实现 | 能准备 allocator 源码/设计分析 | 是阅读、排障还是改造 |
| P05 | 有多 Stream 设计经验 | 能准备异步执行和依赖设计 | 真实场景、方案、结果 |
| P06 | Host 组网/网络开发 | 可迁移到通信和分布式系统 | 协议、网卡、RDMA、NUMA 边界 |
| P07 | 约 1 年检索引擎 | 可迁移缓存、索引、低延迟经验 | 引擎、数据结构、指标和故障 |
| P08 | 早期 Java 服务端 | 可证明并发、服务治理、工程落地 | 规模、SLO、线上案例 |

## 2. 工作区静态证据（S）

### LeetCUDA

目标 checkout 文档标记为 `main` / `4513b31`；本仓库没有把 GPU 构建或运行写成已验证。

- `[S-已确认]` 项目采用 Python `torch.utils.cpp_extension.load`、setup.py 扩展和 interview nvcc 二进制三类入口：`LeetCUDA/README.md`、`LeetCUDA/00-overview/architecture.md`。
- `[S-已确认]` Python 扩展通常借用调用方 Tensor 的 storage；binding 不拥有 Tensor 或 allocator；异步 kernel 前必须满足调用方生命周期：`LeetCUDA/01-modules/M08-pytorch-extension/README.md`、`LeetCUDA/90-cross-module/pooling-and-resource-management.md`。
- `[S-已确认]` LeetCUDA 不是统一生产 memory pool；Python 示例依赖 PyTorch allocator，interview benchmark 显式管理 `malloc/cudaMalloc`：`LeetCUDA/90-cross-module/pooling-and-resource-management.md`。
- `[S-已确认]` 文档区分 Stream/Event、设备完成、host 返回和异步错误边界；具体每个扩展是否使用当前 stream仍需逐模块验证：`LeetCUDA/00-overview/runtime-model.md`、`LeetCUDA/00-overview/global-error-model.md`。
- `[S-已确认]` 性能方法要求固定硬件、版本、shape、dtype、warmup、迭代次数和同步点；推荐 correctness→compute-sanitizer→Nsight：`LeetCUDA/90-cross-module/performance-critical-paths.md`。

### 其他资料

- MindSpore、CUDA、PyTorch、NCCL/HCCL、RDMA、vLLM/SGLang 的版本和源码尚未由用户指定；后续引用必须绑定版本或官方文档日期。
- 论文、博客或招聘 JD 的数字若未由本地实验复现，只能标记为外部报告/待验证。

## 3. 事实标签规范

- `[P-已确认]`：用户明确说过，能证明“接触/负责过某类内容”，不能自动证明具体实现深度。
- `[P-待补充]`：需要用户填写项目目标、职责、约束、方案、结果。
- `[S-已确认]`：工作区源码/文档静态证据直接支持。
- `[推断]`：从证据推导的合理结论，需说明推导链。
- `[待验证]`：需要运行、压测、目标版本源码或硬件确认。
- `[建议]`：面试准备方案，不是事实陈述。
- `[存在争议]`：不同框架、版本、硬件或 provider 语义可能不同。

## 4. 个人项目事实卡（请后续补充）

每个项目尽量只需填写脱敏摘要：

```text
项目名称/方向：
时间和团队边界：
项目目标：
我负责的模块和代码边界：
输入/输出/关键接口：
并发、性能、内存或网络约束：
遇到的最难问题：
我比较过的方案：
最终方案及原因：
验证方法和可披露结果：
失败方案/线上事故/复盘：
能否公开项目名、版本、数字：
```

## 5. 引用纪律

- 源码行号必须在目标 commit 读取并核对后才可写入；本首批优先引用已有文档章节，不伪造用户上游源码路径。
- 不把 LeetCUDA 的教学示例写成 MindSpore、UMD、CUDA Runtime 或生产 allocator 的实现。
- 不把“仓库中有命令”写成“命令已成功运行”。
- 不把用户“了解”写成“主导实现”，不把设计建议写成个人项目成果。
