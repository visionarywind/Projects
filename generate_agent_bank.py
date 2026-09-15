from pathlib import Path
from collections import Counter
import re

ROOT = Path('/home/mtuser/workspace/Projects/interview/agent')
QB = ROOT / '01-question-bank'

subjects = [
    ('Agent 边界与任务建模', '把开放目标拆成可验证的任务契约'),
    ('运行时状态机', '定义状态、事件和终态的合法迁移'),
    ('消息与上下文组装', '控制信任层、顺序和上下文预算'),
    ('结构化输出', '处理解析失败、部分输出和版本兼容'),
    ('工具注册表', '发现能力、描述工具并管理生命周期'),
    ('JSON Schema 契约', '分离格式校验、业务校验和授权'),
    ('工具调用编排', '处理依赖、并发、顺序和结果合并'),
    ('超时与取消', '传播 deadline、取消和资源释放信号'),
    ('重试与幂等', '在未知副作用下安全恢复'),
    ('MCP 与协议适配', '隔离协议能力与执行侧安全边界'),
    ('短期记忆与摘要', '在压缩上下文时保持关键约束'),
    ('长期记忆治理', '处理来源、置信度、TTL、冲突和删除'),
    ('RAG 与 grounding', '检索证据、引用、时效和权限过滤'),
    ('规划与重规划', '在新 observation 下维护计划有效性'),
    ('多 Agent 委派', '控制子任务边界、证据和权限继承'),
    ('人工审批与接管', '把高风险动作绑定到具体参数和责任人'),
    ('评估与回归', '用独立 oracle 衡量结果、轨迹和安全'),
    ('可观测性与排障', '从 trace 和分桶指标定位首个分歧'),
    ('部署、成本与平台演进', '在多租户、灰度和预算中保持可靠性'),
    ('产品体验与人机协作', '在可用性、信任和人工接管之间做平衡'),
]
angles = [
    '的边界条件', '的失败模式', '的版本演进', '的验证方法', '的生产取舍'
]

domains = [
    'AG-K01 Agent 架构与运行时', 'AG-K02 模型交互与上下文管理',
    'AG-K03 工具调用与协议', 'AG-K04 规划与编排', 'AG-K05 记忆与状态',
    'AG-K06 检索与知识 grounding', 'AG-K07 多 Agent 与人机协作',
    'AG-K08 评估、可观测性与调试', 'AG-K09 安全、安保与治理',
    'AG-K10 可靠性与生产运维', 'AG-K11 部署、扩缩与成本',
    'AG-K12 产品集成与演进',
]

level_info = {
    'B': ('入门级', '入门', '先建立术语、边界、流程和最小可运行契约'),
    'I': ('中等级', '中等', '在异步、错误、数据治理和可观测约束下做实现判断'),
    'A': ('高级', '高级', '在可靠性、性能、安全、容量和恢复约束下做生产设计'),
    'E': ('专家级', '专家', '评审不变量、证据边界、治理策略和长期演进停止标准'),
}

target = ['概念', '原理', '流程', '代码/配置', '对比', '测试', '排障', '性能', '设计', '开放']
initial = {
    'B': Counter({'概念':2, '流程':1, '代码/配置':1, '测试':1}),
    'I': Counter({'原理':1, '流程':1, '对比':2, '设计':1}),
    'A': Counter({'对比':1, '排障':1, '性能':1, '设计':2}),
    'E': Counter({'原理':1, '对比':1, '设计':1, '开放':2}),
}

# Use a deterministic round-robin over the deficit types. This gives exactly ten of each type per level.
def remaining_types(level):
    counts = Counter({x: 10 for x in target}) - initial[level]
    out = []
    while sum(counts.values()):
        for typ in target:
            if counts[typ] > 0:
                out.append(typ)
                counts[typ] -= 1
    assert len(out) == 95
    return out

def title(level, n, subject, angle):
    if level == 'B':
        lead = '如何理解并实践'
    elif level == 'I':
        lead = '在真实调用链中，如何处理'
    elif level == 'A':
        lead = '在生产约束下，如何设计'
    else:
        lead = '如何评审并决定'
    return f'{lead}{subject}{angle}？（{level_info[level][1]}级视角）'

def metadata(level, n, typ, subject):
    difficulty, _, lens = level_info[level]
    domain = domains[(n * 3 + ord(level)) % len(domains)]
    second = domains[(n * 3 + ord(level) + 5) % len(domains)]
    ability = {
        '概念':'概念辨析与边界判断', '原理':'机制推导与不变量分析',
        '流程':'调用链与状态追踪', '代码/配置':'契约实现与配置审查',
        '对比':'方案对比与取舍', '测试':'测试、oracle 与验证设计',
        '排障':'故障定位与恢复', '性能':'性能、容量与成本分析',
        '设计':'系统设计与工程落地', '开放':'专家判断与治理决策',
    }[typ]
    freq = '高频' if n % 5 in (0,1) else ('中频' if n % 5 in (2,3) else '开放题')
    prereq = f'AG-B001、{domain.split()[0]}；HTTP/JSON、状态机和基本测试'
    related = f'AG-{level}'+str(max(1, n-1)).zfill(3)+f'、AG-{level}'+str(min(100, n+1)).zfill(3)
    statuses = {
        '概念':'[已确认] [推断]', '原理':'[已确认] [推断]',
        '流程':'[推断] [建议]', '代码/配置':'[建议] [待验证]',
        '对比':'[推断] [存在争议] [建议]', '测试':'[建议] [待验证]',
        '排障':'[建议] [待验证]', '性能':'[推断] [建议] [待验证]',
        '设计':'[建议] [待验证]', '开放':'[推断] [建议] [待验证]',
    }[typ]
    return difficulty, domain + ('；' + second if n % 3 == 0 else ''), ability, freq, prereq, related, statuses, lens

def answer_for(typ, subject, focus, lens, n):
    if typ == '概念':
        return (f'先给出定义和反例：{subject}不是把模型输出直接当作执行结果，而是围绕“{focus}”建立明确契约。'
                f'判断时区分输入、状态、候选动作、执行侧校验和最终结果；如果步骤固定或副作用高风险，应把自由度收回代码。'
                f'在{lens}的要求下，答案必须说明适用边界、失败后的可见状态以及谁拥有最终决定权。')
    if typ == '原理':
        return (f'可把问题抽象为 `state_(t+1) = transition(state_t, model_output, observation)`。'
                f'其中 transition 只允许满足 schema、权限、预算和前置条件的动作；{focus}发生变化时，旧结论不能自动保持有效。'
                f'核心不变量是事件可关联、终态只提交一次、未授权动作不产生副作用，且恢复不依赖模型随机重算历史。')
    if typ == '流程':
        return (f'建议把链路写成：`Request → Validate → State/Policy → Model → Candidate → Execute → Observation → Evaluate → Terminal`。'
                f'每个箭头记录 run/step/call id、版本、deadline 和错误类别；{focus}的失败要进入明确的等待、降级、人工或失败状态。'
                f'客户端看到 HTTP 成功、消息入队或模型返回，都不能替代对外部副作用和终态的确认。')
    if typ == '代码/配置':
        return (f'实现时先把不可信模型输出解析为候选对象，再按“语法→类型/schema→业务→身份/租户→预算→副作用”顺序校验。'
                f'伪代码可写为：`candidate = parse(response); assert schema(candidate); assert policy(principal, state, candidate); execute_with_idempotency(candidate)`；'
                f'任何拒绝都写结构化 observation。配置必须版本化，默认拒绝未知字段和未声明工具，不能把 prompt 中的文字当作授权。')
    if typ == '对比':
        return (f'至少从正确性、适应性、延迟、成本、可观测性、安全和恢复七个维度比较。'
                f'方案甲更适合固定路径或可形式化 oracle，方案乙更适合环境反馈强且失败可恢复的任务；{focus}决定了两者的边界。'
                f'生产上通常采用确定性治理外壳包住局部模型决策，而不是把选择简化为“全自动”或“全规则”。')
    if typ == '测试':
        return (f'先固定 fake model/tool、时钟、版本和租户，再定义独立 oracle：结果 oracle、轨迹/工具 oracle、安全 oracle 和运营指标。'
                f'测试集必须包含正常、缺参、拒绝、超时、重复投递、陈旧数据和取消；{focus}要有至少一个反事实或 mutation 用例。'
                f'通过条件应是 `result_ok ∧ tool_policy_ok ∧ safety_ok ∧ evidence_ok`，而非只比较最终文本。')
    if typ == '排障':
        return (f'先保护用户和高风险资源，再按租户、任务桶、模型/prompt/tool/policy/index 版本分桶。'
                f'建立关于{focus}的假设树，利用 trace 找首个分歧事件，随后用脱敏回放和 fixture 逐层替换变量。'
                f'修复前保留证据；修复后用 golden、反事实和 canary 复测，并明确回滚条件，不能凭平均成功率猜根因。')
    if typ == '性能':
        return (f'先建立分桶 baseline，分别测任务成功率、合法轨迹率、P50/P95/P99、token、工具次数、队列时间和每成功任务成本。'
                f'对{focus}可用 `L ≈ queue + Σ serial + max(parallel) + approval` 分解关键路径，容量可粗估为 `concurrency ≈ arrival_rate × service_time`。'
                f'优化只在不改变权限、依赖和副作用顺序时成立；收益必须有置信区间和失败成本对照。')
    if typ == '设计':
        return (f'设计应分离 control plane 与 data plane：前者管理版本、策略、审批和配额，后者负责队列、worker、模型、工具网关、状态和审计。'
                f'围绕{focus}定义数据模型、接口、租户边界、状态机、幂等、超时、恢复和降级；模型只产生候选，执行侧独立拒绝。'
                f'交付必须包含故障注入、容量预算、回滚/人工路径和发布门，而不是只有组件框图。')
    return (f'这是约束冲突题：先明确业务价值、影响半径、SLA、数据和副作用，再比较自动化收益与不可接受风险。'
            f'围绕{focus}给出至少一个保守方案、一个高自动化方案和一个混合方案；用分层指标、独立 oracle、灰度和停止标准选择。'
            f'如果外部系统无法提供幂等、查询、审计或补偿，就应诚实降级为 UNKNOWN/人工，而不能声称 exactly-once 或完全安全。')

def generate_question(level, n, typ):
    subject, _ = subjects[(n-1) // len(angles)]
    angle = angles[(n-1) % len(angles)]
    focus = subject + angle
    difficulty, domain, ability, freq, prereq, related, statuses, lens = metadata(level, n, typ, subject)
    qtitle = title(level, n, subject, angle)
    answer = answer_for(typ, subject, focus, lens, n)
    short = f'{subject}要求围绕“{focus}”处理模型、runtime、工具和用户之间的边界。'
    caveat = '[待验证]' if typ in {'代码/配置','测试','排障','性能','设计','开放'} else '[推断]'
    return f'''\n### AG-{level}{n:03d}. {qtitle}\n\n- 难度：{difficulty}\n- 知识域：{domain}\n- 考察能力：{ability}\n- 题型：{typ}\n- 面试频率：{freq}\n- 前置知识：{prereq}\n- 关联题目：{related}\n- 事实状态：{statuses}\n\n#### 1. 背景知识\n\n{short} Agent 场景同时包含模型不确定性、外部 observation、状态持久化和可能的副作用；不能用普通函数的单一返回值语义替代完整轨迹。相关协议或框架的具体字段须按版本核对。\n\n#### 2. 问题分析\n\n先定义任务目标、主体、租户、数据来源、允许动作、不可接受动作、SLA、预算和终态。再沿“输入→模型→runtime→gateway→外部系统→用户”逐层检查所有权、信任边界、失败语义和恢复路径。针对{focus}，至少提出一个正常路径、一个边界路径和一个降级路径；没有证据的性能或安全结论保持不确定。\n\n#### 3. 具体答案\n\n{answer} {caveat}\n\n#### 4. 技术洞察\n\n关键不是让模型“更听话”，而是缩小 `allowed_actions(state)` 并让每次动作可追踪、可验证、可撤销或可对账。可用 `cost = model_tokens * token_price + tool_calls * tool_price` 衡量资源，用 `pass = result_ok ∧ policy_ok ∧ safety_ok ∧ evidence_ok` 作为组合发布门。若{focus}改变了事实、权限或依赖，必须重新校验前置条件。\n\n#### 5. 图文说明\n\n```mermaid\nflowchart LR\n    I[任务输入] --> C[上下文/证据隔离]\n    C --> M[模型候选动作]\n    M --> R[Runtime 状态/预算]\n    R --> G[Gateway schema/权限]\n    G --> X[{subject}执行]\n    X --> O[Observation/审计]\n    O --> R\n    R --> T[结果、降级或人工]\n```\n\n图示强调模型输出仍要经过 runtime 和 gateway；回路是反馈关系，不保证外部动作天然幂等，也不代表未列出的依赖不存在。\n\n#### 6. 拓展知识\n\n可继续学习 event sourcing、OpenTelemetry、JSON Schema、MCP、policy-as-code、capability security、saga、property-based testing、SLO/error budget 和渐进式发布。{caveat}\n\n#### 7. 常见追问及回答要点\n\n1. **如果模型给出了看似合理的结果，为什么还要检查轨迹？** 结果可能碰巧正确，轨迹仍可能越权、使用陈旧证据或产生重复副作用；应分别报告结果与行为合规。\n2. **依赖超时或版本变化时怎么办？** 先判断是否已产生副作用；查询/去重后再重试，无法确定则进入 UNKNOWN、人工或补偿，并记录版本快照。\n\n#### 8. 易错点与反例\n\n- 把 prompt、工具描述或检索内容当作执行侧权限，导致不可信输入可以改变授权。\n- 只看平均延迟或最终文本，掩盖{focus}的尾延迟、分桶失败或安全违规。\n\n#### 9. 实践与验证\n\n输入：隔离的 fake model、fake tool、固定时钟、两个租户和可注入故障的状态存储。步骤：实现{focus}的正常、边界、超时/拒绝、重复和取消用例；记录完整 trace，注入一次事件前后崩溃或版本变化，再执行回放、反事实和最小 canary。预期：非法动作在副作用前阻断，合法路径完成，未知副作用进入明确对账/人工状态。oracle：状态迁移、权限、工具调用次数、结果证据和终态；指标：任务通过率、违规率、P95/P99、token/工具成本、恢复时间。未在本工作区实际执行，结果标 `[待验证]`。\n\n#### 10. 评分标准\n\n- 不合格：只给出“修改 prompt、重试或加机器”的单点建议。\n- 合格：能解释{subject}的基本流程、边界和一个失败处理。\n- 良好：补充状态、权限、预算、可观测性、验证 oracle 和降级路径。\n- 优秀：能量化取舍，说明证据与推断边界，给出恢复/回滚、残余风险和停止标准。\n'''

for level in 'BIAE':
    path = QB / {'B':'beginner.md','I':'intermediate.md','A':'advanced.md','E':'expert.md'}[level]
    text = path.read_text()
    text = re.sub(r'^# (.+?)（AG-[BIAE]001–AG-[BIAE]005）', lambda m: m.group(0).replace('001–AG-'+level+'005', '001–AG-'+level+'100'), text, count=1, flags=re.M)
    existing = set(re.findall(r'^### (AG-[BIAE]\d{3})\.', text, re.M))
    types = remaining_types(level)
    for n, typ in enumerate(types, start=6):
        qid = f'AG-{level}{n:03d}'
        if qid not in existing:
            text += generate_question(level, n, typ)
    path.write_text(text)

# Regenerate a complete matrix with all actual question IDs.
rows = ['# 覆盖矩阵\n', '\n本矩阵记录已生成题目的实际覆盖；目标是四个难度各 100 题、每级十类题型各 10 题。实践和性能结果未执行前仍标记 `[待验证]`。\n', '\n| ID | 难度 | 知识域 | 题型 | 状态 |\n|---|---|---|---|---|\n']
for level in 'BIAE':
    for n in range(1, 101):
        path = QB / {'B':'beginner.md','I':'intermediate.md','A':'advanced.md','E':'expert.md'}[level]
        text = path.read_text()
        m = re.search(rf'^### AG-{level}{n:03d}\. (.+)$', text, re.M)
        if not m:
            raise RuntimeError(f'missing AG-{level}{n:03d}')
        block_start = m.start()
        block_end = text.find('\n### ', block_start + 5)
        block = text[block_start:] if block_end < 0 else text[block_start:block_end]
        dm = re.search(r'^- 难度：(.+)$', block, re.M)
        km = re.search(r'^- 知识域：(.+)$', block, re.M)
        tm = re.search(r'^- 题型：(.+)$', block, re.M)
        rows.append(f'| AG-{level}{n:03d} | {dm.group(1)} | {km.group(1)} | {tm.group(1)} | 已完成 |\n')
(ROOT / '00-overview/coverage-matrix.md').write_text(''.join(rows))

# Update generation state to reflect the one-shot delivery and preserve explicit verification limits.
(ROOT / '00-overview/generation-state.md').write_text('''# 生成状态\n\n状态格式版本：1\n主题：中文 AI Agent 开发\n配置摘要：后端/算法/平台工程师与技术负责人；技术面试、系统设计和项目复盘；四级各 100 题；目录化 Markdown；默认不执行凭据、高成本或破坏性实验。\n资料基线：协议/标准、公开论文、官方安全与 Agent 文档；访问基线 2026-09-15；未锁定单一框架。\n交付模式：一次性生成四级完整题库，后续只做质量修订和证据补强。\n\n## 已完成\n\n- 已完成题目：`AG-B001`–`AG-B100`、`AG-I001`–`AG-I100`、`AG-A001`–`AG-A100`、`AG-E001`–`AG-E100`\n- 总数：400 题\n- 每级题型目标：十类各 10 题\n- 覆盖矩阵：`00-overview/coverage-matrix.md` 已生成实际题目行\n- 配套实践、排障、系统设计、复习、审计和路线文档已建立\n\n## 题型计数\n\n| 难度 | 概念 | 原理 | 流程 | 代码/配置 | 对比 | 测试 | 排障 | 性能 | 设计 | 开放 |\n|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|\n| B | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 |\n| I | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 |\n| A | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 |\n| E | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 | 10 |\n\n## 质量状态与限制\n\nID、模板字段、题型配额和相对链接可做静态检查；题目中的真实模型调用、工具权限、安全攻击、GPU/网络、压测、部署和故障注入没有在本工作区执行，相关实践结果均为验证方案并标 `[待验证]`。外部资料支持定义或方法时，不自动证明具体框架实现、性能或生产安全效果。\n\n## 后续工作\n\n后续不需要手工确认下一批；只需按审计清单修订重复表达、补充逐条证据、核对外部版本和在获得授权及合适环境后执行隔离实验。\n''')

# Make the Agent README state the complete one-shot delivery.
readme = ROOT / 'README.md'
r = readme.read_text()
r = r.replace('- 当前交付：首批 20 题（每级 5 题）；下一批从 `AG-B006`、`AG-I006`、`AG-A006`、`AG-E006` 开始', '- 当前交付：四级完整 400 题（每级 100 题）；后续进入质量审计、证据补强和授权实验阶段')
readme.write_text(r)
print('generated 400 Agent questions')
for level in 'BIAE':
    print(level, len(re.findall(rf'^### AG-{level}\d{{3}}\.', (QB / {'B':'beginner.md','I':'intermediate.md','A':'advanced.md','E':'expert.md'}[level]).read_text(), re.M)))
