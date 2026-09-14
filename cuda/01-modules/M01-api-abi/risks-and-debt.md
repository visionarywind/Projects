# M01 风险与技术债

| 风险 | 影响 | 证据/状态 | 建议 |
|---|---|---|---|
| v1/v2 入口并存 | ABI 可链接但宽度/语义错误 | `[inc/cuda.h:76-170]` | 修改时成对核对 common 与导出 |
| 生成 API 链不完整 | 手工修改可能被生成物覆盖 | `[cuda.nvmk:348-376]`；工具未知 | 获取完整 nvmake 日志 |
| 句柄跨 context 使用 | `INVALID_HANDLE` 或状态破坏 | `[src/api/apilaunch.c:239-247]` | 保持统一 context 检查 |
| 失败回滚不对称 | 泄漏或悬挂 public handle | `[src/api/apistream.c:94-109]` | 为新资源补 detach/free 路径 |
| 导出表与实现漂移 | 加载时缺符号 | `[src/cuda_master.def:21-99]` | 做符号表静态比对 |

**深度状态**：入口、正常/错误 wrapper 路径已完成；生成器内部、跨平台链接细节和 ABI 二进制验证未完成。
