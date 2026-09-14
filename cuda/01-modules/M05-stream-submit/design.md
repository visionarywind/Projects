# M05 设计

M05 把 stream 作为异步工作序列，把 channel 作为硬件提交端点，把 marker/semaphore 作为完成和依赖原语。stream public handle 的失效与内部对象回收分离：detach 只撤销用户入口，GPU completion 后才回收到 pool。

QMD、constant bank、pushbuffer 和 memory tracking 共同定义一个 launch 的异步所有权边界（静态确认：[src/cui/cuistream.c:1741-1877]、[src/cui/cuilaunch.c:519-540]）。
