# M04 代表性例子

- interior pointer 传给 `cuMemFree`：能找到对象但不等于 base pointer，因此返回 invalid value，保护 backing block。
- `cuMemFree(0)`：按 wrapper 语义成功返回，不进入对象释放。
- 固定 device VA 或 external sharing：禁止普通 suballocation，避免地址/所有权冲突。
