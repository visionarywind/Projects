# M09 开发指南

1. 新 OpenCL entry 同时更新 ICD fixed table、vendor dispatch、feature macro 和构建清单。
2. 修改 platform selection 保持 properties→device type→first vendor fallback 优先级。
3. 共享 GL/D3D/external memory 时同步 acquire/release、CUI memobj owner、stream/event ordering。
4. 修改 worker/DAG 时检查 callback thread、idle timeout 和 global deinitialize。
5. OpenCL 硬件、图形 API 和 external RM 均未在本工作区验证。
