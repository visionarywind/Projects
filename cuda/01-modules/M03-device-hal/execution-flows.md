# M03 执行流程

```text
物理枚举 → identity remap → predicate/filter → range/duplicate 校验 → PCI/速度排序 → visible ordinal
→ ccArch → InitDeviceArch → 架构 HAL function table
→ channelInit → DMAL backend Init → manager 注册
```

任一架构实现未由 `NVCFG(GLOBAL_ARCH_*)` 编译进来时，对应 case 不存在；未知 `ccArch` 返回失败。channel backend 失败时释放 semaphores、调用 Deinit 并销毁 condition variable。
