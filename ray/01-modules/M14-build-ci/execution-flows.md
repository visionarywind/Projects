# 构建、CI、proto/codegen 与测试：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → workspace/toolchain → proto generation → C++/extension → wheel → test target → 结果/事件
```

## 分支
debug/release、平台/Python/GPU、cache、optional deps。

## 异常
依赖/编译/proto/ABI/test timeout failure。

## 终止
sandbox/cache/artifacts 由 job 生命周期清理。
