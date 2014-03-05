#pragma once

namespace snapshot {

namespace detail {

// TODO

}; // namespace snapshot::detail

template<class Key, class Value, int fanout = 16>
class btree {

    struct node {
        typedef enum {
            NONE,
            ROOT,
            INTERNAL,
            LEAF,
        } kind_t;

        kind_t kind;
        node* parent;
        bool rdonly;

        union {
            struct {
                node* child[fanout];
                Key* key[fanout - 1];
                int key_count;
            };
        };

        static const int max_key_count_s = fanout - 1;

        node(kind_t k = NONE, node* p = nullptr): kind(k), parent(p), rdonly(false) {
            key_count = 0;
            for (int i = 0; i < fanout; i++) {
                child[i] = nullptr;
            }
            for (int i = 0; i < max_key_count_s; i++) {
                key[i] = nullptr;
            }
        }
    };

    node root_;

public:

    btree(): root_(node(node::ROOT)) {}

    size_t size() const {
        // TODO
        return 0;
    }

};

} // namespace snapshot
