# M10 测试

保留固定 6-box regression（重复 5 次）、随机 N=10/100/1024/4096/8192，seed=0/1/2，threshold=0.5/0.7，与 torchvision 逐元素比较；额外覆盖空输入、同分数 ties、阈值边界和大 N。[kernels/nms/nms.py:39-48,87-121]

GPU/benchmark 未在本环境执行。
