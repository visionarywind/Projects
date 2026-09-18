# 构建、CI、proto/codegen 与测试：行级分析

## 证据锚点
- `BUILD.bazel`、`WORKSPACE`、`python/setup.py`、`src/ray/protobuf/BUILD.bazel`、`.buildkite/`、`ci/`

## 结论
- 入口负责验证/归一化，副作用发生于：Bazel 描述 targets，setup.py 打包，proto 生成类型，CI 编排测试矩阵。
- 分支：debug/release、平台/Python/GPU、cache、optional deps。
- 错误：依赖/编译/proto/ABI/test timeout failure。

动态行为未运行，不写成已验证。
