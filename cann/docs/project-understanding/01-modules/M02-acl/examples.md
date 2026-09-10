# M02 ACL 示例与配方

- 证据状态：命令来自仓库文档；本环境未执行

## 构建

```bash
bash build.sh --ascend_install_path=${ASCEND_INSTALL_PATH} --ascend_3rd_lib_path=${THIRD_LIB_PATH}
```

## UT

```bash
bash tests/run_test.sh --ut --ascend_install_path=${ASCEND_INSTALL_PATH} --ascend_3rd_lib_path=${THIRD_LIB_PATH}
```

仓库文档说明 UT 不支持 root；上述命令仅为入口，不是本次环境的验证结果。

## 最小资源顺序

```text
aclInit
  -> aclrtSetDevice
  -> create Context/Stream/Buffer/Model
  -> execute
  -> synchronize / wait callback
  -> destroy Model/Buffer/Stream/Context
  -> aclrtResetDevice
  -> aclFinalize
```

真实模型参数、输入 shape 和输出布局必须以模型生成信息为准。
