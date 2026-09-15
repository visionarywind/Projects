# M03 Runtime 示例与构建

- 文档目的：解释 01-modules/M03-runtime/examples.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：命令来自仓库文档/AGENTS；本环境未执行
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-runtime/examples.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 构建

```bash
bash install_deps.sh
python3 download_3rd_party.py
bash build.sh
```

Runtime README/AGENTS 还提供测试和示例入口；实际依赖安装、工具链和设备环境需先确认。

## 调试一个设备选择失败

1. 在 `aclrtSetDeviceImpl` 或 `rtSetDevice` 记录 device ID 和返回码。
2. 继续到 `Api::SetDevice` 内部实现，确认 Context、设备节点和 Driver 错误。
3. 保存 Runtime 扩展错误、Driver 日志和 `npu-smi` 信息。
4. 分离“参数/状态错误”和“设备/固件错误”，不要盲目重试。

无硬件时只能确认编译和门面 UT，不能确认实际设备成功。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
