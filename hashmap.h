#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

template <typename KeyT, typename ValT>
class HashMap {
 private:
  struct ChainNode {
    const KeyT key;
    ValT value;
    ChainNode* next;

    ChainNode(KeyT key, ValT value) : key(key), value(value), next(nullptr) {
    }

    ChainNode(KeyT key, ValT value, ChainNode* next)
        : key(key), value(value), next(next) {
    }
  };

  ChainNode** data;
  size_t sz;
  size_t capacity;

  // Utility members for begin/next
  ChainNode* curr;
  size_t curr_idx;

 public:
  /**
   * Creates an empty `HashMap` with 10 buckets.
   */
  HashMap() : sz(0), capacity(10), curr(nullptr), curr_idx(0) {
    // TODO_STUDENT
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }
  }

  /**
   * Creates an empty `HashMap` with `capacity` buckets.
   */
  HashMap(size_t capacity)
      : sz(0), capacity(capacity), curr(nullptr), curr_idx(0) {
    // TODO_STUDENT
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
    }
  }

  /**
   * Checks if the `HashMap` is empty. Runs in O(1).
   */
  bool empty() const {
    // TODO_STUDENT
    return sz == 0;
  }

  /**
   * Returns the number of mappings in the `HashMap`. Runs in O(1).
   */
  size_t size() const {
    // TODO_STUDENT
    return sz;
  }

  /**
   * Adds the mapping `{key -> value}` to the `HashMap`. If the key already
   * exists, does not update the mapping (like the C++ STL map).
   *
   * Uses the hash value of the key to determine the location in the
   * underlying hash table. Creates exactly one new node; resizes by doubling
   * when the load factor exceeds 1.5.
   *
   * On resize, doesn't create new nodes, but rearranges existing ones.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  void insert(KeyT key, ValT value) {
    // TODO_STUDENT
    size_t idx = hash<KeyT>{}(key) % capacity;
    for (ChainNode* n = data[idx]; n != nullptr; n = n->next) {
      if (n->key == key) {
        return;
      }
    }

    data[idx] = new ChainNode(key, value, data[idx]);
    sz++;

    if ((double)sz / (double)capacity > 1.5) {
      size_t newCap = capacity * 2;
      ChainNode** newData = new ChainNode*[newCap];

      for (size_t i = 0; i < newCap; i++) {
        newData[i] = nullptr;
      }

      for (size_t i = 0; i < capacity; i++) {
        ChainNode* node = data[i];
        while (node != nullptr) {
          ChainNode* next = node->next;
          size_t newIdx = hash<KeyT>{}(node->key) % newCap;
          node->next = newData[newIdx];
          newData[newIdx] = node;
          node = next;
        }
      }

      delete[] data;
      data = newData;
      capacity = newCap;
    }
  }

  /**
   * Return a reference to the value stored for `key` in the map.
   *
   * If key is not present in the map, throw `out_of_range` exception.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT& at(const KeyT& key) const {
    // TODO_STUDENT
    size_t idx = hash<KeyT>{}(key) % capacity;
    for (ChainNode* n = data[idx]; n != nullptr; n = n->next) {
      if (n->key == key) {
        return n->value;
      }
    }
    throw out_of_range("Key not found");
  }

  /**
   * Returns `true` if the key is present in the map, and false otherwise.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  bool contains(const KeyT& key) const {
    // TODO_STUDENT
    size_t idx = hash<KeyT>{}(key) % capacity;
    for (ChainNode* n = data[idx]; n != nullptr; n = n->next) {
      if (n->key == key) {
        return true;
      }
    }
    return false;
  }

  /**
   * Empties the `HashMap`, freeing all nodes. The bucket array may be left
   * alone.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  void clear() {
    // TODO_STUDENT
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        ChainNode* next = node->next;
        delete node;
        node = next;
      }
      data[i] = nullptr;
    }
    sz = 0;
  }

  /**
   * Destructor, cleans up the `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  ~HashMap() {
    // TODO_STUDENT
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        ChainNode* next = node->next;
        delete node;
        node = next;
      }
    }
    delete[] data;
  }

  /**
   * Removes the mapping for the given key from the `HashMap`, and returns the
   * value.
   *
   * Throws `out_of_range` if the key is not present in the map. Creates no new
   * nodes, and does not update the key or value of any existing nodes.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT erase(const KeyT& key) {
    // TODO_STUDENT
    size_t idx = hash<KeyT>{}(key) % capacity;
    ChainNode* node = data[idx];
    ChainNode* prev = nullptr;

    while (node != nullptr) {
      if (node->key == key) {
        ValT val = node->value;
        if (prev == nullptr) {
          data[idx] = node->next;
        } else {
          prev->next = node->next;
        }
        delete node;
        sz--;
        return val;
      }
      prev = node;
      node = node->next;
    }

    throw out_of_range("Key not found");
  }

  /**
   * Copy constructor.
   *
   * Copies the mappings from the provided `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings in `other`, and B is the
   * number of buckets.
   */
  HashMap(const HashMap& other)
      : sz(other.sz), capacity(other.capacity), curr(nullptr), curr_idx(0) {
    // TODO_STUDENT
    data = new ChainNode*[capacity];

    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
      ChainNode* src = other.data[i];
      ChainNode* tail = nullptr;

      while (src != nullptr) {
        ChainNode* newNode = new ChainNode(src->key, src->value);
        if (tail == nullptr) {
          data[i] = newNode;
        } else {
          tail->next = newNode;
        }
        tail = newNode;
        src = src->next;
      }
    }
  }

  /**
   * Assignment operator; `operator=`.
   *
   * Clears this table, and copies the mappings from the provided `HashMap`.
   *
   * Runs in O((N1+B1) + (N2+B2)), where N1 and B1 are the number of mappings
   * and buckets in `this`, and N2 and B2 are the number of mappings and buckets
   * in `other`.
   */
  HashMap& operator=(const HashMap& other) {
    // TODO_STUDENT
    if (this == &other) {
      return *this;
    }

    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        ChainNode* next = node->next;
        delete node;
        node = next;
      }
    }
    delete[] data;

    sz = other.sz;
    capacity = other.capacity;
    data = new ChainNode*[capacity];

    for (size_t i = 0; i < capacity; i++) {
      data[i] = nullptr;
      ChainNode* src = other.data[i];
      ChainNode* tail = nullptr;

      while (src != nullptr) {
        ChainNode* newNode = new ChainNode(src->key, src->value);
        if (tail == nullptr) {
          data[i] = newNode;
        } else {
          tail->next = newNode;
        }
        tail = newNode;
        src = src->next;
      }
    }

    return *this;
  }

  // =====================

  /**
   *
   * Checks if the contents of `this` and `other` are equivalent.
   *
   * Two `HashMap` objects are equivalent if they contain the same
   * mappings from key to value. To be equivalent, the two `HashMap`
   * objects need not have the elements saved in the same order
   * inside of the buckets.
   *
   * Runs in worst-case O(B*L*L) time, where B is the maximum number
   * of buckets in either of the `HashMap` objects and L is the length
   * of the  largest chain on any of the buckets.
   *
   */
  bool operator==(const HashMap& other) const {
    // TODO_STUDENT
    if (sz != other.sz) {
      return false;
    }

    for (size_t i = 0; i < capacity; i++) {
      for (ChainNode* n = data[i]; n != nullptr; n = n->next) {
        if (!other.contains(n->key)) {
          return false;
        }
        if (other.at(n->key) != n->value) {
          return false;
        }
      }
    }
    return true;
  }

  /**
   * Resets internal state for an iterative traversal.
   *
   * See `next` for usage details. Modifies nothing except for `curr` and
   * `curr_idx`.
   *
   * Runs in worst-case O(B), where B is the number of buckets.
   */
  void begin() {
    // TODO_STUDENT
    curr = nullptr;
    curr_idx = 0;

    while (curr_idx < capacity) {
      if (data[curr_idx] != nullptr) {
        curr = data[curr_idx];
        return;
      }
      curr_idx++;
    }
  }

  /**
   * Uses the internal state to return the "next" key and value
   * by reference, and advances the internal state. Returns `true` if the
   * reference parameters were set, and `false` otherwise.
   *
   * Example usage:
   *
   * ```c++
   * HashMap<string, int> hm;
   * hm.begin();
   * string key;
   * int value;
   * while (hm.next(key, val)) {
   *   cout << key << ": " << val << endl;
   * }
   * ```
   *
   * Does not visit the mappings in any defined order.
   *
   * Modifies nothing except for `curr` and `curr_idx`.
   *
   * Runs in worst-case O(B) where B is the number of buckets.
   */
  bool next(KeyT& key, ValT& value) {
    // TODO_STUDENT

    if (curr == nullptr) {
      return false;
    }
    key = curr->key;
    value = curr->value;
    curr = curr->next;
    while (curr == nullptr && ++curr_idx < capacity) {
      curr = data[curr_idx];
    }
    return true;
  }

  // ===============================================

  /**
   * Returns a pointer to the underlying memory managed by the `HashMap`.
   * For autograder testing purposes only.
   */
  void* get_data() const {
    return this->data;
  }

  /**
   * Returns the capacity of the underlying memory managed by the `HashMap`. For
   * autograder testing purposes only.
   */
  size_t get_capacity() {
    return this->capacity;
  }
};
