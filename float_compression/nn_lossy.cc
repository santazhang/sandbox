#include "nn_lossy.h"

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <algorithm>

#define nullptr NULL

namespace {

class Timer {
    struct timeval begin_;
    struct timeval end_;

public:
    Timer() { reset(); }
    inline void start() {
        reset();
        gettimeofday(&begin_, nullptr);
    }
    inline void stop() {
        gettimeofday(&end_, nullptr);
    }
    inline void reset() {
        begin_.tv_sec = 0;
        begin_.tv_usec = 0;
        end_.tv_sec = 0;
        end_.tv_usec = 0;
    }
    double elapsed() const {
        if (end_.tv_sec == 0 && end_.tv_usec == 0) {
            // not stopped yet
            struct timeval now;
            gettimeofday(&now, nullptr);
            return now.tv_sec - begin_.tv_sec +
                   (now.tv_usec - begin_.tv_usec) / 1000000.0;
        }
        return end_.tv_sec - begin_.tv_sec +
               (end_.tv_usec - begin_.tv_usec) / 1000000.0;
    }
};

struct huffman_node {
    huffman_node(): freq(0), left(NULL), right(NULL), sign_expo(0) {}

    int freq;
    huffman_node* left;
    huffman_node* right;
    int sign_expo;  // only meaningful if is_leaf()

    bool is_leaf() const {
        return left == NULL && right == NULL;
    }

    static bool less_than(const huffman_node* a, const huffman_node* b) {
        return a->freq < b->freq;
    }
};

struct huffman_code {
    uint64_t bits;
    int len;
    huffman_code() : bits(0), len(0) {}

    void push_1() {
        bits |= (1 << len);
        len++;
    }

    void push_0() {
        len++;
    }
};

void gen_codes(huffman_node* node, const huffman_code& code, huffman_code* out_map) {
    if (node->is_leaf()) {
        out_map[node->sign_expo] = code;
    } else {
        huffman_code left_code = code;
        left_code.push_0();
        gen_codes(node->left, left_code, out_map);

        huffman_code right_code = code;
        right_code.push_1();
        gen_codes(node->right, right_code, out_map);
    }
}

void bits_append(char** append_to, int* append_to_offset, uint64_t bits, int bits_len) {
    // TODO
}

}

// TODO if encounter any exception, fallback to 1/2 compression
void nn_compress(const float* farr, int n_floats, char** compressed, int* compressed_bytes) {
    // 1. collect sign-expo frequency
    int dict_size = 0;
    int sign_expo_freq[512];
    for (int i = 0; i < 512; i++) {
        sign_expo_freq[i] = 0;
    }
    const uint32_t* uarr = reinterpret_cast<const uint32_t*>(farr);
    Timer timer;
    timer.start();
    for (int i = 0; i < n_floats; i++) {
        sign_expo_freq[uarr[i] >> 23]++;
    }
    timer.stop();
    printf("collect sign-expo frequency: %f sec, %fMB/s\n",
        timer.elapsed(), n_floats * 4 / (1024 * 1024 * timer.elapsed()));

    *compressed_bytes = 1; // 1 byte to indicate dictionary size

    // 2. build huffman tree
    timer.start();
    huffman_node node_heap[1024];
    huffman_node* node_ptr[512];
    int next_huffman_node_idx = 0;
    for (int i = 0; i < 512; i++) {
        if (sign_expo_freq[i] > 0) {
            huffman_node& leaf_node = node_heap[next_huffman_node_idx];
            leaf_node.sign_expo = i;
            leaf_node.freq = sign_expo_freq[i];
            node_ptr[next_huffman_node_idx] = &node_heap[next_huffman_node_idx];
            next_huffman_node_idx++;

            *compressed_bytes += 2;  // 2 byte for each (sign_expo) pair, also their encoded length
        }
    }

    int tree_count = next_huffman_node_idx;
    dict_size = tree_count;
    // printf("%d huffman tree leaf nodes\n", tree_count);
    std::make_heap(&node_ptr[0], &node_ptr[tree_count], huffman_node::less_than);
    while (tree_count > 1) {
        huffman_node* left_child = node_ptr[0];
        std::pop_heap(&node_ptr[0], &node_ptr[tree_count], huffman_node::less_than);
        tree_count--;

        huffman_node* right_child = node_ptr[0];
        std::pop_heap(&node_ptr[0], &node_ptr[tree_count], huffman_node::less_than);
        tree_count--;

        huffman_node& internal_node = node_heap[next_huffman_node_idx];
        internal_node.freq = left_child->freq + right_child->freq;
        internal_node.left = left_child;
        internal_node.right = right_child;
        node_ptr[tree_count] = &node_heap[next_huffman_node_idx];
        next_huffman_node_idx++;
        tree_count++;
        std::push_heap(&node_ptr[0], &node_ptr[tree_count], huffman_node::less_than);
    }
    huffman_node* root = node_ptr[0];
    huffman_code huffman_map[512];

    gen_codes(root, huffman_code(), huffman_map);
    timer.stop();
    printf("huffman tree: %f sec\n", timer.elapsed());

    int huffman_bits = 0;
    for (int i = 0 ; i < 512; i++) {
        huffman_bits += huffman_map[i].len;
    }
    *compressed_bytes += (huffman_bits + 7) / 8;

    // layout:
    // 1 byte dictionary size
    // 2xN byte (sign_expo + encoding length), N=dictionary size
    // encoded bits (dictionary content)
    // encoded data
    *compressed = new char[*compressed_bytes];

    timer.start();

    (*compressed)[0] = dict_size;
    uint16_t* sign_expo_len_part = reinterpret_cast<uint16_t*>(&(*compressed)[1]);
    char* dict_content = &(*compressed)[1 + 2 * dict_size];
    int dict_content_bit_offset = 0;
    int j = 0;
    for (int i = 0; i < 512; i++) {
        if (sign_expo_freq[i] > 0) {
            sign_expo_len_part[j] = ((i << 9) | (huffman_map[i].len));
            j++;

            bits_append(&dict_content, &dict_content_bit_offset, huffman_map[i].bits, huffman_map[i].len);
        }
    }

    timer.stop();
    printf("encode time: %f sec\n", timer.elapsed());
}

void nn_decompress(const char* compressed, int compressed_bytes, float* farr, int n_floats) {
    
}
