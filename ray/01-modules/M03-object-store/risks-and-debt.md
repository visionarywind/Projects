# Object Store 与 Object Manager：风险与技术债

## 风险
- 对象丢失、传输失败、ObjectStoreFull、owner/node failure。
- ObjectManager RPC、IO 与 Plasma 回调并发。
- 异步引用、资源或 actor 清理不对称可能造成泄漏/悬挂。

## 证据缺口
动态集群、跨节点故障、压力和性能尚未验证；测试边界：src/ray/object_manager/。

## 维护
修改时同步更新 source-map、call-chains、testing 和跨模块索引，并保留已确认/推断/未验证标签。
