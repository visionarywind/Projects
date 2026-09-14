# M06 代表性例子

### 普通 launch

`CUIlaunchData` 由 function 参数、grid/block/shared memory 填充，setup 分配当前 stream QMD/constant banks，之后通过 channel push。

### Capture

同一 `cuLaunchKernel` 在 capturing stream 上只创建 graph kernel node；graph validation 失败会使 capture invalidated 或回滚 node，而不是直接执行 GPU kernel。
