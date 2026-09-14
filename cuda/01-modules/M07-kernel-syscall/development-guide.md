# M07 开发指南

1. 新 syscall 同步修改 CUI callback、imports bitset、mem tracking、架构实现和生成规则。
2. 修改 CNP 时检查 child queue、父 launch 资源、stream/channel、错误传播和 context destroy。
3. 对每个目标架构分别记录 SASS/QMD/constant-bank/寄存器布局，不能假定 binary compatibility。
4. 生成物提交必须注明输入、生成器、目标架构和产物，不以 cubin 替代源代码。
5. 外部 compiler/GPU/firmware 未提供，构建和运行保持未验证。
