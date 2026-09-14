# M05 执行流程

```text
stream create → pool/QMD/semaphore/UVM register → public handle
work → channel begin push → pushbuffer methods/GPFIFO → marker/tracking
stream destroy → detach → marker status → reclaim → FREE pool
```

channel 建立必须先完成 DMAL backend，再分配 tracking/cross-engine semaphores 和 manager entry；失败执行反向清理。push 与 context/manager lock 受 MPS submission lock 影响。
