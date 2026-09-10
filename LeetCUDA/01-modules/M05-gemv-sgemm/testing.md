# M05 测试

参考 `torch.matmul`/cuBLAS，覆盖 M/N/K=0、1、非 tile 倍数、K 非 BK 倍数、不同 dtype 和随机输入；分别验证 naive、shared、thread-tile、double-buffer、WMMA/TF32 版本。检查最后一个 K tile 和输出写回边界，必要时运行 memcheck/racecheck。

benchmark 必须区分首次编译、warmup 和同步后的 kernel 时间；当前未执行。
