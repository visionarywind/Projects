# M09 接口

构建接口：`./build.sh --arch {sm_86|sm_89|sm_90a|sm_120a}` 和 `--clean`。[kernels/interview/build.sh:83-126]

运行接口由 `notes-v2.cu` CLI 和静态 benchmark globals 提供，包括 debug、HGEMM/FA/all benchmark、layout、尺寸、warmup/repeat 等。[kernels/interview/notes-v2.cu:33-79]

不同 phase 的参数和可用架构不是统一 ABI，必须按 CLI/help 和宏实际确认。
