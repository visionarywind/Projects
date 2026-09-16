# Stream 与 Command 调度：数据结构与所有权

## 结构关系

```text
Public handle -> Core object -> optional shared_ptr/unique_ptr -> HAL object
                                      └-> tracker/Context/Stream collection
```

## 所有权问题

- `Context`/`Device` 对资源集合承担登记和查找责任。
- `Memory` 同时保存 Context、HAL memory、物理/虚拟 tracker 关系。
- `Stream` 保存 queue、cmd pool、cmd buffer、semaphore 和命令列表。
- Command 常以 `shared_ptr` 在 waiting/inflight/list 之间转移，保证异步完成前对象存活。[src/musa/core/stream.cpp:1048-1129]

具体模块的字段布局应以对应头文件为准；本文不臆测未读字段。
