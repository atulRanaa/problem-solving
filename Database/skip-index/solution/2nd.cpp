#include <array>
#include <bit>
#include <bitset>
#include <optional>
#include <ranges>
#include <vector>
#include <span>
#include <cstdint>
#include <cassert>
#include <map>
#include <cmath>
#include <cstring>
#include <memory_resource>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include ""Parameters.hpp""

constexpr size_t MAX_THRESHOLD = 140'000;
constexpr size_t FULL_THRESHOLD = 55'000;
constexpr int64_t MIN_THRESHOLD = -400'000;

constexpr size_t MaxEmptySpanCount = 1000;
constexpr uint32_t MinSpanDiff = 1'000'000;
constexpr bool MINMAX_ENABLED = false;

constexpr uint32_t NUM_TABLE_THRESHOLD = 5;

constexpr uint32_t STREAK_THRESHOLD = 30;
constexpr uint32_t MIN_DIFF_THRESHOLD = 1'000'000;

constexpr uint32_t SPANS_MAX_SINGLE = 10;

constexpr bool DEBUG = false;

using Statistics = std::map<uint32_t, uint32_t>;

enum algorithm : uint8_t {
  MINMAX = 0,
  FULL_TABLE = 1,
  EMPTY_SPANS = 2,
  STREAKS = 3,
  STREAKS_MIN = 4
};

struct headerMinMax {
  uint32_t min;
  uint32_t max;
};

inline uint8_t byteCount(const uint32_t highest) {
  if (0xFFFF0000 & highest) return 4;
  if (0xFF00 & highest) return 2;
  return 1;
}

void insertComprTagged(std::vector<std::byte>& index, uint32_t num, bool tag) {
  const uint8_t t = (tag ? 1 : 0) << 2;
  if (num >= 1 << 21) {
    index.emplace_back(static_cast<std::byte>(num << 3 | t | 0b11));
    index.emplace_back(static_cast<std::byte>(num >> 5));
    index.emplace_back(static_cast<std::byte>(num >> 13));
    if (num >= 1 << 28) {
      index.emplace_back(static_cast<std::byte>(num >> 21 | 0x80));
      index.emplace_back(static_cast<std::byte>(num >> 28));
    } else {
      index.emplace_back(static_cast<std::byte>(num >> 21));
    }
  } else if (num >= 1 << 13) {
    index.emplace_back(static_cast<std::byte>(num << 3 | t | 0b10));
    index.emplace_back(static_cast<std::byte>(num >> 5));
    index.emplace_back(static_cast<std::byte>(num >> 13));
  } else if (num >= 1 << 5) {
    index.emplace_back(static_cast<std::byte>(num << 3 | t | 0b01));
    index.emplace_back(static_cast<std::byte>(num >> 5));
  } else {
    index.emplace_back(static_cast<std::byte>(num << 3 | t));
  }
}

bool readComprTagged(uint8_t*& data, uint32_t& entry) {
  const uint8_t by = *data & 0x3;
  const bool tag = *data & 0x4;
  if (by == 0b11) {
    uint32_t d = *std::bit_cast<uint32_t*>(data) >> 3 & 0xFFFFFFF;
    if (*std::bit_cast<uint32_t*>(data) >> 31) {
      entry += d | static_cast<uint32_t>(*(data + 4)) << 28;
      data += 5;
    } else {
      entry += d;
      data += 4;
    }
  } else if (by == 0b10) {
    entry += *std::bit_cast<uint32_t*>(data) >> 3 & 0x1FFFFF;
    data += 3;
  } else if (by == 0b01) {
    entry += *std::bit_cast<uint16_t*>(data) >> 3;
    data += 2;
  } else {
    entry += *data >> 3;
    data += 1;
  }
  return tag;
}


void insertCompr3(std::vector<std::byte>& index, uint32_t num) {
  if (num >= 1 << 28) {
    index.emplace_back(static_cast<std::byte>(num << 1 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 6 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 13 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 20 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 28));
  } else if (num >= 1 << 21) {
    index.emplace_back(static_cast<std::byte>(num << 1 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 6 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 13 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 20 & 0xFE));
  } else if (num >= 1 << 14) {
    index.emplace_back(static_cast<std::byte>(num << 1 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 6 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 13 & 0xFE));
  } else if (num >= 1 << 7) {
    index.emplace_back(static_cast<std::byte>(num << 1 | 0b1));
    index.emplace_back(static_cast<std::byte>(num >> 6 & 0xFE));
  } else {
    index.emplace_back(static_cast<std::byte>(num << 1));
  }
}

void readCompr3(uint8_t*& data, uint32_t& entry) {
  const bool second = *data & 0x1;
  if (second) {
    const bool third = *(data+1) & 0x1;
    if (third) {
      const bool fourth = *(data+2) & 0x1;
      if (fourth) {
        const bool fifth = *(data+3) & 0x1;
        if (fifth) {
          entry += *std::bit_cast<uint8_t*>(data) >> 1;
          entry += (*std::bit_cast<uint8_t*>(data + 1) & 0xFE) << 6;
          entry += (*std::bit_cast<uint8_t*>(data + 2) & 0xFE) << 13;
          entry += (*std::bit_cast<uint8_t*>(data + 3) & 0xFE) << 20;
          entry += (*std::bit_cast<uint8_t*>(data + 4)) << 28;
          data += 5;
        } else {
          entry += *std::bit_cast<uint8_t*>(data) >> 1;
          entry += (*std::bit_cast<uint8_t*>(data + 1) & 0xFE) << 6;
          entry += (*std::bit_cast<uint8_t*>(data + 2) & 0xFE) << 13;
          entry += (*std::bit_cast<uint8_t*>(data + 3) & 0xFE) << 20;
          data += 4;
        }
      } else {
        entry += *std::bit_cast<uint8_t*>(data) >> 1;
        entry += (*std::bit_cast<uint8_t*>(data + 1) & 0xFE) << 6;
        entry += (*std::bit_cast<uint8_t*>(data + 2) & 0xFE) << 13;
        data += 3;
      }
    } else {
      entry += *std::bit_cast<uint8_t*>(data) >> 1;
      entry += (*std::bit_cast<uint8_t*>(data + 1) & 0xFE) << 6;
      data += 2;
    }
  } else {
    entry += *data >> 1;
    data += 1;
  }
}

void insertCompr(bool optimize_small, std::vector<std::byte>& index, uint32_t num) {
  if (optimize_small) insertCompr3(index, num);
  else {
    if (num >= 1 << 22) {
      index.emplace_back(static_cast<std::byte>(num << 2 | 0b11));
      index.emplace_back(static_cast<std::byte>(num >> 6));
      index.emplace_back(static_cast<std::byte>(num >> 14));
      if (num >= 1 << 29) {
        index.emplace_back(static_cast<std::byte>(num >> 22 | 0x80));
        index.emplace_back(static_cast<std::byte>(num >> 29));
      } else {
        index.emplace_back(static_cast<std::byte>(num >> 22));
      }
    } else if (num >= 1 << 14) {
      index.emplace_back(static_cast<std::byte>(num << 2 | 0b10));
      index.emplace_back(static_cast<std::byte>(num >> 6));
      index.emplace_back(static_cast<std::byte>(num >> 14));
    } else if (num >= 1 << 6) {
      index.emplace_back(static_cast<std::byte>(num << 2 | 0b01));
      index.emplace_back(static_cast<std::byte>(num >> 6));
    } else {
      index.emplace_back(static_cast<std::byte>(num << 2));
    }
  }
}

void readCompr(bool optimize_small, uint8_t*& data, uint32_t& entry) {
  if (optimize_small) readCompr3(data, entry);
  else {
    const uint8_t by = *data & 0x3;
    if (by == 0b11) {
      uint32_t d = *std::bit_cast<uint32_t*>(data) >> 2 & 0x1FFFFFFF;
      if (*std::bit_cast<uint32_t*>(data) >> 31) {
        entry += d | static_cast<uint32_t>(*(data + 4)) << 29;
        data += 5;
      } else {
        entry += d;
        data += 4;
      }
    } else if (by == 0b10) {
      entry += *std::bit_cast<uint32_t*>(data) >> 2 & 0x3FFFFF;
      data += 3;
    } else if (by == 0b01) {
      entry += *std::bit_cast<uint16_t*>(data) >> 2;
      data += 2;
    } else {
      entry += *data >> 2;
      data += 1;
    }
  }
}

size_t calcThres(int64_t other_thres, size_t parB, size_t parS) {
  const double frac = static_cast<double>(other_thres) / static_cast<double>(FULL_THRESHOLD);
  const double b = (100 - frac) / 99;
  const double a = 1 - b;
  const double corr = static_cast<double>(parB) / static_cast<double>(parS) * a + b;
  return static_cast<size_t>(std::max(0., FULL_THRESHOLD * corr));
}

uint8_t buffer;
bool state = false;
void insert4(std::vector<std::byte>& index, uint8_t num) {
  if (state) {
    buffer |= num & 0xF;
    index.emplace_back(std::bit_cast<std::byte>(buffer));
    state = false;
  } else {
    buffer = num << 4;
    state = true;
  }
}
uint8_t read4(uint8_t*& index) {
  if (state) {
    state = false;
    return buffer & 0xF;
  } else {
    buffer = *(index++);
    state = true;
    return buffer >> 4;
  }
}
void flush4(std::vector<std::byte>& index) {
  if (state) {
    index.emplace_back(std::bit_cast<std::byte>(buffer));
    state = false;
  }
}

uint8_t stateOf(bool optimize_small, uint32_t num) {
  if (optimize_small) {
    if (num >> 8) return 3;
    if (num >> 4) return 2;
    return num == 0 ? 0 : 1;
  } else {
    if (num >> 16) return 3;
    if (num >> 8) return 2;
    if (num >> 4) return 1;
    return 0;
  }
}
void insertMin(bool optimize_small, std::vector<std::byte>& index, uint32_t num) {
  if (optimize_small) {
    switch (stateOf(optimize_small, num)) {
      case 0:
        break;
      case 1:
        insert4(index, static_cast<uint8_t>(num));
        break;
      case 2:
        insert4(index, static_cast<uint8_t>(num));
        insert4(index, static_cast<uint8_t>(num >> 4));
        break;
      case 3:
        insert4(index, static_cast<uint8_t>(num));
        insert4(index, static_cast<uint8_t>(num >> 4));
        insert4(index, static_cast<uint8_t>(num >> 8));
        insert4(index, static_cast<uint8_t>(num >> 12));
        insert4(index, static_cast<uint8_t>(num >> 16));
        insert4(index, static_cast<uint8_t>(num >> 20));
        break;
      default: ;
    }
  } else {
    switch (stateOf(optimize_small, num)) {
      case 0:
        insert4(index, static_cast<uint8_t>(num));
        break;
      case 1:
        insert4(index, static_cast<uint8_t>(num));
        insert4(index, static_cast<uint8_t>(num >> 4));
        break;
      case 2:
        insert4(index, static_cast<uint8_t>(num));
        insert4(index, static_cast<uint8_t>(num >> 4));
        insert4(index, static_cast<uint8_t>(num >> 8));
        insert4(index, static_cast<uint8_t>(num >> 12));
        break;
      case 3:
        insert4(index, static_cast<uint8_t>(num));
        insert4(index, static_cast<uint8_t>(num >> 4));
        insert4(index, static_cast<uint8_t>(num >> 8));
        insert4(index, static_cast<uint8_t>(num >> 12));
        insert4(index, static_cast<uint8_t>(num >> 16));
        insert4(index, static_cast<uint8_t>(num >> 20));
        break;
      default: ;
    }
  }
}

void readMin(bool optimize_small, uint8_t*& data, uint32_t& num, const uint8_t size) {
  if (optimize_small) {
    switch (size) {
      case 0:
        break;
      case 1:
        num += read4(data);
        break;
      case 2:
        num += read4(data);
        num += read4(data) << 4;
        break;
      case 3:
        num += read4(data);
        num += read4(data) << 4;
        num += read4(data) << 8;
        num += read4(data) << 12;
        num += read4(data) << 16;
        num += read4(data) << 20;
        break;
      default : ;
    }
  } else {
    switch (size) {
      case 0:
        num += read4(data);
        break;
      case 1:
        num += read4(data);
        num += read4(data) << 4;
        break;
      case 2:
        num += read4(data);
        num += read4(data) << 4;
        num += read4(data) << 8;
        num += read4(data) << 12;
        break;
      case 3:
        num += read4(data);
        num += read4(data) << 4;
        num += read4(data) << 8;
        num += read4(data) << 12;
        num += read4(data) << 16;
        num += read4(data) << 20;
        break;
      default : ;
    }
  }
}

std::vector<std::byte> buildFull(Statistics stats, size_t streak_threshold, size_t num_table_threshold, bool USE_MIN, bool USE_DIFF_DICT, bool optimize_small) {
  if (USE_DIFF_DICT)
    USE_MIN = false;
  auto index = std::vector<std::byte>(1);
    index[0] = std::bit_cast<std::byte>(FULL_TABLE);

    std::unordered_set<uint32_t> streak_nums;
    bool use_streak = false;
    {
      uint32_t numStreaks = 0;
      uint32_t last = 0;
      std::vector<uint32_t> streakCounts;
      uint32_t streakStart;
      index.emplace_back(static_cast<std::byte>(0));
      index.emplace_back(static_cast<std::byte>(0));
      for (auto& [entry, count]: stats) {
        if (entry == last + 1) {
          streakCounts.emplace_back(count);
        } else {
          if (streakCounts.size() >= streak_threshold) {
            insertCompr(optimize_small, index, streakStart);
            insertCompr3(index, last - streakStart);
            for (size_t i = streakStart; i <= last; ++i)
              streak_nums.emplace(i);

            state = false;
            for (size_t i = 0; i < streakCounts.size(); i += 2) {
              uint32_t count1 = streakCounts[i] - 1;
              uint32_t count2 = streakCounts.size() - i > 1 ? streakCounts[i + 1] - 1 : 0;

              insert4(index, static_cast<uint8_t>(stateOf(optimize_small, count1) | stateOf(optimize_small, count2) << 2));
              insertMin(optimize_small, index, count1);
              if (streakCounts.size() - i > 1) insertMin(optimize_small, index, count2);
            }
            flush4(index);

            use_streak = true;
            ++numStreaks;
          }
          streakCounts.clear();
          streakCounts.emplace_back(count);
          streakStart = entry;
        }
        last = entry;
      }
      if (use_streak) {
        index[0] = std::bit_cast<std::byte>(STREAKS);
        index[1] = std::bit_cast<std::byte>(static_cast<uint8_t>(numStreaks));
        index[2] = std::bit_cast<std::byte>(static_cast<uint8_t>(numStreaks >> 8));
      } else
        index.resize(1);
    }

    uint32_t maxCount = 0;
    std::unordered_map<uint32_t, std::vector<uint32_t>> counts;
    for (auto &[entry, count]: stats) {
      if (streak_nums.contains(entry)) continue;
      maxCount = std::max(maxCount, count);
      counts[count].emplace_back(entry);
    }

    std::vector<uint32_t> mdiffs;
    uint32_t bigMin = std::numeric_limits<uint32_t>::max();
    uint32_t amdiff = std::numeric_limits<uint32_t>::max();
    std::set<uint32_t> allthediffs;
    for (auto& [count, entries]: counts) {
      uint32_t last = 0;
      uint32_t mdiff = std::numeric_limits<uint32_t>::max();
      for (uint32_t entry : entries) {
        const uint32_t diff = entry - last;
        allthediffs.emplace(diff);
        mdiff = std::min(mdiff, diff);
        last = entry;
      }
      if (entries.size() <= num_table_threshold) {
        bigMin = std::min(mdiff, bigMin);
      } else {
        mdiffs.emplace_back(mdiff);
      }
      amdiff = std::min(mdiff, amdiff);
    }

    if (use_streak)
      index[0] = std::bit_cast<std::byte>(STREAKS);

    std::unordered_map<uint32_t, uint32_t> diffdict;
    if (USE_DIFF_DICT) {
      insertCompr3(index, allthediffs.size());
      uint32_t last = 0;
      uint32_t i = 0;
      for (uint32_t diff: allthediffs) {
        insertCompr(true, index, diff - last);
        last = diff;
        diffdict[diff] = i;
        ++i;
      }
    }

    auto minIt = mdiffs.begin();
    for (auto& [count, entries]: counts) {
      if (entries.size() <= num_table_threshold) continue;
      uint32_t mdiff = *(minIt++);
      const size_t entrSize = entries.size();
      insertCompr3(index, entrSize);
      bool use_min = mdiff > MIN_DIFF_THRESHOLD && USE_MIN;
      insertComprTagged(index, count, use_min);
      if (use_min)
        insertCompr(optimize_small, index, mdiff);
      else
        mdiff = 0;

      uint32_t last = 0;
      for (uint32_t entry : entries) {
        const uint32_t diff = entry - last - mdiff;
        if (USE_DIFF_DICT)
          insertCompr(true, index, diffdict[diff]);
        else
          insertCompr(optimize_small, index, diff);
        last = entry;
      }
    }

    // Indicator for end of compressed
    size_t size_compr = index.size();
    index.emplace_back(static_cast<std::byte>(0));

    std::map<uint32_t, uint32_t> rareNums;
    for (auto& [count, entries]: counts) {
      if (entries.size() > num_table_threshold) continue;

      for (auto& entry: entries) {
        rareNums[entry] = count;
      }
    }

    uint32_t last = 0;
    for (auto& [entry, count]: rareNums) {
      insertCompr(optimize_small, index, entry - last - 1);
      last = entry;
      insertCompr3(index, count);
    }
    double bpe = static_cast<double>(index.size())/static_cast<double>(stats.size());
    if (DEBUG)
      std::cout << ""Count: "" << stats.size()
        << "" Size: "" << index.size()
        << "" - b/e: "" << bpe
        << "" - compr: "" << size_compr
        << "" dict: "" << index.size() - size_compr
        << "" - buckets: "" << counts.size()
        << std::endl;

    if (optimize_small)
      index[0] = std::bit_cast<std::byte>(static_cast<uint8_t>(0x80 | std::bit_cast<uint8_t>(index[0])));
    if (USE_DIFF_DICT)
      index[0] = std::bit_cast<std::byte>(static_cast<uint8_t>(0x40 | std::bit_cast<uint8_t>(index[0])));
    return index;
}

std::vector<std::byte> build_idx(std::span<const uint32_t> data, Parameters config) {
  const size_t full_threshold = config.f_a > config.f_s ?
    calcThres(MAX_THRESHOLD, config.f_a, config.f_s) :
    calcThres(MIN_THRESHOLD, config.f_s, config.f_a);

  uint32_t min = std::numeric_limits<uint32_t>::max();
  uint32_t max = std::numeric_limits<uint32_t>::min();
  auto stats = Statistics();
  for (auto val: data) {
    min = std::min(min, val);
    max = std::max(max, val);
    ++stats.try_emplace(val, 0).first->second;
  }

  if (stats.size() < full_threshold || static_cast<double>(config.f_a) / static_cast<double>(config.f_s) >= 8) {
    auto smallest = buildFull(stats, STREAK_THRESHOLD, NUM_TABLE_THRESHOLD, true, false, false);
    {
      auto a = buildFull(stats, STREAK_THRESHOLD, NUM_TABLE_THRESHOLD, false, false, false);
      if (a.size() < smallest.size()) smallest = a;
    }
    {
      auto a = buildFull(stats, STREAK_THRESHOLD, NUM_TABLE_THRESHOLD, false, false, true);
      if (a.size() < smallest.size()) smallest = a;
    }
    {
      auto a = buildFull(stats, STREAK_THRESHOLD, NUM_TABLE_THRESHOLD, true, false, true);
      if (a.size() < smallest.size()) smallest = a;
    }
    {
      auto a = buildFull(stats, STREAK_THRESHOLD, NUM_TABLE_THRESHOLD, false, true, true);
      if (a.size() < smallest.size()) {
        smallest = a;
        std::cout << ""Use diff dict"" << std::endl;
      }
    }
    return smallest;
  } else {
    if (MINMAX_ENABLED) {
      std::vector<std::byte> index(1 + sizeof(headerMinMax));
      index[0] = std::bit_cast<std::byte>(MINMAX);
      headerMinMax *header = std::bit_cast<headerMinMax *>(index.data() + 1);
      header->min = min;
      header->max = max;
      return index;
    } else {
      // Start to analyze data and find the biggest spans
      std::multimap<uint32_t, std::pair<uint32_t, uint32_t> > ranges;
      {
        uint32_t last = max;
        for (const auto &[entry, count]: stats) {
          uint32_t diff = entry - last;
          if (diff > MinSpanDiff) {
            if (last > entry)
              ranges.emplace(diff, std::make_pair(0, entry - 1));
            else
              ranges.emplace(diff, std::make_pair(last + 1, entry - 1));
          }
          last = entry;
        }
      }

      auto index = std::vector<std::byte>();
      index.emplace_back(std::bit_cast<std::byte>(EMPTY_SPANS));

      {
        const uint32_t count = std::min(ranges.size(), MaxEmptySpanCount);
        insertCompr3(index, count);
        size_t i = 0;
        std::set<uint32_t> range_bounds;
        for (auto &[_, span]: std::ranges::reverse_view(ranges)) {
          range_bounds.emplace(span.first);
          range_bounds.emplace(span.second);
          ++i;
          if (i >= count) break;
        }
        assert(range_bounds.size() == count * 2);
        uint32_t last = 0;
        for (uint32_t bound: range_bounds) {
          if (bound == 0)
            bound = max + 1;
          insertCompr(false, index, bound - last);
          last = bound;
        }
      }
      return index;
    }
  }
}

std::optional<size_t> query_idx(uint32_t predicate, const std::vector<std::byte> &index) {
  auto* data = std::bit_cast<uint8_t *>(index.data() + 1);

  const uint8_t information_byte = std::bit_cast<uint8_t>(index[0]);
  bool optimize_small = information_byte & 0x80;
  bool use_diff_dict = information_byte & 0x40;
  switch (std::bit_cast<algorithm>(static_cast<uint8_t>(information_byte & 0xF))) {
    case MINMAX: {
      const auto *header = std::bit_cast<headerMinMax *>(data);
      if (predicate >= header->min && predicate <= header->max)
        return std::nullopt;
      return 0;
    }
    case STREAKS: {
      const uint32_t streak_num = *data | *(data + 1) << 8;
      data += 2;
      for (size_t i = 0; i < streak_num; ++i) {
        uint32_t streakStart = 0;
        readCompr(optimize_small, data, streakStart);
        uint32_t streakEnd = streakStart;
        readCompr3(data, streakEnd);

        uint8_t entrySizes = 0;
        state = false;
        for (size_t j = streakStart; j <= streakEnd; ++j) {
          if ((j-streakStart) % 2 == 0)
            entrySizes = read4(data);

          uint32_t count = 0;
          readMin(optimize_small, data, count, entrySizes & 0x3);
          entrySizes >>= 2;
          if (j == predicate) return count + 1;
        }
      }
    }
    case FULL_TABLE: {
      std::vector<uint32_t> diffdict;
      if (use_diff_dict) {
        uint32_t count = 0;
        readCompr3(data, count);
        uint32_t entry = 0;
        for (size_t i = 0; i < count; ++i) {
          readCompr(true, data, entry);
          diffdict.emplace_back(entry);
        }
      }


      while (true) {
        if (*data == 0) {
          break;
        }
        uint32_t size = 0;
        readCompr3(data, size);
        assert(size <= 131072);
        uint32_t count = 0;
        const bool use_min = readComprTagged(data, count);
        uint32_t mdiff = 0;
        if (use_min)
          readCompr(optimize_small, data, mdiff);

        if (use_diff_dict) {
          uint32_t entry = 0;
          for (size_t i = 0; i < size; i++) {
            uint32_t diffI = 0;
            readCompr(true, data, diffI);
            entry += diffdict[diffI];
            if (entry == predicate) return count;
          }
        } else {
          uint32_t entry = 0;
          for (size_t i = 0; i < size; i++) {
            readCompr(optimize_small, data, entry);
            entry += mdiff;
            if (entry == predicate) return count;
          }
        }
      }
      data++;

      const uint8_t* end = std::bit_cast<uint8_t*>(index.data() + index.size());

      uint32_t entry = 0;
      while (data < end) {
        readCompr(optimize_small, data, entry);
        entry += 1;
        uint32_t count = 0;
        readCompr3(data, count);
        if (entry == predicate) return count;
      }
      return 0;
    }
    case EMPTY_SPANS: {
      auto *data = std::bit_cast<uint8_t *>(index.data() + 1);
      uint32_t count = 0;
      readCompr3(data, count);

      {
        uint32_t entry = 0;
        for (size_t i = 0; i < count; ++i) {
          readCompr(false, data, entry);
          const uint32_t min = entry;
          readCompr(false, data, entry);
          const uint32_t max = entry;
          if (min <= max) {
            if (min <= predicate && predicate <= max) return 0;
          } else {
            if (predicate >= min || predicate <= max) return 0;
          }
        }
      }
      return std::nullopt;
    }
    default: {
      return std::nullopt;
    }
  }
}