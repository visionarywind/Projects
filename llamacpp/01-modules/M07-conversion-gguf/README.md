# M07 - 转换与 GGUF Python

> 状态：首版职责层分析，实际转换未验证。

## 职责

`conversion/` 存放 Hugging Face 模型族转换实现，`gguf-py` 提供 Python GGUF 包和命令；转换输出 GGUF，供 M02 loader 使用。包要求 Python >=3.10，依赖 numpy、tqdm、pyyaml、requests。[gguf-py/pyproject.toml:1-45]

## 边界

转换不应塞进 C++ runtime；根开发规则要求沿现有 converter pattern 修改。runtime 只消费最终 GGUF metadata/tensors；模型架构支持需要 converter、C++ architecture 和测试协同。[CLAUDE.md:92-100] [src/llama.cpp:316-377]

## 验证命令

```bash
python3 -m pip install -e gguf-py
python3 -m unittest discover ./gguf-py -v
```

命令来源已确认，当前批未执行。模型下载、HF 权限、量化输出和数值正确性均未知。

---

**页尾：** GGUF 文件不是仓库 `models/` 目录中所有资产的同义词。
