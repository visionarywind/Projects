# M01 GE 示例与开发配方

- 证据状态：入口来自仓库文档；本环境未执行

## 构建

```bash
bash build.sh
bash build.sh --ge_compiler
bash build.sh --ge_executor
bash build.sh --dflow
```

GE 还提供 `--asan` 和 `--cov` 模式；这些命令是仓库提供的入口，并非本环境验证结果。

## 图执行排查

```text
GEInitialize
  -> ConstructSession
  -> AddGraph
  -> CompileGraph
  -> LoadGraph
  -> RunGraph / RunGraphAsync
  -> UnLoadGraph / RemoveGraph
  -> Session Finalize / GEFinalize
```

遇到 V2 失败时先确认 Executor 状态、IO 数量/地址、Stream 资源和模型版本，再向 Runtime/Driver 错误边界追踪。

## 代码修改

优先修改最接近职责的层；变更 Graph/Model/Executor 时同步更新 Session 生命周期、Runtime 资源清理和回归测试。
