# 快速开始

## CPU 开发路径

```bash
cmake -B build
cmake --build build --config Release -j$(nproc)
./build/bin/llama --help
```

这些是仓库记录的命令；本次知识库生成没有执行它们。模型推理需要准备 GGUF 文件：[D02](../80-demos/D02-cli-inference/README.md)。

## Server 路径

```bash
cmake --build build --target llama-server -j$(nproc)
./build/bin/llama-server -m PATH_TO_MODEL
```

HTTP 测试安装依赖后从 `tools/server/tests` 执行；单项流程见 [D01](../80-demos/D01-server-chat/README.md)。

## 变更前检查

先读 `AGENTS.md`、`CONTRIBUTING.md`；server 改动先读 `tools/server/README-dev.md`；模型架构改动参考 `docs/development/HOWTO-add-model.md`。不要使用废弃 Makefile。[AGENTS.md:1-6] [CLAUDE.md:4-10]

---

**页尾：** 所有命令状态均为“来源已确认、当前批未验证”。
