# M09 调用链

```text
build.sh -> nvcc compile notes-v2.cu -> link libraries
 -> binary parses CLI
 -> selected phase test/bench
 -> explicit allocation/H2D/kernel/sync/D2H/free
 -> check()/error or result/TFLOPS
```

代表资源路径见 `[kernels/interview/notes-v2.cu:100-499]`；编译/链接见 `[kernels/interview/build.sh:134-185]`。
