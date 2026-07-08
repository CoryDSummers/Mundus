#ifndef MUNDUS_CORE_SMALL_HASH_MAP_SMALL_HASH_MAP_CLASS_HPP_
#define MUNDUS_CORE_SMALL_HASH_MAP_SMALL_HASH_MAP_CLASS_HPP_
#include <algorithm>
#include <array>
#include <memory>
#include <limits>
#include <tuple>
#include <cstddef>

#include "utility/stl-concepts/stl-library-concepts.hpp"
#include "core/small-hash-map/control-group-16-class.hpp"
namespace mundus::core
{
  template <
      typename Key,
      typename T,
      std::size_t kSBOLength = 16ull,
      utility::concepts::Hash Hash = std::hash<Key>,
      typename KeyEqual = std::equal_to<Key>,
      core::concepts::MetadataPolicy MetadataPolicy = ControlGroup16,
      utility::concepts::Allocator Allocator = std::allocator<std::pair<Key, T>>>
  class sbo_hash_map
  {
  public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<Key, T>;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using size_type = std::size_t;
    using allocator_type = Allocator;
    using difference_type = sd::ptrdiff_t;
    using iterator = value_type *;
    using iterator       = typename TableIterator<false>;
    using const_iterator = typename TableIterator<true>;
    sbo_hash_map()
    {
      inline_container_ = SBOContainer{};
      metadata_ = inline_container_.metadata_.data();
      table_ = reinterpret_cast<value_type *>(inline_container_.hash_map_.data());
      InitializeMetadata();
    };
    sbo_hash_map(
        size_type bucket_count,
        const Hash &hash = hasher(),
        const key_equal &equal = key_equal(),
        const Allocator &alloc = Allocator()
      );
    sbo_hash_map(const sbo_hash_map & other);
    sbo_hash_map(sbo_hash_map && other);
    
    sbo_hash_map& operator=(const sbo_hash_map & rhs);
    sbo_hash_map& operator=(sbo_hash_map&& rhs);
    // Observers
    inline hasher hash_function() const { return hasher_; }
    inline key_equal key_eq() const { return key_equal_; }
    //Capacity
    bool      empty()    const noexcept { return (this->size_ == 0); }
    size_type size()     const noexcept { return this->size_; }
    size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }
    bool      on_heap()  const noexcept { return capacity_ > kSBOLength; }
    0
    //Modifiers
    void clear();
    std::pair<iterator, bool> insert(const value_type& value) { return this->emplace(value); }
    std::pair<iterator, bool> insert(value_type&& value)      { return this->emplace(std::move(value)); }

    template <typename P> requires std::is_constructible_v<value_type, P&&>
    std::pair<iterator, bool> insert(P&& value) { return emplace(std::forward<P>(value)); }
    template <typename P> requires std::is_constructible_v<value_type, P&&>
    iterator insert(const_iterator /* hint */, P&& value) { return emplace(std::forward<P>(value)).first; }

    iterator insert(const_iterator /* hint */, const value_type& value) { return emplace(value).first; }
    iterator insert(const_iterator /* hint */, value_type&& value)      { return emplace(std::move(value)).first; }

    template <std::input_iterator InputIt>
    void insert(InputIt first, InputIt last) { std::for_each(first, last, [this](auto & n) { emplace(n); }); } 

    void insert(std::initializer_list<value_type> ilist) { insert(ilist.begin(), ilist.end()); }

    size_type erase(const key_type & key);
    template <class... Args>
    std::pair<iterator, bool> emplace(Args &&...args);
    template <class... Args>
    std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args);
    //Lookup
    bool contains(const key_type &key) const;

    const mapped_type& at(const key_type& key) const;
    mapped_type&       at(const key_type& key);

    mapped_type& operator[](const key_type& key);
    mapped_type& operator[](key_type&& key);

    iterator       find(const key_type& key);
    const_iterator find(const key_type& key) const;

    //Hash Policy
    void rehash(size_type count);
    void reserve(size_type count);
    //iterators
    template <bool IsConst>
    class TableIterator
    {
    public:
      using iterator_category = std::forward_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = std::pair<Key, T>;
      using pointer = std::conditional_t<IsConst, const value_type *, value_type *>;
      using reference = std::conditional_t<IsConst, const value_type &, value_type &>;
      TableIterator(pointer table, const Metadata* metadata, size_type capacity, size_type start_index)
        :table_(table), metadata_(metadata), capacity_(capacity), index_(start_index)
        {
          SkipDeadSpace();
        }
      operator TableIterator<true>() const
      {
        return TableIterator<true>(table_, metadata_, capacity_, index_);
      }
        reference operator*() const  { return table_[index_]; }
        pointer   operator->() const { return &table_[index_]; }

        TableIterator& operator++() 
        {
            index_++;
            SkipDeadSpace();
            return *this;
        }

        TableIterator operator++(int) 
        {
            TableIterator temp = *this;
            ++(*this);
            return temp;
        }
        bool operator==(const TableIterator& other) const { return index_ == other.index_; }
        bool operator!=(const TableIterator& other) const { return index_ != other.index_; }
    private:
      pointer table_;
      const Metadata *metadata_;
      size_type capacity_;
      size_type index_;
      inline void SkipDeadSpace()
      {
        while (index_ < capacity_ &&
               (metadata_[index_] & MetadataPolicy::kActiveMask) == MetadataPolicy::kEmptyState)
        {
          index_++;
        }
      }
    };
    
    iterator begin()             { return iterator(table_, metadata_, capacity_, 0); }
    iterator end()               { return iterator(table_, metadata_, capacity_, capacity_); }
    const_iterator begin() const { return const_iterator(table_, metadata_, capacity_, 0); }
    const_iterator end()   const { return const_iterator(table_, metadata_, capacity_, capacity_); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const   { return end(); }

    void swap(sbo_hash_map & other);
    ~sbo_hash_map();
  private:
    using Metadata = MetaDataPolicy::value_type;
    static constexpr size_type kInvalidIndex = static_cast<size_type>(-1);
    inline size_type AlignBufferSize(size_type size);
    inline size_type GetMetadataSlotsSize(size_type capacity);
    value_type *     CreateHeapBuffer(size_type capacity);
    void             InitializeMetadata();

    void SetControlFlag(size_type index, Metadata tag);

    void MoveTable(value_type * old_table, Metadata* old_metadata, size_type old_capacity);
    void MoveStackToStack(sbo_hash_map & source, sbo_hash_map& destination);
    void SwapStackWithStack(sbo_hash_map& a, sbo_hash_map& b);

    inline size_type FindKeyIndex(const key_type & key) const;

    struct SBOContainer
    {
      static constexpr size_type kBufferByteSize = kSBOLength * sizeof(value_type);
      using MetadataArray = std::array<Metadata, kSBOLength>;
      using HashMap = std::array<std::byte, kBufferByteSize>;
      alignas(alignof(value_type)) HashMap hash_map_;
      MetadataArray metadata_;
    };
    union
    {
      SBOContainer inline_container_;
      std::byte *raw_heap_buffer_;
    };
    Metadata *metadata_ = nullptr;
    value_type *table_ = nullptr;
    size_type size_ = 0;
    size_type capacity_ = kSBOLength;
    [[no_unique_address]] hasher hasher_;
    [[no_unique_address]] key_equal key_equal_;
    [[no_unique_address]] allocator_type allocator_;
    
  };

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::sbo_hash_map(
      size_type bucket_count,
      const Hash &hash,
      const key_equal &equal,
      const Allocator &alloc)
      : hasher_(hash), key_equal_(equal), allocator_(alloc)
  {
    if (bucket_count > kSBOLength)
    {
      size_type buffer_offset = 0;
      capacity_ = AlignBufferSize(bucket_count);
      buffer_offset = GetMetadataSlotsSize(capacity_);
      table_ = reinterpret_cast<value_type *>(CreateHeapBuffer(capacity_, buffer_offset));
      metadata_ = reinterpret_cast<Metadata *>(table_ + capacity_);
      raw_heap_buffer_ = reinterpret_cast<std::byte *>(table_);
    }
    else
    {
      inline_container_ = SBOContainer{};
      metadata_ = inline_container_.metadata_.data();
      table_ = reinterpret_cast<value_type *>(inline_container_.hash_map_.data());
    }
    InitializeMetadata();
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline mundus::core::sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::sbo_hash_map(const sbo_hash_map &other)
    : hasher_(other.hasher_), key_equal_(other.key_equal_), allocator_(other.allocator_)
  {
    inline_container_ = SBOContainer{};
    table_    = reinterpret_cast<value_type*>(inline_container_.hash_map_.data());
    metadata_ = inline_container_.metadata_.data();
    InitializeMetadata();

    reserve(other.size_);
    for (const auto& pair : other) { insert(pair);}
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline mundus::core::sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::sbo_hash_map(sbo_hash_map &&other)
    : hasher_(std::move(other.hasher_))
    , key_equal_(std::move(other.key_equal_))
    , allocator_(std::move(other.allocator_))
  {
    inline_container_ = SBOContainer{};
    table_    = reinterpret_cast<value_type*>(inline_container_.hash_map_.data());
    metadata_ = inline_container_.metadata_.data();
    InitializeMetadata();
    this->swap(other);
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline sbo_hash_map &mundus::core::sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::operator=(const sbo_hash_map &rhs)
  {
    if (this == &rhs) return *this;
    sbo_hash_map temp(rhs);
    this->swap(temp);
    return *this;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline sbo_hash_map &mundus::core::sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::operator=(sbo_hash_map &&rhs)
  {
    if (this == &other) return *this;
    sbo_hash_map temp(std::move(other));
    this->swap(temp);
    return *this;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline size_type sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::erase(const key_type &key)
  {
    size_type target = FindKeyIndex(key);
    if (target != kInvalidIndex)
    {
        std::destroy_at(&table_[target]);
        SetControlTag(target, MetadataPolicy::kDeletedState);
        size_--;
        return 1; // Successfully deleted
    }
    return 0; // Key not found
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline bool sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::contains(const key_type &key) const
  {
    return FindKeyIndex(key) != kInvalidIndex;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline mapped_type &sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::at(const key_type &key)
  {
    size_type index = FindKeyIndex(key);
    if (index == kInvalidIndex)
    {
        throw std::out_of_range("sbo_hash_map::at - Key does not exist");
    }
    return table_[index].second;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline mapped_type &sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::operator[](const key_type &key)
  {
    auto [iterator, inserted] = emplace(key, mapped_type{});
    return iterator->second;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline mapped_type &sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::operator[](key_type &&key)
  {
    auto [iterator, inserted] = emplace(std::move(key), mapped_type{});
    return iterator->second;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline iterator sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::find(const key_type &key)
  {
    size_type index = FindIndexOfKey(key);
    return (index == kInvalidIndex) ? end() : iterator(table_, metadata_, capacity_, index);
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline const_iterator sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::find(const key_type &key) const
  {
    size_type index = FindIndexOfKey(key);
    return (index == kInvalidIndex) ? end() : const_iterator(table_, metadata_, capacity_, index);
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::reserve(size_type count)
  {
    size_type required_capacity = (count * 8) / 7 + 1;
    if (required_capacity > capacity_)
    {
        Rehash(required_capacity);
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline const mapped_type &sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::at(const key_type &key) const
  {
    size_type index = FindKeyIndex(key);
    if (index == kInvalidIndex)
    {
      throw std::out_of_range("sbo_hash_map::at - Key does not exist");
    }
    return table_[index].second;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::clear()
  {
    for (size_type index = 0; index < capacity_; ++index)
    {
      if ((metadata_[index] & MetadataPolicy::kActiveMask) != 0)
      {
        std::destroy_at(&table[index]);
      }
    }
    size_type total_metadata_bytes = capacity_ * sizeof(Metadata);
    if (on_heap())
    {
      total_metadata_bytes += (MetadataPolicy::kAccessWidth - 1);
    }
    std::memset(metadata_, static_cast<int>(MetadataPolicy::kEmptyState), total_metadata_bytes);
    size_ = 0;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::swap(sbo_hash_map &other)
  {
    if(this == &other) return;
    bool this_on_heap  = this->on_heap();
    bool other_on_heap = other.on_heap();
    if(this_on_heap && other_on_heap)
    {
      std::swap(raw_heap_buffer_, other.raw_heap_buffer_);
      std::swap(metadata_, other.metadata_);
      std::swap(table_, other.table_);
      std::swap(capacity_, other.capacity_);
      std::swap(size_, other.size_);
      return;
    }
    if (this_on_heap && !other_on_heap)
    {
        std::byte* temp_heap = raw_heap_buffer_;
        size_type  temp_cap  = capacity_;

        MoveStackToStack(other, *this);

        other.raw_heap_buffer_ = temp_heap;
        other.capacity_        = temp_cap;
        other.table_           = reinterpret_cast<value_type*>(temp_heap);
        other.metadata_        = reinterpret_cast<Metadata*>(other.table_ + temp_cap);

        std::swap(size_, other.size_);
        return;
    }
    if (!this_on_heap && other_on_heap)
    {
      other.swap(*this);
      return;
    }
    if (!this_on_heap && !other_on_heap)
    {
        SwapStackWithStack(*this, other);
        std::swap(size_, other.size_);
        return;
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::~sbo_hash_map()
  {
    this->clear();
    if (this->on_heap())
    {
      size_type   metadata_slots = GetMetadataSlotsSize(capacity_);
      size_type   total_slots    = capacity_ + metadata_slots;
      value_type *heap_ptr       = reinterpret_cast<value_type *>(raw_heap_buffer_);
      allocator_.deallocate(heap_ptr, total_allocated_slots);
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline typename sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::size_type
  sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::AlignBufferSize(size_type size)
  {
    constexpr size_type width = MetadataPolicy::kAccessWidth;
    return (size + width - 1) & ~(width - 1);
  }

  // Adjusts metadata size to allow for SIMD wraparound
  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline typename sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::size_type
  sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::GetMetadataSlotsSize(size_type capacity)
  {
    constexpr std::size_t kMetadataPerValue = sizeof(value_type) / sizeof(Metadata);
    size_type required_metadata_slots = capacity + (MetadataPolicy::kAccessWidth - 1);
    return (required_metadata_slots + kMetadataPerValue - 1) / kMetadataPerValue;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::InitializeMetadata()
  {
    for (size_type index = 0; index < capacity_; ++index)
    {
      metadata_[index] = MetadataPolicy::kEmptyState;
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::Rehash(size_type new_capacity)
  {
    const bool  was_on_heap  = this->on_heap();
    size_type   old_capacity = capacity_;
    value_type* old_table    = table_;
    Metadata*   old_metadata = metadata_;

    capacity_ = AlignBufferSize(new_capacity);
    table_    = CreateHeapBuffer(capacity_, GetMetadataSlotsSize(capacity_));
    metadata_ = reinterpret_cast<Metadata*>(table_ + capacity_);

    InitializeMetadata();
    MoveTable(old_table, old_metadata, old_capacity);
    
    raw_heap_buffer_ = reinterpret_cast<std::byte*>(table_);
    if (was_on_heap)
    {
      size_type old_metadata_slots = GetMetadataSlotsSize(old_capacity);
      allocator_.deallocate(old_table, old_capacity + old_metadata_slots);
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::MoveTable(value_type *old_table, Metadata *old_metadata, size_type old_capacity)
  {
    for(size_type index = 0; index < old_capacity; ++index)
    {
      if((old_metadata[index] & MetadataPolicy::kActiveMask) != MetadataPolicy::kEmptyState)
      {
        std::size_t hash_value = hasher_(old_table[index].first);
        Metadata tag = MetadataPolicy::CreateTag(hash_value);
        size_type new_index = hash_value % capacity_;
        while(true)
        {
          auto empty_match = MetadataPolicy::Match(metadata_ + new_index, MetadataPolicy::kEmptyState, capacity_);
          if (empty_match.bitmask != 0)
          {
              size_type target = (new_index + *empty_match.begin()) % capacity_;
              
              std::construct_at(&table_[target], std::move(old_table[index]));
              SetControlTag(target, tag);
              break;
          }
          new_index = (new_index + MetadataPolicy::kAccessWidth) % capacity_;
        }
        std::destroy_at(&old_table[index]);
      }
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::MoveStackToStack(sbo_hash_map &source, sbo_hash_map &destination)
  {
    dst.inline_container_ = SBOContainer{};
    dst.table_    = reinterpret_cast<value_type*>(dst.inline_container_.hash_map_.data());
    dst.metadata_ = dst.inline_container_.metadata_.data();

    for (size_type i = 0; i < kSBOLength; ++i)
    {
        if ((src.metadata_[i] & MetadataPolicy::kActiveMask) != MetadataPolicy::kEmptyState)
        {
            std::construct_at(&dst.table_[i], std::move(src.table_[i]));
            std::destroy_at(&src.table_[i]);
        }
        dst.metadata_[i] = src.metadata_[i];
        src.metadata_[i] = MetadataPolicy::kEmptyState;
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::SwapStackWithStack(sbo_hash_map &a, sbo_hash_map &b)
  {
    for (size_type i = 0; i < kSBOLength; ++i)
    {
        bool a_active = (a.metadata_[i] & MetadataPolicy::kActiveMask) != MetadataPolicy::kEmptyState;
        bool b_active = (b.metadata_[i] & MetadataPolicy::kActiveMask) != MetadataPolicy::kEmptyState;

        if (a_active && b_active) {
            std::swap(a.table_[i], b.table_[i]);
        } else if (a_active && !b_active) {
            std::construct_at(&b.table_[i], std::move(a.table_[i]));
            std::destroy_at(&a.table_[i]);
        } else if (!a_active && b_active) {
            std::construct_at(&a.table_[i], std::move(b.table_[i]));
            std::destroy_at(&b.table_[i]);
        }
        std::swap(a.metadata_[i], b.metadata_[i]);
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline size_type sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::FindKeyIndex(const key_type &key) const
  {
    std::size_t hash_value = hasher_(key);
    Metadata tag = MetadataPolicy::CreateTag(hash_value);
    size_type index = hash_value % capacity_;
    while (true)
    {
        for (int i : MetadataPolicy::Match(metadata_ + index, tag, capacity_))
        {
            size_type actual_index = (index + i) % capacity_;
            if (key_equal_(table_[actual_index].first, key))
            {
                return actual_index; // Found it!
            }
        }
        auto empty_match = MetadataPolicy::Match(metadata_ + index, MetadataPolicy::kEmptyState, capacity_);
        if (empty_match.bitmask != 0) 
        {
            return kInvalidIndex; 
        }
        index = (index + MetadataPolicy::kAccessWidth) % capacity_;
    }
    return kInvalidIndex;
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline void sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::SetControlTag(size_type index, Metadata tag)
  {
    metadata_[index] = tag;
    if ((on_heap() == true) && index < (MetadataPolicy::kAccessWidth - 1))
    {
      metadata_[capacity_ + index] = tag;
    }
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  inline typename sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::value_type *
  sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::CreateHeapBuffer(size_type capacity, size_type metadata_slots)
  {
    return allocator_.allocate(capacity + metadata_slots);
  }

  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  template <class... Args>
  inline std::pair<iterator, bool> sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::emplace(Args &&...args)
  {
    value_type temp{std::forward<Args>(args)...};
    const key_type &key = temp.first;
    if (size_ >= capacity_ - (capacity_ >> 3))
    {
      Rehash(capacity_ * 1.5);
    }
    std::size_t hash_value = hasher_(key);
    Metadata tag = MetadataPolicy::CreateTag(hash_value);
    size_type index = hash_value % capacity_;
    size_type target_index = static_cast<size_type>(-1);
    while (true)
    {
      for (int i : MetadataPolicy::Match(metadata_ + index, tag, capacity_))
      {
        size_type actual_index = (index + i) % capacity_;
        // Collision Check
        if (key_equal_(table_[actual_index].first, key))
        {
          return {iterator(table_, metadata_, capacity_, actual_index), false};
        }
      }
      // Recycle Tombstone
      if (target_index == static_cast<size_type>(-1))
      {
        auto deleted_match = MetadataPolicy::Match(metadata_ + index, MetadataPolicy::kDeletedState, capacity_);
        if (deleted_match.bitmask != 0)
        {
          target_index = (index + *deleted_match.begin()) % capacity_;
        }
      }
      auto empty_match = MetadataPolicy::Match(metadata_ + index, MetadataPolicy::kEmptyState, capacity_);
      if (empty_match.bitmask != 0)
      {
        if (target_index == static_cast<size_type>(-1))
        {
          target_index = (index + *empty_match.begin()) % capacity_;
        }
        break;
      }
      index = (index + MetadataPolicy::kAccessWidth) % capacity_;
    }
    std::construct_at(&table_[target_index], std::move(temp));
    SetControlTag(target_index, tag);
    ++size_;
    return {iterator(table_, metadata_, capacity_, target_insert_index), true};
  }
  template <typename Key, typename T, std::size_t kSBOLength, utility::concepts::Hash Hash, typename KeyEqual, core::concepts::MetadataPolicy MetadataPolicy, utility::concepts::Allocator Allocator>
  template <class... Args>
  std::pair<iterator, bool> sbo_hash_map<Key, T, kSBOLength, Hash, KeyEqual, MetadataPolicy, Allocator>::try_emplace(const key_type &key, Args &&...args)
  {
    size_type existing_index = FindKeyIndex(key);
    //Key already exists
    if (existing_index != kInvalidIndex)
    {
      return { iterator(table_, metadata_, capacity_, existing_index), false };
    }
    return emplace(std::piecewise_construct,
                       std::forward_as_tuple(key),
                       std::forward_as_tuple(std::forward<Args>(args)...));

  }
  template<typename K, typename T, std::size_t L, typename H, typename E, typename M, typename A>
  inline void swap(sbo_hash_map<K, T, L, H, E, M, A>& lhs, 
                    sbo_hash_map<K, T, L, H, E, M, A>& rhs) noexcept
  {
      lhs.swap(rhs);
  }
}

#endif // MUNDUS_CORE_SMALL_HASH_MAP_SMALL_HASH_MAP_CLASS_HPP_