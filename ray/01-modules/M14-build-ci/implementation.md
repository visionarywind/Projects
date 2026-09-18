# 构建、CI、proto/codegen 与测试：实现

## 入口到副作用
Bazel 描述 targets，setup.py 打包，proto 生成类型，CI 编排测试矩阵。

## 正常
workspace/toolchain → proto generation → C++/extension → wheel → test target

## 分支与异常
- debug/release、平台/Python/GPU、cache、optional deps。
- 依赖/编译/proto/ABI/test timeout failure。

## 清理
sandbox/cache/artifacts 由 job 生命周期清理。

## 数据与执行上下文
- Bazel graph、proto descriptors、wheel metadata、artifacts。
- 开发机或 CI worker 的编译/测试进程。
