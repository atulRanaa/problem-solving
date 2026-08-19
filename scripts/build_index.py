import os
import json
import re

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
OUTPUT_JSON = os.path.join(REPO_ROOT, "docs", "data", "problems.json")
CSES_MAP_FILE = os.path.join(os.path.dirname(__file__), "cses_map.json")

CSES_MAP = {}
if os.path.exists(CSES_MAP_FILE):
    try:
        with open(CSES_MAP_FILE, 'r') as f:
            CSES_MAP = json.load(f)
    except Exception:
        CSES_MAP = {}

EXCLUDE_DIRS = {".git", ".vscode", "docs", "scripts", "node_modules", "brain"}
EXCLUDE_FILES = {".DS_Store", "a.out", "Executable", "0.5", "output.txt", "input.txt", ".gitignore", "README.md"}

# Expanded granular Concept Rules for Data Structures and Algorithms
CONCEPT_RULES = {
    # --- DATA STRUCTURES ---
    "Segment Tree": [
        r'\bsegment[ _]tree\b', r'\bsegtree\b', r'\bkquery\b', r'\bhorrible\b', r'/st/',
        r'tree\[2\s*\*|build\(|update\(|query\('
    ],
    "Fenwick Tree (BIT)": [
        r'\bfenwick\b', r'\bbinary[ _]indexed[ _]tree\b', r'\bbit\b', r'\bupdateit\b', r'\bupdatequery\b', r'/bit/',
        r'lowbit|x\s*&\s*\(-x\)|bit\[x\]'
    ],
    "Disjoint Set Union (DSU)": [
        r'\bdsu\b', r'\bdisjoint[ _]set\b', r'\bunion[ _]find\b', r'\bparent\[\b', r'\bfind_set\b', r'\bfind\('
    ],
    "Trie (Prefix Tree)": [
        r'\btrie\b', r'\bprefix[ _]tree\b', r'\binsert_trie\b', r'struct\s+node\s*\{\s*node\*\s*child'
    ],
    "Suffix Array / Tree": [
        r'\bsuffix[ _]array\b', r'\bsuffix[ _]tree\b', r'/suffix-array/', r'kasai|lcp\[|sa\['
    ],
    "Monotonic Stack / Queue": [
        r'\bmonotonic[ _]stack\b', r'\bmonotonic[ _]queue\b', r'\bnext_greater\b', r'\bprev_smaller\b', r'while\s*\(!s\.empty\(\)'
    ],
    "Priority Queue / Heap": [
        r'\bpriority_queue\b', r'\bmax_heap\b', r'\bmin_heap\b', r'\bheapify\b', r'push_heap|pop_heap'
    ],
    "Binary Search Tree (BST)": [
        r'\bbst\b', r'\bbinary[ _]search[ _]tree\b', r'\btreap\b', r'\bavl[ _]tree\b', r'struct\s+node\s*\{\s*int\s+key'
    ],
    "Linked List": [
        r'\blinked[ _]list\b', r'\bhead\s*->\s*next\b', r'/linked list/', r'node\*\s*next'
    ],
    "Hash Table / Map": [
        r'\bunordered_map\b', r'\bhashmap\b', r'\bhash_table\b', r'map<|unordered_set'
    ],
    "Graph Adjacency List": [
        r'\badjacency[ _]list\b', r'\bvector<int>\s*adj\b', r'\bvector<pair<int,int>>\s*adj\b', r'vector<int>\s*graph'
    ],

    # --- ALGORITHMS & TECHNIQUES ---
    "Dijkstra Algorithm": [
        r'\bdijkstra\b', r'\bshortest_path\b', r'\btrafficn\b', r'dist\[u\]\s*\+\s*w\s*<\s*dist\[v\]'
    ],
    "DFS / BFS Graph Traversal": [
        r'\bdfs\b', r'\bbfs\b', r'\bdepth_first\b', r'\bbreadth_first\b', r'\bcam5\b', r'\bmicemaze\b', r'visited\[u\]\s*=\s*true', r'queue<int>\s*q'
    ],
    "Minimum Spanning Tree (MST)": [
        r'\bkruskal\b', r'\bprim\b', r'\bmst\b'
    ],
    "Floyd-Warshall": [
        r'\bfloyd\b', r'\ball_pairs_shortest\b', r'dist\[i\]\[j\]'
    ],
    "Topological Sort": [
        r'\btopological\b', r'\btopo_sort\b', r'\bindegree\b'
    ],
    "Lowest Common Ancestor (LCA)": [
        r'\blca\b', r'\blowest_common_ancestor\b', r'\bbinary_lifting\b', r'up\[u\]\[i\]'
    ],
    "Heavy-Light Decomposition (HLD)": [
        r'\bhld\b', r'\bheavy_light\b'
    ],
    "KMP String Matching": [
        r'\bkmp\b', r'\bknuth_morris\b', r'\bprefix_function\b', r'lps\['
    ],
    "Z-Algorithm / Rabin-Karp": [
        r'\bz[ _]algorithm\b', r'\brabin[ _]karp\b', r'\bstring[ _]hash\b'
    ],
    "Binary Search": [
        r'\bbinary[ _]search\b', r'\blower[ _]bound\b', r'\bupper[ _]bound\b', r'\bsearch_range\b', r'low\s*<=\s*high'
    ],
    "Ternary Search": [
        r'\bternary[ _]search\b', r'\bternarysearch\b'
    ],
    "Two Pointers": [
        r'\btwo[ _]pointers\b', r'\bleft\s*<\s*right\b'
    ],
    "Sliding Window": [
        r'\bsliding[ _]window\b', r'window_sum'
    ],
    "Knapsack DP": [
        r'\bknapsack\b', r'\b01_knapsack\b', r'\bunbounded_knapsack\b'
    ],
    "Bitmask DP": [
        r'\bbitmask[ _]dp\b', r'mask\s*&\s*\(1\s*<<'
    ],
    "Subsequence DP (LCS / LIS)": [
        r'\blcs\b', r'\blis\b', r'\blongest_common\b', r'\blongest_increasing\b', r'\bedit[ _]distance\b'
    ],
    "Dynamic Programming": [
        r'\bdp\b', r'\bmemoization\b', r'\bfibonacci\b', r'\bfarida\b', r'\bbytesm2\b', r'\bmiserman\b', r'\bedist\b', r'\bvacation\b', r'\bfrog\b', r'\bgrid[ _]paths\b', r'dp\[i\]\s*='
    ],
    "Matrix Exponentiation": [
        r'\bmatrix[ _]exponentiation\b', r'\bmatrix_mul\b'
    ],
    "Sieve of Eratosthenes": [
        r'\bsieve\b', r'\bprime_sieve\b', r'\bis_prime\b', r'prime\[i\]'
    ],
    "Euler's Totient (ETF)": [
        r'\betf\b', r'\btotient\b', r'\beuler_phi\b', r'phi\[i\]'
    ],
    "GCD & Euclidean Algo": [
        r'\bgcd\b', r'\blcm\b', r'\bextgcd\b', r'\beuclidean\b', r'__gcd'
    ],
    "Modular Arithmetic": [
        r'\bmodulo\b', r'\bmod_pow\b', r'\bmod_inverse\b', r'\bncr\b', r'\bfermat\b', r'1e9\+7|1000000007'
    ],
    "Bit Manipulation": [
        r'\bxor\b', r'\bbitwise\b', r'\bsubxor\b', r'\band[ _]sum\b', r'\bcontinuousones\b', r'\bbitmask\b', r'\(1\s*<<\s*[a-z0-9]+\)'
    ],
    "Greedy Strategy": [
        r'\bgreedy\b', r'\binterval[ _]scheduling\b', r'\bactivity[ _]selection\b', r'\bbusyman\b', r'\bcadydist\b', r'\bstamps\b'
    ],
    "Backtracking & Recursion": [
        r'\bbacktracking\b', r'\brecursion\b', r'\bn_queens\b', r'\bsudoku\b', r'\bpermutations\b'
    ],
    "Computational Geometry": [
        r'\bconvex_hull\b', r'\bgeometry\b', r'\bpolygon\b', r'\bcross_product\b'
    ],
    "CUDA Parallel Computing": [
        r'\bcuda\b', r'\b__global__\b', r'\bblockidx\b', r'\bthreadidx\b', r'/cuda/'
    ],

    # --- SYSTEM DESIGN & SYSTEM CONCEPTS ---
    "System Design": [
        r'\bsystem design\b', r'/system design/'
    ],
    "Change Data Capture (CDC)": [
        r'\bcdc\b', r'\bchange data capture\b'
    ],
    "Event Streaming (Kafka)": [
        r'\bkafka\b', r'\bmessage queue\b', r'\bpub sub\b'
    ],
    "Distributed Caching (Redis)": [
        r'\bredis\b', r'\bdynamodb\b', r'\bdistributed cache\b'
    ],
    "Rate Limiter Design": [
        r'\brate limiter\b', r'\btoken bucket\b', r'\bleaky bucket\b'
    ],
    "Real-Time WebSockets": [
        r'\bwebsocket\b', r'\brealtime\b'
    ],
    "Database & SQL Indexing": [
        r'\bdatabase\b', r'\bsql\b', r'\bquery\b', r'\bb-tree index\b', r'\bjoin\b', r'\bschema\b'
    ]
}

PLATFORM_MAP = {
    "CodeForces": "CodeForces",
    "Spoj": "SPOJ",
    "CSES": "CSES",
    "Codechef": "CodeChef",
    "atCoder": "AtCoder",
    "HackerEarth": "HackerEarth",
    "HackerRank": "HackerRank",
    "InterviewBIT": "InterviewBIT",
    "GeekforGeeks": "GeeksforGeeks",
    "Google Kick Start": "Google KickStart",
    "ProjectEuler": "ProjectEuler",
    "IARCS": "IARCS",
    "System Design": "System Design",
    "Database": "Database",
    "Linked List": "Linked List",
    "Cuda": "CUDA / Parallel",
    "Numerical Analysis": "Numerical Analysis",
    "TODO": "TODO / Unsolved",
    "CodeVita": "CodeVita",
    "Coding": "Coding",
    "Distributed Systems": "Distributed Systems"
}

LANG_MAP = {
    ".cpp": "C++",
    ".cp": "C++",
    ".hpp": "C++",
    ".h": "C++",
    ".py": "Python",
    ".c": "C",
    ".java": "Java",
    ".scala": "Scala",
    ".cu": "CUDA C++",
    ".md": "Markdown",
    ".sql": "SQL",
    ".query": "SQL",
    ".rb": "Ruby",
    ".r": "R",
    ".js": "JavaScript",
    ".html": "HTML",
    ".css": "CSS"
}

def clean_problem_title(filename):
    name, _ = os.path.splitext(filename)
    cleaned = re.sub(r'^(BIT_|Spoj_|Codechef_|CF_|AT_|\d+-)', '', name, flags=re.IGNORECASE)
    if '_' in cleaned or '-' in cleaned:
        cleaned = cleaned.replace('_', ' ').replace('-', ' ')
    words = cleaned.split()
    if all(w.islower() or w.isupper() for w in words):
        cleaned = " ".join([w.capitalize() for w in words])
    return cleaned

def infer_platform(rel_path, content_head):
    parts = rel_path.split(os.sep)
    first_folder = parts[0]
    if first_folder in PLATFORM_MAP:
        return PLATFORM_MAP[first_folder]
    
    head_lower = content_head.lower()
    if "spoj" in head_lower:
        return "SPOJ"
    elif "codeforces" in head_lower or "code forces" in head_lower:
        return "CodeForces"
    elif "codechef" in head_lower:
        return "CodeChef"
    elif "hackerrank" in head_lower:
        return "HackerRank"
    elif "hackerearth" in head_lower:
        return "HackerEarth"
    elif "cses" in head_lower:
        return "CSES"
    elif "atcoder" in head_lower:
        return "AtCoder"
    elif "leetcode" in head_lower:
        return "LeetCode"
    elif "geeksforgeeks" in head_lower or "gfg" in head_lower:
        return "GeeksforGeeks"
    
    return "General Programming"

def infer_concepts(filename, rel_path, content):
    cleaned_content = re.sub(r'#include\s*<bits/stdc\+\+\.h>', '', content, flags=re.IGNORECASE)
    cleaned_content = re.sub(r'using\s+namespace\s+std;', '', cleaned_content, flags=re.IGNORECASE)
    
    text_to_scan = f"{filename} {rel_path}\n{cleaned_content[:4000]}".lower()
    matched = set()
    
    rel_lower = rel_path.lower()
    if "bit" in rel_lower or "fenwick" in rel_lower:
        matched.add("Fenwick Tree (BIT)")
    if "st" in rel_lower or "segment" in rel_lower:
        matched.add("Segment Tree")
    if "suffix" in rel_lower:
        matched.add("Suffix Array / Tree")
    if "linked list" in rel_lower:
        matched.add("Linked List")
    if "system design" in rel_lower:
        matched.add("System Design")
    if "database" in rel_lower:
        matched.add("Database & SQL Indexing")

    for concept, rules in CONCEPT_RULES.items():
        for pattern in rules:
            if r"\b" in pattern or "/" in pattern or r"\\" in pattern or r"\s" in pattern or "(" in pattern or "[" in pattern:
                if re.search(pattern, text_to_scan, re.IGNORECASE):
                    matched.add(concept)
                    break
            else:
                if pattern.lower() in text_to_scan:
                    matched.add(concept)
                    break
                
    if not matched:
        matched.add("Algorithmic Logic")
        
    return sorted(list(matched))

def infer_difficulty(filename, concepts, line_count):
    hard_tags = {"Heavy-Light Decomposition (HLD)", "Suffix Array / Tree", "Dijkstra Algorithm", "Segment Tree", "Bitmask DP", "Matrix Exponentiation", "Computational Geometry", "Lowest Common Ancestor (LCA)"}
    medium_tags = {"DFS / BFS Graph Traversal", "Binary Search", "Trie (Prefix Tree)", "Disjoint Set Union (DSU)", "Dynamic Programming", "Sieve of Eratosthenes", "Fenwick Tree (BIT)", "Subsequence DP (LCS / LIS)", "Monotonic Stack / Queue"}
    
    concept_set = set(concepts)
    if concept_set.intersection(hard_tags) or line_count > 70:
        return "Hard"
    elif concept_set.intersection(medium_tags) or line_count > 35:
        return "Medium"
    else:
        return "Easy"

def infer_complexity(concepts):
    if "Segment Tree" in concepts or "Fenwick Tree (BIT)" in concepts:
        return {"time": "O(N log N)", "space": "O(N)"}
    elif "Dijkstra Algorithm" in concepts or "DFS / BFS Graph Traversal" in concepts:
        return {"time": "O((V + E) log V)", "space": "O(V + E)"}
    elif "Binary Search" in concepts:
        return {"time": "O(log N)", "space": "O(1)"}
    elif "Dynamic Programming" in concepts or "Subsequence DP (LCS / LIS)" in concepts:
        return {"time": "O(N²)", "space": "O(N²)"}
    elif "GCD & Euclidean Algo" in concepts or "Sieve of Eratosthenes" in concepts:
        return {"time": "O(N log log N)", "space": "O(N)"}
    elif "System Design" in concepts:
        return {"time": "Distributed", "space": "Scalable"}
    else:
        return {"time": "O(N)", "space": "O(1)"}

def extract_meaningful_snippet(content):
    lines = content.splitlines()
    meaningful = []
    
    for line in lines:
        stripped = line.strip()
        if not stripped:
            continue
        if (stripped.startswith("#include") or 
            stripped.startswith("using namespace") or 
            stripped.startswith("#define") or 
            stripped.startswith("typedef") or 
            stripped.startswith("const int N") or
            stripped.startswith("const ll N") or
            stripped.startswith("ios_base::") or
            stripped.startswith("cin.tie(")):
            continue
        meaningful.append(line)
        if len(meaningful) >= 6:
            break
            
    if not meaningful:
        meaningful = [l for l in lines if l.strip()][:6]
        
    return "\n".join(meaningful) if meaningful else "// Solution code"

def build_problem_url(platform, filename, clean_title, content_head):
    title_upper = filename.upper()
    name_no_ext = os.path.splitext(filename)[0]
    
    if platform == "SPOJ":
        code = name_no_ext.upper()
        return f"https://www.spoj.com/problems/{code}/"
    elif platform == "CodeForces":
        m = re.search(r'([A-Z])?(\d{3,4})([A-Z])?', title_upper)
        if m:
            contest_id = m.group(2)
            prob_index = m.group(1) or m.group(3) or "A"
            return f"https://codeforces.com/problemset/problem/{contest_id}/{prob_index}"
        return f"https://codeforces.com/problemset?q={clean_title}"
    elif platform == "CodeChef":
        return f"https://www.codechef.com/problems/{name_no_ext.upper()}"
    elif platform == "CSES":
        raw_name = re.sub(r'^\d+-', '', name_no_ext)
        clean_key = re.sub(r'[^a-z0-9]', '', raw_name.lower())
        if clean_key in CSES_MAP:
            return f"https://cses.fi/problemset/task/{CSES_MAP[clean_key]}"
        return f"https://cses.fi/problemset/"
    elif platform == "ProjectEuler":
        m = re.search(r'\d+', name_no_ext)
        if m:
            p_num = int(m.group(0))
            return f"https://projecteuler.net/problem={p_num}"
    elif platform == "AtCoder":
        return f"https://atcoder.jp/contests/search?q={clean_title}"
    elif platform == "HackerRank":
        slug = clean_title.lower().replace(" ", "-")
        return f"https://www.hackerrank.com/challenges/{slug}"
    
    search_q = clean_title.replace(" ", "+")
    return f"https://www.google.com/search?q={platform}+{search_q}"

def scan_problems():
    problems = []
    id_counter = 1
    
    for root, dirs, files in os.walk(REPO_ROOT):
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS and not d.startswith(".")]
        
        for f in files:
            if f in EXCLUDE_FILES or f.startswith("."):
                continue
                
            ext = os.path.splitext(f)[1].lower()
            if ext not in LANG_MAP:
                continue
                
            abs_path = os.path.join(root, f)
            rel_path = os.path.relpath(abs_path, REPO_ROOT)
            
            content = ""
            status = "Solved"
            try:
                with open(abs_path, 'r', encoding='utf-8', errors='ignore') as fp:
                    content = fp.read()
            except Exception as e:
                content = ""
                
            head_lower = content[:500].lower()
            if "todo" in rel_path.lower() or "incomplete" in head_lower or "incom" in head_lower:
                status = "Incomplete / Draft"
                
            platform = infer_platform(rel_path, content[:500])
            language = LANG_MAP.get(ext, "Other")
            clean_title = clean_problem_title(f)
            concepts = infer_concepts(f, rel_path, content)
            problem_url = build_problem_url(platform, f, clean_title, content[:500])
            snippet = extract_meaningful_snippet(content)
            
            lines = [line for line in content.splitlines() if line.strip()]
            line_count = len(lines)
            difficulty = infer_difficulty(f, concepts, line_count)
            complexity = infer_complexity(concepts)
            
            problem_entry = {
                "id": f"prob-{id_counter}",
                "title": clean_title,
                "filename": f,
                "filePath": rel_path,
                "platform": platform,
                "language": language,
                "concepts": concepts,
                "difficulty": difficulty,
                "complexity": complexity,
                "status": status,
                "problemUrl": problem_url,
                "snippet": snippet,
                "lineCount": line_count
            }
            
            problems.append(problem_entry)
            id_counter += 1
            
    print(f"Successfully indexed {len(problems)} problems with difficulty and complexity metadata.")
    
    os.makedirs(os.path.dirname(OUTPUT_JSON), exist_ok=True)
    with open(OUTPUT_JSON, 'w', encoding='utf-8') as f:
        json.dump(problems, f, indent=2)
        
    print(f"Saved problem database to {OUTPUT_JSON}")

if __name__ == "__main__":
    scan_problems()
