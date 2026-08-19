#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>
#include <algorithm>
#include <cmath>
#include <span>
#include <unordered_map>
#include <limits>
#include <utility>
#include <tuple>
#include ""Parameters.hpp""


inline uint32_t load_u32_le(const std::byte* p) { return *reinterpret_cast<const uint32_t*>(p); }
inline uint16_t load_u16_le(const std::byte* p) { return *reinterpret_cast<const uint16_t*>(p); }
inline uint8_t  load_u8(const std::byte* p) { return (uint8_t)(*p); }
inline void     store_u32_le(std::byte* p, uint32_t x) { *reinterpret_cast<uint32_t*>(p) = x; }
inline void     store_u16_le(std::byte* p, uint16_t x) { *reinterpret_cast<uint16_t*>(p) = x; }
inline void     store_u8(std::byte* p, uint8_t x) { *p = std::byte{x}; }
inline uint32_t read_u32_le(const std::vector<std::byte>& buf, size_t off) { return load_u32_le(buf.data() + off); }
inline uint16_t read_u16_le(const std::vector<std::byte>& buf, size_t off) { return load_u16_le(buf.data() + off); }
inline uint8_t  read_u8(const std::vector<std::byte>& buf, size_t off) { return (uint8_t)buf[off]; }

// --- 24-bit little-endian helpers ---
inline uint32_t load_u24_le(const std::byte* p) {
	return (uint32_t(uint8_t(p[0]))) |
	       (uint32_t(uint8_t(p[1])) << 8) |
	       (uint32_t(uint8_t(p[2])) << 16);
}

inline void store_u24_le(std::byte* p, uint32_t x) {
	p[0] = std::byte(x & 0xFF);
	p[1] = std::byte((x >> 8) & 0xFF);
	p[2] = std::byte((x >> 16) & 0xFF);
}

inline void append_u24(std::vector<std::byte>& out, uint32_t x) {
	const size_t off = out.size();
	out.resize(off + 3);
	store_u24_le(out.data() + off, x);
}

inline uint32_t read_u24_le(const std::vector<std::byte>& buf, size_t off) {
	return load_u24_le(buf.data() + off);
}

// helpers to read from uint8_t* using existing loaders
inline uint16_t load_u16_le_p(const uint8_t* p) { return load_u16_le(reinterpret_cast<const std::byte*>(p)); }
inline uint32_t load_u32_le_p(const uint8_t* p) { return load_u32_le(reinterpret_cast<const std::byte*>(p)); }

inline void append_u32(std::vector<std::byte>& out, uint32_t x) {
	const size_t off = out.size();
	out.resize(off + 4);
	store_u32_le(out.data() + off, x);
}

inline void append_u16(std::vector<std::byte>& out, uint16_t x) {
	const size_t off = out.size();
	out.resize(off + 2);
	store_u16_le(out.data() + off, x);
}

inline void append_u8(std::vector<std::byte>& out, uint8_t x) { out.push_back(std::byte{x}); }

inline uint32_t needed_bits32(uint32_t x) {
	if (!x)
		return 0;
#if defined(__GNUC__)
	return 32u - (uint32_t)__builtin_clz(x);
#else
	uint32_t n = 0; while (x) {
		++n;
		x >>= 1;
	} return n;
#endif
}

inline uint32_t popcount8(uint8_t x) {
#if defined(__GNUC__)
	return (uint32_t)__builtin_popcount((unsigned)x);
#else
	uint32_t c = 0; for (int i = 0; i < 8; ++i)
		c += (x >> i) & 1u; return c;
#endif
}

inline uint32_t rank1_upto(const uint8_t* mask, uint32_t, uint32_t end_pos_inclusive) {
	uint32_t bits = end_pos_inclusive + 1;
	uint32_t cnt  = 0, full = bits >> 3;
	for (uint32_t i = 0; i < full; ++i)
		cnt += popcount8(mask[i]);
	uint32_t rem = bits & 7u;
	if (rem) {
		uint8_t last = mask[full] & uint8_t((1u << rem) - 1u);
		cnt += popcount8(last);
	}
	return cnt;
}

// === helper: safe low mask up to 32 bits ===
inline uint32_t low_mask32(uint32_t b) {
	return (b == 32) ? 0xFFFFFFFFu : ((1u << b) - 1u);
}

// === 2-bit tag packing: tag in bits [7:6], bbase in [5:0] ===
enum : uint8_t {
	TAG_NONE = 0,   // no exceptions present
	TAG_BITMAP = 1, // exceptions via bitmap + highs
	TAG_LIST = 2,   // exceptions via list + highs
	TAG_RLE = 3     // PfaTable ONLY: RLE of VALUES (counts), no bitmap/list/highs
};

inline uint8_t pack_btag(uint32_t bbase, uint8_t tag) {
	return uint8_t((bbase & 0x3Fu) | ((uint32_t(tag & 0x3u)) << 6));
}

inline void unpack_btag(uint8_t b, uint32_t& bbase, uint8_t& tag) {
	tag   = uint8_t((b >> 6) & 0x3u);
	bbase = uint32_t(b & 0x3Fu);
}

struct BitWriter {
	std::vector<std::byte>& out;
	uint64_t                bitpos{0};

	void reserve_bits(uint64_t extra_bits) {
		size_t need = size_t((bitpos + extra_bits + 7) >> 3);
		if (out.size() < need)
			out.resize(need, std::byte{0});
	}

	void write(uint32_t v, uint32_t b) {
		if (!b)
			return;
		reserve_bits(b);
		uint64_t bytepos = bitpos >> 3;
		uint32_t bitoff  = uint32_t(bitpos & 7u);
		uint64_t val     = (uint64_t)v << bitoff;
		for (int k = 0; k < 5; ++k) {
			size_t idx = size_t(bytepos + k);
			if (idx >= out.size())
				break;
			uint8_t prev = (uint8_t)out[idx];
			uint8_t add  = uint8_t((val >> (8 * k)) & 0xFFu);
			out[idx]     = std::byte(prev | add);
		}
		bitpos += b;
	}

	void write_many(const uint32_t* vals, uint32_t n, uint32_t b) {
		if (!b || !n)
			return;
		reserve_bits(uint64_t(n) * b);
		for (uint32_t i = 0; i < n; ++i)
			write(vals[i], b);
	}
};

struct BitReader {
	std::span<const std::byte> buf;
	uint64_t                   off_bits{0};

	uint32_t read(uint32_t b) const {
		if (!b)
			return 0;
		size_t   bytepos = off_bits >> 3;
		uint32_t bitoff  = uint32_t(off_bits & 7u);
		uint64_t acc     = 0;
		for (int i = 0; i < 5; ++i) {
			size_t idx = bytepos + i;
			if (idx >= buf.size())
				break;
			acc |= (uint64_t(uint8_t(buf[idx])) << (8 * i));
		}
		acc >>= bitoff;
		uint64_t mask = (b == 32 ? 0xFFFFFFFFull : ((1ull << b) - 1ull));
		return uint32_t(acc & mask);
	}

	uint32_t read_at(uint64_t pos_bits, uint32_t b) const {
		BitReader tmp{buf, pos_bits};
		return tmp.read(b);
	}
};

inline uint64_t mix64(uint64_t x) {
	x ^= x >> 30;
	x *= 0xbf58476d1ce4e5b9ULL;
	x ^= x >> 27;
	x *= 0x94d049bb133111ebULL;
	x ^= x >> 31;
	return x;
}

struct DoubleHash {
	uint64_t h1, h2;

	static DoubleHash for_key(uint32_t v) {
		return {mix64(uint64_t(v) ^ 0x9e3779b97f4a7c15ULL), mix64(uint64_t(v) ^ 0xc2b2ae3d27d4eb4ULL)};
	}

	inline uint64_t at(uint8_t i) const { return h1 + uint64_t(i) * h2; }
};

static constexpr uint32_t BLOOM_MAGIC = 0x304D4C42u;
static constexpr uint32_t kFixedN     = 131072u; // fixed number of values

// === Pack m_bits (21) + k_hash (3) into 24 bits (little-endian) ===
// bits [20:0]  -> m_bits  (0..2,097,151)
// bits [23:21] -> (k_hash-1) (0..7)  representing k_hash in [1..8]
inline uint32_t pack_mbits_k24(uint32_t m_bits, uint8_t k_hash) {
	uint32_t mb = std::min<uint32_t>(m_bits, (1u << 21) - 1u);
	uint8_t  kk = (uint8_t)std::clamp<int>(int(k_hash), 1, 8);
	return (mb & 0x1FFFFFu) | (uint32_t(kk - 1) << 21);
}

inline void unpack_mbits_k24(uint32_t packed24, uint32_t& m_bits, uint8_t& k_hash) {
	m_bits = packed24 & 0x1FFFFFu;
	k_hash = uint8_t(((packed24 >> 21) & 0x7u) + 1);
}

struct BloomParams {
	uint32_t m_bits = 0;
	uint8_t  k_hash = 0; // 1..8
};

inline bool storage_is_cheap(int FS);

inline BloomParams choose_bloom_params_dict(uint32_t ndv, bool aggressive, int FS) {
	if (!ndv || ndv < 256)
		return {0, 0};

	double         bpk_target = aggressive ? 3.2 : ((ndv < 1024) ? 1.8 : (ndv < 8192 ? 2.2 : 2.6));
	const uint32_t m_cap_bits = aggressive ? (24u * 1024u * 8u) : (16u * 1024u * 8u);
	const uint32_t m_min_bits = 512u;
	double         want_bits  = std::ceil(std::min<double>(double(ndv) * bpk_target, double(m_cap_bits)));
	uint32_t       m_bits     = std::max<uint32_t>(m_min_bits, (uint32_t)want_bits);

	double  k_d = (m_bits / double(ndv)) * std::log(2.0);
	uint8_t k   = (uint8_t)std::clamp<int>((int)std::round(k_d + (aggressive ? 0.15 : 0.0)), 1, 8);

	if (storage_is_cheap(FS) && m_bits) {
		uint64_t scaled = uint64_t(std::ceil(1.79 * double(m_bits)));
		m_bits          = (scaled > std::numeric_limits<uint32_t>::max())
			                  ? std::numeric_limits<uint32_t>::max()
			                  : (uint32_t)scaled;
		double kd2 = (m_bits / double(ndv)) * std::log(2.0);
		k          = (uint8_t)std::clamp<int>((int)std::round(kd2 + 0.15), 1, 8);
	}
	return {m_bits, k};
}

struct Bloom {
	uint32_t         m_bits{};
	uint8_t          k{}; // 1..8
	const std::byte* bits{};

	bool maybe_contains(uint32_t v) const {
		if (!m_bits || !k || !bits)
			return true;
		DoubleHash dh = DoubleHash::for_key(v);
		for (uint8_t i = 0; i < k; ++i) {
			uint64_t h   = dh.at(i);
			uint32_t pos = uint32_t(h % m_bits);
			uint8_t  bb  = uint8_t(*(bits + (pos >> 3)));
			if (((bb >> (pos & 7u)) & 1u) == 0u)
				return false;
		}
		return true;
	}

	static void add_to(uint32_t v, uint32_t m_bits, uint8_t k, std::byte* out_bits) {
		if (!k)
			return;
		DoubleHash dh = DoubleHash::for_key(v);
		for (uint8_t i = 0; i < k; ++i) {
			uint64_t h    = dh.at(i);
			uint32_t pos  = uint32_t(h % m_bits);
			size_t   by   = pos >> 3, bt = pos & 7u;
			uint8_t  prev = (uint8_t)out_bits[by];
			out_bits[by]  = std::byte(prev | uint8_t(1u << bt));
		}
	}
};

/* ======================= Hybrid PFOR with three exception layouts ======================= */
/* FINAL LAYOUT:
 *  - Each table uses fixed blocks of kBlock=256 except the last; lengths are implied by ndv/nvals & bidx.
 *  - PfdTable block layout (CHANGED): [v0:4][B:4][btag:1] ...
 *  - TAG_NONE: only lows are written (no e, no mask/list, no highs).
 */

/// ===== Packed size directory helpers (rsum) =====
inline size_t ceil_div8(size_t bits) { return (bits + 7) >> 3; }

struct PackedSizesDir {
	// dir starts at dir_off: [ e:1 byte ][ sizes: nblocks * e bits ]
	static uint8_t read_e(const std::vector<std::byte>& buf, uint32_t dir_off) {
		return load_u8(buf.data() + dir_off);
	}

	static size_t dir_bytes(uint16_t nblocks, uint8_t e) {
		// 1 byte for e + packed sizes
		return size_t(1) + ceil_div8(size_t(nblocks) * e);
	}

	// rsum of sizes[0..bidx-1], i.e., byte offset of block bidx relative to blocks region
	static uint32_t rsum_offset_at(const std::vector<std::byte>& buf,
	                               uint32_t                      dir_off,
	                               uint16_t /*nblocks*/,
	                               uint32_t bidx) {
		if (bidx == 0)
			return 0u;
		uint8_t e = read_e(buf, dir_off);
		if (e == 0)
			return 0u;
		const uint64_t base_bits = (uint64_t(dir_off) + 1ull) * 8ull;
		uint32_t       off       = 0;
		for (uint32_t i = 0; i < bidx; ++i) {
			BitReader tmp{std::span<const std::byte>(buf.data(), buf.size()),
			              base_bits + uint64_t(i) * e};
			off += tmp.read(e);
		}
		return off;
	}
};

struct PfdTable {
	static constexpr uint32_t kBlock = 256u;

	struct Meta {
		uint16_t nblocks{};
		uint32_t dir_off{};
		uint32_t packed_bytes{};
		uint32_t next_off{};
	};

	struct BlockView {
		uint32_t v0{};
		// NEW: store base delta (B) always
		uint32_t       base_delta{};
		uint32_t       bbase{};
		uint32_t       len{};
		size_t         base_off_bits{}; // len is computed by caller
		uint32_t       e{};
		uint8_t        tag{}; // TAG_NONE/TAG_BITMAP/TAG_LIST
		const uint8_t* mask_ptr{};
		uint32_t       mask_bits{};
		size_t         highs_off_bits{};
		uint8_t        exc_cnt{};
		const uint8_t* exc_pos_ptr{};
		size_t         list_highs_off_bits{};
	};

	static void choose_pfd_params(const uint32_t* vals_like,
	                              uint32_t        len,
	                              uint32_t&       b_out,
	                              uint32_t&       e_out,
	                              uint32_t&       exc_cnt_out) {
		if (len <= 1) {
			b_out = e_out = exc_cnt_out = 0;
			return;
		}
		uint32_t n = len - 1, vmax = 0;
		for (uint32_t i = 1; i < len; ++i)
			vmax        = std::max(vmax, vals_like[i]);
		uint32_t bwmax = needed_bits32(vmax);
		if (bwmax == 0) {
			b_out = e_out = exc_cnt_out = 0;
			return;
		}
		uint32_t best_b    = 0, best_e = 0, best_exc = 0;
		uint64_t best_cost = UINT64_MAX;
		uint32_t b_upper   = std::min<uint32_t>(bwmax, 32);
		for (uint32_t b = 0; b <= b_upper; ++b) {
			uint32_t exc = 0, hi_max = 0;
			for (uint32_t i = 1; i < len; ++i) {
				uint32_t v  = vals_like[i];
				uint32_t hi = (b == 32) ? 0u : (v >> b);
				if (hi) {
					++exc;
					hi_max = std::max(hi_max, hi);
				}
			}
			uint32_t e         = needed_bits32(hi_max);
			uint64_t base_bits = uint64_t(n) * b;
			uint64_t mask_bits = (exc == 0) ? 0u : (((n + 7) / 8) * 8);
			uint64_t exc_bits  = uint64_t(exc) * e;
			uint64_t cost      = base_bits + mask_bits + exc_bits + 16;
			if (cost < best_cost) {
				best_cost = cost;
				best_b    = b;
				best_e    = e;
				best_exc  = exc;
			}
			if (exc == 0 && b > 0)
				break;
		}
		b_out       = best_b;
		e_out       = best_e;
		exc_cnt_out = best_exc;
	}

	// ===== ALWAYS-BASE-DELTA write: [v0:4][B:4][btag:1]...
	static void write_block(std::vector<std::byte>& out, const uint32_t* vals, uint32_t len) {
		append_u32(out, vals[0]); // v0

		// Compute deltas and base delta B = min(d[1..n])
		const uint32_t        n = (len > 0 ? len - 1 : 0u);
		uint32_t              B = 0;
		std::vector<uint32_t> residuals(len, 0u);
		if (n > 0) {
			uint32_t              mn = std::numeric_limits<uint32_t>::max();
			std::vector<uint32_t> deltas(len, 0u);
			for (uint32_t j = 1; j < len; ++j) {
				deltas[j] = vals[j] - vals[j - 1];
				if (deltas[j] < mn)
					mn = deltas[j];
			}
			B = (mn == std::numeric_limits<uint32_t>::max() ? 0u : mn);
			for (uint32_t j  = 1; j < len; ++j)
				residuals[j] = deltas[j] - B;
		}
		append_u32(out, B); // ALWAYS store base delta

		// Choose params on residuals
		uint32_t bbase = 0, e = 0, exc_cnt = 0;
		choose_pfd_params(residuals.data(), len, bbase, e, exc_cnt);

		// header byte (bbase|tag)
		const size_t head_off = out.size();
		append_u8(out, 0); // placeholder

		// lows from residuals[1..n]
		if (bbase) {
			BitWriter bw{out, out.size() * 8ull};
			uint32_t  mask = low_mask32(bbase);
			for (uint32_t j = 1; j < len; ++j) {
				uint32_t low = residuals[j] & mask;
				bw.write(low, bbase);
			}
		}

		// collect exceptions
		std::vector<uint8_t>  positions;
		std::vector<uint32_t> highs;
		if (e) {
			positions.reserve(exc_cnt);
			highs.reserve(exc_cnt);
			for (uint32_t j = 0; j < n; ++j) {
				uint32_t r  = residuals[j + 1];
				uint32_t hi = (bbase == 32) ? 0u : (r >> bbase);
				if (hi) {
					positions.push_back((uint8_t)j);
					highs.push_back(hi);
				}
			}
		}

		// TAG_NONE: no exceptions
		if (positions.empty()) {
			out[head_off] = std::byte{pack_btag(bbase, TAG_NONE)};
			return;
		}

		const bool use_list = (positions.size() < 32);
		out[head_off]       = std::byte{pack_btag(bbase, use_list ? TAG_LIST : TAG_BITMAP)};

		if (!use_list) {
			const uint32_t mask_bytes = (n + 7) / 8;
			size_t         mask_off   = out.size();
			out.resize(out.size() + mask_bytes, std::byte{0});
			for (uint8_t p : positions) {
				size_t  by         = p >> 3, bt = p & 7u;
				uint8_t prev       = (uint8_t)out[mask_off + by];
				out[mask_off + by] = std::byte(prev | uint8_t(1u << bt));
			}
			append_u8(out, (uint8_t)e);
			if (e && !highs.empty()) {
				BitWriter bw{out, out.size() * 8ull};
				for (uint32_t h : highs)
					bw.write(h, e);
			}
		} else {
			append_u8(out, (uint8_t)positions.size());
			for (uint8_t p : positions)
				append_u8(out, p);
			append_u8(out, (uint8_t)e);
			if (e && !highs.empty()) {
				BitWriter bw{out, out.size() * 8ull};
				for (uint32_t h : highs)
					bw.write(h, e);
			}
		}
	}

	// ===== write with packed-size directory (rsum) =====
	static Meta write(std::vector<std::byte>& out, std::span<const uint32_t> values_sorted) {
		const uint32_t ndv     = (uint32_t)values_sorted.size();
		const uint16_t nblocks = (uint16_t)((ndv + kBlock - 1) / kBlock);

		// 1) Build blocks into a temp buffer while recording sizes (bytes)
		std::vector<std::byte> blocks_buf;
		blocks_buf.reserve(1024);

		std::vector<uint32_t> block_sizes;
		block_sizes.reserve(nblocks);

		uint32_t idx = 0;
		for (uint16_t b = 0; b < nblocks; ++b) {
			const uint32_t begin  = idx;
			const uint32_t remain = ndv - begin;
			const uint32_t len    = std::min<uint32_t>(kBlock, remain);
			const size_t   before = blocks_buf.size();
			write_block(blocks_buf, values_sorted.data() + begin, len);
			const size_t after = blocks_buf.size();
			block_sizes.push_back((uint32_t)(after - before));
			idx += len;
		}

		// 2) Compute e for packed sizes
		uint32_t max_sz = 0;
		for (uint32_t s : block_sizes)
			max_sz = std::max(max_sz, s);
		const uint8_t e = (uint8_t)needed_bits32(max_sz); // 0 if nblocks==0

		// 3) Emit header + dir + blocks to 'out'
		append_u16(out, nblocks);

		const uint32_t packed_bytes = (uint32_t)blocks_buf.size();
		append_u32(out, packed_bytes);

		const uint32_t dir_off = (uint32_t)out.size();
		append_u8(out, e); // e

		if (e) {
			BitWriter bw{out, out.size() * 8ull};
			for (uint32_t s : block_sizes)
				bw.write(s, e);
		}

		const uint32_t blocks_off = (uint32_t)out.size();
		out.insert(out.end(), blocks_buf.begin(), blocks_buf.end());

		Meta m{};
		m.nblocks      = nblocks;
		m.dir_off      = dir_off;
		m.packed_bytes = packed_bytes;
		m.next_off     = blocks_off + packed_bytes;
		return m;
	}

	// ===== read_meta that accounts for packed-size dir =====
	static std::optional<Meta> read_meta(const std::vector<std::byte>& buf, size_t off) {
		if (buf.size() < off + 6)
			return std::nullopt;
		Meta m{};
		m.nblocks      = load_u16_le(buf.data() + off + 0);
		m.packed_bytes = load_u32_le(buf.data() + off + 2);
		m.dir_off      = (uint32_t)(off + 6);

		if (buf.size() < m.dir_off + 1)
			return std::nullopt;
		const uint8_t e = load_u8(buf.data() + m.dir_off);

		const size_t   dir_bytes  = PackedSizesDir::dir_bytes(m.nblocks, e);
		const uint32_t blocks_off = (uint32_t)(m.dir_off + dir_bytes);

		if (buf.size() < blocks_off + m.packed_bytes)
			return std::nullopt;

		m.next_off = blocks_off + m.packed_bytes;
		return m;
	}

	// ===== block_offset computed from rsum of packed sizes =====
	static uint32_t block_offset(const std::vector<std::byte>& buf, const Meta& m, uint32_t bidx) {
		const uint8_t  e          = load_u8(buf.data() + m.dir_off);
		const size_t   dir_bytes  = PackedSizesDir::dir_bytes(m.nblocks, e);
		const uint32_t blocks_off = (uint32_t)(m.dir_off + dir_bytes);
		const uint32_t rsum       = PackedSizesDir::rsum_offset_at(buf, m.dir_off, m.nblocks, bidx);
		return blocks_off + rsum;
	}

	// parse uses ndv & bidx to imply len
	// NEW layout: [v0:4][B:4][btag:1]...
	static bool parse_block(const std::vector<std::byte>& buf, size_t off, uint32_t ndv, uint32_t bidx, BlockView& bv) {
		if (buf.size() < off + 9)
			return false; // v0(4) + B(4) + btag(1)
		bv.v0         = load_u32_le(buf.data() + off + 0);
		bv.base_delta = load_u32_le(buf.data() + off + 4);
		uint8_t btag  = load_u8(buf.data() + off + 8);
		unpack_btag(btag, bv.bbase, bv.tag);

		uint32_t begin  = bidx * kBlock;
		uint32_t remain = (ndv > begin ? ndv - begin : 0);
		bv.len          = std::min<uint32_t>(kBlock, remain);
		if (bv.len == 0)
			return false;

		uint32_t n        = bv.len - 1;
		size_t   cur      = off + 9;
		bv.base_off_bits  = cur * 8ull;
		size_t base_bytes = (size_t(n) * bv.bbase + 7) / 8;
		cur += base_bytes;

		if (bv.tag == TAG_NONE) {
			bv.e                   = 0;
			bv.mask_ptr            = nullptr;
			bv.mask_bits           = 0;
			bv.highs_off_bits      = 0;
			bv.exc_cnt             = 0;
			bv.exc_pos_ptr         = nullptr;
			bv.list_highs_off_bits = 0;
			return true;
		} else if (bv.tag == TAG_BITMAP) {
			size_t mask_bytes = (n + 7) / 8;
			if (buf.size() < cur + mask_bytes + 1)
				return false;
			bv.mask_ptr  = reinterpret_cast<const uint8_t*>(buf.data() + cur);
			bv.mask_bits = n;
			cur += mask_bytes;
			bv.e = load_u8(buf.data() + cur);
			cur += 1;
			bv.highs_off_bits = cur * 8ull;
			return true;
		} else if (bv.tag == TAG_LIST) {
			if (buf.size() < cur + 1)
				return false;
			bv.exc_cnt = load_u8(buf.data() + cur);
			cur += 1;
			if (buf.size() < cur + bv.exc_cnt + 1)
				return false;
			bv.exc_pos_ptr = reinterpret_cast<const uint8_t*>(buf.data() + cur);
			cur += bv.exc_cnt;
			bv.e = load_u8(buf.data() + cur);
			cur += 1;
			bv.list_highs_off_bits = cur * 8ull;
			return true;
		}
		return false;
	}

	static uint32_t read_delta_t(const std::vector<std::byte>& buf, const BlockView& bv, uint32_t t) {
		// return FULL delta d[t] = B + residual_at(t)
		BitReader br{std::span<const std::byte>(buf.data(), buf.size()), bv.base_off_bits + uint64_t(t - 1) * bv.bbase};
		uint32_t  low      = (bv.bbase ? br.read(bv.bbase) : 0u);
		uint32_t  residual = low;
		if (bv.tag == TAG_NONE)
			return bv.base_delta + residual;

		uint32_t p = t - 1;
		if (bv.tag == TAG_BITMAP) {
			uint32_t by     = p >> 3, bt = p & 7u;
			bool     is_exc = ((bv.mask_ptr[by] >> bt) & 1u) != 0;
			if (!is_exc || bv.e == 0)
				return bv.base_delta + residual;
			uint32_t  rank = rank1_upto(bv.mask_ptr, bv.mask_bits, p);
			BitReader brh{std::span<const std::byte>(buf.data(), buf.size()),
			              bv.highs_off_bits + uint64_t(rank - 1) * bv.e};
			uint32_t high = brh.read(bv.e);
			residual      = (high << bv.bbase) | low;
			return bv.base_delta + residual;
		} else {
			if (bv.exc_cnt == 0 || bv.e == 0)
				return bv.base_delta + residual;
			uint32_t k = UINT32_MAX;
			for (uint32_t i = 0; i < bv.exc_cnt; ++i) {
				if (bv.exc_pos_ptr[i] == p) {
					k = i;
					break;
				}
			}
			if (k == UINT32_MAX)
				return bv.base_delta + residual;
			BitReader brh{std::span<const std::byte>(buf.data(), buf.size()),
			              bv.list_highs_off_bits + uint64_t(k) * bv.e};
			uint32_t high = brh.read(bv.e);
			residual      = (high << bv.bbase) | low;
			return bv.base_delta + residual;
		}
	}

	static uint32_t value_at_local(const std::vector<std::byte>& buf, const BlockView& bv, uint32_t local_idx) {
		if (local_idx == 0)
			return bv.v0;
		uint32_t sum = 0;
		for (uint32_t t = 1; t <= local_idx; ++t)
			sum += read_delta_t(buf, bv, t);
		return bv.v0 + sum;
	}

	static uint32_t value_at(const std::vector<std::byte>& buf, const Meta& m, uint32_t ndv, uint32_t i) {
		uint32_t  bidx  = i / kBlock;
		uint32_t  local = i % kBlock;
		uint32_t  off   = block_offset(buf, m, bidx);
		BlockView bv{};
		if (!parse_block(buf, off, ndv, bidx, bv))
			return 0;
		return value_at_local(buf, bv, local);
	}
};

// ======= Value-RLE helpers for PfaTable =======
inline void rle_values_build(const uint32_t*        vals,
                             uint32_t               len,
                             std::vector<uint32_t>& run_vals,
                             std::vector<uint32_t>& run_lens,
                             uint32_t&              max_val,
                             uint32_t&              max_len) {
	run_vals.clear();
	run_lens.clear();
	max_val = 0;
	max_len = 0;
	if (len == 0)
		return;
	uint32_t v = vals[0], l = 1;
	max_val    = std::max(max_val, v);
	for (uint32_t i = 1; i < len; ++i) {
		if (vals[i] == v) { ++l; } else {
			run_vals.push_back(v);
			run_lens.push_back(l);
			max_len = std::max(max_len, l);
			v       = vals[i];
			l       = 1;
			max_val = std::max(max_val, v);
		}
	}
	run_vals.push_back(v);
	run_lens.push_back(l);
	max_len = std::max(max_len, l);
}

inline uint8_t width_u8u16u32(uint32_t maxv) {
	if (maxv <= std::numeric_limits<uint8_t>::max())
		return 1;
	if (maxv <= std::numeric_limits<uint16_t>::max())
		return 2;
	return 4;
}

inline void write_words_le(std::vector<std::byte>& out, const std::vector<uint32_t>& xs, uint8_t w) {
	size_t base = out.size();
	out.resize(base + xs.size() * w);
	std::byte* p = out.data() + base;
	for (uint32_t v : xs) {
		if (w == 1) {
			store_u8(p, (uint8_t)v);
			p += 1;
		} else if (w == 2) {
			store_u16_le(p, (uint16_t)v);
			p += 2;
		} else {
			store_u32_le(p, v);
			p += 4;
		}
	}
}

struct PfaTable {
	static constexpr uint32_t kBlock = 256u;

	struct Meta {
		uint16_t nblocks{};
		uint32_t dir_off{};
		uint32_t packed_bytes{};
		uint32_t next_off{};
	};

	struct BlockParse {
		uint32_t       len{};
		uint32_t       bbase{};
		size_t         lows_off_bits{};
		uint8_t        tag{}; // TAG_NONE/TAG_BITMAP/TAG_LIST/TAG_RLE
		uint32_t       e{};
		const uint8_t* mask_ptr{};
		uint32_t       mask_bits{};
		size_t         highs_off_bits{};
		uint8_t        exc_cnt{};
		const uint8_t* exc_pos_ptr{};
		size_t         list_highs_off_bits{};

		// RLE-of-values (counts)
		const uint8_t* rle_vals_ptr{};
		const uint8_t* rle_lens_ptr{};
		uint16_t       rle_nruns{};
		uint8_t        rle_vw{};
		uint8_t        rle_lw{};
	};

	static void choose_params_abs(const uint32_t* vals,
	                              uint32_t        len,
	                              uint32_t&       b_out,
	                              uint32_t&       e_out,
	                              uint32_t&       exc_cnt_out) {
		if (len == 0) {
			b_out = e_out = exc_cnt_out = 0;
			return;
		}
		uint32_t vmax = 0;
		for (uint32_t i = 0; i < len; ++i)
			vmax        = std::max(vmax, vals[i]);
		uint32_t bwmax = needed_bits32(vmax);
		if (bwmax == 0) {
			b_out = e_out = exc_cnt_out = 0;
			return;
		}
		uint32_t best_b    = 0, best_e = 0, best_exc = 0;
		uint64_t best_cost = UINT64_MAX;
		uint32_t b_upper   = std::min<uint32_t>(bwmax, 32);
		for (uint32_t b = 0; b <= b_upper; ++b) {
			uint32_t exc = 0, hi_max = 0;
			for (uint32_t i = 0; i < len; ++i) {
				uint32_t v  = vals[i];
				uint32_t hi = (b == 32) ? 0u : (v >> b);
				if (hi) {
					++exc;
					hi_max = std::max(hi_max, hi);
				}
			}
			uint32_t e         = needed_bits32(hi_max);
			uint64_t base_bits = uint64_t(len) * b;
			uint64_t mask_bits = (exc == 0) ? 0u : (((len + 7) / 8) * 8);
			uint64_t exc_bits  = uint64_t(exc) * e;
			uint64_t cost      = base_bits + mask_bits + exc_bits + 16;
			if (cost < best_cost) {
				best_cost = cost;
				best_b    = b;
				best_e    = e;
				best_exc  = exc;
			}
			if (exc == 0 && b > 0)
				break;
		}
		b_out       = best_b;
		e_out       = best_e;
		exc_cnt_out = best_exc;
	}

	// ===== REFACTORED: write_block that can choose PACK vs VALUE-RLE =====
	static void write_block(std::vector<std::byte>& out, const uint32_t* vals, uint32_t len) {
		// 1) Compute parameters & exception positions for PACK path (no output yet)
		uint32_t bbase = 0, e = 0, exc_cnt = 0;
		choose_params_abs(vals, len, bbase, e, exc_cnt);

		std::vector<uint8_t> positions; // indices with high!=0
		if (e) {
			positions.reserve(exc_cnt);
			for (uint32_t i = 0; i < len; ++i) {
				uint32_t hi = (bbase == 32) ? 0u : (vals[i] >> bbase);
				if (hi)
					positions.push_back((uint8_t)i);
			}
		}

		// 2) Cost PACK (with best of LIST/BITMAP) without emitting
		uint64_t cost_pack = 1 /*tag+bb*/ + ((uint64_t)len * bbase + 7) / 8; // lows
		if (e) {
			const uint32_t mask_bytes  = (len + 7) / 8;
			const uint64_t highs_bits  = uint64_t(positions.size()) * e;
			uint64_t       cost_list   = 1 /*exc_cnt*/ + positions.size() /*pos*/ + 1 /*e*/ + ((highs_bits + 7) / 8);
			uint64_t       cost_bitmap = mask_bytes + 1 /*e*/ + ((highs_bits + 7) / 8);
			cost_pack += std::min(cost_list, cost_bitmap);
		}

		// 3) Build VALUE-RLE and cost it
		std::vector<uint32_t> run_vals,    run_lens;
		uint32_t              max_val = 0, max_len = 0;
		rle_values_build(vals, len, run_vals, run_lens, max_val, max_len);
		uint8_t  vw       = width_u8u16u32(max_val);
		uint8_t  lw       = width_u8u16u32(max_len);
		uint64_t cost_rle = 1 /*tag*/ + 1 /*vw*/ + 1 /*lw*/ + 2 /*nruns*/
		                    + uint64_t(run_vals.size()) * (vw + lw);

		// 4) Emit best
		if (cost_rle < cost_pack) {
			// TAG_RLE (values): [tag][vw:1][lw:1][nruns:2][run_vals][run_lens]
			append_u8(out, pack_btag(0 /*unused*/, TAG_RLE));
			append_u8(out, vw);
			append_u8(out, lw);
			append_u16(out, (uint16_t)run_vals.size());
			write_words_le(out, run_vals, vw);
			write_words_le(out, run_lens, lw);
			return;
		}

		// PACK path: emit as original
		const size_t head_off = out.size();
		append_u8(out, 0); // placeholder (bbase|tag)

		// lows
		if (bbase) {
			BitWriter bw{out, out.size() * 8ull};
			uint32_t  mask = low_mask32(bbase);
			for (uint32_t i = 0; i < len; ++i) {
				uint32_t low = vals[i] & mask;
				bw.write(low, bbase);
			}
		}

		// no highs?
		if (positions.empty()) {
			out[head_off] = std::byte{pack_btag(bbase, TAG_NONE)};
			return;
		}

		// choose list vs bitmap
		const bool use_list = (positions.size() < 32);
		out[head_off]       = std::byte{pack_btag(bbase, use_list ? TAG_LIST : TAG_BITMAP)};

		if (!use_list) {
			const uint32_t mask_bytes = (len + 7) / 8;
			size_t         mask_off   = out.size();
			out.resize(out.size() + mask_bytes, std::byte{0});
			for (uint8_t p : positions) {
				size_t  by         = p >> 3, bt = p & 7u;
				uint8_t prev       = (uint8_t)out[mask_off + by];
				out[mask_off + by] = std::byte(prev | uint8_t(1u << bt));
			}
			append_u8(out, (uint8_t)e);
			if (e) {
				BitWriter bw{out, out.size() * 8ull};
				for (uint32_t i = 0; i < len; ++i) {
					uint32_t hi = (bbase == 32) ? 0u : (vals[i] >> bbase);
					if (hi)
						bw.write(hi, e);
				}
			}
		} else {
			append_u8(out, (uint8_t)positions.size());
			for (uint8_t p : positions)
				append_u8(out, p);
			append_u8(out, (uint8_t)e);
			if (e) {
				BitWriter bw{out, out.size() * 8ull};
				for (uint32_t i = 0; i < len; ++i) {
					uint32_t hi = (bbase == 32) ? 0u : (vals[i] >> bbase);
					if (hi)
						bw.write(hi, e);
				}
			}
		}
	}

	// ===== write with packed-size directory (rsum) =====
	static Meta write(std::vector<std::byte>& out, std::span<const uint32_t> values) {
		const uint32_t nvals   = (uint32_t)values.size();
		const uint16_t nblocks = (uint16_t)((nvals + kBlock - 1) / kBlock);

		std::vector<std::byte> blocks_buf;
		blocks_buf.reserve(1024);

		std::vector<uint32_t> block_sizes;
		block_sizes.reserve(nblocks);

		uint32_t idx = 0;
		for (uint16_t b = 0; b < nblocks; ++b) {
			const uint32_t remain = nvals - idx;
			const uint32_t len    = std::min<uint32_t>(kBlock, remain);
			const size_t   before = blocks_buf.size();
			write_block(blocks_buf, values.data() + idx, len);
			const size_t after = blocks_buf.size();
			block_sizes.push_back((uint32_t)(after - before));
			idx += len;
		}

		uint32_t max_sz = 0;
		for (uint32_t s : block_sizes)
			max_sz = std::max(max_sz, s);
		const uint8_t e = (uint8_t)needed_bits32(max_sz);

		append_u16(out, nblocks);

		const uint32_t packed_bytes = (uint32_t)blocks_buf.size();
		append_u32(out, packed_bytes);

		const uint32_t dir_off = (uint32_t)out.size();
		append_u8(out, e);
		if (e) {
			BitWriter bw{out, out.size() * 8ull};
			for (uint32_t s : block_sizes)
				bw.write(s, e);
		}

		const uint32_t blocks_off = (uint32_t)out.size();
		out.insert(out.end(), blocks_buf.begin(), blocks_buf.end());

		Meta m{};
		m.nblocks      = nblocks;
		m.dir_off      = dir_off;
		m.packed_bytes = packed_bytes;
		m.next_off     = blocks_off + packed_bytes;
		return m;
	}

	// ===== read_meta that accounts for packed-size dir =====
	static std::optional<Meta> read_meta(const std::vector<std::byte>& buf, size_t off) {
		if (buf.size() < off + 6)
			return std::nullopt;
		Meta m{};
		m.nblocks      = load_u16_le(buf.data() + off + 0);
		m.packed_bytes = load_u32_le(buf.data() + off + 2);
		m.dir_off      = (uint32_t)(off + 6);

		if (buf.size() < m.dir_off + 1)
			return std::nullopt;
		const uint8_t e = load_u8(buf.data() + m.dir_off);

		const size_t   dir_bytes  = PackedSizesDir::dir_bytes(m.nblocks, e);
		const uint32_t blocks_off = (uint32_t)(m.dir_off + dir_bytes);

		if (buf.size() < blocks_off + m.packed_bytes)
			return std::nullopt;

		m.next_off = blocks_off + m.packed_bytes;
		return m;
	}

	// ===== block_offset computed from rsum of packed sizes =====
	static uint32_t block_offset(const std::vector<std::byte>& buf, const Meta& m, uint32_t bidx) {
		const uint8_t  e          = load_u8(buf.data() + m.dir_off);
		const size_t   dir_bytes  = PackedSizesDir::dir_bytes(m.nblocks, e);
		const uint32_t blocks_off = (uint32_t)(m.dir_off + dir_bytes);
		const uint32_t rsum       = PackedSizesDir::rsum_offset_at(buf, m.dir_off, m.nblocks, bidx);
		return blocks_off + rsum;
	}

	static uint32_t value_at_local(const std::vector<std::byte>& buf, const BlockParse& bp, uint32_t local_idx) {
		if (bp.tag == TAG_RLE) {
			uint32_t       idx = local_idx;
			const uint8_t* pv  = bp.rle_vals_ptr;
			const uint8_t* pl  = bp.rle_lens_ptr;
			for (uint32_t r = 0; r < bp.rle_nruns; ++r) {
				uint32_t v = (bp.rle_vw == 1) ? *pv : (bp.rle_vw == 2) ? load_u16_le_p(pv) : load_u32_le_p(pv);
				pv += bp.rle_vw;

				uint32_t l = (bp.rle_lw == 1) ? *pl : (bp.rle_lw == 2) ? load_u16_le_p(pl) : load_u32_le_p(pl);
				pl += bp.rle_lw;

				if (idx < l)
					return v; // counts_m1 value
				idx -= l;
			}
			return 0; // malformed fallback
		}

		uint32_t low = 0;
		if (bp.bbase) {
			BitReader br{std::span<const std::byte>(buf.data(), buf.size()),
			             bp.lows_off_bits + uint64_t(local_idx) * bp.bbase};
			low = br.read(bp.bbase);
		}
		if (bp.tag == TAG_NONE) {
			return low;
		} else if (bp.tag == TAG_BITMAP) {
			uint32_t pos    = local_idx;
			uint32_t by     = pos >> 3, bt = pos & 7u;
			bool     is_exc = ((bp.mask_ptr[by] >> bt) & 1u) != 0;
			if (!is_exc)
				return low;
			uint32_t rank = rank1_upto(bp.mask_ptr, bp.mask_bits, pos);
			uint32_t hi   = 0;
			if (bp.e) {
				BitReader brh{std::span<const std::byte>(buf.data(), buf.size()),
				              bp.highs_off_bits + uint64_t(rank - 1) * bp.e};
				hi = brh.read(bp.e);
			}
			return (hi << bp.bbase) | low;
		} else {
			// TAG_LIST
			if (bp.exc_cnt == 0 || bp.e == 0)
				return low;
			uint32_t k = UINT32_MAX;
			for (uint32_t i = 0; i < bp.exc_cnt; ++i) {
				if (bp.exc_pos_ptr[i] == local_idx) {
					k = i;
					break;
				}
			}
			if (k == UINT32_MAX)
				return low;
			BitReader brh{std::span<const std::byte>(buf.data(), buf.size()),
			              bp.list_highs_off_bits + uint64_t(k) * bp.e};
			uint32_t hi = brh.read(bp.e);
			return (hi << bp.bbase) | low;
		}
	}

	static bool parse_block(const std::vector<std::byte>& buf,
	                        size_t                        off,
	                        uint32_t                      nvals,
	                        uint32_t                      bidx,
	                        BlockParse&                   bp) {
		if (buf.size() < off + 1)
			return false;
		uint8_t btag = load_u8(buf.data() + off + 0);
		unpack_btag(btag, bp.bbase, bp.tag);

		uint32_t begin  = bidx * kBlock;
		uint32_t remain = (nvals > begin ? nvals - begin : 0);
		bp.len          = std::min<uint32_t>(kBlock, remain);
		if (bp.len == 0)
			return false;

		size_t cur = off + 1;

		if (bp.tag == TAG_NONE) {
			bp.e                   = 0;
			bp.exc_cnt             = 0;
			bp.mask_ptr            = nullptr;
			bp.exc_pos_ptr         = nullptr;
			bp.mask_bits           = 0;
			bp.highs_off_bits      = 0;
			bp.list_highs_off_bits = 0;
			bp.lows_off_bits       = cur * 8ull;
			// lows follow directly; compute and skip lows bytes
			size_t lows_bytes = (size_t(bp.len) * bp.bbase + 7) / 8;
			cur += lows_bytes;
			return true;
		} else if (bp.tag == TAG_BITMAP) {
			bp.lows_off_bits  = cur * 8ull;
			size_t lows_bytes = (size_t(bp.len) * bp.bbase + 7) / 8;
			cur += lows_bytes;
			size_t mask_bytes = (bp.len + 7) / 8;
			if (buf.size() < cur + mask_bytes + 1)
				return false;
			bp.mask_ptr  = reinterpret_cast<const uint8_t*>(buf.data() + cur);
			bp.mask_bits = bp.len;
			cur += mask_bytes;
			bp.e = load_u8(buf.data() + cur);
			cur += 1;
			bp.highs_off_bits = cur * 8ull;
			return true;
		} else if (bp.tag == TAG_LIST) {
			bp.lows_off_bits  = cur * 8ull;
			size_t lows_bytes = (size_t(bp.len) * bp.bbase + 7) / 8;
			cur += lows_bytes;
			if (buf.size() < cur + 1)
				return false;
			bp.exc_cnt = load_u8(buf.data() + cur);
			cur += 1;
			if (buf.size() < cur + bp.exc_cnt + 1)
				return false;
			bp.exc_pos_ptr = reinterpret_cast<const uint8_t*>(buf.data() + cur);
			cur += bp.exc_cnt;
			bp.e = load_u8(buf.data() + cur);
			cur += 1;
			bp.list_highs_off_bits = cur * 8ull;
			return true;
		} else if (bp.tag == TAG_RLE) {
			// Layout: [tag][vw:1][lw:1][nruns:2][run_vals][run_lens]
			if (buf.size() < cur + 1 + 1 + 2)
				return false;
			bp.rle_vw = load_u8(buf.data() + cur);
			cur += 1;
			bp.rle_lw = load_u8(buf.data() + cur);
			cur += 1;
			bp.rle_nruns = read_u16_le(buf, cur);
			cur += 2;

			size_t vals_bytes = size_t(bp.rle_nruns) * bp.rle_vw;
			size_t lens_bytes = size_t(bp.rle_nruns) * bp.rle_lw;
			if (buf.size() < cur + vals_bytes + lens_bytes)
				return false;

			bp.rle_vals_ptr = reinterpret_cast<const uint8_t*>(buf.data() + cur);
			cur += vals_bytes;
			bp.rle_lens_ptr = reinterpret_cast<const uint8_t*>(buf.data() + cur);
			cur += lens_bytes;

			// no lows/highs in this mode
			bp.bbase = 0;
			bp.e     = 0;
			return true;
		}
		return false;
	}

	static uint32_t value_at(const std::vector<std::byte>& buf, const Meta& m, uint32_t nvals, uint32_t i) {
		uint32_t   bidx  = i / kBlock;
		uint32_t   local = i % kBlock;
		uint32_t   off   = block_offset(buf, m, bidx);
		BlockParse bp{};
		if (!parse_block(buf, off, nvals, bidx, bp))
			return 0;
		return value_at_local(buf, bp, local);
	}
};

enum class Pattern : uint32_t { Infrequent = 0u, Normal = 1u, Frequent = 2u };

struct DistStats {
	uint32_t ndv          = 0;
	double   top_share    = 0.0;
	double   entropy_norm = 0.0;
	uint32_t max_count    = 0;
};

inline DistStats analyze_distribution(std::span<const uint32_t> data) {
	DistStats                              st;
	std::unordered_map<uint32_t, uint32_t> hist;
	hist.reserve(std::max<size_t>(16, kFixedN / 4));
	for (uint32_t v : data) {
		uint32_t& c = hist[v];
		c += 1;
		if (c > st.max_count)
			st.max_count = c;
	}
	st.ndv            = (uint32_t)hist.size();
	const double invN = 1.0 / double(kFixedN);
	st.top_share      = (kFixedN ? double(st.max_count) / double(kFixedN) : 0.0);
	double H          = 0.0;
	for (auto& kv : hist) {
		double p = kv.second * invN;
		if (p > 0.0)
			H -= p * std::log2(p);
	}
	st.entropy_norm = (st.ndv > 1) ? (H / std::log2(double(st.ndv))) : 0.0;
	return st;
}

inline uint32_t choose_dict_threshold(int FA, int FS) {
	double   ratio       = FA / double(std::max(1, FS));
	double   scale       = std::clamp(ratio, 0.1, 16.0);
	double   superlinear = std::pow(scale, 1.5);
	uint32_t base        = 16384;
	return std::clamp<uint32_t>(uint32_t(base * superlinear), 4096u, 262144u);
}

inline Pattern recognize_pattern(const DistStats& st, uint32_t dict_thr_nominal) {
	const double ndv_ratio      = double(st.ndv) / double(kFixedN);
	const bool   c_ndv_small    = (st.ndv <= std::min<uint32_t>(dict_thr_nominal / 4u, 640u));
	const bool   c_ndv_dense    = (ndv_ratio <= 0.035);
	const bool   c_top_heavy    = (st.top_share >= 0.35);
	const bool   c_low_entropy  = (st.entropy_norm <= 0.45);
	int          frequent_score = 0;
	frequent_score += c_top_heavy ? 2 : 0;
	frequent_score += c_ndv_small ? 1 : 0;
	frequent_score += c_ndv_dense ? 1 : 0;
	frequent_score += c_low_entropy ? 1 : 0;
	const bool is_frequent        = (frequent_score >= 3);
	const bool is_infrequent_base = (st.ndv >= std::max<uint32_t>(8192u, dict_thr_nominal * 3u)) && (ndv_ratio >= 0.72)
	                                && (st.top_share <= 0.008) && (st.entropy_norm >= 0.97);
	if (is_frequent)
		return Pattern::Frequent;
	Pattern p = is_infrequent_base ? Pattern::Infrequent : Pattern::Normal;
	if (p != Pattern::Frequent && st.ndv > (dict_thr_nominal * 5u))
		p = Pattern::Infrequent;
	return p;
}

enum : uint8_t { MODE_NONE = 1u, MODE_BLOOM = 2u, MODE_TOPK = 3u, MODE_PFOR = 7u };

struct Header {
	uint8_t  mode{};
	uint32_t header_bytes{1};
};

inline std::optional<Header> read_header(const std::vector<std::byte>& buf) {
	if (buf.size() < 1)
		return std::nullopt;
	Header h{};
	h.mode         = read_u8(buf, 0);
	h.header_bytes = 1;
	return h;
}

enum class BuildPlanKindEx : uint32_t { UseNone, UseTopK, UseBloom, UsePFor };

struct Decision {
	Pattern         pattern = Pattern::Normal;
	DistStats       stats{};
	BuildPlanKindEx kind = BuildPlanKindEx::UsePFor;
	BloomParams     bloom{};
};

inline bool storage_is_cheap(int FS) { return FS <= 6; }

inline Decision make_decision(std::span<const uint32_t> data, int FA, int FS) {
	Decision       d;
	const uint32_t dict_thr_nominal = choose_dict_threshold(FA, FS);
	d.stats                         = analyze_distribution(data);
	d.pattern                       = recognize_pattern(d.stats, dict_thr_nominal);
	const uint64_t N                = kFixedN;
	const uint64_t NDV              = (uint64_t)d.stats.ndv;
	if (!storage_is_cheap(FS)) {
		const double ratio = (100.0 * double(NDV)) / double(N);
		if (ratio >= 43.0) {
			d.kind = BuildPlanKindEx::UseNone;
			return d;
		}
		if (ratio >= 35.0) {
			d.kind = BuildPlanKindEx::UseTopK;
			return d;
		}
		if (ratio >= 12.5) {
			d.kind  = BuildPlanKindEx::UseBloom;
			d.bloom = choose_bloom_params_dict(d.stats.ndv, true, FS);
			return d;
		}
		d.kind = BuildPlanKindEx::UsePFor;
		return d;
	}
	switch (d.pattern) {
	case Pattern::Frequent:
	case Pattern::Normal:
		d.kind = BuildPlanKindEx::UsePFor;
		break;
	case Pattern::Infrequent:
		d.kind = BuildPlanKindEx::UseBloom;
		d.bloom = choose_bloom_params_dict(d.stats.ndv, true, FS);
		break;
	}
	return d;
}

struct Pair {
	uint32_t v;
	uint32_t c;
};

inline void write_pfor_index(std::vector<std::byte>& out, const std::vector<Pair>& pairs, uint32_t ndv) {
	append_u32(out, ndv);
	{
		std::vector<uint32_t> values;
		values.reserve(ndv);
		for (auto& p : pairs)
			values.push_back(p.v);
		(void)PfdTable::write(out, values);
	}
	{
		std::vector<uint32_t> counts_m1(ndv);
		for (uint32_t i = 0; i < ndv; ++i) {
			uint32_t c   = pairs[i].c;
			counts_m1[i] = (c == 0 ? 0u : (c - 1u));
		}
		(void)PfaTable::write(out, counts_m1);
	}
}

// --- TOP-K payload helpers ---

inline void write_topk_payload(std::vector<std::byte>&  out,
                               const std::vector<Pair>& topk,
                               uint32_t                 mn,
                               uint32_t                 mx) {
	append_u32(out, mn);
	append_u32(out, mx);
	uint8_t k = (uint8_t)std::min<size_t>(topk.size(), 255);
	append_u8(out, k);
	for (size_t i = 0; i < k; ++i) {
		append_u32(out, topk[i].v);
		append_u32(out, topk[i].c);
	}
}

inline std::optional<size_t> topk_mode_maybe_count(uint32_t                      predicate,
                                                   const std::vector<std::byte>& index,
                                                   size_t                        payload_off) {
	size_t off = payload_off;
	if (index.size() < off + 4 + 4 + 1)
		return std::nullopt;
	uint32_t mn = read_u32_le(index, off);
	off += 4;
	uint32_t mx = read_u32_le(index, off);
	off += 4;
	if (predicate < mn || predicate > mx)
		return size_t{0};
	uint8_t k = read_u8(index, off);
	off += 1;
	if (index.size() < off + size_t(k) * (4 + 4))
		return std::nullopt;
	for (uint8_t i = 0; i < k; ++i) {
		uint32_t v = read_u32_le(index, off);
		off += 4;
		uint32_t c = read_u32_le(index, off);
		off += 4;
		if (v == predicate)
			return size_t(c);
	}
	return std::nullopt; // unknown: might exist, but not in top-K
}

// --- BLOOM payload helper ---
// New payload layout: [mn:4][mx:4][packed24:3][bits: ceil(m_bits/8)]
inline void write_bloom_payload(std::vector<std::byte>&                       out,
                                const BloomParams&                            bp,
                                const std::unordered_map<uint32_t, uint32_t>& hist,
                                uint32_t                                      mn,
                                uint32_t                                      mx) {
	append_u32(out, mn);
	append_u32(out, mx);
	append_u24(out, pack_mbits_k24(bp.m_bits, bp.k_hash));
	const uint32_t bloom_bytes = (bp.m_bits + 7u) / 8u;
	const size_t   bloom_off   = out.size();
	out.resize(out.size() + bloom_bytes, std::byte{0});
	std::byte* bits = out.data() + bloom_off;
	if (bp.m_bits && bp.k_hash)
		for (auto& kv : hist)
			Bloom::add_to(kv.first, bp.m_bits, bp.k_hash, bits);
}

// === BuildPlan & builders (templated Top-K selection) ===

struct BuildPlan {
	BuildPlanKindEx                        kind{BuildPlanKindEx::UsePFor};
	uint32_t                               mn_raw{0};
	uint32_t                               mx_raw{0};
	uint32_t                               ndv{0};
	std::vector<Pair>                      pairs;
	BloomParams                            bp{};
	std::unordered_map<uint32_t, uint32_t> hist;
	std::vector<Pair>                      topk; // for MODE_TOPK
};

// select top-K pairs by count (desc), tie-break by value (asc)
template <size_t K = 5>
inline std::vector<Pair> select_topk(const std::unordered_map<uint32_t, uint32_t>& hist) {
	std::vector<Pair> tmp;
	tmp.reserve(hist.size());
	for (auto& kv : hist)
		tmp.push_back({kv.first, kv.second});
	if (tmp.empty())
		return tmp;
	const size_t take = std::min<size_t>(tmp.size(), K);
	std::nth_element(tmp.begin(),
	                 tmp.begin() + take - 1,
	                 tmp.end(),
	                 [](const Pair& a, const Pair& b) {
		                 if (a.c != b.c)
			                 return a.c > b.c;
		                 return a.v < b.v;
	                 });
	tmp.resize(take);
	std::sort(tmp.begin(),
	          tmp.end(),
	          [](const Pair& a, const Pair& b) {
		          if (a.c != b.c)
			          return a.c > b.c;
		          return a.v < b.v;
	          });
	return tmp;
}

template <size_t K = 5>
inline BuildPlan make_build_plan(std::span<const uint32_t> data, const Parameters parameters) {
	BuildPlan                              plan{};
	std::unordered_map<uint32_t, uint32_t> hist;
	hist.reserve(kFixedN / 4);
	uint32_t mn = std::numeric_limits<uint32_t>::max();
	uint32_t mx = std::numeric_limits<uint32_t>::min();
	for (uint32_t v : data) {
		++hist[v];
		if (v < mn)
			mn = v;
		if (v > mx)
			mx = v;
	}
	const uint32_t ndv = (uint32_t)hist.size();
	plan.mn_raw        = (ndv ? mn : 0);
	plan.mx_raw        = (ndv ? mx : 0);
	plan.ndv           = ndv;
	const int cfg_fa   = parameters.f_a, cfg_fs = parameters.f_s;
	auto      decision = make_decision(data, cfg_fa, cfg_fs);
	switch (decision.kind) {
	case BuildPlanKindEx::UseNone:
		plan.kind = BuildPlanKindEx::UseNone;
		return plan;
	case BuildPlanKindEx::UsePFor:
		plan.kind = BuildPlanKindEx::UsePFor;
		plan.pairs.reserve(ndv);
		for (auto& kv : hist)
			plan.pairs.push_back({kv.first, kv.second});
		std::sort(plan.pairs.begin(), plan.pairs.end(), [](auto& a, auto& b) { return a.v < b.v; });
		return plan;
	case BuildPlanKindEx::UseBloom:
		plan.kind = BuildPlanKindEx::UseBloom;
		plan.hist = std::move(hist);
		plan.bp   = decision.bloom;
		return plan;
	case BuildPlanKindEx::UseTopK:
		plan.kind = BuildPlanKindEx::UseTopK;
		plan.topk = select_topk<K>(hist);
		// keep hist only if bloom also needed; here we drop to save RAM
		return plan;
	}
	return plan;
}

template <size_t K = 5>
inline std::vector<std::byte> build_idx(std::span<const uint32_t> data, const Parameters parameters) {
	BuildPlan              plan = make_build_plan<K>(data, parameters);
	std::vector<std::byte> out;
	switch (plan.kind) {
	case BuildPlanKindEx::UseNone:
		append_u8(out, MODE_NONE);
		append_u32(out, plan.mn_raw);
		append_u32(out, plan.mx_raw);
		return out;
	case BuildPlanKindEx::UsePFor:
		append_u8(out, MODE_PFOR);
		write_pfor_index(out, plan.pairs, plan.ndv);
		return out;
	case BuildPlanKindEx::UseBloom:
		append_u8(out, MODE_BLOOM);
		write_bloom_payload(out, plan.bp, plan.hist, plan.mn_raw, plan.mx_raw);
		return out;
	case BuildPlanKindEx::UseTopK:
		append_u8(out, MODE_TOPK);
		write_topk_payload(out, plan.topk, plan.mn_raw, plan.mx_raw);
		return out;
	}
	return out;
}

// --- Query helpers for NONE/BLOOM/TOPK/PFOR ---

inline std::optional<size_t> none_mode_maybe_count(uint32_t                      predicate,
                                                   const std::vector<std::byte>& index,
                                                   size_t                        payload_off) {
	if (index.size() < payload_off + 8)
		return std::nullopt;
	uint32_t mn = read_u32_le(index, payload_off + 0);
	uint32_t mx = read_u32_le(index, payload_off + 4);
	if (predicate < mn || predicate > mx)
		return size_t{0};
	return std::nullopt;
}

inline std::optional<size_t> bloom_mode_maybe_count(uint32_t                      predicate,
                                                    const std::vector<std::byte>& index,
                                                    size_t                        payload_off) {
	size_t off = payload_off;
	if (index.size() < off + 4 + 4 + 3)
		return std::nullopt;
	uint32_t mn = read_u32_le(index, off);
	off += 4;
	uint32_t mx = read_u32_le(index, off);
	off += 4;
	if (predicate < mn || predicate > mx)
		return size_t{0};

	// read packed 24-bit m_bits + k_hash
	uint32_t packed = read_u24_le(index, off);
	off += 3;
	uint32_t m_bits = 0;
	uint8_t  k_hash = 0;
	unpack_mbits_k24(packed, m_bits, k_hash);

	uint32_t bloom_bytes = (m_bits + 7u) / 8u;
	if (index.size() < off + bloom_bytes)
		return std::nullopt;

	Bloom b{m_bits, k_hash, index.data() + off};
	bool  maybe = b.maybe_contains(predicate);
	return maybe ? std::nullopt : std::optional<size_t>(size_t{0});
}

inline std::optional<size_t> query_idx(uint32_t predicate, const std::vector<std::byte>& index) {
	auto h = read_header(index);
	if (!h)
		return std::nullopt;
	const size_t payload_off = h->header_bytes;
	switch (h->mode) {
	case MODE_NONE:
		return none_mode_maybe_count(predicate, index, payload_off);
	case MODE_PFOR: {
		if (index.size() < payload_off + 4)
			return std::nullopt;
		uint32_t ndv       = read_u32_le(index, payload_off);
		size_t   vmeta_off = payload_off + 4;
		auto     vmeta_opt = PfdTable::read_meta(index, vmeta_off);
		if (!vmeta_opt)
			return std::nullopt;
		const auto& vmeta = *vmeta_opt; // binary search value
		size_t      lo    = 0, hi = ndv;
		while (lo < hi) {
			size_t   mid         = (lo + hi) >> 1;
			uint32_t v           = PfdTable::value_at(index, vmeta, ndv, (uint32_t)mid);
			(v < predicate) ? lo = mid + 1 : hi = mid;
		}
		if (lo >= ndv)
			return size_t{0};
		{
			uint32_t v = PfdTable::value_at(index, vmeta, ndv, (uint32_t)lo);
			if (v != predicate)
				return size_t{0};
		}
		size_t counts_off = vmeta.next_off;
		auto   cmeta_opt  = PfaTable::read_meta(index, counts_off);
		if (!cmeta_opt)
			return std::nullopt;
		const auto& cmeta  = *cmeta_opt;
		uint32_t    cnt_m1 = PfaTable::value_at(index, cmeta, ndv, (uint32_t)lo);
		return size_t(cnt_m1 + 1);
	}
	case MODE_BLOOM:
		return bloom_mode_maybe_count(predicate, index, payload_off);
	case MODE_TOPK:
		return topk_mode_maybe_count(predicate, index, payload_off);
	default:
		return std::nullopt;
	}
}