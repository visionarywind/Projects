#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "rbtree/llrb_tree.hpp"
#include "rbtree/rb_tree.hpp"
#include "rbtree/skip_list.hpp"

namespace {
using clock_type = std::chrono::steady_clock;
using key_type = int;
using tree_type = rbtree::rb_tree<key_type>;
using llrb_tree_type = rbtree::llrb_tree<key_type>;
using skip_type = rbtree::skip_list<key_type, std::less<key_type>, 12>;
using std_set_type = std::set<key_type>;

struct options {
    std::vector<std::size_t> sizes{1024, 16384, 262144};
    int repetitions = 7;
    unsigned seed = 20260911U;
    std::string csv_path;
};

struct result {
    std::string container;
    std::string workload;
    std::size_t size = 0;
    double median_ns = 0.0;
    double min_ns = 0.0;
    double max_ns = 0.0;
    std::uint64_t checksum = 0;
};

std::vector<std::size_t> parse_sizes(const std::string& text) {
    std::vector<std::size_t> result;
    std::size_t begin = 0;
    while (begin < text.size()) {
        const std::size_t comma = text.find(',', begin);
        const std::string token = text.substr(
            begin, comma == std::string::npos ? std::string::npos : comma - begin);
        result.push_back(static_cast<std::size_t>(std::stoull(token)));
        if (comma == std::string::npos) {
            break;
        }
        begin = comma + 1;
    }
    return result;
}

options parse_options(int argc, char** argv) {
    options result;
    for (int index = 1; index < argc; ++index) {
        const std::string argument(argv[index]);
        const std::size_t equal = argument.find('=');
        if (equal == std::string::npos) {
            continue;
        }
        const std::string name = argument.substr(0, equal);
        const std::string value = argument.substr(equal + 1);
        if (name == "--sizes") {
            result.sizes = parse_sizes(value);
        } else if (name == "--repetitions") {
            result.repetitions = std::stoi(value);
        } else if (name == "--seed") {
            result.seed = static_cast<unsigned>(std::stoul(value));
        } else if (name == "--csv") {
            result.csv_path = value;
        }
    }
    return result;
}

template <class Fill, class Run>
result measure(const std::string& container_name, const std::string& workload,
               std::size_t size, int repetitions, Fill&& fill, Run&& run) {
    // The fill phase is outside the timed interval for find, iterate, and
    // erase.  Insert starts from an empty container, so it includes allocation
    // and, for skip_list, random-level generation.
    std::vector<double> samples;
    samples.reserve(static_cast<std::size_t>(repetitions));
    std::uint64_t checksum = 0;
    for (int repetition = 0; repetition < repetitions; ++repetition) {
        auto container = fill();
        const auto start = clock_type::now();
        checksum ^= run(*container);
        const auto finish = clock_type::now();
        samples.push_back(static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start)
                .count()));
    }
    std::sort(samples.begin(), samples.end());
    result output{container_name, workload, size, samples[samples.size() / 2],
                  samples.front(), samples.back(), checksum};
    std::cout << std::left << std::setw(12) << output.container
              << std::setw(16) << output.workload << std::setw(10) << output.size
              << std::right << std::setw(14) << std::fixed << std::setprecision(0)
              << output.median_ns << " ns\n";
    return output;
}

template <class Container>
std::uint64_t insert_all(Container& container,
                          const std::vector<key_type>& data) {
    for (key_type key : data) {
        container.insert(key);
    }
    return static_cast<std::uint64_t>(container.size());
}

template <class Container>
std::uint64_t lookup(Container& container,
                     const std::vector<key_type>& keys) {
    // The lookup workload has one hit for every miss.  The checksum makes the
    // result observable, preventing the optimizer from removing the loop.
    std::uint64_t checksum = 0;
    for (key_type key : keys) {
        const auto iterator = container.find(key);
        if (iterator != container.end()) {
            checksum += static_cast<std::uint64_t>(*iterator);
        }
    }
    return checksum;
}

template <class Container>
std::uint64_t iterate(const Container& container) {
    std::uint64_t checksum = 0;
    for (key_type key : container) {
        checksum += static_cast<std::uint64_t>(key);
    }
    return checksum;
}

template <class Container>
std::uint64_t erase_all(Container& container,
                        const std::vector<key_type>& keys) {
    std::uint64_t checksum = 0;
    for (key_type key : keys) {
        checksum += static_cast<std::uint64_t>(container.erase(key));
    }
    return checksum;
}

std::vector<key_type> make_data(std::size_t size, unsigned seed) {
    // Odd keys make the equally sized even-key half of lookup_keys guaranteed
    // misses.  The insertion order is shared by all three containers.
    std::vector<key_type> data;
    data.reserve(size);
    for (std::size_t index = 0; index < size; ++index) {
        data.push_back(static_cast<key_type>(index * 2 + 1));
    }
    std::mt19937 generator(seed);
    std::shuffle(data.begin(), data.end(), generator);
    return data;
}

template <class Container, class Factory>
void add_container_results(std::vector<result>& results, const std::string& name,
                           std::size_t size, const options& config,
                           const std::vector<key_type>& data,
                           const std::vector<key_type>& lookup_keys,
                           const std::vector<key_type>& erase_keys,
                           Factory&& factory) {
    results.push_back(measure(
        name, "insert", size, config.repetitions, factory,
        [&](Container& container) { return insert_all(container, data); }));

    auto filled = [&] {
        auto container = factory();
        insert_all(*container, data);
        return container;
    };
    results.push_back(measure(
        name, "find", size, config.repetitions, filled,
        [&](Container& container) { return lookup(container, lookup_keys); }));
    results.push_back(measure(
        name, "iterate", size, config.repetitions, filled,
        [&](Container& container) { return iterate(container); }));
    results.push_back(measure(
        name, "erase", size, config.repetitions, filled,
        [&](Container& container) { return erase_all(container, erase_keys); }));
}

std::vector<result> run_size(std::size_t size, const options& config) {
    const auto data = make_data(
        size, config.seed + static_cast<unsigned>(size));
    std::vector<key_type> lookup_keys;
    lookup_keys.reserve(size * 2);
    for (key_type key : data) {
        lookup_keys.push_back(key);
    }
    for (std::size_t index = 0; index < size; ++index) {
        lookup_keys.push_back(static_cast<key_type>(index * 2));
    }
    const auto erase_keys = data;

    std::vector<result> results;
    add_container_results<std_set_type>(
        results, "std::set", size, config, data, lookup_keys, erase_keys,
        [] { return std::make_unique<std_set_type>(); });
    add_container_results<tree_type>(
        results, "rb_tree", size, config, data, lookup_keys, erase_keys,
        [] { return std::make_unique<tree_type>(); });
    add_container_results<llrb_tree_type>(
        results, "llrb_tree", size, config, data, lookup_keys, erase_keys,
        [] { return std::make_unique<llrb_tree_type>(); });
    add_container_results<skip_type>(
        results, "skip_list", size, config, data, lookup_keys, erase_keys,
        [&config, size] {
            return std::make_unique<skip_type>(
                std::less<key_type>{},
                config.seed + static_cast<unsigned>(size));
        });
    return results;
}

void write_csv(const std::string& path, const std::vector<result>& results) {
    if (path.empty()) {
        return;
    }
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
