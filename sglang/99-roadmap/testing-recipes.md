# 测试配方

## 静态文档检查

本批已执行：递归 Markdown 本地链接检查、fenced block 平衡检查和 `git diff --check -- source-doc`。当前结果：43 个 Markdown、188 个本地链接、0 个链接/fence 错误；source-doc 仍为未跟踪目录。

## Python focused test

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
