# M01 GE 开发指南

## 修改前

1. 判断变更属于 Graph MetaDef、Parser、Compiler、Executor 还是 Runtime 边界。
2. 读取相关状态机、模型格式、公共头文件和 CMake target。
3. 列出 Graph/Model/Session/Stream/Event/Buffer 的所有权和异常清理路径。
4. 确定 V1、V2、静态 shape、动态 shape 和异步路径是否都受影响。

## 修改中

- 保持 Graph 语义与设备资源实现分离。
- V2 Load/Execute/UnLoad 做对称状态和资源处理。
- 不改变公共 Graph、Tensor、模型格式和 Executor ABI，除非完成兼容性评审。
- 错误路径保留 GE 状态、Runtime 原始错误和 Graph/Model 标识。

## 修改后

补充元数据、编译、执行、异常清理和跨模块测试；在匹配 Toolkit/Driver/Firmware/NPU 上验证动态和异步行为，并记录未验证项。
