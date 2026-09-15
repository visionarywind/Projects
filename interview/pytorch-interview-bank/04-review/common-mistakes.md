# 常见误区

- 把 eval() 当成 no_grad()；模块状态和 autograd 状态必须分开验证。
- 把 state_dict 能加载当成完整可恢复；继续训练还需要 optimizer、scheduler、RNG 和数据位置。
- 只看平均吞吐，不看 P95/P99、首轮编译、通信和 GPU idle。
- 只在单卡/固定 shape 上验证，就声称分布式、动态输入或跨版本兼容。
- 把未运行实验、推断和建议写成已确认事实。
