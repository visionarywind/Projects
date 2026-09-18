# Object Store 与 Object Manager：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → put/get → ObjectManager Pull/Push → Plasma/filesystem → chunk receive → available/free → 结果/事件
```

## 分支
本地 Plasma、spill、pull 合并、取消、节点移除。

## 异常
对象丢失、传输失败、ObjectStoreFull、owner/node failure。

## 终止
CancelPull、FreeObjects、Stop 终止传输并释放 buffers。
