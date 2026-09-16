#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>

namespace rbtree {

// A set-like left-leaning red-black tree (LLRB).
//
// LLRB represents 2-3 tree links explicitly: red links must lean left, and
// the insertion/deletion algorithms split or join temporary 3-nodes while
// descending the tree.  Nodes do not store parent pointers, which keeps the
// node small but makes iterator successor/predecessor lookup start at root_.
template <typename Key, typename Compare = std::less<Key>>
class llrb_tree {
private:
    enum class color { red, black };

    struct node {
        explicit node(const Key& value) : key(value) {}

        Key key;
        color colour = color::red;
        node* left = nullptr;
        node* right = nullptr;
    };

    Compare compare_{};
    node* root_ = nullptr;
    std::size_t size_ = 0;

    static bool is_red(const node* current) noexcept {
        return current != nullptr && current->colour == color::red;
    }

    bool equivalent(const Key& left, const Key& right) const {
        return !compare_(left, right) && !compare_(right, left);
    }

    static node* minimum(node* current) noexcept {
        while (current != nullptr && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    static const node* minimum(const node* current) noexcept {
        while (current != nullptr && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    static node* maximum(node* current) noexcept {
        while (current != nullptr && current->right != nullptr) {
            current = current->right;
        }
        return current;
    }

    static const node* maximum(const node* current) noexcept {
        while (current != nullptr && current->right != nullptr) {
            current = current->right;
        }
        return current;
    }

    node* rotate_left(node* current) noexcept {
        node* promoted = current->right;
        current->right = promoted->left;
        promoted->left = current;
        promoted->colour = current->colour;
        current->colour = color::red;
        return promoted;
    }

    node* rotate_right(node* current) noexcept {
        node* promoted = current->left;
        current->left = promoted->right;
        promoted->right = current;
        promoted->colour = current->colour;
        current->colour = color::red;
        return promoted;
    }

    static void flip_colors(node* current) noexcept {
        current->colour = current->colour == color::red ? color::black : color::red;
        if (current->left != nullptr) {
            current->left->colour = current->left->colour == color::red
                                         ? color::black
                                         : color::red;
        }
        if (current->right != nullptr) {
            current->right->colour = current->right->colour == color::red
                                          ? color::black
                                          : color::red;
        }
    }

    node* balance(node* current) noexcept {
        // A red right link violates the left-leaning representation.
        if (is_red(current->right)) {
            current = rotate_left(current);
        }
        // Two consecutive left red links represent an overfull 4-node.
        if (is_red(current->left) && is_red(current->left->left)) {
            current = rotate_right(current);
        }
        if (is_red(current->left) && is_red(current->right)) {
            flip_colors(current);
        }
        return current;
    }

    node* insert_node(node* current, const Key& key) {
        if (current == nullptr) {
            return new node(key);
        }
        if (compare_(key, current->key)) {
            current->left = insert_node(current->left, key);
        } else if (compare_(current->key, key)) {
            current->right = insert_node(current->right, key);
        }
        return balance(current);
    }

    node* move_red_left(node* current) noexcept {
        flip_colors(current);
        if (current->right != nullptr && is_red(current->right->left)) {
            current->right = rotate_right(current->right);
            current = rotate_left(current);
            flip_colors(current);
        }
        return current;
    }

    node* move_red_right(node* current) noexcept {
        flip_colors(current);
        if (current->left != nullptr && is_red(current->left->left)) {
            current = rotate_right(current);
            flip_colors(current);
        }
        return current;
    }

    node* delete_min(node* current) noexcept {
        if (current->left == nullptr) {
            delete current;
            return nullptr;
        }
        if (!is_red(current->left) &&
            !is_red(current->left->left)) {
            current = move_red_left(current);
        }
        current->left = delete_min(current->left);
        return balance(current);
    }

    node* erase_node(node* current, const Key& key) {
        if (compare_(key, current->key)) {
            if (current->left != nullptr) {
                if (!is_red(current->left) &&
                    !is_red(current->left->left)) {
                    current = move_red_left(current);
                }
                current->left = erase_node(current->left, key);
            }
        } else {
            if (is_red(current->left)) {
                current = rotate_right(current);
            }
            if (equivalent(key, current->key) && current->right == nullptr) {
                delete current;
                return nullptr;
            }
            if (current->right != nullptr) {
                if (!is_red(current->right) &&
                    !is_red(current->right->left)) {
                    current = move_red_right(current);
                }
                if (equivalent(key, current->key)) {
                    node* successor = minimum(current->right);
                    current->key = successor->key;
                    current->right = delete_min(current->right);
                } else {
                    current->right = erase_node(current->right, key);
                }
            }
        }
        return balance(current);
    }

    const node* find_node(const Key& key) const {
        const node* current = root_;
        while (current != nullptr) {
            if (compare_(key, current->key)) {
                current = current->left;
            } else if (compare_(current->key, key)) {
                current = current->right;
            } else {
                return current;
            }
        }
        return nullptr;
    }

    node* find_node(const Key& key) {
        return const_cast<node*>(static_cast<const llrb_tree&>(*this).find_node(key));
    }

    node* successor(const node* current) const {
        const node* candidate = nullptr;
        const node* cursor = root_;
        while (cursor != nullptr) {
            if (compare_(current->key, cursor->key)) {
                candidate = cursor;
                cursor = cursor->left;
            } else {
                cursor = cursor->right;
            }
        }
        return const_cast<node*>(candidate);
    }

    node* predecessor(const node* current) const {
        const node* candidate = nullptr;
        const node* cursor = root_;
        while (cursor != nullptr) {
            if (compare_(cursor->key, current->key)) {
                candidate = cursor;
                cursor = cursor->right;
            } else {
                cursor = cursor->left;
            }
        }
        return const_cast<node*>(candidate);
    }

    void destroy_subtree(node* current) noexcept {
        if (current == nullptr) {
            return;
        }
        destroy_subtree(current->left);
        destroy_subtree(current->right);
        delete current;
    }

    struct verification_result {
        bool valid = true;
        std::size_t count = 0;
        std::size_t black_height = 1;
    };

    verification_result verify_subtree(const node* current, const Key* lower,
                                       const Key* upper) const {
        if (current == nullptr) {
            return {};
        }
        if (lower != nullptr && !compare_(*lower, current->key)) {
            return {false, 0, 0};
        }
        if (upper != nullptr && !compare_(current->key, *upper)) {
            return {false, 0, 0};
        }
        if (is_red(current->right) ||
            (is_red(current) &&
             (is_red(current->left) || is_red(current->right)))) {
            return {false, 0, 0};
        }
        verification_result left =
            verify_subtree(current->left, lower, &current->key);
        verification_result right =
            verify_subtree(current->right, &current->key, upper);
        if (!left.valid || !right.valid ||
            left.black_height != right.black_height) {
            return {false, 0, 0};
        }
        return {true,
                left.count + right.count + 1,
                left.black_height + (current->colour == color::black ? 1U : 0U)};
    }

public:
    using key_type = Key;
    using value_type = Key;
    using size_type = std::size_t;

    class const_iterator;

    class iterator {
        friend class llrb_tree;
        friend class const_iterator;
        llrb_tree* owner_ = nullptr;
        node* current_ = nullptr;

        iterator(llrb_tree* owner, node* current) noexcept
            : owner_(owner), current_(current) {}

    public:
        using value_type = Key;
        using reference = const Key&;
        using pointer = const Key*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator() = default;
        reference operator*() const noexcept { return current_->key; }
        pointer operator->() const noexcept { return &current_->key; }
        iterator& operator++() {
            current_ = current_ == nullptr ? nullptr : owner_->successor(current_);
            return *this;
        }
        iterator operator++(int) {
            iterator copy = *this;
            ++(*this);
            return copy;
        }
        iterator& operator--() {
            current_ = current_ == nullptr ? maximum(owner_->root_)
                                           : owner_->predecessor(current_);
            return *this;
        }
        iterator operator--(int) {
            iterator copy = *this;
            --(*this);
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
        friend class llrb_tree;
        const llrb_tree* owner_ = nullptr;
        const node* current_ = nullptr;

        const_iterator(const llrb_tree* owner, const node* current) noexcept
            : owner_(owner), current_(current) {}

    public:
        using value_type = Key;
        using reference = const Key&;
        using pointer = const Key*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator() = default;
        const_iterator(iterator other) noexcept
            : owner_(other.owner_), current_(other.current_) {}
        reference operator*() const noexcept { return current_->key; }
        pointer operator->() const noexcept { return &current_->key; }
        const_iterator& operator++() {
            current_ = current_ == nullptr ? nullptr : owner_->successor(current_);
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator copy = *this;
            ++(*this);
            return copy;
        }
        const_iterator& operator--() {
            current_ = current_ == nullptr ? maximum(owner_->root_)
                                           : owner_->predecessor(current_);
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator copy = *this;
            --(*this);
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

    llrb_tree() = default;
    explicit llrb_tree(Compare compare) : compare_(std::move(compare)) {}
    ~llrb_tree() { clear(); }

    llrb_tree(const llrb_tree&) = delete;
    llrb_tree& operator=(const llrb_tree&) = delete;
    llrb_tree(llrb_tree&&) = delete;
    llrb_tree& operator=(llrb_tree&&) = delete;

    bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }

    bool insert(const Key& key) {
        if (find_node(key) != nullptr) {
            return false;
        }
        root_ = insert_node(root_, key);
        root_->colour = color::black;
        ++size_;
        return true;
    }

    iterator find(const Key& key) {
        return iterator(this, find_node(key));
    }

    const_iterator find(const Key& key) const {
        return const_iterator(this, find_node(key));
    }

    bool contains(const Key& key) const { return find_node(key) != nullptr; }

    iterator begin() noexcept { return iterator(this, minimum(root_)); }
    const_iterator begin() const noexcept {
        return const_iterator(this, minimum(root_));
    }
    const_iterator cbegin() const noexcept { return begin(); }
    iterator end() noexcept { return iterator(this, nullptr); }
    const_iterator end() const noexcept { return const_iterator(this, nullptr); }
    const_iterator cend() const noexcept { return end(); }

    bool erase(const Key& key) {
        if (!contains(key)) {
            return false;
        }
        if (!is_red(root_->left) && !is_red(root_->right)) {
            root_->colour = color::red;
        }
        root_ = erase_node(root_, key);
        if (root_ != nullptr) {
            root_->colour = color::black;
        }
        --size_;
        return true;
    }

    void erase(iterator position) {
        if (position.current_ != nullptr) {
            erase(position.current_->key);
        }
    }

    void clear() noexcept {
        destroy_subtree(root_);
        root_ = nullptr;
        size_ = 0;
    }

    bool verify_invariants() const {
        if (root_ == nullptr) {
            return size_ == 0;
        }
        if (root_->colour != color::black) {
            return false;
        }
        verification_result result = verify_subtree(root_, nullptr, nullptr);
        return result.valid && result.count == size_;
    }
};

}  // namespace rbtree
