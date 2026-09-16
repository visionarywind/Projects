# HAL/M3D 适配：调用链

## 主链

```text
调用方
 -> Driver/API 或配置入口
 -> 对象解析/状态检查
 -> M3D device/queue/memory/cmd buffer
 -> MUresult/异步状态
```

## 相关证据

[src/hal/m3d/CMakeLists.txt:1-59] [src/musa/core/stream.cpp:927-1005]

## 链路缺口

若链路进入 `src/hal/m3d/m3d` 子模块，当前只确认构建和接口边界，未把私有子模块内部函数写成已确认事实。若涉及动态导出或回调，需同时检查 `mu_entry.cpp` 和生成脚本。
