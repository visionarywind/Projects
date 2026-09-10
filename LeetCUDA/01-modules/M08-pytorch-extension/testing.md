# M08 测试

先 import/编译最小扩展，再做小 tensor 对拍；检查 dtype/device/shape/contiguous/空输入和错误信息；然后执行 warmup+同步 benchmark。wheel 路径额外验证安装后 import 和 ABI；动态路径额外清理 extension cache 后重建。当前无统一 CI，命令未执行。
