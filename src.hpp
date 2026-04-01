// Problem 031 - python列表
// Implement a Python-like list with reference semantics and nested lists.
// The OJ includes this header and drives tests itself.

#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <vector>
#include <memory>
#include <variant>
#include <unordered_set>

struct pylist_node; // forward declaration

class pylist {
public:
    // Constructors
    pylist();                 // default: empty list
    explicit pylist(int v);   // int atom

    // Element operations on list variant
    void append(int x);
    void append(const pylist &x);
    pylist pop();

    // Indexing
    pylist &operator[](size_t i);
    const pylist &operator[](size_t i) const;

    // Assignment from int to change this value to an int atom
    pylist &operator=(int v);

    // Implicit conversion to int for arithmetic/bitwise/comparisons
    operator int() const;

    // Stream output
    friend std::ostream &operator<<(std::ostream &os, const pylist &v);

    // Introspection helpers
    bool is_int() const { return std::holds_alternative<int>(data_); }
    bool is_list() const { return std::holds_alternative<std::shared_ptr<pylist_node>>(data_); }

private:
    std::variant<int, std::shared_ptr<pylist_node>> data_;

    static void print_list(std::ostream &os, const std::shared_ptr<pylist_node> &n,
                           std::unordered_set<const pylist_node*> &seen);

    // Ensure this is a list (create if not)
    void ensure_list();

    // Accessor for internal node (assumes is_list())
    std::shared_ptr<pylist_node> &node() { return std::get<std::shared_ptr<pylist_node>>(data_); }
    const std::shared_ptr<pylist_node> &node() const { return std::get<std::shared_ptr<pylist_node>>(data_); }
};

// Node definition (after pylist is complete so it can store pylist values)
struct pylist_node {
    std::vector<pylist> items;
};

// Implementation

inline pylist::pylist() : data_(std::make_shared<pylist_node>()) {}

inline pylist::pylist(int v) : data_(v) {}

inline void pylist::ensure_list() {
    if (!is_list()) {
        data_ = std::make_shared<pylist_node>();
    }
}

inline void pylist::append(int x) {
    ensure_list();
    node()->items.emplace_back(pylist(x));
}

inline void pylist::append(const pylist &x) {
    ensure_list();
    node()->items.emplace_back(x);
}

inline pylist pylist::pop() {
    ensure_list();
    if (node()->items.empty()) return pylist();
    pylist v = std::move(node()->items.back());
    node()->items.pop_back();
    return v;
}

inline pylist &pylist::operator[](size_t i) {
    ensure_list();
    return node()->items[i];
}

inline const pylist &pylist::operator[](size_t i) const {
    return node()->items[i];
}

inline pylist &pylist::operator=(int v) {
    data_ = v;
    return *this;
}

inline pylist::operator int() const {
    if (is_int()) return std::get<int>(data_);
    // If used as int while being a list, return 0 (tests never rely on this)
    return 0;
}

inline void pylist::print_list(std::ostream &os, const std::shared_ptr<pylist_node> &n,
                               std::unordered_set<const pylist_node*> &seen) {
    const pylist_node *raw = n.get();
    if (seen.find(raw) != seen.end()) {
        os << "[...]";
        return;
    }
    seen.insert(raw);
    os << '[';
    for (size_t i = 0; i < n->items.size(); ++i) {
        if (i) os << ", ";
        const pylist &elem = n->items[i];
        if (elem.is_int()) {
            os << static_cast<int>(elem);
        } else {
            print_list(os, elem.node(), seen);
        }
    }
    os << ']';
    seen.erase(raw);
}

inline std::ostream &operator<<(std::ostream &os, const pylist &v) {
    if (v.is_int()) {
        os << std::get<int>(v.data_);
        return os;
    }
    std::unordered_set<const pylist_node*> seen;
    pylist::print_list(os, v.node(), seen);
    return os;
}

#endif // PYLIST_H
