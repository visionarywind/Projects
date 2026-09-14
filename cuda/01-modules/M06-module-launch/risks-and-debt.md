# M06 风险与技术债

| 风险 | 证据 | 影响 |
|---|---|---|
| 参数 metadata 缺失 | `[src/api/apilaunch.c:141-147]` | 现代 kernelParams 被拒绝 |
| packed/unpacked 双路径 | `[src/api/apilaunch.c:155-205]` | 兼容复制和生命周期复杂 |
| capture 失败使 graph invalidated | `[src/api/apilaunch.c:270-278]` | 后续 API 需观察 capture invalidated |
| launch 异步引用 | `[src/cui/cuilaunch.c:163-218]` | function/module/memobj 不能过早释放 |
| module unload 反向清理 | shared ELF、function、syscall、UVM、CNP cache 和 tools 状态需按顺序撤销 | 部分失败/重复 unload 的观察点跨多个所有权域 |
| graph 动态 context | launch stream 可能不在 instantiate context 集合，且 API stream 指针临时替换 | 锁数组、marker 和 UVM rollback 必须保持一致 |
| instantiate 锁数组错误回收 | `ctxLocks` 写入 exec 后，QMD pool 注册失败仍进入 Error；Error 先 free 数组再 destroy graph | 静态上可能出现释放后再次 lock/unlock；需注入失败并用 ASan 验证（[src/cui/cuigraph.c:3457-3492,1144-1187]） |
| launch 失败未恢复 API stream | `replaceApiStream` 只在成功提交后恢复；node launch 失败直接跳到 Done | graph exec 可能留下临时 stream 指针，影响后续 launch；需构造失败节点验证（[src/cui/cuigraph.c:4116-4157]） |
| scheduler backing 生命周期 | device graph nodes/indexes 是 `CUmemobj` + host backing，依赖 stream tracking 和 context lock | 过早释放会导致设备读取悬挂；destroy 必须走 memobj 同步边界（[src/cui/cuigraph.c:1898-1932,1035-1064]） |

**深度状态**：API launch、参数、tracking、setup、module/ELF/JIT、graph capture/instantiate/launch/unload/update 和资源 reverse path 已完成静态主线；架构 QMD/ABI 具体字段、compiler/gpgpucomp、设备执行和上述失败风险复现仍未闭合。
