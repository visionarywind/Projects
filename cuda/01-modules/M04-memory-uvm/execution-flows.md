# M04 执行流程

## Allocate

```text
output=0 → current context → descriptor → memobjAlloc → global register → tools notify → pointer
```

## Free

```text
unified/device lookup → API source → base pointer → unregister → context synchronize → notify → freeNoSync
```

## Suballocation

`canSuballocate` 检查 noSuballoc、固定地址、mapping、全局禁用开关和 sharing；HAL 给出的 generic block size 决定是否切换 noSuballoc。证据：[src/cui/memobj.c:82-110,210-280]。
