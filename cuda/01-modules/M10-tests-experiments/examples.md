# M10 代表性例子

- `basic_sanity:kernel`：贯穿 Runtime 设置、内部 context/function、kernel launch、stream push、CNP expected 和 synchronize。
- `stream/unittest_stream_push.cpp`：检查 begin/end push 状态和 channel 关联。
- `memcpy/`、`memmgr/`、`sync/`：分别覆盖内存、manager、marker/semaphore。
- `experiments/cnp/*.cubin`：历史生成产物，只能作为实验边界资料。
