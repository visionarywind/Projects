# HAL/M3D 适配：源码地图

| 层 | 路径 | 关注点 |
|---|---|---|
| 配置/入口 | `src/hal/hal*.h、src/hal/m3d/*` | 对外契约、选项、入口函数 |
| Core | `src/musa/core` | 对象、状态、生命周期 |
| Command/Graph | `src/musa/core/command`、`node`、`graph` | 异步工作表示 |
| HAL | `src/hal/hal*.h`、`src/hal/m3d` | 资源映射和硬件调用 |
| 观测 | `src/driver/mupti`、`mugdb`、`muasan` | trace/accessor/debug |

## 证据锚点

[src/hal/m3d/CMakeLists.txt:1-59] [src/musa/core/stream.cpp:927-1005]

## 搜索建议

```bash
grep -R "hal" src/driver src/musa/core src/hal
```

上述命令是阅读建议，不代表已在本地或远端执行。
