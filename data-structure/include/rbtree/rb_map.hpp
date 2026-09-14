#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace rbtree {

// A map-oriented red-black tree.  The sentinel is a link-only object, so real
// nodes do not carry an optional value or pay for a sentinel payload.
//
// Unlike rb_tree, this class is designed as the performance experiment: it
// stores std::pair<const Key, T>, uses one black nil_ per tree, caches the two
// boundary nodes, and obtains real nodes through the supplied allocator.
template <class Key, class T, class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
class rb_map {
private:
    using value_type_impl = std::pair<const Key, T>;
    enum class colour : unsigned char { red, black };

    // link is the part shared by real nodes and the sentinel.  Keeping the
    // map value out of link means nil_ has no fake Key/T object to construct.
    struct link {
        link* parent = nullptr;
        link* left = nullptr;
        link* right = nullptr;
        colour colour_value = colour::black;
    };

    // Key is const by design: changing a key in place would invalidate the
    // BST ordering.  T remains mutable through iterator->second.
    struct node final : link {
        template <class K, class... Args>
        explicit node(K&& key, Args&&... args)
            : value(std::piecewise_construct,
                    std::forward_as_tuple(std::forward<K>(key)),
                    std::forward_as_tuple(std::forward<Args>(args)...)) {
            this->colour_value = colour::red;
        }

        value_type_impl value;
    };

    // The allocator supplied for pair<const Key, T> is rebound to the actual
    // node type.  All allocate/construct calls must be paired with
    // destroy/deallocate using these traits.
    using node_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<node>;
    using node_allocator_traits = std::allocator_traits<node_allocator>;

    Compare compare_{};
    node_allocator allocator_{};
    // nil_ is a real link object owned by this container, never an allocated
    // node.  root_/leftmost_/rightmost_ point at it when the tree is empty.
    link nil_{};
    link* root_ = nullptr;
    link* leftmost_ = nullptr;
    link* rightmost_ = nullptr;
    std::size_t size_ = 0;

    static bool is_red(const link* current) noexcept {
        return current->colour_value == colour::red;
    }

    static bool is_black(const link* current) noexcept {
        return current->colour_value == colour::black;
    }

    static link* minimum(link* current, const link* nil) noexcept {
        while (current != nil && current->left != nil) {
            current = current->left;
        }
        return current;
    }

    static const link* minimum(const link* current, const link* nil) noexcept {
        while (current != nil && current->left != nil) {
            current = current->left;
        }
        return current;
    }

    static link* maximum(link* current, const link* nil) noexcept {
        while (current != nil && current->right != nil) {
            current = current->right;
        }
        return current;
    }

    static link* successor(link* current, const link* nil) noexcept {
        if (current->right != nil) {
            return minimum(current->right, nil);
        }
        link* parent = current->parent;
        while (parent != nil && current == parent->right) {
            current = parent;
            parent = parent->parent;
        }
        return parent;
    }

    static link* predecessor(link* current, const link* nil) noexcept {
        if (current->left != nil) {
            return maximum(current->left, nil);
        }
        link* parent = current->parent;
        while (parent != nil && current == parent->left) {
            current = parent;
            parent = parent->parent;
        }
        return parent;
    }

    static const Key& key_of(const link* current) noexcept {
        return static_cast<const node*>(current)->value.first;
    }

    static T& mapped_of(link* current) noexcept {
        return static_cast<node*>(current)->value.second;
    }

    static const T& mapped_of(const link* current) noexcept {
        return static_cast<const node*>(current)->value.second;
    }

    // Reinitialize the non-owning boundary links after construction/clear.
    // The sentinel is black and is never counted as a real map element.
    void reset_nil() noexcept {
        nil_.parent = &nil_;
        nil_.left = &nil_;
        nil_.right = &nil_;
        nil_.colour_value = colour::black;
        root_ = &nil_;
        leftmost_ = &nil_;
        rightmost_ = &nil_;
    }

    // Rotations preserve in-order key order.  Do not update a shared nil_'s
    // parent as if it were a real child: its parent is temporary state used by
    // deletion fixup and must not be accidentally overwritten by a rotation.
    void left_rotate(link* pivot) noexcept {
        link* child = pivot->right;
        pivot->right = child->left;
        if (child->left != &nil_) {
            child->left->parent = pivot;
        }
        child->parent = pivot->parent;
        if (pivot->parent == &nil_) {
            root_ = child;
        } else if (pivot == pivot->parent->left) {
            pivot->parent->left = child;
        } else {
            pivot->parent->right = child;
        }
        child->left = pivot;
        pivot->parent = child;
    }

    void right_rotate(link* pivot) noexcept {
        link* child = pivot->left;
        pivot->left = child->right;
        if (child->right != &nil_) {
            child->right->parent = pivot;
        }
        child->parent = pivot->parent;
        if (pivot->parent == &nil_) {
            root_ = child;
        } else if (pivot == pivot->parent->right) {
            pivot->parent->right = child;
        } else {
            pivot->parent->left = child;
        }
        child->right = pivot;
        pivot->parent = child;
    }

    // insert_fixup starts with a red leaf.  A red parent is the only possible
    // new violation; recoloring moves it upward, while the triangle/line
    // cases use one or two rotations.  The second half is the mirror image.
    void insert_fixup(link* current) noexcept {
        while (is_red(current->parent)) {
            link* parent = current->parent;
            link* grandparent = parent->parent;
            if (parent == grandparent->left) {
                link* uncle = grandparent->right;
                if (is_red(uncle)) {
                    // Case 1: parent and uncle are red. Recolor both black
                    // and move the possible red-red violation upward.
                    parent->colour_value = colour::black;
                    uncle->colour_value = colour::black;
                    grandparent->colour_value = colour::red;
                    current = grandparent;
                } else {
                    if (current == parent->right) {
                        // Case 2: an inner triangle.  Rotate parent to turn it
                        // into the outer-line case below.
                        current = parent;
                        left_rotate(current);
                        parent = current->parent;
                        grandparent = parent->parent;
                    }
                    // Case 3: outer line. Recolor and rotate grandparent to
                    // restore the local black-height relationship.
                    parent->colour_value = colour::black;
                    grandparent->colour_value = colour::red;
                    right_rotate(grandparent);
                }
            } else {
                link* uncle = grandparent->left;
                if (is_red(uncle)) {
                    // Mirror of the left-side case 1.
                    parent->colour_value = colour::black;
                    uncle->colour_value = colour::black;
                    grandparent->colour_value = colour::red;
                    current = grandparent;
                } else {
                    if (current == parent->left) {
                        // Mirror of the inner-triangle case.
                        current = parent;
                        right_rotate(current);
                        parent = current->parent;
                        grandparent = parent->parent;
                    }
                    // Mirror of case 3: recolor and rotate the grandparent.
                    parent->colour_value = colour::black;
                    grandparent->colour_value = colour::red;
                    left_rotate(grandparent);
                }
            }
        }
        root_->colour_value = colour::black;
        root_->parent = &nil_;
    }

    // transplant changes a tree position and records the replacement's parent.
    // Setting nil_.parent here is intentional: erase_fixup needs the parent of
    // a missing child, but no caller may treat nil_ as a user-visible node.
    void transplant(link* old_node, link* new_node) noexcept {
        if (old_node->parent == &nil_) {
            root_ = new_node;
        } else if (old_node == old_node->parent->left) {
            old_node->parent->left = new_node;
        } else {
            old_node->parent->right = new_node;
        }
        new_node->parent = old_node->parent;
    }

    // Removing a black node leaves an extra-black position at current.  The
    // sibling cases are the standard four CLRS cases; the right branch mirrors
    // the left branch.  The final assignments restore the root and sentinel
    // contracts even when current is nil_.
    void erase_fixup(link* current) noexcept {
        while (current != root_ && is_black(current)) {
            if (current == current->parent->left) {
                link* sibling = current->parent->right;
                if (is_red(sibling)) {
                    sibling->colour_value = colour::black;
                    current->parent->colour_value = colour::red;
                    left_rotate(current->parent);
                    sibling = current->parent->right;
                }
                if (is_black(sibling->left) && is_black(sibling->right)) {
                    if (sibling != &nil_) {
                        sibling->colour_value = colour::red;
                    }
                    current = current->parent;
                } else {
                    if (is_black(sibling->right)) {
                        sibling->left->colour_value = colour::black;
                        sibling->colour_value = colour::red;
                        right_rotate(sibling);
                        sibling = current->parent->right;
                    }
                    sibling->colour_value = current->parent->colour_value;
                    current->parent->colour_value = colour::black;
                    sibling->right->colour_value = colour::black;
                    left_rotate(current->parent);
                    current = root_;
                }
            } else {
                link* sibling = current->parent->left;
                if (is_red(sibling)) {
                    sibling->colour_value = colour::black;
                    current->parent->colour_value = colour::red;
                    right_rotate(current->parent);
                    sibling = current->parent->left;
                }
                if (is_black(sibling->right) && is_black(sibling->left)) {
                    if (sibling != &nil_) {
                        sibling->colour_value = colour::red;
                    }
                    current = current->parent;
                } else {
                    if (is_black(sibling->left)) {
                        sibling->right->colour_value = colour::black;
                        sibling->colour_value = colour::red;
                        left_rotate(sibling);
                        sibling = current->parent->left;
                    }
                    sibling->colour_value = current->parent->colour_value;
                    current->parent->colour_value = colour::black;
                    sibling->left->colour_value = colour::black;
                    right_rotate(current->parent);
                    current = root_;
                }
            }
        }
        current->colour_value = colour::black;
        root_->parent = &nil_;
        nil_.colour_value = colour::black;
    }

    // Constructing can throw (for example, T's constructor).  Deallocate the
    // raw slot on failure; after success, initialize all links to this tree's
    // sentinel before the node is attached to the search tree.
    template <class... Args>
    node* allocate_node(Args&&... args) {
        node* result = node_allocator_traits::allocate(allocator_, 1);
        try {
            node_allocator_traits::construct(allocator_, result,
                                             std::forward<Args>(args)...);
        } catch (...) {
            node_allocator_traits::deallocate(allocator_, result, 1);
            throw;
        }
        result->parent = &nil_;
        result->left = &nil_;
        result->right = &nil_;
        return result;
    }

    void destroy_node(link* current) noexcept {
        node* actual = static_cast<node*>(current);
        node_allocator_traits::destroy(allocator_, actual);
        node_allocator_traits::deallocate(allocator_, actual, 1);
    }

    void destroy_subtree(link* current) noexcept {
        if (current == &nil_) {
            return;
        }
        destroy_subtree(current->left);
        destroy_subtree(current->right);
        destroy_node(current);
    }

    // Search follows comparator directions until an equivalent key or nil_.
    // It performs no allocation, and is O(log n) under the tree invariants.
    link* find_link(const Key& key) noexcept(noexcept(std::declval<Compare>()(key, key))) {
        link* current = root_;
        while (current != &nil_) {
            if (compare_(key, key_of(current))) {
                current = current->left;
            } else if (compare_(key_of(current), key)) {
                current = current->right;
            } else {
                return current;
            }
        }
        return &nil_;
    }

    const link* find_link(const Key& key) const noexcept(noexcept(std::declval<Compare>()(key, key))) {
        const link* current = root_;
        while (current != &nil_) {
            if (compare_(key, key_of(current))) {
                current = current->left;
            } else if (compare_(key_of(current), key)) {
                current = current->right;
            } else {
                return current;
            }
        }
        return &nil_;
    }

    // Search and insertion are combined so the parent and side found by the
    // search are reused.  Duplicate keys return before allocation, preserving
    // the existing mapped value and keeping insertion O(log n).
    template <class... Args>
    std::pair<link*, bool> insert_node(const Key& key, Args&&... args) {
        link* parent = &nil_;
        link* current = root_;
        bool goes_left = false;
        while (current != &nil_) {
            parent = current;
            if (compare_(key, key_of(current))) {
                current = current->left;
                goes_left = true;
            } else if (compare_(key_of(current), key)) {
                current = current->right;
                goes_left = false;
            } else {
                return {current, false};
            }
        }

        node* inserted = allocate_node(key, std::forward<Args>(args)...);
        inserted->parent = parent;
        if (parent == &nil_) {
            root_ = inserted;
            leftmost_ = inserted;
            rightmost_ = inserted;
        } else if (goes_left) {
            parent->left = inserted;
            if (leftmost_ == parent || compare_(key, key_of(leftmost_))) {
                leftmost_ = inserted;
            }
        } else {
            parent->right = inserted;
            if (rightmost_ == parent || compare_(key_of(rightmost_), key)) {
                rightmost_ = inserted;
            }
        }
        ++size_;
        insert_fixup(inserted);
        return {inserted, true};
    }

    struct verification_result {
        bool valid = true;
        std::size_t count = 0;
        std::size_t black_height = 1;
    };

    verification_result verify_subtree(const link* current, const Key* lower,
                                       const Key* upper) const {
        if (current == &nil_) {
            return {};
        }
        if (lower != nullptr && !compare_(*lower, key_of(current))) {
            return {false, 0, 0};
        }
        if (upper != nullptr && !compare_(key_of(current), *upper)) {
            return {false, 0, 0};
        }
        if ((current->left != &nil_ && current->left->parent != current) ||
            (current->right != &nil_ && current->right->parent != current)) {
            return {false, 0, 0};
        }
        if (is_red(current) && (is_red(current->left) || is_red(current->right))) {
            return {false, 0, 0};
        }
        verification_result left = verify_subtree(current->left, lower, &key_of(current));
        verification_result right = verify_subtree(current->right, &key_of(current), upper);
        if (!left.valid || !right.valid || left.black_height != right.black_height) {
            return {false, 0, 0};
        }
        return {true, left.count + right.count + 1,
                left.black_height + (is_black(current) ? 1U : 0U)};
    }

public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = value_type_impl;
    using size_type = std::size_t;
    using allocator_type = Allocator;

    class const_iterator;

    class iterator {
        friend class rb_map;
        friend class const_iterator;
        rb_map* owner_ = nullptr;
        link* current_ = nullptr;
        iterator(rb_map* owner, link* current) noexcept : owner_(owner), current_(current) {}

    public:
        using reference = value_type&;
        using pointer = value_type*;
        iterator() = default;
        reference operator*() const noexcept { return static_cast<node*>(current_)->value; }
        pointer operator->() const noexcept { return &static_cast<node*>(current_)->value; }
        iterator& operator++() noexcept {
            current_ = successor(current_, &owner_->nil_);
            return *this;
        }
        iterator operator++(int) noexcept { iterator copy = *this; ++(*this); return copy; }
        // nil_ is the end marker.  Caching rightmost_ makes --end() O(1),
        // while ordinary predecessor traversal remains O(height).
        iterator& operator--() noexcept {
            current_ = current_ == &owner_->nil_ ? owner_->rightmost_ : predecessor(current_, &owner_->nil_);
            return *this;
        }
        iterator operator--(int) noexcept { iterator copy = *this; --(*this); return copy; }
        friend bool operator==(iterator left, iterator right) noexcept { return left.current_ == right.current_; }
        friend bool operator!=(iterator left, iterator right) noexcept { return !(left == right); }
    };

    class const_iterator {
        friend class rb_map;
        const rb_map* owner_ = nullptr;
        const link* current_ = nullptr;
        const_iterator(const rb_map* owner, const link* current) noexcept : owner_(owner), current_(current) {}

    public:
        using reference = const value_type&;
        using pointer = const value_type*;
        const_iterator() = default;
        const_iterator(iterator other) noexcept : owner_(other.owner_), current_(other.current_) {}
        reference operator*() const noexcept { return static_cast<const node*>(current_)->value; }
        pointer operator->() const noexcept { return &static_cast<const node*>(current_)->value; }
        const_iterator& operator++() noexcept {
            current_ = successor(const_cast<link*>(current_), &owner_->nil_);
            return *this;
        }
        const_iterator operator++(int) noexcept { const_iterator copy = *this; ++(*this); return copy; }
        const_iterator& operator--() noexcept {
            current_ = current_ == &owner_->nil_ ? owner_->rightmost_ : predecessor(const_cast<link*>(current_), &owner_->nil_);
            return *this;
        }
        const_iterator operator--(int) noexcept { const_iterator copy = *this; --(*this); return copy; }
        friend bool operator==(const_iterator left, const_iterator right) noexcept { return left.current_ == right.current_; }
        friend bool operator!=(const_iterator left, const_iterator right) noexcept { return !(left == right); }
    };

    rb_map() { reset_nil(); }
    explicit rb_map(const Compare& compare, const Allocator& allocator = Allocator())
        : compare_(compare), allocator_(allocator) { reset_nil(); }
    explicit rb_map(const Allocator& allocator) : allocator_(allocator) { reset_nil(); }
    ~rb_map() { clear(); }

    rb_map(const rb_map&) = delete;
    rb_map& operator=(const rb_map&) = delete;
    rb_map(rb_map&&) = delete;
    rb_map& operator=(rb_map&&) = delete;

    bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }
    iterator begin() noexcept { return iterator(this, leftmost_); }
    const_iterator begin() const noexcept { return const_iterator(this, leftmost_); }
    const_iterator cbegin() const noexcept { return begin(); }
    iterator end() noexcept { return iterator(this, &nil_); }
    const_iterator end() const noexcept { return const_iterator(this, &nil_); }
    const_iterator cend() const noexcept { return end(); }

    // insert(value) copies both key and mapped value.  try_emplace constructs
    // T only after the key search proves that the key is absent.
    std::pair<iterator, bool> insert(const value_type& value) {
        auto result = insert_node(value.first, value.second);
        return {iterator(this, result.first), result.second};
    }

    template <class... Args>
    std::pair<iterator, bool> try_emplace(const Key& key, Args&&... args) {
        auto result = insert_node(key, std::forward<Args>(args)...);
        return {iterator(this, result.first), result.second};
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(const Key& key, Args&&... args) {
        return try_emplace(key, std::forward<Args>(args)...);
    }

    mapped_type& operator[](const Key& key) { return try_emplace(key).first->second; }

    iterator find(const Key& key) noexcept(noexcept(std::declval<Compare>()(key, key))) {
        return iterator(this, find_link(key));
    }
    const_iterator find(const Key& key) const noexcept(noexcept(std::declval<Compare>()(key, key))) {
        return const_iterator(this, find_link(key));
    }
    bool contains(const Key& key) const noexcept(noexcept(std::declval<Compare>()(key, key))) {
        return find_link(key) != &nil_;
    }

    std::size_t erase(const Key& key) {
        link* target = find_link(key);
        if (target == &nil_) return 0;
        erase(iterator(this, target));
        return 1;
    }

    // Erasing a node with two children physically transplants its successor;
    // only target is destroyed.  If the physically removed node was black,
    // erase_fixup restores black height and red/black constraints.
    void erase(iterator position) noexcept {
        link* target = position.current_;
        if (target == &nil_) return;
        link* removed = target;
        colour removed_colour = removed->colour_value;
        link* replacement;
        if (target->left == &nil_) {
            // Zero/one-child case: replacement takes target's position.
            replacement = target->right;
            transplant(target, target->right);
        } else if (target->right == &nil_) {
            // Symmetric one-child case.
            replacement = target->left;
            transplant(target, target->left);
        } else {
            // Two-child case: move the in-order successor.  Its original
            // color, not target's color, determines whether fixup is needed.
            removed = minimum(target->right, &nil_);
            removed_colour = removed->colour_value;
            replacement = removed->right;
            if (removed->parent == target) {
                replacement->parent = removed;
            } else {
                transplant(removed, removed->right);
                removed->right = target->right;
                removed->right->parent = removed;
            }
            transplant(target, removed);
            removed->left = target->left;
            removed->left->parent = removed;
            removed->colour_value = target->colour_value;
        }
        const bool was_leftmost = target == leftmost_;
        const bool was_rightmost = target == rightmost_;
        destroy_node(target);
        --size_;
        if (removed_colour == colour::black) erase_fixup(replacement);
        if (size_ == 0) {
            reset_nil();
        } else {
            if (was_leftmost) leftmost_ = minimum(root_, &nil_);
            if (was_rightmost) rightmost_ = maximum(root_, &nil_);
            root_->parent = &nil_;
            nil_.colour_value = colour::black;
        }
    }

    // clear destroys only real nodes.  The embedded nil_ remains alive and is
    // reset so the object can be reused immediately.
    void clear() noexcept {
        destroy_subtree(root_);
        size_ = 0;
        reset_nil();
    }

    // This is a diagnostic traversal, not a hot-path operation.  It checks
    // ordering, parent links, red edges, black height, node count and cached
    // boundaries; nil_ itself is never included in the count.
    bool verify_invariants() const {
        if (root_ == &nil_) return size_ == 0 && leftmost_ == &nil_ && rightmost_ == &nil_;
        if (root_->parent != &nil_ || !is_black(root_) || !is_black(&nil_)) return false;
        verification_result result = verify_subtree(root_, nullptr, nullptr);
        return result.valid && result.count == size_ && leftmost_ == minimum(root_, &nil_) &&
               rightmost_ == maximum(root_, &nil_);
    }
};

}  // namespace rbtree
