# M01 测试

## 已确认路径

从 `notes-v2.cu` 的小尺寸 smoke/correctness 开始，再按 `build.sh --arch` 选择目标架构；动态 shared memory 先调用 `check_smem_feasible`。Interview 测试会显式分配、同步、比较并释放资源。[kernels/interview/notes-v2.cu:82-97,100-499]

## 必测边界

- 每个支持 SM 的编译目标。
- 动态 shared memory 超限。
- 输入尺寸不足一个 tile、非 tile 倍数。
- kernel launch/runtime error 和资源释放路径。

当前均未在本环境执行。
