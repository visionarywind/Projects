# M06 执行流程

```text
module image → ELF/fatbinary/JIT parse → CUmod → CUfunc metadata
→ API parameter validation/pack → CUIlaunchData
→ capture: graph node
→ normal: setup → QMD/const bank → stream push
```

launch setup 失败不应留下未跟踪的 syscall、const bank 或 UVM running 状态；已进入 push 后统一经 `streamEndPush`，最后发 launch-end callback。

## Graph exec

```text
capture → node/dependency graph → end validation
→ clone/flatten → per-context resource allocation
→ internal streams + completion markers + QMD/constant-bank
→ optional scheduler host/device backing
→ launch: API marker + UVM running + topological node push
→ completion markers
→ destroy: detach streams + release QMD/const/memobj/staging
```

失败路径需要同时覆盖 capture graph 销毁、instantiate 部分资源回滚、launch UVM rollback、临时 context lock 和 API stream 替换恢复；当前后两者存在静态疑点，尚未运行验证。
