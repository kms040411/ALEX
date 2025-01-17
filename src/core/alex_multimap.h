// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/*
 * Implements the STL multimap. It can be used as a near drop-in
 * replacement for std::multimap, with a few important differences:
 * 1) The iterators are ForwardIterators instead of BidirectionalIterators.
 * 2) Keys and payloads are stored separately, so dereferencing the iterator
 *  does not return a reference.
 */

#pragma once

#include "alex.h"

namespace alex {

template <class P, class Compare = AlexCompare,
          class Alloc = std::allocator<std::pair<AlexKey, P>>>
class AlexMultimap {
  //static_assert(std::is_arithmetic<T>::value, "ALEX key type must be numeric.");
  static_assert(std::is_same<Compare,AlexCompare>::value, "Must use AlexCompare.");

 public:
  // Value type, returned by dereferencing an iterator
  typedef std::pair<AlexKey, P> V;

  // ALEX class aliases
  typedef AlexMultimap<P, Compare, Alloc> self_type;
  typedef Alex<P, Compare, Alloc, true> alex_impl;
  typedef typename alex_impl::Iterator iterator;
  typedef typename alex_impl::ConstIterator const_iterator;
  typedef typename alex_impl::ReverseIterator reverse_iterator;
  typedef typename alex_impl::ConstReverseIterator const_reverse_iterator;

 private:
  alex_impl alex_;

  /*** Constructors and setters ***/

 public:
  AlexMultimap() : alex_() {}

  AlexMultimap(unsigned int max_key_length, int key_type,
    const Compare& comp, const Alloc& alloc = Alloc())
      : alex_(max_key_length, key_type, comp, alloc) {}

  AlexMultimap(int key_type, const Compare& comp, const Alloc& alloc = Alloc())
      : alex_(key_type, comp, alloc) {}

  AlexMultimap(const Compare& comp, const Alloc& alloc = Alloc())
      : alex_(comp, alloc) {}

  AlexMultimap(const Alloc& alloc) : alex_(alloc) {}

  ~AlexMultimap() {}

  // Below 4 constructors initializes with range [first, last). 
  // The range does not need to be sorted. 
  // This creates a temporary copy of the data. 
  // If possible, we recommend directly using bulk_load() instead.
  template <class InputIterator>
  explicit AlexMultimap(InputIterator first, InputIterator last,
                        unsigned int max_key_length, int key_type = DOUBLE,
                        const Compare& comp, const Alloc& alloc = Alloc())
      : alex_(first, last, max_key_length, key_type, comp, alloc) {}
  
  template <class InputIterator>
  explicit AlexMultimap(InputIterator first, InputIterator last,
                        int key_type, const Compare& comp, const Alloc& alloc = Alloc())
      : alex_(first, last, key_type, comp, alloc) {}
  
  template <class InputIterator>
  explicit AlexMultimap(InputIterator first, InputIterator last,
                        const Compare& comp, const Alloc& alloc = Alloc())
      : alex_(first, last, comp, alloc) {}

  template <class InputIterator>
  explicit AlexMultimap(InputIterator first, InputIterator last,
                        const Alloc& alloc = Alloc())
      : alex_(first, last, alloc) {}

  explicit AlexMultimap(const self_type& other) : alex_(other.alex_) {}

  AlexMultimap& operator=(const self_type& other) {
    if (this != &other) {
      alex_ = other.alex_;
    }
    return *this;
  }

  void swap(const self_type& other) { alex_.swap(other.alex_); }

 public:
  // When bulk loading, Alex can use provided knowledge of the expected fraction
  // of operations that will be inserts
  // For simplicity, operations are either point lookups ("reads") or inserts
  // ("writes)
  // i.e., 0 means we expect a read-only workload, 1 means write-only
  // This is only useful if you set it before bulk loading
  void set_expected_insert_frac(double expected_insert_frac) {
    alex_.set_expected_insert_frac(expected_insert_frac);
  }

  // Maximum node size, in bytes.
  // Higher values result in better average throughput, but worse tail/max
  // insert latency.
  void set_max_node_size(int max_node_size) {
    alex_.set_max_node_size(max_node_size);
  }

  // Bulk load faster by using sampling to train models.
  // This is only useful if you set it before bulk loading.
  void set_approximate_model_computation(bool approximate_model_computation) {
    alex_.set_approximate_model_computation(approximate_model_computation);
  }

  // Bulk load faster by using sampling to compute cost.
  // This is only useful if you set it before bulk loading.
  void set_approximate_cost_computation(bool approximate_cost_computation) {
    alex_.set_approximate_cost_computation(approximate_cost_computation);
  }

  /*** Allocators and comparators ***/

 public:
  Alloc get_allocator() const { return alex_.get_allocator(); }

  Compare key_comp() const { return alex_.key_comp(); }

  /*** Bulk loading ***/

 public:
  // values should be the sorted array of key-payload pairs.
  // The number of elements should be num_keys.
  // The index must be empty when calling this method.
  void bulk_load(const V values[], int num_keys) {
    alex_.bulk_load(values, num_keys);
  }

  /*** Lookup ***/

 public:
  // Looks for an exact match of the key
  // If the key does not exist, returns an end iterator
  // If there are multiple keys with the same value, returns an iterator to the
  // right-most key
  // If you instead want an iterator to the left-most key with the input value,
  // use lower_bound()
  iterator find(const AlexKey& key) { return alex_.find(key); }

  const_iterator find(const AlexKey& key) const { return alex_.find(key); }

  size_t count(const AlexKey& key) { return alex_.size(key); }

  // Returns an iterator to the first key no less than the input value
  iterator lower_bound(const AlexKey& key) { return alex_.lower_bound(key); }

  const_iterator lower_bound(const AlexKey& key) const {
    return alex_.lower_bound(key);
  }

  // Returns an iterator to the first key greater than the input value
  iterator upper_bound(const AlexKey& key) { return alex_.upper_bound(key); }

  const_iterator upper_bound(const AlexKey& key) const {
    return alex_.upper_bound(key);
  }

  std::pair<iterator, iterator> equal_range(const AlexKey& key) {
    return alex_.equal_range(key);
  }

  std::pair<const_iterator, const_iterator> equal_range(const AlexKey& key) const {
    return alex_.equal_range(key);
  }

  iterator begin() { return alex_.begin(); }

  iterator end() { return alex_.end(); }

  const_iterator cbegin() const { return alex_.cbegin(); }

  const_iterator cend() const { return alex_.cend(); }

  reverse_iterator rbegin() { return alex_.rbegin(); }

  reverse_iterator rend() { return alex_.rend(); }

  const_reverse_iterator crbegin() const { return alex_.crbegin(); }

  const_reverse_iterator crend() const { return alex_.crend(); }

  /*** Insert ***/

 public:
  iterator insert(const V& value) { return alex_.insert(value).first; }

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    alex_.insert(first, last);
  }

  // This will NOT do an update of an existing key.
  // To perform an update or read-modify-write, do a lookup and modify the
  // payload's value.
  iterator insert(const AlexKey& key, const P& payload) {
    return alex_.insert(key, payload).first;
  }

  /*** Delete ***/

 public:
  // Erases all keys with a certain key value
  int erase(const AlexKey& key) { return alex_.erase(key); }

  // Erases element pointed to by iterator
  void erase(iterator it) { alex_.erase(it); }

  // Removes all elements
  void clear() { alex_.clear(); }

  /*** Stats ***/

 public:
  // Number of elements
  size_t size() const { return alex_.size(); }

  // True if there are no elements
  bool empty() const { return alex_.empty(); }

  // This is just a function required by the STL standard. ALEX can hold more
  // items.
  size_t max_size() const { return alex_.max_size(); }

  // Return a const reference to the current statistics
  const struct alex_impl::Stats& get_stats() const { return alex_.stats_; }
};
}
