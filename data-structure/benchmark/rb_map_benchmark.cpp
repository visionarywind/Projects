#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "rbtree/rb_map.hpp"

namespace {
using clock_type = std::chrono::steady_clock;
using key_type = int;
using mapped_type = std::uint64_t;
using pair_type = std::pair<key_type, mapped_type>;

struct options {
    // All defaults are deliberately explicit so a result can be replayed from
    // the command line and compared across implementations.
    std::vector<std::size_t> sizes{1024, 16384, 262144};
    int repetitions = 7;
    unsigned seed = 20260911U;
    std::string csv_path;
};

struct result {
    // CSV/report fields: checksum is retained to compare that both containers
    // performed the same logical work, not merely to measure elapsed time.
    std::string container;
    std::string workload;
    std::size_t size = 0;
    double median_ns = 0;
    double min_ns = 0;
    double max_ns = 0;
    std::uint64_t checksum = 0;
};

std::vector<std::size_t> parse_sizes(const std::string& text) {
    std::vector<std::size_t> result;
    std::size_t begin = 0;
    while (begin < text.size()) {
        const std::size_t comma = text.find(',', begin);
        const std::string token = text.substr(begin, comma == std::string::npos ? std::string::npos : comma - begin);
        result.push_back(static_cast<std::size_t>(std::stoull(token)));
        if (comma == std::string::npos) break;
        begin = comma + 1;
    }
    return result;
}

options parse_options(int argc, char** argv) {
    options result;
    for (int i = 1; i < argc; ++i) {
        const std::string argument(argv[i]);
        const auto equal = argument.find('=');
        if (equal == std::string::npos) continue;
        const std::string name = argument.substr(0, equal);
        const std::string value = argument.substr(equal + 1);
        if (name == "--sizes") result.sizes = parse_sizes(value);
        else if (name == "--repetitions") result.repetitions = std::stoi(value);
        else if (name == "--seed") result.seed = static_cast<unsigned>(std::stoul(value));
        else if (name == "--csv") result.csv_path = value;
    }
    return result;
}

template <class Fill, class Run>
result measure(const std::string& name, const std::string& workload,
               std::size_t size, int repetitions, Fill&& fill, Run&& run) {
    // Fill is intentionally outside the timed interval for find/iterate/erase;
    // insert uses an empty container, so its result includes allocation and
    // balancing.  Repeating and taking the median reduces scheduler noise.
    std::vector<double> samples;
    samples.reserve(static_cast<std::size_t>(repetitions));
    std::uint64_t checksum = 0;
    for (int repetition = 0; repetition < repetitions; ++repetition) {
        auto container = fill();
        const auto start = clock_type::now();
        checksum ^= run(*container);
        const auto finish = clock_type::now();
        samples.push_back(static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count()));
    }
    std::sort(samples.begin(), samples.end());
    result output{name, workload, size, samples[samples.size() / 2], samples.front(), samples.back(), checksum};
    std::cout << std::left << std::setw(12) << output.container
              << std::setw(16) << output.workload << std::setw(10) << output.size
              << std::right << std::setw(14) << std::fixed << std::setprecision(0)
              << output.median_ns << " ns\n";
    return output;
}

template <class Map>
std::uint64_t lookup(Map& map, const std::vector<key_type>& keys) {
    // The key vector contains an equal mixture of hits and misses.  The
    // checksum prevents the optimizer from discarding the find loop.
    std::uint64_t checksum = 0;
    for (key_type key : keys) {
        auto it = map.find(key);
        if (it != map.end()) checksum += static_cast<std::uint64_t>(it->second);
    }
    return checksum;
}

template <class Map>
std::uint64_t iterate(Map& map) {
    std::uint64_t checksum = 0;
    for (const auto& entry : map) checksum += static_cast<std::uint64_t>(entry.first) ^ entry.second;
    return checksum;
}

template <class Map>
std::uint64_t insert_all(Map& map, const std::vector<pair_type>& data) {
    for (const auto& entry : data) map.emplace(entry.first, entry.second);
    return map.size();
}

template <class Map>
std::uint64_t erase_all(Map& map, const std::vector<key_type>& keys) {
    std::uint64_t checksum = 0;
    for (key_type key : keys) checksum += map.erase(key);
    return checksum;
}

std::vector<pair_type> make_data(std::size_t size, unsigned seed, bool shuffled) {
    // Keys are odd so the even keys built in run_size() are guaranteed misses.
    // Shuffling is deterministic for a fixed seed.
    std::vector<pair_type> data;
    data.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        data.emplace_back(static_cast<key_type>(i * 2 + 1), static_cast<mapped_type>(i + 1));
    }
    if (shuffled) {
        std::mt19937 generator(seed);
        std::shuffle(data.begin(), data.end(), generator);
    }
    return data;
}

std::vector<result> run_size(std::size_t size, const options& config) {
    const auto data = make_data(size, config.seed + static_cast<unsigned>(size), true);
    // First append all hits, then equally many guaranteed misses.  Every
    // implementation therefore receives exactly the same lookup workload.
    std::vector<key_type> lookup_keys;
    lookup_keys.reserve(size * 2);
    for (const auto& entry : data) lookup_keys.push_back(entry.first);
    for (std::size_t i = 0; i < size; ++i) lookup_keys.push_back(static_cast<key_type>(i * 2));
    const auto erase_keys = [&] {
        std::vector<key_type> keys;
        keys.reserve(size);
        for (const auto& entry : data) keys.push_back(entry.first);
        return keys;
    }();

    std::vector<result> results;
    results.push_back(measure("std::map", "insert", size, config.repetitions,
        [&] { return std::make_unique<std::map<key_type, mapped_type>>(); },
        [&](auto& map) { return insert_all(map, data); }));
    results.push_back(measure("rb_map", "insert", size, config.repetitions,
        [&] { return std::make_unique<rbtree::rb_map<key_type, mapped_type>>(); },
        [&](auto& map) { return insert_all(map, data); }));

    auto std_filled = [&] {
        auto map = std::make_unique<std::map<key_type, mapped_type>>();
        insert_all(*map, data);
        return map;
    };
    auto rb_filled = [&] {
        auto map = std::make_unique<rbtree::rb_map<key_type, mapped_type>>();
        insert_all(*map, data);
        return map;
    };
    results.push_back(measure("std::map", "find", size, config.repetitions,
        std_filled, [&](auto& map) { return lookup(map, lookup_keys); }));
    results.push_back(measure("rb_map", "find", size, config.repetitions,
        rb_filled, [&](auto& map) { return lookup(map, lookup_keys); }));
    results.push_back(measure("std::map", "iterate", size, config.repetitions,
        std_filled, [&](auto& map) { return iterate(map); }));
    results.push_back(measure("rb_map", "iterate", size, config.repetitions,
        rb_filled, [&](auto& map) { return iterate(map); }));
    results.push_back(measure("std::map", "erase", size, config.repetitions,
        std_filled, [&](auto& map) { return erase_all(map, erase_keys); }));
    results.push_back(measure("rb_map", "erase", size, config.repetitions,
        rb_filled, [&](auto& map) { return erase_all(map, erase_keys); }));
    return results;
}

void write_csv(const std::string& path, const std::vector<result>& results) {
    if (path.empty()) return;
    std::ofstream output(path);
    output << "container,workload,size,median_ns,min_ns,max_ns,checksum\n";
    for (const auto& item : results) {
        output << item.container << ',' << item.workload << ',' << item.size << ','
               << item.median_ns << ',' << item.min_ns << ',' << item.max_ns << ','
               << item.checksum << '\n';
    }
}
}  // namespace

int main(int argc, char** argv) {
    const options config = parse_options(argc, argv);
    std::cout << "container    workload        size           median\n";
    std::vector<result> results;
    for (std::size_t size : config.sizes) {
        const auto batch = run_size(size, config);
        results.insert(results.end(), batch.begin(), batch.end());
    }
    write_csv(config.csv_path, results);
    return 0;
}
