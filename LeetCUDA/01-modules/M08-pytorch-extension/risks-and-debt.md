# M08 风险与债务

- Python、PyTorch、CUDA toolkit、C++ ABI 和 extension cache 组合未锁定。
- 各模块输入检查、stream 使用和 launch error 检查不统一。
- module-level `load()`/benchmark 可能产生 import 副作用。
- wheel、即时扩展和 standalone 路径的行为不能互换。

缓解优先级：统一契约文档和最小 smoke matrix，而不是先做大规模重构。
