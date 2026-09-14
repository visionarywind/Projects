#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

namespace rbtree {

// Teaching-oriented set-like red-black tree.
//
// This type stores only one Key per node and uses nullptr as the external leaf
// representation.  The implementation intentionally keeps ownership and the
// balancing algorithm visible: each real node is allocated with new and is
// destroyed exactly once by erase() or clear().  rb_map (in rb_map.hpp) uses a
// different, more allocation-oriented representation, so changes made here
// remain easy to compare with the textbook algorithm.
template <typename Key, typename Compare = std::less<Key>>
class rb_tree {
private:
    // nullptr is deliberately used as the external black leaf in this
    // teaching implementation.  A node owns no child through these pointers;
    // all links are non-owning, and the tree owns every node reachable from
    // root_.
    enum class color { red, black };

    struct node {
        explicit node(const Key& value) : key(value) {}

        Key key;
        color colour = color::red;
        node* parent = nullptr;
        node* left = nullptr;
        node* right = nullptr;
    };

    // compare_ defines the BST order. root_ is the only owning entry point;
    // size_ counts real nodes and never counts nullptr leaves.
    Compare compare_{};
    node* root_ = nullptr;
    std::size_t size_ = 0;

    static color node_color(const node* current) noexcept {
        return current == nullptr ? color::black : current->colour;
    }

    // Boundary search follows one root-to-leaf path, so it is O(height),
    // which is O(log n) when the red-black invariants hold.  Empty subtrees
    // return nullptr.
    static node* minimum(node* current) noexcept {
        if (current == nullptr) {
            return nullptr;
        }
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    static const node* minimum(const node* current) noexcept {
        if (current == nullptr) {
            return nullptr;
        }
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    static node* maximum(node* current) noexcept {
        if (current == nullptr) {
            return nullptr;
        }
        while (current->right != nullptr) {
            current = current->right;
        }
        return current;
    }

    // The successor either descends into the right subtree or climbs until it
    // leaves a left edge.  predecessor() below is its mirror; both are
    // O(height) and do not move nodes.
    static node* successor(node* current) noexcept {
        if (current == nullptr) {
            return nullptr;
        }
        if (current->right != nullptr) {
            return minimum(current->right);
        }
        node* parent = current->parent;
        while (parent != nullptr && current == parent->right) {
            current = parent;
            parent = parent->parent;
        }
        return parent;
    }

    static const node* successor(const node* current) noexcept {
        if (current == nullptr) {
            return nullptr;
        }
        if (current->right != nullptr) {
            return minimum(current->right);
        }
        const node* parent = current->parent;
        while (parent != nullptr && current == parent->right) {
            current = parent;
            parent = parent->parent;
        }
        return parent;
    }

    static node* predecessor(node* current) noexcept {
        if (current == nullptr) {
            return nullptr;
        }
        if (current->left != nullptr) {
            return maximum(current->left);
        }
        node* parent = current->parent;
        while (parent != nullptr && current == parent->left) {
            current = parent;
            parent = parent->parent;
        }
        return parent;
    }

    static const node* predecessor(const node* current) noexcept {
        if (current == nullptr) {
            return nullptr;
        }
        if (current->left != nullptr) {
            const node* result = current->left;
            while (result->right != nullptr) {
                result = result->right;
            }
            return result;
        }
        const node* parent = current->parent;
        while (parent != nullptr && current == parent->left) {
            current = parent;
            parent = parent->parent;
        }
        return parent;
    }

    // Left rotation preserves in-order order while changing the local shape:
    // (1) move the promoted child's left subtree to pivot's right,
    // (2) give the child pivot's old parent, and (3) put pivot below child.
    // No node is allocated or moved, so unrelated iterators remain valid.
    void left_rotate(node* pivot) noexcept {
        node* child = pivot->right;
        pivot->right = child->left;
        if (child->left != nullptr) {
            child->left->parent = pivot;
        }
        child->parent = pivot->parent;
        if (pivot->parent == nullptr) {
            root_ = child;
        } else if (pivot == pivot->parent->left) {
            pivot->parent->left = child;
        } else {
            pivot->parent->right = child;
        }
        child->left = pivot;
        pivot->parent = child;
    }

    void right_rotate(node* pivot) noexcept {
        // Mirror operation: move child->right to pivot->left, then put pivot
        // below the promoted child while preserving all parent links.
        node* child = pivot->left;
        pivot->left = child->right;
        if (child->right != nullptr) {
            child->right->parent = pivot;
        }
        child->parent = pivot->parent;
        if (pivot->parent == nullptr) {
            root_ = child;
        } else if (pivot == pivot->parent->right) {
            pivot->parent->right = child;
        } else {
            pivot->parent->left = child;
        }
        child->right = pivot;
        pivot->parent = child;
    }

    // insert_fixup repairs only violations introduced by inserting a red leaf.
    // A red parent creates a red-red edge.  The uncle-red case pushes the
    // problem upward; the uncle-black cases turn an inner triangle into an
    // outer line and rotate.  The right branch is the exact mirror.
    void insert_fixup(node* current) noexcept {
        while (current != root_ && node_color(current->parent) == color::red) {
            node* parent = current->parent;
            node* grandparent = parent->parent;
            if (parent == grandparent->left) {
                node* uncle = grandparent->right;
                if (node_color(uncle) == color::red) {
                    parent->colour = color::black;
                    uncle->colour = color::black;
                    grandparent->colour = color::red;
                    current = grandparent;
                } else {
                    if (current == parent->right) {
                        current = parent;
                        left_rotate(current);
                        parent = current->parent;
                        grandparent = parent->parent;
                    }
                    parent->colour = color::black;
                    grandparent->colour = color::red;
                    right_rotate(grandparent);
                }
            } else {
                node* uncle = grandparent->left;
                if (node_color(uncle) == color::red) {
                    parent->colour = color::black;
                    uncle->colour = color::black;
                    grandparent->colour = color::red;
                    current = grandparent;
                } else {
                    if (current == parent->left) {
                        current = parent;
                        right_rotate(current);
                        parent = current->parent;
                        grandparent = parent->parent;
                    }
                    parent->colour = color::black;
                    grandparent->colour = color::red;
                    left_rotate(grandparent);
                }
            }
        }
        if (root_ != nullptr) {
            root_->colour = color::black;
            root_->parent = nullptr;
        }
    }

    // transplant rewires one position but never destroys either node.  The
    // caller must retain the node that represents the replacement and decide
    // separately which physical node is eventually deleted.
    void transplant(node* old_node, node* new_node) noexcept {
        if (old_node->parent == nullptr) {
            root_ = new_node;
        } else if (old_node == old_node->parent->left) {
            old_node->parent->left = new_node;
        } else {
            old_node->parent->right = new_node;
        }
        if (new_node != nullptr) {
            new_node->parent = old_node->parent;
        }
    }

    // Removing a black node creates a temporary extra-black position.  Since
    // nullptr has no parent field, parent_of_current carries the missing
    // parent while current is null.  The four sibling cases are mirrored for
    // the right side.
    void erase_fixup(node* current, node* parent_of_current) noexcept {
        while (current != root_ && node_color(current) == color::black) {
            if (parent_of_current == nullptr) {
                break;
            }
            if (current == parent_of_current->left) {
                node* sibling = parent_of_current->right;
                if (node_color(sibling) == color::red) {
                    sibling->colour = color::black;
                    parent_of_current->colour = color::red;
                    left_rotate(parent_of_current);
                    sibling = parent_of_current->right;
                }
                if (sibling == nullptr ||
                    (node_color(sibling->left) == color::black &&
                     node_color(sibling->right) == color::black)) {
                    // Case 2: the sibling has no red child, so move the
                    // temporary extra blackness up to the parent.
                    if (sibling != nullptr) {
                        sibling->colour = color::red;
                    }
                    current = parent_of_current;
                    parent_of_current = current->parent;
                } else {
                    if (node_color(sibling->right) == color::black) {
                        if (sibling->left != nullptr) {
                            sibling->left->colour = color::black;
                        }
                        sibling->colour = color::red;
                        right_rotate(sibling);
                        sibling = parent_of_current->right;
                    }
                    sibling->colour = parent_of_current->colour;
                    parent_of_current->colour = color::black;
                    if (sibling->right != nullptr) {
                        sibling->right->colour = color::black;
                    }
                    left_rotate(parent_of_current);
                    current = root_;
                    parent_of_current = nullptr;
                }
            } else {
                node* sibling = parent_of_current->left;
                if (node_color(sibling) == color::red) {
                    sibling->colour = color::black;
                    parent_of_current->colour = color::red;
                    right_rotate(parent_of_current);
                    sibling = parent_of_current->left;
                }
                if (sibling == nullptr ||
                    (node_color(sibling->right) == color::black &&
                     node_color(sibling->left) == color::black)) {
                    if (sibling != nullptr) {
                        sibling->colour = color::red;
                    }
                    current = parent_of_current;
                    parent_of_current = current->parent;
                } else {
                    if (node_color(sibling->left) == color::black) {
                        if (sibling->right != nullptr) {
                            sibling->right->colour = color::black;
                        }
                        sibling->colour = color::red;
                        left_rotate(sibling);
                        sibling = parent_of_current->left;
                    }
                    sibling->colour = parent_of_current->colour;
                    parent_of_current->colour = color::black;
                    if (sibling->left != nullptr) {
                        sibling->left->colour = color::black;
                    }
                    right_rotate(parent_of_current);
                    current = root_;
                    parent_of_current = nullptr;
                }
            }
        }
        if (current != nullptr) {
            current->colour = color::black;
        }
        if (root_ != nullptr) {
            root_->colour = color::black;
            root_->parent = nullptr;
        }
    }

    void destroy_subtree(node* current) noexcept {
        if (current == nullptr) {
            return;
        }
        destroy_subtree(current->left);
        destroy_subtree(current->right);
        delete current;
    }

    node* find_node(const Key& key) noexcept(noexcept(std::declval<Compare>()(key, key))) {
        node* current = root_;
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

    const node* find_node(const Key& key) const noexcept(noexcept(std::declval<Compare>()(key, key))) {
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

    struct verification_result {
        bool valid = true;
        std::size_t count = 0;
        std::size_t black_height = 1;
    };

    verification_result verify_subtree(const node* current,
                                       const Key* lower,
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
        if (current->left != nullptr && current->left->parent != current) {
            return {false, 0, 0};
        }
        if (current->right != nullptr && current->right->parent != current) {
            return {false, 0, 0};
        }
        if (current->colour == color::red &&
            (node_color(current->left) == color::red ||
             node_color(current->right) == color::red)) {
            return {false, 0, 0};
        }

        verification_result left = verify_subtree(current->left, lower, &current->key);
        verification_result right = verify_subtree(current->right, &current->key, upper);
        if (!left.valid || !right.valid || left.black_height != right.black_height) {
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
        friend class rb_tree;
        friend class const_iterator;

        rb_tree* owner_ = nullptr;
        node* current_ = nullptr;

        iterator(rb_tree* owner, node* current) noexcept
            : owner_(owner), current_(current) {}

    public:
        using value_type = Key;
        using reference = const Key&;
        using pointer = const Key*;

        iterator() = default;

        reference operator*() const noexcept { return current_->key; }
        pointer operator->() const noexcept { return &current_->key; }

        iterator& operator++() noexcept {
            current_ = successor(current_);
            return *this;
        }

        iterator operator++(int) noexcept {
            iterator copy = *this;
            ++(*this);
            return copy;
        }

        // end() is represented by nullptr.  Keeping owner_ lets --end()
        // return the maximum node instead of dereferencing nullptr.
        iterator& operator--() noexcept {
            current_ = current_ == nullptr ? maximum(owner_->root_) : predecessor(current_);
            return *this;
        }

        iterator operator--(int) noexcept {
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
        friend class rb_tree;
        const rb_tree* owner_ = nullptr;
        const node* current_ = nullptr;

        const_iterator(const rb_tree* owner, const node* current) noexcept
            : owner_(owner), current_(current) {}

    public:
        using value_type = Key;
        using reference = const Key&;
        using pointer = const Key*;

        const_iterator() = default;
        const_iterator(iterator other) noexcept
            : owner_(other.owner_), current_(other.current_) {}

        reference operator*() const noexcept { return current_->key; }
        pointer operator->() const noexcept { return &current_->key; }

        const_iterator& operator++() noexcept {
            current_ = successor(current_);
            return *this;
        }

        const_iterator operator++(int) noexcept {
            const_iterator copy = *this;
            ++(*this);
            return copy;
        }

        const_iterator& operator--() noexcept {
            current_ = current_ == nullptr ? maximum(owner_->root_) : predecessor(current_);
            return *this;
        }

        const_iterator operator--(int) noexcept {
            const_iterator copy = *this;
            --(*this);
            return copy;
        }

        friend bool operator==(const_iterator left, const_iterator right) noexcept {
            return left.current_ == right.current_;
        }
        friend bool operator!=(const_iterator left, const_iterator right) noexcept {
            return !(left == right);
        }
    };

    rb_tree() = default;
    explicit rb_tree(Compare compare) : compare_(std::move(compare)) {}

    ~rb_tree() { clear(); }

    rb_tree(const rb_tree&) = delete;
    rb_tree& operator=(const rb_tree&) = delete;
    rb_tree(rb_tree&&) = delete;
    rb_tree& operator=(rb_tree&&) = delete;

    bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }

    // Search before allocation means duplicate insertion has no allocation
    // side effect.  unique_ptr owns the new node until it has been linked;
    // after release(), root_ becomes the ownership root for that node.
    bool insert(const Key& key) {
        node* parent = nullptr;
        node* current = root_;
        while (current != nullptr) {
            parent = current;
            if (compare_(key, current->key)) {
                current = current->left;
            } else if (compare_(current->key, key)) {
                current = current->right;
            } else {
                return false;
            }
        }

        std::unique_ptr<node> holder(new node(key));
        holder->parent = parent;
        if (parent == nullptr) {
            root_ = holder.get();
        } else if (compare_(holder->key, parent->key)) {
            parent->left = holder.get();
        } else {
            parent->right = holder.get();
        }
        node* inserted = holder.release();
        ++size_;
        insert_fixup(inserted);
        return true;
    }

    bool contains(const Key& key) const {
        return find_node(key) != nullptr;
    }

    iterator find(const Key& key) noexcept(noexcept(std::declval<Compare>()(key, key))) {
        return iterator(this, find_node(key));
    }

    const_iterator find(const Key& key) const noexcept(noexcept(std::declval<Compare>()(key, key))) {
        return const_iterator(this, find_node(key));
    }

    iterator begin() noexcept { return iterator(this, minimum(root_)); }
    const_iterator begin() const noexcept { return const_iterator(this, minimum(root_)); }
    const_iterator cbegin() const noexcept { return begin(); }

    iterator end() noexcept { return iterator(this, nullptr); }
    const_iterator end() const noexcept { return const_iterator(this, nullptr); }
    const_iterator cend() const noexcept { return end(); }

    bool erase(const Key& key) {
        node* target = find_node(key);
        if (target == nullptr) {
            return false;
        }
        erase(iterator(this, target));
        return true;
    }

    // Erase invalidates only the iterator pointing at target.  With two
    // children, the successor node is physically transplanted rather than
    // copied, preserving the key/value object and its address.
    void erase(iterator position) noexcept {
        node* target = position.current_;
        if (target == nullptr) {
            return;
        }

        node* moved = target;
        color original_colour = moved->colour;
        node* replacement = nullptr;
        node* parent_of_replacement = nullptr;

        if (target->left == nullptr) {
            replacement = target->right;
            parent_of_replacement = target->parent;
            transplant(target, target->right);
        } else if (target->right == nullptr) {
            replacement = target->left;
            parent_of_replacement = target->parent;
            transplant(target, target->left);
        } else {
            moved = minimum(target->right);
            original_colour = moved->colour;
            replacement = moved->right;
            if (moved->parent == target) {
                parent_of_replacement = moved;
                if (replacement != nullptr) {
                    replacement->parent = moved;
                }
            } else {
                parent_of_replacement = moved->parent;
                transplant(moved, moved->right);
                moved->right = target->right;
                moved->right->parent = moved;
            }
            transplant(target, moved);
            moved->left = target->left;
            moved->left->parent = moved;
            moved->colour = target->colour;
        }

        delete target;
        --size_;
        if (original_colour == color::black) {
            erase_fixup(replacement, parent_of_replacement);
        }
        if (root_ != nullptr) {
            root_->parent = nullptr;
        }
    }

    void clear() noexcept {
        destroy_subtree(root_);
        root_ = nullptr;
        size_ = 0;
    }

    // Debug diagnostic for BST ordering, parent links, root color, the
    // red-red prohibition, equal black height, and size consistency.
    bool verify_invariants() const {
        if (root_ == nullptr) {
            return size_ == 0;
        }
        if (root_->parent != nullptr || root_->colour != color::black) {
            return false;
        }
        verification_result result = verify_subtree(root_, nullptr, nullptr);
        return result.valid && result.count == size_;
    }
};

}  // namespace rbtree
