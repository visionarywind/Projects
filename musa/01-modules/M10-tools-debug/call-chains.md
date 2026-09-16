# 工具与调试接口：调用链

## 主链

```text
调用方
 -> Driver/API 或配置入口
 -> 对象解析/状态检查
 -> 输出/回调
 -> MUresult/异步状态
```

## 相关证据

[src/tools/CMakeLists.txt:10-18] [src/tools/muInfo.cpp:120-388] [src/driver/mu_entry.cpp:1085-1249]

## 链路缺口

若链路进入 `src/hal/m3d/m3d` 子模块，当前只确认构建和接口边界，未把私有子模块内部函数写成已确认事实。若涉及动态导出或回调，需同时检查 `mu_entry.cpp` 和生成脚本。
