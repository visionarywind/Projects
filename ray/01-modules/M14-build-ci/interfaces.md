# 构建、CI、proto/codegen 与测试：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`BUILD.bazel`、`WORKSPACE`、`python/setup.py`、`src/ray/protobuf/BUILD.bazel`、`.buildkite/`、`ci/`。

## 输出与副作用
- workspace/toolchain → proto generation → C++/extension → wheel → test target
- 核心数据：Bazel graph、proto descriptors、wheel metadata、artifacts。

## 失败
依赖/编译/proto/ABI/test timeout failure。

跨模块修改需同步 binding/proto/日志/metrics/测试。
