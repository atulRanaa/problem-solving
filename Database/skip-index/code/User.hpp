#include <optional>
#include <vector>
#include <span>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <cstring>


#include "Parameters.hpp"


// Simple hash functions for Bloom filter
static uint32_t hash1(uint32_t x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}


static uint32_t hash2(uint32_t x) {
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = (x >> 16) ^ x;
    return x;
}


static uint32_t hash3(uint32_t x) {
    x ^= x >> 17;
    x *= 0xed5ad4bb;
    x ^= x >> 11;
    x *= 0xac4c1b51;
    x ^= x >> 15;
    return x;
}


// Varint encoding
static void encode_varint(std::vector<uint8_t>& out, uint32_t val) {
    while (val >= 0x80) {
        out.push_back((val & 0x7F) | 0x80);
        val >>= 7;
    }
    out.push_back(val & 0x7F);
}


// Varint decoding
static uint32_t decode_varint(const uint8_t*& ptr, const uint8_t* end) {
    uint32_t result = 0;
    int shift = 0;
    while (ptr < end) {
        uint8_t byte = *ptr++;
        result |= (byte & 0x7F) << shift;
        if (!(byte & 0x80)) break;
        shift += 7;
    }
    return result;
}

void mostoccuringele(std::unordered_map<uint32_t, uint32_t> &freq_map) {
    std::vector< std::pair<uint32_t, uint32_t> > occ(freq_map.begin(), freq_map.end());
    std::sort(occ.begin(), occ.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for(int i = 0; i < 10; i++)
        std::cout << occ[i].first << ":" << occ[i].second << ", ";
    std::cout<<std::endl;
}

void mostoccuringfreq(std::unordered_map<uint32_t, uint32_t> &freq_map) {
    std::unordered_map<uint32_t, uint32_t> freq_occ;
    for (const auto& [val, cnt] : freq_map) {
        freq_occ[cnt]++;
    }
    std::vector< std::pair<uint32_t, uint32_t> > occ(freq_occ.begin(), freq_occ.end());
    std::sort(occ.begin(), occ.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for(int i = 0; i < 10; i++)
        std::cout << occ[i].first << ":" << occ[i].second << ", ";
    std::cout<<std::endl;
}

std::vector<std::byte> build_idx(std::span<const uint32_t> data, Parameters config) {
    // Count frequencies
    std::unordered_map<uint32_t, uint32_t> freq_map;
    for (uint32_t val : data) {
        freq_map[val]++;
    }
    
    if (freq_map.empty()) {
        return std::vector<std::byte>(1, std::byte{0});
    }
    size_t cardinality = freq_map.size();
    double ratio = static_cast<double>(config.f_a) / static_cast<double>(config.f_s);
    
    // Decide strategy based on cardinality
    bool use_bloom = false;
    size_t bloom_bits = 0;
    size_t max_exact_entries = 0;
    
    if (cardinality < 5000) {
        // FREQUENT: store all exactly, no bloom filter
        max_exact_entries = cardinality;
        
    } else if (cardinality < 10000) {
        // NORMAL: hybrid approach
        // Use small bloom filter + selective exact counts
        use_bloom = true;
        bloom_bits = 4096;  // 512 bytes
        
        if (ratio >= 1.0) {
            max_exact_entries = static_cast<size_t>(ratio * 500);
        } else {
            max_exact_entries = static_cast<size_t>(ratio * 200);
        }
        
    } else {
        // INFREQUENT: larger bloom + fewer exact
        use_bloom = true;
        bloom_bits = 8192;  // 1 KB
        
        if (ratio >= 1.0) {
            max_exact_entries = static_cast<size_t>(ratio * 200);
        } else {
            max_exact_entries = static_cast<size_t>(ratio * 50);
        }
    }
    
    max_exact_entries = std::min(max_exact_entries, cardinality);

    mostoccuringele(freq_map);
    mostoccuringfreq(freq_map);
    std::cout << "cardinality: " << cardinality << "/" << data.size() << " " << "store: " << max_exact_entries << std::endl;
    std::cout << std::endl;

    std::vector<uint8_t> temp;
    
    // Format: [mode:1][bloom_size:varint][bloom_bits...][num_exact:varint][exact_entries...]
    
    if (use_bloom && bloom_bits > 0) {
        // Mode 1: Bloom + Exact
        temp.push_back(1);
        
        // Build bloom filter
        std::vector<uint8_t> bloom((bloom_bits + 7) / 8, 0);
        
        for (const auto& [val, cnt] : freq_map) {
            uint32_t h1 = hash1(val) % bloom_bits;
            uint32_t h2 = hash2(val) % bloom_bits;
            uint32_t h3 = hash3(val) % bloom_bits;
            
            bloom[h1 / 8] |= (1 << (h1 % 8));
            bloom[h2 / 8] |= (1 << (h2 % 8));
            bloom[h3 / 8] |= (1 << (h3 % 8));
        }
        
        // Store bloom filter
        encode_varint(temp, bloom_bits);
        temp.insert(temp.end(), bloom.begin(), bloom.end());
        
    } else {
        // Mode 0: Exact only
        temp.push_back(0);
    }
    
    // Select top-frequency entries for exact counts
    std::vector<std::pair<uint32_t, uint32_t>> entries(freq_map.begin(), freq_map.end());
    
    if (max_exact_entries < entries.size()) {
        std::partial_sort(entries.begin(), 
                         entries.begin() + max_exact_entries,
                         entries.end(),
                         [](const auto& a, const auto& b) { 
                             return a.second > b.second; 
                         });
        entries.resize(max_exact_entries);
    }
    
    // Sort by value for delta encoding
    std::sort(entries.begin(), entries.end());
    
    // Encode exact entries
    encode_varint(temp, entries.size());
    
    uint32_t prev_val = 0;
    for (const auto& [val, cnt] : entries) {
        encode_varint(temp, val - prev_val);
        encode_varint(temp, cnt);
        prev_val = val;
    }
    
    // Convert to std::byte
    std::vector<std::byte> result(temp.size());
    std::memcpy(result.data(), temp.data(), temp.size());
    
    return result;
}


std::optional<size_t> query_idx(uint32_t predicate, const std::vector<std::byte>& index) {
    if (index.empty()) return std::nullopt;
    
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(index.data());
    const uint8_t* end = ptr + index.size();
    
    uint8_t mode = *ptr++;
    
    // Check bloom filter first (if present)
    if (mode == 1) {
        uint32_t bloom_bits = decode_varint(ptr, end);
        size_t bloom_bytes = (bloom_bits + 7) / 8;
        
        if (ptr + bloom_bytes > end) return std::nullopt;
        
        const uint8_t* bloom = ptr;
        ptr += bloom_bytes;
        
        // Check bloom filter
        uint32_t h1 = hash1(predicate) % bloom_bits;
        uint32_t h2 = hash2(predicate) % bloom_bits;
        uint32_t h3 = hash3(predicate) % bloom_bits;
        
        bool bit1 = bloom[h1 / 8] & (1 << (h1 % 8));
        bool bit2 = bloom[h2 / 8] & (1 << (h2 % 8));
        bool bit3 = bloom[h3 / 8] & (1 << (h3 % 8));
        
        // Bloom filter says "definitely not present"
        if (!bit1 || !bit2 || !bit3) {
            return 0;  // Return exact count of 0
        }
        
        // Bloom says "maybe present" - check exact counts
    }
    
    // Search exact entries
    uint32_t num_entries = decode_varint(ptr, end);
    
    uint32_t current_val = 0;
    for (uint32_t i = 0; i < num_entries && ptr < end; i++) {
        uint32_t delta = decode_varint(ptr, end);
        uint32_t count = decode_varint(ptr, end);
        current_val += delta;
        
        if (current_val == predicate) {
            return count;
        }
        if (current_val > predicate) {
            break;
        }
    }
    
    // Not in exact list
    // If we had bloom filter that said "maybe", we don't know - scan
    // If no bloom or bloom said "yes", we don't know - scan
    return std::nullopt;
}