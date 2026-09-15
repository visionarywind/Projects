# M01 开发指南

- 文档目的：指导新增/修改公共 API。
- 适用范围：M01。
- 源码版本：`main` / `7ee830d`。
- 证据状态：规则来自头文件、CMake 和 README。
- 最后更新：2026-09-10
- 前置阅读：[M01 line-level-analysis](line-level-analysis.md)
- 后续阅读：[跨模块影响](../../90-cross-module/change-impact-map.md)

## 修改步骤

1. 明确 API 是新增、语义修正还是 ABI/默认值变化。
2. 更新公开头文件和内部实现；如适用同步 C API (`include/leveldb/c.h`、`db/c.cc`)。
3. 写成功、错误、并发、生命周期和重开测试。
4. 检查 CMake 的公开安装文件清单；公共头文件必须仍能被安装使用。[CMakeLists.txt:193-210](../../../source/leveldb/CMakeLists.txt#L193-L210)
5. 检查 README/doc 示例和稳定 API 要求。[README.md:89-109](../../../source/leveldb/README.md#L89-L109)
6. 追踪对 M02、M03、M04/M05 和 M06 的影响。

## 契约检查表

- 返回的指针谁拥有？
- 返回 Slice 借用谁的内存？
- 是否允许多线程共享？
- 错误使用哪个 Status 类型？
- 是否改变序列化格式或默认配置？
- 旧客户端能否继续编译和运行？

## 不应做的事

不要把内部 `db/`、`table/` 头文件当作应用 ABI；不要为单个 API 引入异常/RTTI；不要在未覆盖平台和测试前改变稳定接口。
