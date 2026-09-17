# D01 构建与运行

- 版本：HEAD `cfe4725d23`；所有命令仅为待验证配方。

## 结论摘要

Demo 是仓库内 Python 脚本，不应在未安装/构建 Ray 的情况下宣称能运行。上游贡献说明要求使用虚拟环境；Ray 的安装/构建入口由 `python/setup.py`、`pyproject.toml`、`build-wheel.sh` 和开发文档定义。[已确认文件存在；运行未验证]

## 参考运行方式

```bash
cd source/ray
python release/hello_world_tests/hello_world.py
```

预期脚本逻辑输出 `Hello, world!`，这是由函数体和 `print` 静态推断，不是本轮命令输出。若当前 Python 环境没有可导入的 Ray 或 `_raylet`，运行会失败。

## 验证记录

- 构建：未执行。
- 运行：未执行。
- Ray local cluster：未执行。
- GPU/多节点：不适用本 Demo，未执行。

## 相关文档
[Ray 开发](../../../source/ray/doc/source/ray-contribute/development.md) · [Demo 总览](README.md)

## 源码证据摘要
`release/hello_world_tests/hello_world.py:1-13`；`AGENTS.md:64-92`。

## 未解决问题
需记录真实 import、启动日志、退出日志、进程树和耗时后才能升级为已验证。

## 下一步阅读建议
运行前先确认虚拟环境和构建产物，不要修改源码目录中的依赖。
