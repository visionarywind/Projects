# M06 开发指南

新增 kernel 先接入现有 correctness 和 CLI tag，再加入 benchmark。同步更新 `setup.py` source/include/arch、makefile target、Python 参数、README API 和误差阈值。改变 layout 时画出 A/B/C 的 logical-to-shared/register 映射，并至少在一个旧架构保留回归。
