# Object Store 与 Object Manager：实现

## 入口到副作用
ObjectManager 启动 RPC，按 Plasma 或 filesystem source 发送对象块，接收端完成后通知等待者。

## 正常
put/get → ObjectManager Pull/Push → Plasma/filesystem → chunk receive → available/free

## 分支与异常
- 本地 Plasma、spill、pull 合并、取消、节点移除。
- 对象丢失、传输失败、ObjectStoreFull、owner/node failure。

## 清理
CancelPull、FreeObjects、Stop 终止传输并释放 buffers。

## 数据与执行上下文
- ObjectID、buffer、chunk、spill URI、ownership。
- ObjectManager RPC、IO 与 Plasma 回调并发。
