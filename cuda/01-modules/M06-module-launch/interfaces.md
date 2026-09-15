# M06 接口契约

- `cuiFuncValidateParams`：依赖 function metadata，输出合法 packed size/参数状态。
- `cuiLaunchSetup_common`：输入 context/function/stream/QMD/const banks，配置 syscall、HAL 和 ABI staging。
- `cuiLaunch`：要求 context lock；要求 begin push 与 end push 配对。
- `cuiGraphCreateKernelNode`：capture 期间取得已验证参数与依赖；更新失败负责 node 回滚。
- `cuiGraphInstantiate`：成功后 graph exec 持有 per-context locks、QMD、constant-bank、internal streams/markers 和可选 scheduler backing；任一步失败必须销毁全部已分配资源。
- `cuiGraphLaunch`：要求 executable graph；通过 marker/UVM DAG/stream tracking 延长资源生命周期，失败必须恢复临时 graph state。
- `cuiGraphExecDestroy`：允许先前 launch 尚未完成，但资源回收必须经 marker、stream detach 和 `memobjFree` 的等待边界。
- `cuGraphExecUpdate`：只允许不改变拓扑和资源形状的兼容参数更新；function/context/memory type 等变化会被拒绝。
- `cuiLaunchFinalizeParams`：仅在未 packed 且有 packed size 时复制参数。
