# M02 ACL 设计

- 文档目的：解释 01-modules/M02-acl/design.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`50be4c922`
- 证据状态：包装层已确认；完整内部对象模型部分未知
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 定位

ACL 提供稳定的应用级接口，将用户参数转换成 Runtime 可接受的句柄、buffer、模型和执行请求。`acl/CMakeLists.txt` 显示其实现链接 Runtime、HAL、MetaDef 和 DFX 等能力 `[acl/CMakeLists.txt:110-126,167-190]`。

## 典型分层

```text
ACL public C API
  -> acl*Impl wrapper
  -> rt* Runtime API
  -> Runtime Api::Instance()
  -> Driver/HAL
```

设备 API 的实现直接调用 `rtSetDevice`，失败时映射错误并记录统计 `[acl/runtime/device.cpp:47-59]`。这表明 ACL 的设备层主要承担参数/日志/错误适配，而非实现设备调度。

## 状态与所有权

应用需在使用设备资源前完成 ACL 初始化和设备选择；Context、Stream、Event、DataBuffer、Model 等对象有各自创建和销毁 API。异步提交后，输入 buffer、输出 buffer、callback 和关联 stream 必须保持有效；这是调用者责任，精确 API 例外需查公共头文件。

## 设计取舍

统一应用接口降低上层接入成本，但跨 ACL/Runtime 的错误转换、句柄 ABI 和版本耦合增加排查成本。该代价是从 CMake 依赖和包装实现可确认的结果；更深层动机标为推断。

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
