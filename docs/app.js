// --- PROBLEM SOLVING DIRECTORY APP (CLEAN, ELEGANT & ROBUST) --- //

let allProblems = [];
let filteredProblems = [];
let currentPage = 1;
const pageSize = 24;

let currentView = localStorage.getItem('catalogView') || 'editorial';
let currentTab = 'catalog';

let currentFilters = {
  platform: 'ALL',
  concept: 'ALL',
  lang: 'ALL',
  difficulty: 'ALL',
  search: ''
};

let bookmarks = new Set(JSON.parse(localStorage.getItem('userBookmarks') || '[]'));
let activeModalProblem = null;

const GITHUB_RAW_BASE = "https://raw.githubusercontent.com/atulRanaa/problem-solving/main/";
const GITHUB_REPO_BASE = "https://github.com/atulRanaa/problem-solving/blob/main/";

document.addEventListener('DOMContentLoaded', () => {
  initTheme();
  initNavTabs();
  initLayoutSwitcher();
  setupKeyboardShortcuts();
  setupCmdKPalette();
  loadProblems();
  setupEventListeners();
});

// Theme Management
function initTheme() {
  const savedTheme = localStorage.getItem('theme') || 'dark';
  document.documentElement.setAttribute('data-theme', savedTheme);
  
  const themeBtn = document.getElementById('theme-toggle');
  if (themeBtn) {
    themeBtn.addEventListener('click', () => {
      const current = document.documentElement.getAttribute('data-theme');
      const next = current === 'dark' ? 'light' : 'dark';
      document.documentElement.setAttribute('data-theme', next);
      localStorage.setItem('theme', next);
    });
  }
}

// Nav Tabs (All Problems | Bookmarks)
function initNavTabs() {
  document.querySelectorAll('.nav-tab').forEach(tab => {
    tab.addEventListener('click', () => {
      currentTab = tab.dataset.tab;
      document.querySelectorAll('.nav-tab').forEach(t => t.classList.remove('active'));
      tab.classList.add('active');

      document.querySelectorAll('.tab-page').forEach(page => {
        page.hidden = (page.id !== `tab-content-${currentTab}`);
      });

      const filterSec = document.getElementById('filter-section');
      if (filterSec) {
        filterSec.style.display = (currentTab === 'catalog') ? 'flex' : 'none';
      }

      if (currentTab === 'bookmarks') renderBookmarks();
    });
  });
}

// Layout Switcher
function initLayoutSwitcher() {
  const container = document.getElementById('problems-container');
  if (container) {
    container.className = `problems-view view-${currentView}`;
  }

  document.querySelectorAll('.layout-btn').forEach(btn => {
    btn.classList.toggle('active', btn.dataset.view === currentView);
    btn.addEventListener('click', () => {
      currentView = btn.dataset.view;
      localStorage.setItem('catalogView', currentView);
      document.querySelectorAll('.layout-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');

      if (container) {
        container.className = `problems-view view-${currentView}`;
      }
      currentPage = 1;
      renderProblems(false);
    });
  });
}

// Keyboard Shortcuts & Cmd+K Command Palette
function setupKeyboardShortcuts() {
  document.addEventListener('keydown', (e) => {
    if ((e.metaKey || e.ctrlKey) && e.key.toLowerCase() === 'k') {
      e.preventDefault();
      toggleCmdK(true);
    } else if (e.key === 'Escape') {
      toggleCmdK(false);
      closeModal();
    } else if (e.key === '/' && document.activeElement.tagName !== 'INPUT' && document.activeElement.tagName !== 'TEXTAREA') {
      e.preventDefault();
      const input = document.getElementById('search-input');
      if (input) {
        input.focus();
        input.select();
      }
    }
  });
}

function setupCmdKPalette() {
  const trigger = document.getElementById('cmd-k-trigger');
  const backdrop = document.getElementById('cmd-k-backdrop');
  const input = document.getElementById('cmd-k-input');
  const resultsContainer = document.getElementById('cmd-k-results');

  if (trigger) trigger.addEventListener('click', () => toggleCmdK(true));
  if (backdrop) {
    backdrop.addEventListener('click', (e) => {
      if (e.target === backdrop) toggleCmdK(false);
    });
  }

  if (input) {
    input.addEventListener('input', (e) => {
      if (!resultsContainer) return;
      const q = e.target.value.trim().toLowerCase();
      if (!q) {
        resultsContainer.innerHTML = '<div class="cmd-k-item"><span class="cmd-k-meta">Type to search solutions...</span></div>';
        return;
      }

      const matches = allProblems.filter(p => 
        p.title.toLowerCase().includes(q) ||
        p.platform.toLowerCase().includes(q) ||
        p.concepts.some(c => c.toLowerCase().includes(q))
      ).slice(0, 10);

      if (matches.length === 0) {
        resultsContainer.innerHTML = '<div class="cmd-k-item"><span class="cmd-k-meta">No matching solutions found</span></div>';
        return;
      }

      resultsContainer.innerHTML = matches.map(p => `
        <div class="cmd-k-item" data-id="${p.id}">
          <span class="cmd-k-title">${escapeHtml(p.title)}</span>
          <span class="cmd-k-meta">${escapeHtml(p.platform)} • ${escapeHtml(p.language)}</span>
        </div>
      `).join('');

      resultsContainer.querySelectorAll('.cmd-k-item').forEach(item => {
        item.addEventListener('click', () => {
          const id = item.dataset.id;
          const prob = allProblems.find(p => p.id === id);
          if (prob) {
            toggleCmdK(false);
            openCodeModal(prob);
          }
        });
      });
    });
  }
}

function toggleCmdK(show) {
  const backdrop = document.getElementById('cmd-k-backdrop');
  const input = document.getElementById('cmd-k-input');
  if (backdrop) backdrop.hidden = !show;
  if (show && input) {
    input.value = '';
    input.focus();
    const res = document.getElementById('cmd-k-results');
    if (res) res.innerHTML = '<div class="cmd-k-item"><span class="cmd-k-meta">Type to search solutions...</span></div>';
  }
}

// Fetch Problems JSON
async function loadProblems() {
  try {
    const res = await fetch('data/problems.json');
    if (!res.ok) throw new Error('Failed to load problems data');
    allProblems = await res.json();
    
    buildFilterButtons();
    applyFilters();
  } catch (err) {
    console.error('Error loading problems:', err);
    const container = document.getElementById('problems-container');
    if (container) {
      container.innerHTML = `
        <div class="empty-state">
          <p>Error loading problem database. Please ensure data/problems.json is present.</p>
        </div>
      `;
    }
  }
}

// Build Filter Pills Dynamically
function buildFilterButtons() {
  // Platform pills
  const platformCounts = {};
  allProblems.forEach(p => {
    platformCounts[p.platform] = (platformCounts[p.platform] || 0) + 1;
  });

  const platformContainer = document.getElementById('platform-filters');
  if (platformContainer) {
    Object.entries(platformCounts)
      .sort((a, b) => b[1] - a[1])
      .forEach(([plat, count]) => {
        const btn = document.createElement('button');
        btn.className = 'filter-btn';
        btn.dataset.filterType = 'platform';
        btn.dataset.value = plat;
        btn.textContent = `${plat} (${count})`;
        platformContainer.appendChild(btn);
      });
  }

  // Concept pills
  const conceptCounts = {};
  allProblems.forEach(p => {
    p.concepts.forEach(c => {
      conceptCounts[c] = (conceptCounts[c] || 0) + 1;
    });
  });

  const conceptContainer = document.getElementById('concept-filters');
  if (conceptContainer) {
    Object.entries(conceptCounts)
      .sort((a, b) => b[1] - a[1])
      .forEach(([conc, count]) => {
        const btn = document.createElement('button');
        btn.className = 'filter-btn';
        btn.dataset.filterType = 'concept';
        btn.dataset.value = conc;
        btn.textContent = `${conc} (${count})`;
        conceptContainer.appendChild(btn);
      });
  }
}

// Event Listeners Setup
function setupEventListeners() {
  const searchInput = document.getElementById('search-input');
  const clearSearch = document.getElementById('clear-search');

  let debounceTimer;
  if (searchInput) {
    searchInput.addEventListener('input', (e) => {
      clearTimeout(debounceTimer);
      const query = e.target.value.trim();
      if (clearSearch) clearSearch.hidden = !query;

      debounceTimer = setTimeout(() => {
        currentFilters.search = query.toLowerCase();
        applyFilters();
      }, 180);
    });
  }

  if (clearSearch) {
    clearSearch.addEventListener('click', () => {
      if (searchInput) searchInput.value = '';
      clearSearch.hidden = true;
      currentFilters.search = '';
      applyFilters();
    });
  }

  // Filter button clicks
  const filterSection = document.getElementById('filter-section');
  if (filterSection) {
    filterSection.addEventListener('click', (e) => {
      const btn = e.target.closest('.filter-btn');
      if (!btn) return;

      const type = btn.dataset.filterType;
      const value = btn.dataset.value;

      const group = btn.closest('.filter-pills');
      if (group) {
        group.querySelectorAll('.filter-btn').forEach(b => b.classList.remove('active'));
      }
      btn.classList.add('active');

      currentFilters[type] = value;
      applyFilters();
    });
  }

  // Load More Button
  const loadMoreBtn = document.getElementById('load-more-btn');
  if (loadMoreBtn) {
    loadMoreBtn.addEventListener('click', () => {
      currentPage++;
      renderProblems(true);
    });
  }

  // Reset all filters button
  const resetBtn = document.getElementById('reset-all-filters');
  if (resetBtn) {
    resetBtn.addEventListener('click', resetAllFilters);
  }

  // Modal events
  const closeBtn = document.getElementById('close-modal-btn');
  if (closeBtn) closeBtn.addEventListener('click', closeModal);

  const backdrop = document.getElementById('code-modal-backdrop');
  if (backdrop) {
    backdrop.addEventListener('click', (e) => {
      if (e.target === backdrop) closeModal();
    });
  }

  const copyBtn = document.getElementById('copy-code-btn');
  if (copyBtn) copyBtn.addEventListener('click', copyCodeToClipboard);

  const bookmarkBtn = document.getElementById('bookmark-btn');
  if (bookmarkBtn) bookmarkBtn.addEventListener('click', toggleBookmarkActiveModal);
}

function resetAllFilters() {
  currentFilters = { platform: 'ALL', concept: 'ALL', lang: 'ALL', difficulty: 'ALL', search: '' };
  const input = document.getElementById('search-input');
  if (input) input.value = '';
  const clear = document.getElementById('clear-search');
  if (clear) clear.hidden = true;

  document.querySelectorAll('.filter-btn').forEach(b => {
    b.classList.toggle('active', b.dataset.value === 'ALL');
  });

  applyFilters();
}

function filterByConcept(conceptName) {
  currentFilters.concept = conceptName;
  const container = document.getElementById('concept-filters');
  if (container) {
    container.querySelectorAll('.filter-btn').forEach(b => {
      b.classList.toggle('active', b.dataset.value === conceptName);
    });
  }
  
  if (currentTab !== 'catalog') {
    const catalogTab = document.querySelector('.nav-tab[data-tab="catalog"]');
    if (catalogTab) catalogTab.click();
  }
  
  applyFilters();
}

// Filter Logic
function applyFilters() {
  filteredProblems = allProblems.filter(p => {
    if (currentFilters.platform !== 'ALL' && p.platform !== currentFilters.platform) return false;
    if (currentFilters.concept !== 'ALL' && !p.concepts.includes(currentFilters.concept)) return false;
    if (currentFilters.lang !== 'ALL' && p.language !== currentFilters.lang) return false;
    if (currentFilters.difficulty !== 'ALL' && p.difficulty !== currentFilters.difficulty) return false;

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
  if (countEl) {
    countEl.textContent = `Showing ${filteredProblems.length.toLocaleString()} solutions`;
  }

  const tagsBar = document.getElementById('active-filters-bar');
  if (!tagsBar) return;
  tagsBar.innerHTML = '';

  const active = [];
  if (currentFilters.difficulty !== 'ALL') active.push(`Diff: ${currentFilters.difficulty}`);
  if (currentFilters.platform !== 'ALL') active.push(`Platform: ${currentFilters.platform}`);
  if (currentFilters.concept !== 'ALL') active.push(`Concept: ${currentFilters.concept}`);
  if (currentFilters.lang !== 'ALL') active.push(`Lang: ${currentFilters.lang}`);
  if (currentFilters.search) active.push(`Search: "${currentFilters.search}"`);

  active.forEach(tagText => {
    const pill = document.createElement('span');
    pill.className = 'concept-tag';
    pill.textContent = `${tagText} ×`;
    pill.addEventListener('click', () => {
      if (tagText.startsWith('Diff:')) currentFilters.difficulty = 'ALL';
      if (tagText.startsWith('Platform:')) currentFilters.platform = 'ALL';
      if (tagText.startsWith('Concept:')) currentFilters.concept = 'ALL';
      if (tagText.startsWith('Lang:')) currentFilters.lang = 'ALL';
      if (tagText.startsWith('Search:')) {
        currentFilters.search = '';
        const input = document.getElementById('search-input');
        if (input) input.value = '';
        const clear = document.getElementById('clear-search');
        if (clear) clear.hidden = true;
      }
      syncFilterPills();
      applyFilters();
    });
    tagsBar.appendChild(pill);
  });
}

function syncFilterPills() {
  ['difficulty', 'platform', 'concept', 'lang'].forEach(type => {
    const val = currentFilters[type];
    const container = document.getElementById(`${type}-filters`);
    if (container) {
      container.querySelectorAll('.filter-btn').forEach(b => {
        b.classList.toggle('active', b.dataset.value === val);
      });
    }
  });
}

// Render Problems Container
function renderProblems(append = false) {
  const container = document.getElementById('problems-container');
  const emptyState = document.getElementById('empty-state');
  const pag = document.getElementById('pagination-container');

  if (!container) return;

  if (!append) {
    container.innerHTML = '';
  }

  if (filteredProblems.length === 0) {
    if (emptyState) emptyState.hidden = false;
    container.innerHTML = '';
    if (pag) pag.hidden = true;
    return;
  }

  if (emptyState) emptyState.hidden = true;

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
  if (pag) pag.hidden = !hasMore;
}

// EDITORIAL LIST VIEW RENDERER
function renderEditorialView(container, pageSlice) {
  pageSlice.forEach(prob => {
    const row = document.createElement('div');
    row.className = 'editorial-row';

    const isBookmarked = bookmarks.has(prob.id);

    row.innerHTML = `
      <span class="editorial-platform">${escapeHtml(prob.platform)}</span>

      <div class="editorial-main">
        <div>
          <a href="#" class="editorial-title view-code-link" data-id="${prob.id}">${escapeHtml(prob.title)}</a>
          <span class="diff-badge ${prob.difficulty}">${prob.difficulty}</span>
        </div>
        <div class="editorial-concepts">
          ${prob.concepts.map(c => `<span class="concept-tag" data-concept="${escapeHtml(c)}">${escapeHtml(c)}</span>`).join('')}
          <span class="lang-tag">• ${escapeHtml(prob.language)} (${prob.lineCount} lines)</span>
        </div>
      </div>

      <div class="editorial-actions">
        <button class="action-btn star-btn" data-id="${prob.id}">${isBookmarked ? '⭐' : '☆'}</button>
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

    row.querySelector('.star-btn').addEventListener('click', (e) => {
      e.stopPropagation();
      toggleBookmark(prob.id, e.target);
    });

    row.querySelector('.view-code-btn').addEventListener('click', () => openCodeModal(prob));
    row.querySelector('.view-code-link').addEventListener('click', (e) => {
      e.preventDefault();
      openCodeModal(prob);
    });

    container.appendChild(row);
  });
}

// MINIMAL CARDS GRID RENDERER
function renderGridView(container, pageSlice) {
  pageSlice.forEach(prob => {
    const card = document.createElement('article');
    card.className = 'problem-card';

    card.innerHTML = `
      <div class="card-top">
        <div class="card-header-meta">
          <span class="editorial-platform">${escapeHtml(prob.platform)}</span>
          <div>
            <span class="diff-badge ${prob.difficulty}">${prob.difficulty}</span>
          </div>
        </div>
        <h3 class="problem-title">${escapeHtml(prob.title)}</h3>
        <div class="editorial-concepts">
          ${prob.concepts.map(c => `<span class="concept-tag" data-concept="${escapeHtml(c)}">${escapeHtml(c)}</span>`).join('')}
        </div>
      </div>

      <div class="card-footer">
        <span class="lang-tag">${escapeHtml(prob.language)}</span>
        <div style="display:flex;gap:0.35rem;">
          <button class="action-btn primary view-code-btn" data-id="${prob.id}">View Code</button>
          <a href="${prob.problemUrl}" target="_blank" rel="noopener" class="action-btn">🔗</a>
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

// COMPACT TABLE VIEW RENDERER
function renderTableView(container, pageSlice, append) {
  let table = container.querySelector('table');
  if (!table || !append) {
    container.innerHTML = `
      <table class="view-table">
        <thead>
          <tr>
            <th>Platform</th>
            <th>Problem Title</th>
            <th>Diff</th>
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
      <td><span class="editorial-platform">${escapeHtml(prob.platform)}</span></td>
      <td><strong><a href="#" class="view-code-link" style="color:var(--text-heading);text-decoration:none;">${escapeHtml(prob.title)}</a></strong></td>
      <td><span class="diff-badge ${prob.difficulty}">${prob.difficulty}</span></td>
      <td>
        <div class="editorial-concepts">
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

// ⭐ RENDER BOOKMARKS
function renderBookmarks() {
  const container = document.getElementById('bookmarks-container');
  if (!container) return;
  container.innerHTML = '';

  const bookmarkedProbs = allProblems.filter(p => bookmarks.has(p.id));

  if (bookmarkedProbs.length === 0) {
    container.innerHTML = '<div class="empty-state"><p>No bookmarked problems yet. Click the ⭐ star button on any problem to save it here!</p></div>';
    return;
  }

  renderEditorialView(container, bookmarkedProbs);
}

function toggleBookmark(probId, btnEl) {
  if (bookmarks.has(probId)) {
    bookmarks.delete(probId);
    if (btnEl) btnEl.textContent = '☆';
  } else {
    bookmarks.add(probId);
    if (btnEl) btnEl.textContent = '⭐';
  }
  localStorage.setItem('userBookmarks', JSON.stringify(Array.from(bookmarks)));
}

function toggleBookmarkActiveModal() {
  if (!activeModalProblem) return;
  toggleBookmark(activeModalProblem.id, null);
  const btn = document.getElementById('bookmark-btn');
  if (btn) {
    const isBookmarked = bookmarks.has(activeModalProblem.id);
    btn.textContent = isBookmarked ? '⭐ Bookmarked' : '☆ Bookmark';
  }
}

// Code Viewer Modal Drawer
async function openCodeModal(prob) {
  activeModalProblem = prob;
  const modal = document.getElementById('code-modal-backdrop');
  const title = document.getElementById('modal-title');
  const filepath = document.getElementById('modal-filepath');
  
  if (title) title.textContent = prob.title;
  if (filepath) filepath.textContent = prob.filePath;
  
  const badgesContainer = document.getElementById('modal-badges');
  if (badgesContainer) {
    badgesContainer.innerHTML = `
      <span class="editorial-platform">${escapeHtml(prob.platform)}</span>
      <span class="diff-badge ${prob.difficulty}">${prob.difficulty}</span>
      <span class="lang-tag">${escapeHtml(prob.language)}</span>
    `;
  }

  const bookmarkBtn = document.getElementById('bookmark-btn');
  if (bookmarkBtn) {
    bookmarkBtn.textContent = bookmarks.has(prob.id) ? '⭐ Bookmarked' : '☆ Bookmark';
  }

  const ghLink = document.getElementById('modal-github-link');
  if (ghLink) ghLink.href = `${GITHUB_REPO_BASE}${encodeURIComponent(prob.filePath)}`;

  const probLink = document.getElementById('modal-problem-link');
  if (probLink) probLink.href = prob.problemUrl;

  const codeEl = document.getElementById('modal-code-content');
  if (codeEl) codeEl.textContent = 'Loading source code...';

  if (modal) modal.hidden = false;

  try {
    const rawUrl = `${GITHUB_RAW_BASE}${encodeURIComponent(prob.filePath)}`;
    const res = await fetch(rawUrl);
    if (res.ok) {
      const codeText = await res.text();
      if (codeEl) codeEl.textContent = codeText;
    } else {
      if (codeEl) codeEl.textContent = prob.snippet;
    }
  } catch (err) {
    if (codeEl) codeEl.textContent = prob.snippet;
  }
}

function closeModal() {
  const modal = document.getElementById('code-modal-backdrop');
  if (modal) modal.hidden = true;
  activeModalProblem = null;
}

function copyCodeToClipboard() {
  const codeEl = document.getElementById('modal-code-content');
  if (!codeEl) return;
  navigator.clipboard.writeText(codeEl.textContent).then(() => {
    const btn = document.getElementById('copy-code-btn');
    if (btn) {
      btn.textContent = '✅ Copied!';
      setTimeout(() => btn.textContent = 'Copy Code', 2000);
    }
  });
}

function escapeHtml(str) {
  if (!str) return '';
  return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
}
