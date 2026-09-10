# M09 - Web UI

> 状态：首版构建边界分析；UI 源码调用链待专项展开。

## 技术边界

UI 位于 `tools/ui`，package scripts 使用 SvelteKit/Vite、Vitest、Playwright、Storybook 和 PWA asset generator。[tools/ui/package.json:1-27] server development 文档归纳其层次为 Routes -> Components -> Hooks -> Stores -> Services -> Storage/API，并使用 IndexedDB/LocalStorage。[tools/server/README-dev.md:343-379]

## 与 server 的关系

UI 通过 server HTTP API 进行聊天、模型管理、附件和流式输出；server 可嵌入构建后的 UI 资源。改变 UI 后应执行 `npm run build` 并重建 server（命令来源已确认，未执行）。

## 验证

```bash
cd tools/ui
npm install
npm run check
npm run lint
npm run test
npm run build
```

状态：未验证。具体 stores/services、SSE reconnect、IndexedDB schema 和 route 权限需后续逐文件确认。

---

**页尾：** 不把 UI 开发依赖当作 C++ runtime 的链接依赖。
