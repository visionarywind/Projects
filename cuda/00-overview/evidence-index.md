# CUDA 源码证据索引

本索引列出当前知识库反复使用的关键源码证据。路径均相对于 `/home/mtuser/workspace/cuda`；“静态确认”表示已阅读源码文本，不表示构建或运行成功。

## 版本、入口与构建

- `[inc/cuda.h:76-170]`：API 版本、旧名到 v2、PTDS/PTSZ 宏。
- `[inc/cuda.h:209-211]`：`CUDA_VERSION 10020`。
- `[common/version.h:16-28]`：内部/NVCUDA 版本字段。
- `[src/api/apiinit.c:19-47]`：`cuapiInit` flags 校验与 `cuiInit` 入口。
- `[cuda.nvmk:348-376,380-492]`：API/CUI 源文件清单。
- `[cuda.nvmk:513-597]`：架构 HAL 条件源文件。
- `[cuda.nvmk:1695-1704,1713-1817]`：导出与 Linux/Windows 目标规则。

## Runtime、Context、Device

- `[src/cui/cuiinit.c:2814-2870]`：全局 mutex 一次性初始化、fork-child 重置。
- `[src/cui/cuiinit.c:2910-3039]`：初始化检查、TLS current context、sticky error。
- `[src/cui/cuiinit.c:3060-3208]`：内部初始化顺序和失败回滚。
- `[src/cui/cuiinit.c:3225-3313]`：`cuiInit` 锁、早退、工具/atfork 顺序。
- `[src/api/apictx.c:38-95]`：context create wrapper、TLS push 和失败销毁。
- `[src/cui/devmgr.c:39-121,175-199,201-277]`：设备 remap、过滤、排序。
- `[src/cui/hal/hal.c:8-79,82-157]`：架构 HAL 编译过滤和运行时选择。
- `[src/cui/channel.c:43-173]`：DMAL channel 初始化、资源分配和回滚。

## Memory、Stream、Launch

- `[src/api/apimem.c:52-118]`：device allocation wrapper、memdesc、登记和回滚。
- `[src/api/apimem.c:231-339]`：device free 查找、base pointer、同步释放。
- `[src/api/apimem.c:352-414]`：host allocation 参数与 descriptor。
- `[src/cui/memobj.c:82-110,113-163,210-280]`：子分配条件、兼容性和 backing。
- `[src/api/apistream.c:43-109,113-136]`：stream flags、创建和失败 detach。
- `[src/cui/cuistream.c:1741-1877]`：stream 资源创建。
- `[src/cui/cuistream.c:1897-1954,2004-2088]`：detach、marker reclaim、free pool。
- `[src/cui/cuistream.c:1956-1986]`：QMD 分配和轮换。
- `[src/api/apilaunch.c:124-209,212-301]`：参数验证、capture/normal launch 分支。
- `[src/cui/cuilaunch.c:118-218]`：launch memory tracking 和 syscall imports。
- `[src/cui/cuilaunch.c:229-338]`：syscall setup、HAL check、constant bank、ABI 编码。
- `[src/cui/cuilaunch.c:419-503]`：launch begin、工具 skip 和状态。
- `[src/cui/cuilaunch.c:582-817]`：stream push、HAL finalize/control、tracking、结束/回滚。
- `[src/cui/cuilaunch.c:1195-1211]`：packed parameter finalize。
- `[src/cui/cuimod.c:919-1003]`：内部 module/link、外部 text relocation、bulk download 和失败卸载。
- `[src/cui/cuimod.c:1835-1891,2363-2548,2767-2943]`：ELF shared image、JIT/module load transaction 和 unload cleanup。
- `[src/cui/cuifunc.c:2501-2637,2803-2985]`：function resource merge、参数 packing/validation。
- `[src/cui/cuijitlink.c:79-142,219-255,360-402]`：JIT link state、输入副本、finish/destroy。
- `[src/cui/cuigraph.c:3304-3492,4056-4162]`：graph instantiate 多 context 锁、QMD 注册、launch/marker/UVM rollback。
- `[src/cui/cuiuvm.c:1252-1328,2313-2428,3950-4235]`：UVM submemblock/managed registration、AL 选择、Kd8 特殊映射。
- `[src/cui/peermap.c:12-134]`：P2P access 按类型 refcount 与零计数清理。
- `[src/cui/gpfifo.c:313-416,437-490,716-796]`：GPFIFO entry 编码、tracking reclaim、flush unit 与 DMAL advance 边界。
- `[src/cui/pushbuffer.c:22-99,181-235]`：pushbuffer put/get、对齐和环形空间回收。
- `[src/cui/marker.c:655-813,1008-1140]`：marker 合并、异构完成源、flush/wait/blocking path。
- `[src/cui/cuisyscall.c:124-205,1157-1227,1233-1322]`：syscall asm/trampoline、kernel callback、module import refcount 与 trap activation。
- `[cudaSyscalls.nvmk:46-71,111-160]`：按 `DEV_ARCHS` 编译并通过 bin2c 生成 CUDA/OpenCL per-arch syscall cubin arrays。

## Tools、Debug、OpenCL

- `[src/etbl/tools/tools_callbacks_internal.h:22-41,45-219]`：callback enable table 和内存描述转换。
- `[src/devtools/debugger/cudbgapi.c:401-557]`：debugger context map 与 API 检查依赖闭包。
- `[src/devtools/debugger/cudbgdriver.c:103-210]`：debugger IPC/RPC/launch blocking 等共享状态。
- `[src/devtools/memcheck/memcheck.c:120-180,206-256]`：memcheck device 表分配、下载和错误路径。
- `[src/devtools/debugger/cudbgapi.c:401-557]`：debugger module/context/function/grid 状态检查和 TLS callback inactive 闸门。
- `[src/devtools/debugger/cudbgdriver.c:103-210]`：debugger attach/session、launch blocking、integrated memcheck 和 preemption shared state。
- `[src/profiler/profiler_common.c:21-56,85-96]`：profiler mode 锁、兼容检查和环境变量。
- `[src/cui/cuilaunch.c:635-710]`：launch push 中 wait-for-idle、perfmon 和 counter trigger。
- `[src/icd_rel/icd.c:43-227]`：ICD vendor 动态加载与 platform 注册。
- `[src/icd_rel/icd_dispatch.c:43-247]`：ICD public entry validation/dispatch。
- `[src/cl/cliglobals.c:12-139]`：OpenCL 全局配置、初始化和销毁。
- `[src/cl/cliicd.c:6-14,24-144,146-235]`：固定 256-entry dispatch table。
- `[src/cl/cliobject.h:98-169]`：public object 首字段、ICD dispatch、public/internal refcount、parent/children 树和 destroy contract。
- `[src/cl/clcommandqueue.c:15-76,189-215]`：queue parent、CUI queue 创建失败回滚和 public retain/release。
- `[src/cl/clicontext.c:467-545]`：worker/callback thread、task graph、pinned memory、CUctx 与 GL TLS 的 context destroy 顺序。
- `[src/cl/clevent.c:106-217]`：event submitted 等待、按 device marker 聚合、flush/wait 和 pinned memcpy 清理。
- `[src/cl/clgl.c:93-204]`：GL mem object/resource registration、image/size 查询和错误 unregister。
- `[src/cui/cuiextinterop.c:21-171]`：external handle open、semaphore/buffer memobj backing 及错误 free。

## Tests、实验与聚合

- `[tests/cuda_test/cuda_test.nvmk:10-56,60-90,93-120,140-153,166-170]`：测试 binary、device-link、case 注册、dispatcher 生成和 fatbinary 宏规避。
- `[tests/cuda_test/dispatch_main.cpp:22-50]`：GPU 列表、过滤显示、list/run 入口。
- `[tests/run_tests.py:8-73,85-132,171-189,191-245]`：DVS 解析、双 pipe 消费、timeout、无结果失败和 Linux GPU 前置动作。
- `[tests/dvs-build-unix.sh:1-43]`：Linux nvmake/DVS 打包入口。
- `[tests/cuda_test/basic_sanity.cu:49-100,127-145,133-163,169-237]`：初始化、设备、module/function、kernel/CNP sanity。
- `[tests/cuda_test/stream/unittest_stream_push.cpp:30-196,198-259]`：push 状态和 channel 关联。
- `[tests/cuda_test/memcpy/unittest_memcpy.cpp:43-85,87-170]`：memcpy/P2P/多 context。

## 证据使用规则

- 读到的源码只能证明静态控制流、字段和分支；不能证明 GPU 行为、硬件兼容性或最终生成物正确。
- 生成的 `.c`、`.bin`、`.cubin`、`.exe`、SASS 和历史实验资料用于边界或回归线索，不替代手写源码证据。
- 外部 common、resman/RM、compiler/gpgpucomp、firmware 和 nvmake 配置未完整纳入，涉及这些边界时使用“未知/未验证”。
