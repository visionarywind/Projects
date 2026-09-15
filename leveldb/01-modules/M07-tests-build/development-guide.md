# M07 开发指南

- 文档目的：指导修改后更新测试、构建和 CI。
- 适用范围：M07。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：流程来自仓库配置和 README。
- 最后更新：2026-09-10
- 前置阅读：[M07 line-level-analysis](line-level-analysis.md)
- 后续阅读：[跨模块影响](../../90-cross-module/change-impact-map.md)
## 结论摘要

本页聚焦 01-modules/M07-tests-build/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 修改源码

1. 找到最小模块测试；先补回归，再修改实现。
2. 若涉及公共头文件、ABI、持久化格式或平台接口，扩大到重开/安装/跨平台验证。
3. 保持 C++17、无 exceptions/RTTI 和现有 clang-format 风格。
4. 更新 CMake 目标只在确有必要时进行；项目 README 明确对构建配置变更持保守态度。[README.md:111-115](../../../source/leveldb/README.md#L111-L115)

## 建议命令

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

CI 使用 `ctest -C <config> --verbose`，Windows/多配置生成器应带 configuration。[.github/workflows/build.yml:80-87](../../../source/leveldb/.github/workflows/build.yml#L80-L87)

## 性能变更

固定 benchmark 参数和数据库规模，至少比较写入、随机读、顺序读、反向读、compact 和压缩；不要把 README 中 2011 年的性能数字当作当前硬件基线。[README.md:129-139](../../../source/leveldb/README.md#L129-L139)

## 发布/安装

检查 `cmake --build build --target install`、导出 target、公共头文件和库版本；安装成功不代表 API/数据格式兼容已证明。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
