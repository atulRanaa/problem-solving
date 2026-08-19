// --- PROBLEM SOLVING DIRECTORY APP (MULTI-VIEW LAYOUT) --- //

let allProblems = [];
let filteredProblems = [];
let currentPage = 1;
const pageSize = 24;

let currentView = localStorage.getItem('catalogView') || 'editorial';

let currentFilters = {
  platform: 'ALL',
  concept: 'ALL',
  lang: 'ALL',
  search: ''
};

const GITHUB_RAW_BASE = "https://raw.githubusercontent.com/atulRanaa/problem-solving/main/";
const GITHUB_REPO_BASE = "https://github.com/atulRanaa/problem-solving/blob/main/";

document.addEventListener('DOMContentLoaded', () => {
  initTheme();
  initLayoutSwitcher();
  setupKeyboardShortcuts();
  loadProblems();
  setupEventListeners();
});

// Theme Management
function initTheme() {
  const savedTheme = localStorage.getItem('theme') || 'dark';
  document.documentElement.setAttribute('data-theme', savedTheme);
  
  document.getElementById('theme-toggle').addEventListener('click', () => {
    const current = document.documentElement.getAttribute('data-theme');
    const next = current === 'dark' ? 'light' : 'dark';
    document.documentElement.setAttribute('data-theme', next);
    localStorage.setItem('theme', next);
  });
}

// Layout Switcher
function initLayoutSwitcher() {
  const container = document.getElementById('problems-container');
  container.className = `problems-view view-${currentView}`;

  document.querySelectorAll('.layout-btn').forEach(btn => {
    btn.classList.toggle('active', btn.dataset.view === currentView);
    btn.addEventListener('click', () => {
      currentView = btn.dataset.view;
      localStorage.setItem('catalogView', currentView);
      document.querySelectorAll('.layout-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');

      container.className = `problems-view view-${currentView}`;
      currentPage = 1;
      renderProblems(false);
    });
  });
}

// Keyboard Shortcuts
function setupKeyboardShortcuts() {
  document.addEventListener('keydown', (e) => {
    if (e.key === '/' && document.activeElement.tagName !== 'INPUT' && document.activeElement.tagName !== 'TEXTAREA') {
      e.preventDefault();
      const input = document.getElementById('search-input');
      input.focus();
      input.select();
    }
  });
}

// Fetch Problems JSON
async function loadProblems() {
  try {
    const res = await fetch('data/problems.json');
    if (!res.ok) throw new Error('Failed to load problems data');
    allProblems = await res.json();
    
    updateStatsBar();
    buildFilterButtons();
    applyFilters();
  } catch (err) {
    console.error('Error loading problems:', err);
    document.getElementById('problems-container').innerHTML = `
      <div class="empty-state">
        <p>Error loading problem database. Please ensure data/problems.json is present.</p>
      </div>
    `;
  }
}

// Update Stats Dashboard
function updateStatsBar() {
  const total = allProblems.length;
  const platforms = new Set(allProblems.map(p => p.platform)).size;
  const concepts = new Set(allProblems.flatMap(p => p.concepts)).size;
  const sysdesigns = allProblems.filter(p => p.platform === 'System Design' || p.concepts.includes('System Design')).length;

  document.getElementById('stat-total').textContent = total.toLocaleString();
  document.getElementById('stat-platforms').textContent = `${platforms}+`;
  document.getElementById('stat-concepts').textContent = concepts;
  document.getElementById('stat-sysdesign').textContent = sysdesigns;
  document.getElementById('total-badge').textContent = `${total.toLocaleString()} Problems`;
}

// Build Filter Pills Dynamically
function buildFilterButtons() {
  // Platform pills
  const platformCounts = {};
  allProblems.forEach(p => {
    platformCounts[p.platform] = (platformCounts[p.platform] || 0) + 1;
  });

  const topPlatforms = Object.entries(platformCounts)
    .sort((a, b) => b[1] - a[1]);

  const platformContainer = document.getElementById('platform-filters');
  topPlatforms.forEach(([plat, count]) => {
    const btn = document.createElement('button');
    btn.className = 'filter-btn';
    btn.dataset.filterType = 'platform';
    btn.dataset.value = plat;
    btn.textContent = `${plat} (${count})`;
    platformContainer.appendChild(btn);
  });

  // Concept pills
  const conceptCounts = {};
  allProblems.forEach(p => {
    p.concepts.forEach(c => {
      conceptCounts[c] = (conceptCounts[c] || 0) + 1;
    });
  });

  const sortedConcepts = Object.entries(conceptCounts)
    .sort((a, b) => b[1] - a[1]);

  const conceptContainer = document.getElementById('concept-filters');
  sortedConcepts.forEach(([conc, count]) => {
    const btn = document.createElement('button');
    btn.className = 'filter-btn';
    btn.dataset.filterType = 'concept';
    btn.dataset.value = conc;
    btn.textContent = `${conc} (${count})`;
    conceptContainer.appendChild(btn);
  });

  // Language pills
  const langCounts = {};
  allProblems.forEach(p => {
    langCounts[p.language] = (langCounts[p.language] || 0) + 1;
  });

  const langContainer = document.getElementById('lang-filters');
  Object.entries(langCounts)
    .sort((a, b) => b[1] - a[1])
    .forEach(([lang, count]) => {
      const btn = document.createElement('button');
      btn.className = 'filter-btn';
      btn.dataset.filterType = 'lang';
      btn.dataset.value = lang;
      btn.textContent = `${lang} (${count})`;
      langContainer.appendChild(btn);
    });
}

// Event Listeners Setup
function setupEventListeners() {
  const searchInput = document.getElementById('search-input');
  const clearSearch = document.getElementById('clear-search');

  let debounceTimer;
  searchInput.addEventListener('input', (e) => {
    clearTimeout(debounceTimer);
    const query = e.target.value.trim();
    clearSearch.hidden = !query;

    debounceTimer = setTimeout(() => {
      currentFilters.search = query.toLowerCase();
      applyFilters();
    }, 180);
  });

  clearSearch.addEventListener('click', () => {
    searchInput.value = '';
    clearSearch.hidden = true;
    currentFilters.search = '';
    applyFilters();
  });

  // Filter button clicks
  document.querySelector('.filter-groups').addEventListener('click', (e) => {
    const btn = e.target.closest('.filter-btn');
    if (!btn) return;

    const type = btn.dataset.filterType;
    const value = btn.dataset.value;

    const group = btn.closest('.filter-pills');
    group.querySelectorAll('.filter-btn').forEach(b => b.classList.remove('active'));
    btn.classList.add('active');

    currentFilters[type] = value;
    applyFilters();
  });

  // Stat pill click shortcuts
  document.getElementById('stats-bar').addEventListener('click', (e) => {
    const pill = e.target.closest('.stat-pill');
    if (!pill) return;
    const stat = pill.dataset.stat;
    if (stat === 'sysdesign') {
      filterByConcept('System Design');
    } else if (stat === 'all') {
      resetAllFilters();
    }
  });

  // Load More Button
  document.getElementById('load-more-btn').addEventListener('click', () => {
    currentPage++;
    renderProblems(true);
  });

  // Reset all filters button
  document.getElementById('reset-all-filters').addEventListener('click', resetAllFilters);

  // Modal events
  document.getElementById('close-modal-btn').addEventListener('click', closeModal);
  document.getElementById('code-modal-backdrop').addEventListener('click', (e) => {
    if (e.target === e.currentTarget) closeModal();
  });

  document.getElementById('copy-code-btn').addEventListener('click', copyCodeToClipboard);
}

function resetAllFilters() {
  currentFilters = { platform: 'ALL', concept: 'ALL', lang: 'ALL', search: '' };
  document.getElementById('search-input').value = '';
  document.getElementById('clear-search').hidden = true;

  document.querySelectorAll('.filter-btn').forEach(b => {
    b.classList.toggle('active', b.dataset.value === 'ALL');
  });

  applyFilters();
}

function filterByConcept(conceptName) {
  currentFilters.concept = conceptName;
  const container = document.getElementById('concept-filters');
  container.querySelectorAll('.filter-btn').forEach(b => {
    b.classList.toggle('active', b.dataset.value === conceptName);
  });
  applyFilters();
}

// Filter Logic
function applyFilters() {
  filteredProblems = allProblems.filter(p => {
    if (currentFilters.platform !== 'ALL' && p.platform !== currentFilters.platform) {
      return false;
    }
    if (currentFilters.concept !== 'ALL' && !p.concepts.includes(currentFilters.concept)) {
      return false;
    }
    if (currentFilters.lang !== 'ALL' && p.language !== currentFilters.lang) {
      return false;
    }

    if (currentFilters.search) {
      const q = currentFilters.search;
      const titleMatch = p.title.toLowerCase().includes(q);
      const filenameMatch = p.filename.toLowerCase().includes(q);
      const pathMatch = p.filePath.toLowerCase().includes(q);
      const conceptMatch = p.concepts.some(c => c.toLowerCase().includes(q));
      const platformMatch = p.platform.toLowerCase().includes(q);

      if (!titleMatch && !filenameMatch && !pathMatch && !conceptMatch && !platformMatch) {
        return false;
      }
    }

    return true;
  });

  currentPage = 1;
  updateResultsHeader();
  renderProblems(false);
}

// Update Active Filters Header
function updateResultsHeader() {
  const countEl = document.getElementById('results-count');
  countEl.textContent = `Showing ${filteredProblems.length.toLocaleString()} of ${allProblems.length.toLocaleString()} problems`;

  const tagsBar = document.getElementById('active-filters-bar');
  tagsBar.innerHTML = '';

  const active = [];
  if (currentFilters.platform !== 'ALL') active.push(`Platform: ${currentFilters.platform}`);
  if (currentFilters.concept !== 'ALL') active.push(`Concept: ${currentFilters.concept}`);
  if (currentFilters.lang !== 'ALL') active.push(`Lang: ${currentFilters.lang}`);
  if (currentFilters.search) active.push(`Search: "${currentFilters.search}"`);

  active.forEach(tagText => {
    const pill = document.createElement('span');
    pill.className = 'concept-tag';
    pill.textContent = `${tagText} ×`;
    pill.addEventListener('click', () => {
      if (tagText.startsWith('Platform:')) currentFilters.platform = 'ALL';
      if (tagText.startsWith('Concept:')) currentFilters.concept = 'ALL';
      if (tagText.startsWith('Lang:')) currentFilters.lang = 'ALL';
      if (tagText.startsWith('Search:')) {
        currentFilters.search = '';
        document.getElementById('search-input').value = '';
        document.getElementById('clear-search').hidden = true;
      }
      syncFilterPills();
      applyFilters();
    });
    tagsBar.appendChild(pill);
  });
}

function syncFilterPills() {
  ['platform', 'concept', 'lang'].forEach(type => {
    const val = currentFilters[type];
    const container = document.getElementById(`${type}-filters`);
    if (container) {
      container.querySelectorAll('.filter-btn').forEach(b => {
        b.classList.toggle('active', b.dataset.value === val);
      });
    }
  });
}

// Render Problems Container (Editorial, Cards, or Table)
function renderProblems(append = false) {
  const container = document.getElementById('problems-container');
  const emptyState = document.getElementById('empty-state');

  if (!append) {
    container.innerHTML = '';
  }

  if (filteredProblems.length === 0) {
    emptyState.hidden = false;
    container.innerHTML = '';
    document.getElementById('pagination-container').hidden = true;
    return;
  }

  emptyState.hidden = true;

  const startIndex = (currentPage - 1) * pageSize;
  const endIndex = currentPage * pageSize;
  const pageSlice = filteredProblems.slice(startIndex, endIndex);

  if (currentView === 'editorial') {
    renderEditorialView(container, pageSlice);
  } else if (currentView === 'grid') {
    renderGridView(container, pageSlice);
  } else if (currentView === 'table') {
    renderTableView(container, pageSlice, append);
  }

  const hasMore = endIndex < filteredProblems.length;
  document.getElementById('pagination-container').hidden = !hasMore;
}

// 📰 EDITORIAL LIST VIEW RENDERER (tdd.cat inspired)
function renderEditorialView(container, pageSlice) {
  pageSlice.forEach(prob => {
    const row = document.createElement('div');
    row.className = 'editorial-row';

    row.innerHTML = `
      <span class="editorial-platform">${escapeHtml(prob.platform)}</span>

      <div class="editorial-main">
        <a href="#" class="editorial-title view-code-link" data-id="${prob.id}">${escapeHtml(prob.title)}</a>
        <div class="editorial-concepts">
          ${prob.concepts.map(c => `<span class="concept-tag" data-concept="${escapeHtml(c)}">${escapeHtml(c)}</span>`).join('')}
          <span class="lang-tag">• ${escapeHtml(prob.language)} (${prob.lineCount} lines)</span>
        </div>
      </div>

      <div class="editorial-actions">
        <button class="action-btn primary view-code-btn" data-id="${prob.id}">View Code ↗</button>
        <a href="${prob.problemUrl}" target="_blank" rel="noopener" class="action-btn" title="Open Problem">🔗</a>
      </div>
    `;

    row.querySelectorAll('.concept-tag').forEach(tagEl => {
      tagEl.addEventListener('click', (e) => {
        e.stopPropagation();
        filterByConcept(tagEl.dataset.concept);
      });
    });

    row.querySelector('.view-code-btn').addEventListener('click', () => openCodeModal(prob));
    row.querySelector('.view-code-link').addEventListener('click', (e) => {
      e.preventDefault();
      openCodeModal(prob);
    });

    container.appendChild(row);
  });
}

// 🎴 MINIMAL CARDS GRID RENDERER
function renderGridView(container, pageSlice) {
  pageSlice.forEach(prob => {
    const card = document.createElement('article');
    card.className = 'problem-card';

    card.innerHTML = `
      <div class="card-top">
        <div class="card-header-meta">
          <span class="platform-badge">${escapeHtml(prob.platform)}</span>
          <span class="lang-badge">${escapeHtml(prob.language)}</span>
        </div>
        <h3 class="problem-title">${escapeHtml(prob.title)}</h3>
        <div class="concept-tags">
          ${prob.concepts.map(c => `<span class="concept-tag" data-concept="${escapeHtml(c)}">${escapeHtml(c)}</span>`).join('')}
        </div>
      </div>

      <div class="card-footer">
        <div class="card-actions">
          <button class="action-btn primary view-code-btn" data-id="${prob.id}">View Code</button>
          <a href="${prob.problemUrl}" target="_blank" rel="noopener" class="action-btn">Problem ↗</a>
        </div>
      </div>
    `;

    card.querySelectorAll('.concept-tag').forEach(tagEl => {
      tagEl.addEventListener('click', (e) => {
        e.stopPropagation();
        filterByConcept(tagEl.dataset.concept);
      });
    });

    card.querySelector('.view-code-btn').addEventListener('click', () => openCodeModal(prob));
    container.appendChild(card);
  });
}

// 📊 COMPACT TABLE VIEW RENDERER
function renderTableView(container, pageSlice, append) {
  let table = container.querySelector('table');
  if (!table || !append) {
    container.innerHTML = `
      <table class="view-table">
        <thead>
          <tr>
            <th>Platform</th>
            <th>Problem Title</th>
            <th>Algorithms & Data Structures</th>
            <th>Lang</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody></tbody>
      </table>
    `;
    table = container.querySelector('table');
  }

  const tbody = table.querySelector('tbody');

  pageSlice.forEach(prob => {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td><span class="platform-badge">${escapeHtml(prob.platform)}</span></td>
      <td><strong><a href="#" class="view-code-link" style="color:var(--text-heading);text-decoration:none;">${escapeHtml(prob.title)}</a></strong></td>
      <td>
        <div class="concept-tags">
          ${prob.concepts.map(c => `<span class="concept-tag" data-concept="${escapeHtml(c)}">${escapeHtml(c)}</span>`).join('')}
        </div>
      </td>
      <td><span class="lang-tag">${escapeHtml(prob.language)}</span></td>
      <td>
        <div style="display:flex;gap:0.35rem;">
          <button class="action-btn primary view-code-btn" data-id="${prob.id}">View</button>
          <a href="${prob.problemUrl}" target="_blank" rel="noopener" class="action-btn">🔗</a>
        </div>
      </td>
    `;

    tr.querySelectorAll('.concept-tag').forEach(tagEl => {
      tagEl.addEventListener('click', (e) => {
        e.stopPropagation();
        filterByConcept(tagEl.dataset.concept);
      });
    });

    tr.querySelector('.view-code-btn').addEventListener('click', () => openCodeModal(prob));
    tr.querySelector('.view-code-link').addEventListener('click', (e) => {
      e.preventDefault();
      openCodeModal(prob);
    });

    tbody.appendChild(tr);
  });
}

// Code Viewer Modal Drawer
async function openCodeModal(prob) {
  const modal = document.getElementById('code-modal-backdrop');
  document.getElementById('modal-title').textContent = prob.title;
  document.getElementById('modal-filepath').textContent = prob.filePath;
  
  const badgesContainer = document.getElementById('modal-badges');
  badgesContainer.innerHTML = `
    <span class="platform-badge">${escapeHtml(prob.platform)}</span>
    <span class="lang-badge">${escapeHtml(prob.language)}</span>
  `;

  document.getElementById('modal-github-link').href = `${GITHUB_REPO_BASE}${encodeURIComponent(prob.filePath)}`;
  document.getElementById('modal-problem-link').href = prob.problemUrl;

  const codeEl = document.getElementById('modal-code-content');
  codeEl.textContent = 'Loading source code from repository...';
  modal.hidden = false;

  try {
    const rawUrl = `${GITHUB_RAW_BASE}${encodeURIComponent(prob.filePath)}`;
    const res = await fetch(rawUrl);
    if (res.ok) {
      const codeText = await res.text();
      codeEl.textContent = codeText;
    } else {
      codeEl.textContent = prob.snippet + '\n\n// [Full file preview dynamically available when hosted on GitHub]';
    }
  } catch (err) {
    codeEl.textContent = prob.snippet + '\n\n// [Preview snippet shown above]';
  }
}

function closeModal() {
  document.getElementById('code-modal-backdrop').hidden = true;
}

function copyCodeToClipboard() {
  const codeText = document.getElementById('modal-code-content').textContent;
  navigator.clipboard.writeText(codeText).then(() => {
    const btn = document.getElementById('copy-code-btn');
    btn.textContent = '✅ Copied!';
    setTimeout(() => btn.textContent = '📋 Copy Code', 2000);
  });
}

function escapeHtml(str) {
  if (!str) return '';
  return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
}
