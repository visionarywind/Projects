# 构建、CI、proto/codegen 与测试：调用链

## 主链
```text
workspace/toolchain → proto generation → C++/extension → wheel → test target
```

## 实现落点
Bazel 描述 targets，setup.py 打包，proto 生成类型，CI 编排测试矩阵。

## 失败链
```text
依赖/输入失败 → 依赖/编译/proto/ABI/test timeout failure。 → 重试、失败传播或清理
```
