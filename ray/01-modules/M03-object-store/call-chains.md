# Object Store 与 Object Manager：调用链

## 主链
```text
put/get → ObjectManager Pull/Push → Plasma/filesystem → chunk receive → available/free
```

## 实现落点
ObjectManager 启动 RPC，按 Plasma 或 filesystem source 发送对象块，接收端完成后通知等待者。

## 失败链
```text
依赖/输入失败 → 对象丢失、传输失败、ObjectStoreFull、owner/node failure。 → 重试、失败传播或清理
```
