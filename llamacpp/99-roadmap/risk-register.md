# 风险登记

| ID | 风险 | 触发条件 | 缓解/验证 | 状态 |
|---|---|---|---|---|
| R01 | 行号与源码漂移 | 更新 commit | 重新核对 evidence index | 已知 |
| R02 | GGUF metadata/tensor 不匹配 | 新模型转换或 loader 改动 | converter + dummy + 实际模型质量测试 | 已知 |
| R03 | backend/硬件差异 | 改 ggml 或 device split | `test-backend-ops` + 目标硬件 CI | 已知 |
| R04 | context/KV 约束导致失败 | cache type、flash-attn、split 组合改变 | 检查 `llama_init_from_model` 错误分支 | 已知 |
| R05 | server queue/thread 回归 | handler、slot、response 改动 | 非流式/流式/断开/并行 pytest | 已知 |
| R06 | 默认文件暴露 | 新增 MCP/model save/load 能力 | 保持默认关闭并审查权限 | 已知 |
| R07 | UI/server API 不一致 | endpoint/schema 改动 | UI check/e2e + server tests | 已知 |
| R08 | 未验证文档被误读为事实 | 没有执行模型 Demo | 明确“未验证”并记录输出 | 当前存在 |
| R09 | stream ring 丢失旧 offset | 大响应或晚重连 | 检查 4 MiB/TTL/offset 错误路径 | 已知 |
| R10 | 重 post-processing 阻塞推理 | server context 线程新增工作 | 将 JSON/template 留在 HTTP 层 | 已知 |

---

**页尾：** 风险严重度尚未按生产部署环境重新排序。
