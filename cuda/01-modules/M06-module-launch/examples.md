# M06 代表性例子

### 普通 launch

`CUIlaunchData` 由 function 参数、grid/block/shared memory 填充，setup 分配当前 stream QMD/constant banks，之后通过 channel push。

### Capture

同一 `cuLaunchKernel` 在 capturing stream 上只创建 graph kernel node；graph validation 失败会使 capture invalidated 或回滚 node，而不是直接执行 GPU kernel。

### Graph exec 资源示例（静态推演）

包含两个同 context kernel node 的 graph 在 instantiate 时按两节点分配 QMD、parameter constant-bank node、HAL staging 和 completion marker；可复用同一 internal stream 的依赖链会继承 parent stream。若 conditional/device scheduler 被启用，还会创建 host graph/index backing 和一个 driver-owned device `CUmemobj`。重复 launch 复用这些已实例化资源，但每次仍重建异步 memory-tracking list；destroy 最后统一 detach stream 并释放 QMD/constant-bank/marker/backing（证据：[src/cui/cuigraph.c:1835-1933,2054-2141,3495-3575,1035-1205]）。
