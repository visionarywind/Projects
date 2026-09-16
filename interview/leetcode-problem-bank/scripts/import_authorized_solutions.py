#!/usr/bin/env python3
"""Import authorized source explanations for the LeetCode problem bank.

This preview importer records the source article/category text under an
explicit authorization section and creates a local-original enhancement section.
It avoids guessing per-problem solution bodies when LeetCode does not expose a
stable article-content payload in the public SSR data.
"""

from __future__ import annotations

import argparse
import html
import json
import re
import time
from dataclasses import asdict, dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from urllib.error import HTTPError, URLError
from urllib.parse import parse_qs, unquote, urljoin, urlparse, urlunparse
from urllib.request import Request, urlopen

SOURCE_URL = "https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd"
BASE_DIR = Path(__file__).resolve().parents[1]
CACHE_FILE = BASE_DIR / "05-solution-audit" / "solution-article-cache.json"
USER_AGENT = "Mozilla/5.0 (compatible; authorized-solution-importer/1.0)"

METHOD_LABELS = {
    "0viNMK": "滑动窗口与双指针",
    "SqopEo": "二分算法",
    "9oZFK9": "单调栈",
    "YiXPXW": "网格图",
    "dHn9Vk": "位运算",
    "01LUak": "图论算法",
    "tXLS3i": "动态规划",
    "mOr1u6": "常用数据结构",
    "IYT3ss": "数学算法",
    "g6KTKL": "贪心与思维",
    "K0n2gO": "链表、树与回溯",
    "SJFwQI": "字符串",
}


@dataclass
class ProblemRow:
    order: int
    problem_id: str
    title: str
    url: str
    rating: str
    category_path: str
    source_file: Path
    source_title: str


@dataclass
class SolutionArticle:
    title: str
    slug: str
    content: str
    author_username: str
    author_name: str
    topic_id: int | None
    question_slug: str
    question_id: str


@dataclass
class SourcePage:
    title: str
    short_title: str
    url: str
    source_id: str
    content: str = ""
    solution_refs: list[dict[str, str]] = field(default_factory=list)
    category_notes: dict[str, list[str]] = field(default_factory=dict)


class RateLimitError(RuntimeError):
    """The source rejected the request; retry later instead of marking absent."""


class TemporaryFetchError(RuntimeError):
    """A network failure that must remain retryable."""


def load_article_cache() -> dict[str, dict]:
    if not CACHE_FILE.exists():
        return {}
    try:
        data = json.loads(CACHE_FILE.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return {}
    return data if isinstance(data, dict) else {}


def save_article_cache(cache: dict[str, dict]) -> None:
    CACHE_FILE.parent.mkdir(parents=True, exist_ok=True)
    CACHE_FILE.write_text(json.dumps(cache, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def article_from_cache(data: dict) -> SolutionArticle:
    return SolutionArticle(**data)


def is_retryable_error(exc: Exception) -> bool:
    message = str(exc)
    return "请求过于频繁" in message or "timed out" in message or "timeout" in message.lower()


def graphql_request(query: str, variables: dict) -> dict:
    body = json.dumps({"query": query, "variables": variables}).encode("utf-8")
    req = Request(
        "https://leetcode.cn/graphql/",
        data=body,
        headers={
            "User-Agent": USER_AGENT,
            "Content-Type": "application/json",
            "Referer": "https://leetcode.cn/",
        },
    )
    try:
        with urlopen(req, timeout=45) as resp:
            payload = json.loads(resp.read().decode("utf-8"))
    except HTTPError as exc:
        if exc.code in {408, 425, 429, 500, 502, 503, 504}:
            raise TemporaryFetchError(f"HTTP {exc.code}: {exc.reason}") from exc
        raise
    except (TimeoutError, URLError, OSError, json.JSONDecodeError) as exc:
        raise TemporaryFetchError(str(exc)) from exc
    errors = payload.get("errors") or []
    if errors:
        message = json.dumps(errors, ensure_ascii=False)
        if "请求过于频繁" in message or "too many" in message.lower() or "rate" in message.lower():
            raise RateLimitError(message)
        raise ValueError(message)
    return payload.get("data", {})


def parse_problem_rows() -> list[ProblemRow]:
    rows: list[ProblemRow] = []
    source_dir = BASE_DIR / "01-question-bank" / "by-source-category"
    row_re = re.compile(
        r"^\|\s*(\d+)\s*\|\s*([^|]+?)\s*\|\s*\[([^\]]+)\]\((https://leetcode\.cn/problems/[^)]+)\)\s*\|\s*([^|]+?)\s*\|"
    )
    for path in sorted(source_dir.glob("*.md")):
        text = path.read_text(encoding="utf-8")
        source_title = text.splitlines()[0].lstrip("# ").strip()
        category = ""
        for line in text.splitlines():
            if line.startswith("## "):
                category = line[3:].strip()
                continue
            match = row_re.match(line)
            if not match:
                continue
            order, problem_id, linked_title, url, rating = [item.strip() for item in match.groups()]
            title = re.sub(r"^(.+?)\.\s*", "", linked_title)
            rows.append(ProblemRow(int(order), problem_id, title, url, rating, category, path, source_title))
    return rows


def problem_slug(url: str) -> str:
    parts = [part for part in urlparse(url).path.split("/") if part]
    if len(parts) >= 2 and parts[0] == "problems":
        return parts[1]
    return re.sub(r"[^a-zA-Z0-9]+", "-", url).strip("-")


def detail_path_for_problem(problem: ProblemRow) -> Path:
    filename = f"{problem.problem_id}-{problem_slug(problem.url)}.md".replace("/", "-").replace(" ", "-")
    return BASE_DIR / "01-question-bank" / "detailed-solutions" / problem.source_file.stem / filename


def find_endlesscheng_article(question_slug: str, max_pages: int = 4) -> SolutionArticle | None:
    query = """query questionTopicsList($questionSlug: String!, $skip: Int, $first: Int, $orderBy: SolutionArticleOrderBy, $userInput: String) {
  questionSolutionArticles(questionSlug: $questionSlug, skip: $skip, first: $first, orderBy: $orderBy, userInput: $userInput) {
    totalNum
    edges {
      node {
        title
        slug
        author { username profile { realName userSlug } }
        question { titleSlug questionFrontendId }
        topic { id }
      }
    }
  }
}
"""
    for user_input in ("灵茶山艾府", "endlesscheng", None):
        for page in range(max_pages):
            data = graphql_request(
                query,
                {
                    "questionSlug": question_slug,
                    "skip": page * 20,
                    "first": 20,
                    "orderBy": "DEFAULT",
                    "userInput": user_input,
                },
            )
            conn = data.get("questionSolutionArticles") or {}
            edges = conn.get("edges") or []
            for edge in edges:
                node = (edge or {}).get("node") or {}
                author = node.get("author") or {}
                profile = author.get("profile") or {}
                if author.get("username") == "endlesscheng" or profile.get("userSlug") == "endlesscheng" or profile.get("realName") == "灵茶山艾府":
                    return fetch_solution_article(node["slug"])
            if len(edges) < 20:
                break
            time.sleep(0.15)
    return None


def fetch_solution_article(article_slug: str) -> SolutionArticle:
    query = """query discussTopic($slug: String) {
  solutionArticle(slug: $slug, orderBy: DEFAULT) {
    title
    slug
    content
    author { username profile { realName userSlug } }
    topic { id }
    question { titleSlug questionFrontendId }
  }
}
"""
    data = graphql_request(query, {"slug": article_slug})
    article = data.get("solutionArticle")
    if not article or not article.get("content"):
        raise ValueError(f"missing solution article content: {article_slug}")
    author = article.get("author") or {}
    profile = author.get("profile") or {}
    question = article.get("question") or {}
    topic = article.get("topic") or {}
    return SolutionArticle(
        title=article.get("title") or article_slug,
        slug=article.get("slug") or article_slug,
        content=article.get("content") or "",
        author_username=author.get("username") or "",
        author_name=profile.get("realName") or "",
        topic_id=topic.get("id"),
        question_slug=question.get("titleSlug") or "",
        question_id=question.get("questionFrontendId") or "",
    )


def render_authorized_article(article: SolutionArticle, fetched_at: str) -> str:
    article_url = f"https://leetcode.cn/problems/{article.question_slug}/solutions/{article.topic_id}/{article.slug}/" if article.question_slug and article.topic_id else f"https://leetcode.cn/problems/{article.question_slug}/solutions/{article.slug}/"
    lines = [
        f"- 题解标题：[{article.title}]({article_url})",
        f"- 作者：{article.author_name or article.author_username} (`{article.author_username}`)",
        f"- 题解 slug：`{article.slug}`",
        f"- topic id：`{article.topic_id}`",
        f"- 授权状态：authorized-by-user-confirmation",
        f"- 导入时间：{fetched_at}",
        "",
        article.content.rstrip(),
    ]
    return "\n".join(lines).rstrip() + "\n"


def replace_section(text: str, heading: str, replacement_body: str) -> str:
    pattern = re.compile(rf"({re.escape(heading)}\n\n)(.*?)(?=\n## |\Z)", re.S)
    match = pattern.search(text)
    if not match:
        return text.rstrip() + f"\n\n{heading}\n\n{replacement_body.rstrip()}\n"
    return text[: match.start(2)] + replacement_body.rstrip() + "\n" + text[match.end(2) :]


def update_metadata(text: str, article: SolutionArticle | None, status: str | None = None) -> str:
    if article:
        external_status = "authorized-import"
    else:
        external_status = status or "missing-endlesscheng-solution"
    replacements = {
        "外部题解授权状态": external_status,
        "本地解析状态": "draft-preview",
    }
    if article:
        replacements["外部题解来源"] = f"https://leetcode.cn/problems/{article.question_slug}/solutions/{article.topic_id}/{article.slug}/"
    for key, value in replacements.items():
        text = re.sub(rf"^- {re.escape(key)}：.*$", f"- {key}：{value}", text, flags=re.M)
    return text


def solution_status(text: str) -> str:
    match = re.search(r"^- 外部题解授权状态：(.+)$", text, flags=re.M)
    return match.group(1).strip() if match else "unknown"


def fetch_article_with_retries(
    question_slug: str,
    max_retries: int,
    retry_delay: float,
) -> SolutionArticle | None:
    for attempt in range(max_retries + 1):
        try:
            return find_endlesscheng_article(question_slug)
        except (RateLimitError, TemporaryFetchError) as exc:
            if attempt >= max_retries:
                raise
            delay = retry_delay * (2**attempt)
            print(
                f"warning: transient fetch failure for {question_slug} "
                f"(attempt {attempt + 1}/{max_retries + 1}): {exc}; retrying in {delay:.1f}s"
            )
            time.sleep(delay)
    raise AssertionError("unreachable")


def import_problem_solution_bodies(
    limit: int | None,
    fetched_at: str,
    offset: int = 0,
    request_delay: float = 1.0,
    max_retries: int = 2,
    retry_delay: float = 15.0,
) -> tuple[int, int, int, int]:
    rows = parse_problem_rows()
    if offset:
        rows = rows[offset:]
    if limit is not None:
        rows = rows[:limit]
    imported = 0
    missing = 0
    skipped = 0
    temporary_failures = 0
    article_cache_data = load_article_cache()
    article_cache: dict[str, SolutionArticle] = {}
    for slug, data in article_cache_data.items():
        try:
            if isinstance(data, dict) and "article" in data:
                data = data["article"]
            article_cache[slug] = article_from_cache(data)
        except (TypeError, KeyError):
            print(f"warning: ignoring invalid cached article for {slug}")

    for problem in rows:
        path = detail_path_for_problem(problem)
        if not path.exists():
            skipped += 1
            continue
        text = path.read_text(encoding="utf-8")
        if solution_status(text) == "authorized-import":
            skipped += 1
            continue
        slug = problem_slug(problem.url)
        article = article_cache.get(slug)
        if article is None:
            try:
                article = fetch_article_with_retries(slug, max_retries, retry_delay)
            except (RateLimitError, TemporaryFetchError) as exc:
                temporary_failures += 1
                print(f"warning: leaving {slug} retryable after transient failure: {exc}")
                # Do not overwrite pending/missing metadata: a failed request is
                # not evidence that the author has no solution.
                time.sleep(request_delay)
                continue
            if article is not None:
                article_cache[slug] = article
                article_cache_data[slug] = {"status": "authorized-import", "article": asdict(article)}
                save_article_cache(article_cache_data)
        if article:
            text = update_metadata(text, article)
            text = replace_section(
                text,
                "## 授权导入：灵茶山艾府题解过程",
                render_authorized_article(article, fetched_at),
            )
            imported += 1
        else:
            # A completed, non-error search with no matching author is the only
            # case that may be recorded as permanently missing for now.
            text = update_metadata(text, None)
            missing += 1
        path.write_text(text.rstrip() + "\n", encoding="utf-8")
        time.sleep(request_delay)
    return imported, missing, skipped, temporary_failures


def fetch(url: str) -> str:
    req = Request(url, headers={"User-Agent": USER_AGENT})
    with urlopen(req, timeout=45) as resp:
        return resp.read().decode("utf-8", "ignore")


def extract_next_data(page: str) -> dict:
    m = re.search(r'<script id="__NEXT_DATA__" type="application/json">(.*?)</script>', page, re.S)
    if not m:
        raise ValueError("missing __NEXT_DATA__")
    return json.loads(html.unescape(m.group(1)))


def extract_question(data: dict) -> dict:
    queries = data.get("props", {}).get("pageProps", {}).get("dehydratedState", {}).get("queries", [])
    for q in queries:
        question = q.get("state", {}).get("data", {}).get("qaQuestion")
        if isinstance(question, dict) and question.get("content"):
            return question
    raise ValueError("missing qaQuestion")


def strip_markdown(text: str) -> str:
    text = re.sub(r"!\[[^\]]*\]\([^)]*\)(?:\{[^}]*\})?", "", text)
    text = re.sub(r"\[([^\]]+)\]\([^)]*\)", r"\1", text)
    text = re.sub(r"[*_`$]", "", text)
    text = re.sub(r"<[^>]+>", "", text)
    return html.unescape(text).strip()


def normalize_link(url: str, base_url: str) -> str:
    joined = urljoin(base_url, html.unescape(url.strip()))
    parsed = urlparse(joined)
    if parsed.path == "/link/":
        target = parse_qs(parsed.query).get("target", [""])[0]
        if target:
            parsed = urlparse(unquote(target))
    path = parsed.path
    if path.startswith("/problems/"):
        parts = [part for part in path.split("/") if part]
        if len(parts) >= 2:
            path = f"/problems/{parts[1]}/" if "solutions" not in parts else "/" + "/".join(parts) + "/"
    return urlunparse((parsed.scheme or "https", parsed.netloc or "leetcode.cn", path, "", "", ""))


def markdown_links(markdown: str) -> list[tuple[str, str]]:
    return re.findall(r"\[([^\]]+)\]\(([^)]+)\)", markdown)


def discover_pages() -> list[SourcePage]:
    main_question = extract_question(extract_next_data(fetch(SOURCE_URL)))
    pages: list[SourcePage] = []
    in_complete = False
    for raw in main_question["content"].splitlines():
        line = raw.strip()
        if line.startswith("#### 完整题单"):
            in_complete = True
            continue
        if in_complete and line.startswith("## "):
            break
        if not in_complete:
            continue
        m = re.match(r"\d+\.\s+\[([^\]]+)\]\(([^)]+)\)", line)
        if not m:
            continue
        url = normalize_link(m.group(2), SOURCE_URL)
        sid = [part for part in urlparse(url).path.split("/") if part][-1]
        title = strip_markdown(m.group(1))
        pages.append(SourcePage(title=title, short_title=METHOD_LABELS.get(sid, title), url=url, source_id=sid))
    return pages


def slugify(text: str, fallback: str) -> str:
    mapping = {
        "滑动窗口与双指针": "sliding-window-two-pointers",
        "二分算法": "binary-search",
        "单调栈": "monotonic-stack",
        "网格图": "grid-graph",
        "位运算": "bit-manipulation",
        "图论算法": "graph-algorithms",
        "动态规划": "dynamic-programming",
        "常用数据结构": "data-structures",
        "数学算法": "math-algorithms",
        "贪心与思维": "greedy-thinking",
        "链表、树与回溯": "linked-list-tree-backtracking",
        "字符串": "string-algorithms",
    }
    return mapping.get(text, re.sub(r"[^a-zA-Z0-9]+", "-", text).strip("-").lower() or fallback)


def collect_page_content(page: SourcePage) -> SourcePage:
    question = extract_question(extract_next_data(fetch(page.url)))
    page.title = question.get("title") or page.title
    page.content = question["content"]
    headings: dict[int, str] = {}
    notes: dict[str, list[str]] = {}
    refs: list[dict[str, str]] = []
    for raw in page.content.splitlines():
        line = raw.strip()
        if not line:
            continue
        hm = re.match(r"^(#{2,4})\s+(.+)$", line)
        if hm:
            level = len(hm.group(1))
            headings[level] = strip_markdown(hm.group(2))
            for old in list(headings):
                if old > level:
                    del headings[old]
            continue
        path = " / ".join(headings[level] for level in sorted(headings) if headings[level]) or page.short_title
        for link_text, link_url in markdown_links(line):
            url = normalize_link(link_url, page.url)
            if "/solutions/" in url:
                refs.append({"category_path": path, "title": strip_markdown(link_text), "url": url, "status": "pending-fetch"})
        no_links = strip_markdown(line)
        if no_links and not re.match(r"^-?\s*((\d+|LCP|LCR|剑指|面试题).*)", no_links):
            if len(no_links) >= 8 and not no_links.startswith("http"):
                notes.setdefault(path, []).append(raw)
    page.solution_refs = refs
    page.category_notes = notes
    return page


def render_source_explanation(page: SourcePage, fetched_at: str) -> str:
    lines = [
        f"# {page.short_title}",
        "",
        f"- 来源 URL：{page.url}",
        f"- 来源标题：{page.title}",
        f"- 抓取时间：{fetched_at}",
        "- 授权状态：authorized-by-user-confirmation",
        "- 导入状态：preview",
        "",
        "## 授权导入：专题/分类解析",
        "",
        "> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。",
    ]
    for path, notes in page.category_notes.items():
        lines.extend(["", f"### {path}", ""])
        for note in notes[:12]:
            lines.append(note)
    lines.extend([
        "",
        "## 授权题解链接索引",
        "",
        "| 分类路径 | 标题 | 链接 | 导入状态 |",
        "|---|---|---|---|",
    ])
    if page.solution_refs:
        for ref in page.solution_refs:
            lines.append(f"| {ref['category_path']} | {ref['title']} | {ref['url']} | {ref['status']} |")
    else:
        lines.append("| - | - | - | no-solution-link-detected |")
    lines.extend([
        "",
        "## 本地原创增强解析",
        "",
        f"{page.short_title} 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。",
        "",
        "### 分类层级说明",
        "",
        "本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。",
        "",
        "### 建议刷题节奏",
        "",
        "先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。",
    ])
    return "\n".join(lines).rstrip() + "\n"


def write(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--batch", default="preview", choices=["preview"])
    parser.add_argument("--import-problem-bodies", action="store_true", help="fill existing detailed solution files with endlesscheng article bodies")
    parser.add_argument("--limit", type=int, default=None, help="limit problem body imports for smoke tests")
    parser.add_argument("--offset", type=int, default=0, help="skip this many source-category rows")
    parser.add_argument("--request-delay", type=float, default=1.0, help="seconds between problem lookups")
    parser.add_argument("--max-retries", type=int, default=2, help="retries for rate limits and transient network errors")
    parser.add_argument("--retry-delay", type=float, default=15.0, help="initial delay before retrying a transient failure")
    args = parser.parse_args()
    fetched_at = datetime.now(timezone.utc).astimezone().strftime("%Y-%m-%d %H:%M:%S %z")
    pages = [collect_page_content(page) for page in discover_pages()]
    out_dir = BASE_DIR / "02-source-explanations"
    for idx, page in enumerate(pages, 1):
        filename = f"{idx:02d}-{slugify(page.short_title, page.source_id)}.md"
        write(out_dir / filename, render_source_explanation(page, fetched_at))
    print(f"imported {len(pages)} source explanation files ({args.batch})")
    if args.import_problem_bodies:
        imported, missing, skipped, temporary_failures = import_problem_solution_bodies(
            args.limit,
            fetched_at,
            offset=args.offset,
            request_delay=args.request_delay,
            max_retries=args.max_retries,
            retry_delay=args.retry_delay,
        )
        print(
            f"imported {imported} problem solution bodies; missing {missing}; "
            f"skipped {skipped}; transient failures {temporary_failures}"
        )
    return 0
