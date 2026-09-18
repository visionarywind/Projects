# RLlib：测试边界

|路径|覆盖目标|
|---|---|
|正常|config → Algorithm.setup → sample → Learner.update → step result/checkpoint|
|分支|新旧 API stack、multi-agent、evaluation、offline、device。|
|异常|env/model/learner/worker/config failure。|
|清理|Algorithm.cleanup 与 worker/learner shutdown。|

测试位置：rllib/tests/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
