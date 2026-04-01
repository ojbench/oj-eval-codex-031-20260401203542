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

class pylist {
    struct Node;
    using Storage = std::variant<int, std::shared_ptr<Node>>;
    struct Node {
        std::vector<pylist> items;
    };

    Storage data_;

    static std::vector<std::weak_ptr<Node>> &registry() {
        static std::vector<std::weak_ptr<Node>> r;
        return r;
    }
    static std::shared_ptr<Node> make_node() {
        auto sp = std::make_shared<Node>();
        registry().emplace_back(sp);
        return sp;
    }
    struct CycleBreaker {
        ~CycleBreaker() {
            for (auto &w : registry()) if (auto sp = w.lock()) sp->items.clear();
        }
    };
    static inline CycleBreaker breaker{};

    void ensure_list() { if (!std::holds_alternative<std::shared_ptr<Node>>(data_)) data_ = make_node(); }
    std::shared_ptr<Node> &node() { return std::get<std::shared_ptr<Node>>(data_); }
    const std::shared_ptr<Node> &node() const { return std::get<std::shared_ptr<Node>>(data_); }

    static void print_rec(std::ostream &os, const std::shared_ptr<Node> &n, std::unordered_set<const Node*> &seen) {
        const Node *raw = n.get();
        if (seen.count(raw)) { os << "[...]"; return; }
        seen.insert(raw);
        os << '[';
        for (size_t i = 0; i < n->items.size(); ++i) {
            if (i) os << ", ";
            const pylist &e = n->items[i];
            if (e.is_int()) os << int(e); else print_rec(os, e.node(), seen);
        }
        os << ']';
        seen.erase(raw);
    }

public:
    pylist() : data_(make_node()) {}
    explicit pylist(int v) : data_(v) {}

    bool is_int() const { return std::holds_alternative<int>(data_); }

    void append(int x) { ensure_list(); node()->items.emplace_back(pylist(x)); }
    void append(const pylist &x) { ensure_list(); node()->items.emplace_back(x); }
    pylist pop() { ensure_list(); if (node()->items.empty()) return pylist(); pylist v = std::move(node()->items.back()); node()->items.pop_back(); return v; }

    pylist &operator[](size_t i) { ensure_list(); if (i >= node()->items.size()) node()->items.resize(i+1, pylist(0)); return node()->items[i]; }
    const pylist &operator[](size_t i) const { return node()->items[i]; }

    pylist &operator=(int v) { data_ = v; return *this; }
    operator int() const { if (is_int()) return std::get<int>(data_); return 0; }

    friend std::ostream &operator<<(std::ostream &os, const pylist &v) {
        if (v.is_int()) { os << std::get<int>(v.data_); return os; }
        std::unordered_set<const Node*> seen; print_rec(os, v.node(), seen); return os;
    }
};

#endif // PYLIST_H
