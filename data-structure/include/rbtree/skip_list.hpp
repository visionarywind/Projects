#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <new>
#include <random>
#include <utility>

namespace rbtree {

// A Redis-inspired, learning-oriented skip list with unique keys.
// Nodes store only the forward links for the levels they actually reach.
// This keeps one ordinary allocation per node (no pool or arena) while
// avoiding the unused MaxLevel-sized pointer array of the baseline version.
template <typename Key, typename Compare = std::less<Key>,
          std::size_t MaxLevel = 16>
class skip_list {
    static_assert(MaxLevel > 0, "MaxLevel must be positive");
    static_assert(MaxLevel <= 255, "MaxLevel must fit in node height");

private:
    static constexpr std::uint32_t default_seed = 0x5EED1234U;

    struct node {
        struct level_link {
            node* forward = nullptr;
        };

        explicit node(const Key& value, std::uint8_t node_height)
            : key(value), height(node_height) {}

        static node* create(const Key& value, std::size_t node_height) {
            const std::size_t bytes =
                sizeof(node) + (node_height - 1) * sizeof(level_link);
            void* storage = ::operator new(bytes);
            try {
                node* result =
                    new (storage) node(value, static_cast<std::uint8_t>(node_height));
                for (std::size_t index = 0; index < node_height; ++index) {
                    result->levels[index] = level_link{};
                }
                return result;
            } catch (...) {
                ::operator delete(storage);
                throw;
            }
        }

        static void destroy(node* target) noexcept {
            target->~node();
            ::operator delete(target);
        }

        level_link& link(std::size_t index) noexcept { return levels[index]; }
        const level_link& link(std::size_t index) const noexcept {
            return levels[index];
        }

        Key key;
        std::uint8_t height;
        level_link levels[1];
    };

    // The header is an embedded sentinel without a Key.  Its arrays are
    // bounded by MaxLevel, while real nodes carry only their actual height.
    std::array<node*, MaxLevel> header_{};
    Compare compare_{};
    std::mt19937 generator_{};
    std::size_t level_ = 1;
    std::size_t size_ = 0;

    node* next(node* current, std::size_t index) noexcept {
        return current == nullptr ? header_[index] : current->link(index).forward;
    }

    const node* next(const node* current, std::size_t index) const noexcept {
        return current == nullptr ? header_[index] : current->link(index).forward;
    }

    std::size_t random_level() noexcept {
        // A raw engine draw avoids distribution bookkeeping for every
        // promotion attempt.  The low two bits provide the 1/4 promotion
        // decision used by this fixed-seed benchmark.
        std::size_t result = 1;
        while (result < MaxLevel && (generator_() & 3U) == 0U) {
            ++result;
        }
        return result;
    }

    node* find_node(const Key& key, std::array<node*, MaxLevel>* update) {
        node* current = nullptr;
        for (std::size_t index = level_; index-- > 0;) {
            node* candidate = next(current, index);
            while (candidate != nullptr && compare_(candidate->key, key)) {
                current = candidate;
                candidate = current->link(index).forward;
            }
            if (update != nullptr) {
                (*update)[index] = current;
            }
        }
        node* result = next(current, 0);
        return result != nullptr && !compare_(key, result->key) ? result : nullptr;
    }

    const node* find_node(const Key& key) const {
        const node* current = nullptr;
        for (std::size_t index = level_; index-- > 0;) {
            const node* candidate = next(current, index);
            while (candidate != nullptr && compare_(candidate->key, key)) {
                current = candidate;
                candidate = current->link(index).forward;
            }
        }
        const node* result = next(current, 0);
        return result != nullptr && !compare_(key, result->key) ? result : nullptr;
    }

public:
    using key_type = Key;
    using key_compare = Compare;
    using value_type = Key;
    using size_type = std::size_t;

    class const_iterator;

    class iterator {
        friend class skip_list;
        friend class const_iterator;
        skip_list* owner_ = nullptr;
        node* current_ = nullptr;

        iterator(skip_list* owner, node* current) noexcept
            : owner_(owner), current_(current) {}

    public:
        using reference = const Key&;
        using pointer = const Key*;

        iterator() = default;
        reference operator*() const noexcept { return current_->key; }
        pointer operator->() const noexcept { return &current_->key; }

        iterator& operator++() noexcept {
            current_ = current_ == nullptr ? nullptr : current_->link(0).forward;
            return *this;
        }
        iterator operator++(int) noexcept {
            iterator copy = *this;
            ++(*this);
            return copy;
        }
        friend bool operator==(iterator left, iterator right) noexcept {
            return left.current_ == right.current_;
        }
        friend bool operator!=(iterator left, iterator right) noexcept {
            return !(left == right);
        }
    };

    class const_iterator {
        friend class skip_list;
        const skip_list* owner_ = nullptr;
        const node* current_ = nullptr;

        const_iterator(const skip_list* owner, const node* current) noexcept
            : owner_(owner), current_(current) {}

    public:
        using reference = const Key&;
        using pointer = const Key*;

        const_iterator() = default;
        const_iterator(iterator other) noexcept
            : owner_(other.owner_), current_(other.current_) {}
        reference operator*() const noexcept { return current_->key; }
        pointer operator->() const noexcept { return &current_->key; }

        const_iterator& operator++() noexcept {
            current_ = current_ == nullptr ? nullptr : current_->link(0).forward;
            return *this;
        }
        const_iterator operator++(int) noexcept {
            const_iterator copy = *this;
            ++(*this);
            return copy;
        }
        friend bool operator==(const_iterator left,
                               const_iterator right) noexcept {
            return left.current_ == right.current_;
        }
        friend bool operator!=(const_iterator left,
                               const_iterator right) noexcept {
            return !(left == right);
        }
    };

    skip_list() : skip_list(Compare{}, default_seed) {}

    explicit skip_list(Compare compare, std::uint32_t seed = default_seed)
        : compare_(std::move(compare)), generator_(seed) {}

    skip_list(const skip_list&) = delete;
    skip_list& operator=(const skip_list&) = delete;
    skip_list(skip_list&&) = delete;
    skip_list& operator=(skip_list&&) = delete;

    ~skip_list() { clear(); }

    // Search once and reuse the predecessor of every level for insertion.
    // Duplicate keys return before randomization and allocation.
    bool insert(const Key& key) {
        std::array<node*, MaxLevel> update;
        if (find_node(key, &update) != nullptr) {
            return false;
        }

        const std::size_t node_height = random_level();
        node* inserted = node::create(key, node_height);
        if (node_height > level_) {
            for (std::size_t index = level_; index < node_height; ++index) {
                update[index] = nullptr;
            }
            level_ = node_height;
        }

        for (std::size_t index = 0; index < node_height; ++index) {
            node* predecessor = update[index];
            inserted->link(index).forward = next(predecessor, index);
            if (predecessor == nullptr) {
                header_[index] = inserted;
            } else {
                predecessor->link(index).forward = inserted;
            }
        }
        ++size_;
        return true;
    }

    // Unlink the target from every level it participates in, then release the
    // one allocation belonging to that node.  The predecessor path is rebuilt
    // by the same top-down search used by insert.
    size_type erase(const Key& key) {
        std::array<node*, MaxLevel> update;
        node* target = find_node(key, &update);
        if (target == nullptr) {
            return 0;
        }

        for (std::size_t index = 0; index < target->height; ++index) {
            node* predecessor = update[index];
            if (predecessor == nullptr) {
                header_[index] = target->link(index).forward;
            } else {
                predecessor->link(index).forward = target->link(index).forward;
            }
        }
        node::destroy(target);
        --size_;
        while (level_ > 1 && header_[level_ - 1] == nullptr) {
            --level_;
        }
        return 1;
    }

    iterator find(const Key& key) noexcept(
        noexcept(std::declval<Compare>()(key, key))) {
        return iterator(this, find_node(key, nullptr));
    }

    const_iterator find(const Key& key) const noexcept(
        noexcept(std::declval<Compare>()(key, key))) {
        return const_iterator(this, find_node(key));
    }

    bool contains(const Key& key) const noexcept(
        noexcept(std::declval<Compare>()(key, key))) {
        return find_node(key) != nullptr;
    }

    void clear() noexcept {
        node* current = header_[0];
        while (current != nullptr) {
            node* following = current->link(0).forward;
            node::destroy(current);
            current = following;
        }
        header_.fill(nullptr);
        level_ = 1;
        size_ = 0;
    }

    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    iterator begin() noexcept { return iterator(this, header_[0]); }
    iterator end() noexcept { return iterator(this, nullptr); }
    const_iterator begin() const noexcept {
        return const_iterator(this, header_[0]);
    }
    const_iterator end() const noexcept { return const_iterator(this, nullptr); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    // Verify ordering, level membership, cycle freedom, and logical size.
    bool verify_invariants() const {
        if (level_ == 0 || level_ > MaxLevel) {
            return false;
        }
        for (std::size_t index = level_; index < MaxLevel; ++index) {
            if (header_[index] != nullptr) {
                return false;
            }
        }
        if (size_ == 0) {
            return level_ == 1 && header_[0] == nullptr;
        }
        if (header_[0] == nullptr) {
            return false;
        }

        std::size_t bottom_count = 0;
        const node* previous = nullptr;
        const node* current = header_[0];
        while (current != nullptr && bottom_count <= size_) {
            if (current->height == 0 || current->height > MaxLevel ||
                (previous != nullptr &&
                 !compare_(previous->key, current->key))) {
                return false;
            }
            previous = current;
            current = current->link(0).forward;
            ++bottom_count;
        }
        if (current != nullptr || bottom_count != size_) {
            return false;
        }

        for (std::size_t index = 0; index < level_; ++index) {
            std::size_t count = 0;
            previous = nullptr;
            current = header_[index];
            while (current != nullptr && count <= size_) {
                if (current->height <= index ||
                    (previous != nullptr &&
                     !compare_(previous->key, current->key))) {
                    return false;
                }
                previous = current;
                current = current->link(index).forward;
                ++count;
            }
            if (current != nullptr || count > size_) {
                return false;
            }
        }
        return true;
    }
};

}  // namespace rbtree
