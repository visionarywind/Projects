# D01 CUDA 测试主线

## 目的

以 `basic_sanity.cu` 串起 Runtime 设置、内部 context/function 查找、kernel launch、CNP expected、stream push 检查和结果校验。

## 静态步骤

1. 枚举可见 GPU 并建立 context（静态确认：[tests/cuda_test/basic_sanity.cu:49-100]）。
2. 从 module/function 结构找到 `gpuIncrement`，检查 launchable 属性（静态确认：[tests/cuda_test/basic_sanity.cu:127-163]）。
3. 使用 `<<<1,1>>>` 发起 kernel，按设备 CNP capability 计算 expected，并检查 launch/push 状态（静态确认：[tests/cuda_test/basic_sanity.cu:169-237]）。
4. 将结果与同步/资源状态关联；任何实际结果均需在有 GPU 的测试日志中确认。

## 状态

源码路径已静态阅读；nvcc、nvmake、cuda_test 和 GPU 均未运行。
