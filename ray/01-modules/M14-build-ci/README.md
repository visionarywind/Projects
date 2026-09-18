# M14 构建、CI、Proto/Codegen 与测试

- 文档目的：说明 Bazel target graph、Python wheel、protobuf/codegen、CI 矩阵和测试边界。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：根 BUILD/WORKSPACE、setup.py、protobuf BUILD、CI/ci 目录已静态确认；上游构建和测试未运行。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：[实现](implementation.md)。

## 结论摘要

M14 不产生用户运行时状态，而是为 M01-M13 建立可重复构建、代码生成、打包和验证边界。Bazel 描述 C++/Python/protobuf targets，`python/setup.py` 组装 Python 包，`.buildkite`/`ci` 按平台、依赖和测试标签编排验证。

## 代表链

```text
WORKSPACE/toolchain/deps
→ BUILD target graph
→ protobuf/codegen
→ C++/extension compilation
→ Python package/wheel assembly
→ selected pytest/unit/integration targets
→ CI artifact/report/publish gate
```

## 分支与错误

Debug/release、CPU/GPU、Python 版本、平台、remote cache 和 optional dependencies 改变 target graph。依赖下载、编译、proto 不一致、ABI/extension load、测试超时和集群启动失败必须在 CI artifact 中保留，不可用“脚本存在”证明成功。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|M14|BUILD/WORKSPACE/setup/protobuf/CI|resolve→generate→compile→package→test|platform/debug/cache/deps|build/proto/ABI/test failure|sandbox/cache/artifact cleanup|targets/descriptors/wheel/reports|开发机/CI worker|代表文件已列|验证 D01-D03 的前置|未运行|

## 相关文档

[项目架构](../../00-overview/architecture.md) · [实现](implementation.md) · [开发指南](development-guide.md)

## 源码证据摘要

`BUILD.bazel`; `WORKSPACE`; `python/setup.py`; `src/ray/protobuf/BUILD.bazel`; `.buildkite/`; `ci/`。

## 未解决问题

完整 target graph、proto compatibility、wheel 组装、CI tag/release matrix 和平台构建结果需在上游开发环境验证；静态构建边界已建立。

## 下一步阅读建议

先读根 BUILD/WORKSPACE，再看 protobuf BUILD 和 Python packaging，最后按 `.buildkite/test.rules.txt` 追测试选择。
