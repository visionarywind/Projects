# D03：Graph 候选

候选入口：`tests/childGraph.cu`、`tests/conditionalNode.cu`、`tests/allocnode.cu`。预计覆盖 M04/M05/M06/M08/M09。当前未逐行读取和运行。

推荐追踪：`Stream::BeginCapture/EndCapture` 或 `muGraph*` -> Context 创建 node/graph exec -> Stream graph launch -> GraphCommand/底层队列。
