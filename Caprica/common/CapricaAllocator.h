#pragma once

#include <stdlib.h>

#include <atomic>
#include <unordered_map>
#include <vector>

#include <boost/container/static_vector.hpp>
#include <boost/utility/string_ref.hpp>

#include <common/CapricaReportingContext.h>

namespace std {
template<>
struct hash<boost::string_ref>
{
  size_t operator()(boost::string_ref const& str) const {
    constexpr size_t offsetBasis = 0xcbf29ce484222325ULL;
    constexpr size_t prime = 0x100000001B3ULL;
    const char* cStr = str.data();
    const char* eStr = cStr + str.size();

    size_t val = offsetBasis;
    for (; cStr < eStr; cStr++) {
      val ^= (size_t)*cStr;
      val *= prime;
    }
    return val;
  }
};
}

namespace caprica {

struct ConcurrentPooledBufferAllocator final
{
  explicit ConcurrentPooledBufferAllocator(size_t hpSize) : heapSize(hpSize), base(hpSize) { }
  ~ConcurrentPooledBufferAllocator() = default;

  char* allocate(size_t size);

private:
  struct Heap final {
    size_t allocedHeapSize;
    std::atomic<size_t> freeBytes;
    void* baseAlloc;
    std::atomic<Heap*> next{ nullptr };

    Heap() = delete;
    Heap(const Heap&) = delete;
    Heap(Heap&&) = delete;
    Heap& operator=(const Heap&) = delete;
    Heap& operator=(Heap&&) = delete;

    explicit Heap(size_t heapSize);
    ~Heap();

    bool tryAlloc(size_t size, void** retBuf);
  };

  size_t heapSize;
  std::atomic<Heap*> current{ &base };
  Heap base;

  void* allocHeap(size_t newHeapSize, size_t firstAllocSize);
};

struct ReffyStringPool final
{
  size_t lookup(boost::string_ref str);
  boost::string_ref byIndex(size_t v) const { return allocedStrings[v]; }
  size_t size() const { return allocedStrings.size(); };

  void reserve(size_t size) {
    allocedStrings.reserve(size);
    idxLookup.reserve(size);
  }

  void push_back(boost::string_ref str);

private:
  ConcurrentPooledBufferAllocator alloc{ 1024 * 4 };
  std::vector<boost::string_ref> allocedStrings{ };
  std::unordered_map<boost::string_ref, size_t> idxLookup{ };
};


template<typename T, size_t heapSize>
struct FlatishDestructedPooledBufferAllocator final
{
private:
  struct Heap final
  {
    boost::container::static_vector<T, heapSize> heap;
    Heap* next{ nullptr };

    Heap() { }
    ~Heap() {
      if (next != nullptr)
        delete next;
    }
  };

  size_t count{ 0 };
  Heap* current{ &base };
  Heap base;

public:
  FlatishDestructedPooledBufferAllocator() { }
  ~FlatishDestructedPooledBufferAllocator() = default;

  template<typename... Args>
  void emplace_back(Args&&... args) {
    if (current->heap.size() >= current->heap.capacity()) {
      Heap* next = new Heap();
      current->next = next;
      current = next;
    }
    count++;
    current->heap.emplace_back(std::forward<Args>(args)...);
  }

  size_t size() const { return count; }

  template<typename F>
  void evacuate(F func) {
    auto cur = &base;
    while (cur != nullptr) {
      for (auto&& r : cur->heap) {
        func(std::move(r));
      }
      cur = cur->next;
    }
  }
};

}
