# 测试配方

## 静态检查

- 本知识库：`git diff --check`
- 上游源码格式：按 Megatron-LM 的 locked environment 和 autoformat 文档运行。

## 逐层测试

1. Dataset/sampler unit tests。
2. parallel_state group tests。
3. model focused unit tests。
4. optimizer/checkpoint round-trip。
5. D01 torchrun。
6. 正式 pretrain smoke。
7. inference offline/server smoke。

## 记录要求

每次测试记录：源码版本、命令、硬件、rank 数、环境变量、结果和失败日志。没有执行的命令必须标为“未验证”。