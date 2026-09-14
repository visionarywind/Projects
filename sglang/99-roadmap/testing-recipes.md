# 测试配方

## 静态文档检查

本批已执行：递归 Markdown 本地链接检查、fenced block 平衡检查和 `git diff --check`。当前结果：70 个 Markdown、256 个本地 Markdown/源码引用，其中 199 个文档链接和 57 个源码证据链接均可解析；未发现链接/fence/whitespace 错误。

## 本轮模块覆盖

M11-M18 已建立静态模块文章和测试入口地图：speculative、multimodal、KV transfer/HiCache、quant/MoE/LoRA、IPC、kernel/platform、Rust/router/gateway 和 suite/benchmark。它们只证明源码和测试入口已定位；未执行真实 GPU、模型、跨机传输、Rust build、完整 CI 或端到端服务。


仓库指导示例：

```bash
python3 test/registered/unit/mem_cache/test_radix_cache_unit.py
pytest test/registered/unit/mem_cache/ -v
```

这些命令本批未执行，不能写成通过。

## Scheduler 重点

- abort 与 metrics；
- decode retraction；
- mixed chunked prefill；
- priority scheduling；
- prefill delayer；
- chunk gate；
- batch overlap。

具体路径登记在 M04 文档，均未在本批执行。

## 运行测试的记录格式

记录 commit、硬件、模型、命令、环境变量、stdout/stderr、耗时和是否清理残留进程。GPU、多卡、NCCL/RCCL/HCCL、CUDA Graph 和端到端输出必须单独标记。
