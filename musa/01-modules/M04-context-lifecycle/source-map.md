# Context 与资源生命周期：源码地图

| 层 | 路径 | 关注点 |
|---|---|---|
| 配置/入口 | `src/musa/core/context.cpp、device.cpp` | 对外契约、选项、入口函数 |
| Core | `src/musa/core` | 对象、状态、生命周期 |
| Command/Graph | `src/musa/core/command`、`node`、`graph` | 异步工作表示 |
| HAL | `src/hal/hal*.h`、`src/hal/m3d` | 资源映射和硬件调用 |
| 观测 | `src/driver/mupti`、`mugdb`、`muasan` | trace/accessor/debug |

## 证据锚点

[src/musa/core/context.cpp:41-475] [src/musa/core/context.cpp:1875-2040]

## 搜索建议

```bash
grep -R "context" src/driver src/musa/core src/hal
```

上述命令是阅读建议，不代表已在本地或远端执行。
