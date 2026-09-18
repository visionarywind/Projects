# 构建、CI、proto/codegen 与测试：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`BUILD.bazel`、`WORKSPACE`、`python/setup.py`、`src/ray/protobuf/BUILD.bazel`、`.buildkite/`、`ci/`|公共/模块入口|已确认|
|实现|Bazel 描述 targets，setup.py 打包，proto 生成类型，CI 编排测试矩阵。|真实状态改变|已确认|
|测试|CI/Bazel targets（未运行）|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
