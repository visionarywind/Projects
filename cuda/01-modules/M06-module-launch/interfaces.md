# M06 接口契约

- `cuiFuncValidateParams`：依赖 function metadata，输出合法 packed size/参数状态。
- `cuiLaunchSetup_common`：输入 context/function/stream/QMD/const banks，配置 syscall、HAL 和 ABI staging。
- `cuiLaunch`：要求 context lock；要求 begin push 与 end push 配对。
- `cuiGraphCreateKernelNode`：capture 期间取得已验证参数与依赖；更新失败负责 node 回滚。
- `cuiLaunchFinalizeParams`：仅在未 packed 且有 packed size 时复制参数。
