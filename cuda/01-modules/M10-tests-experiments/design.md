# M10 设计

M10 采用“测试 binary 与测试 case 分离、dispatcher 统一启动、runner 统一聚合”的设计。make-time 生成 test list，runtime 只执行生成的二进制；实验目录保留专项性能、bug review 和历史生成物，不与产品源码混淆。

静态测试证据证明代码意图、断言和覆盖关系；只有实际 GPU/DVS 执行才能证明运行结果。
