# 快速上手

## 目标

先用最小成本确认源码、安装和测试入口，再决定是否启动模型。不要把进程启动成功误认为模型 ready，也不要把 Python import 成功误认为 GPU 路径正确。

## 推荐顺序

1. 阅读仓库 `CLAUDE.md` 和 `sglang/00-overview/analysis-state.md`。
2. 确认 Python 3.10+、PyTorch、CUDA/目标硬件和模型可用性。
3. 按仓库指导安装：`python3 -m pip install -e "python"`。**未验证**。
4. 运行环境诊断：`python3 -m sglang.check_env`。**未验证**。
5. 先运行无 GPU 依赖的静态检查或 focused unit test。
6. 有模型和 GPU 后再运行 D01 或 server；观察 ready、scheduler、KV pool 和输出状态。

## 第一条源码阅读线

```text
examples/runtime/engine/offline_batch_inference.py
  -> ServerArgs
  -> Engine
  -> TokenizerManager
  -> Scheduler
  -> ScheduleBatch
  -> ForwardBatch/ModelRunner
  -> BatchStrOutput/ReqState
```

## 不要跳过的判断

- `process started` 不等于 scheduler ready；
- tokenized request 不等于 scheduler-side `Req`；
- `next_token_ids` 不等于最终文本；
- CUDA Graph eligibility 不等于 graph replay 一定成功；
- 未运行命令必须保留“未验证”标记。
