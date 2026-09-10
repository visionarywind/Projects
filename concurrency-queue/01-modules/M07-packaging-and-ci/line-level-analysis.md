# M07 构建与 CI 行级审计

- 文档目的：定位构建和持续集成的实际副作用。
- 证据状态：配置静态确认，运行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M07 README](README.md)
- 后续阅读：[CI 矩阵](ci-matrix.md)

| 入口 | 作用 | 证据 |
|---|---|---|
| `CMakeLists.txt` | interface target、安装头、导出 config | `CMakeLists.txt:1-16` |
| `build/makefile` | tests/benchmarks 默认和对象链接 | `build/makefile:28-50` |
| `tests/*/makefile` | run 包装目标 | 各测试目录 |
| `.github/workflows/ci.yml` | native/RISC-V 构建运行 | workflow 文件 |
| `concurrentqueueConfig.cmake.in` | 安装后 target include | config template |

## 修改影响

改变公共头文件需要同时考虑 CMake 安装和 legacy Make 编译；改变 C API `.cpp` 需要 Make 的对象链接和 C API unit tests；改变编译 flags 需要 native 与 RISC-V 条件宏复核。
