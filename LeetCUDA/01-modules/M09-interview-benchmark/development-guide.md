# M09 开发指南

新增 phase 或 kernel 时更新 include、CLI/help、静态 globals、build architecture guards、test/bench dispatch、清理路径和 README。保持 `check()` 覆盖 CUDA API 与同步边界；不要只在一个 SM 编译通过就宣称全架构支持。
