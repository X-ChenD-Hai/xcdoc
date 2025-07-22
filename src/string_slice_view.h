#pragma once
#include <assert.h>

#include <iostream>
#include <vector>
class string_slice_view {
    struct StrRef {
        const char *str;
        size_t len;
    };
    std::vector<StrRef> str_queue;

   public:
    class iterator {
        friend class string_slice_view;
        constexpr static const char __EOF = '\x00';
        const char *str{&__EOF};
        const std::vector<StrRef> *str_queue;
        size_t offset{0};

       public:
        iterator();
        iterator(const std::vector<StrRef> *_q);
        iterator(const iterator &other);
        iterator &operator=(const iterator &other);
        const char &operator*() const;
        iterator &operator++();
        iterator operator++(int);
        iterator operator+(int n) const;
        iterator &operator+=(int n);
        bool operator==(const iterator &other) const;
    };

   public:
    void push(const char *str, size_t len);
    void push(const std::string &str);
    void push(iterator start, iterator end);
    void push(const string_slice_view &other);
    void insert(const iterator &pos, iterator start, const iterator &end);
    void insert(size_t pos, iterator start, const iterator &end);
    void insert(size_t pos, const string_slice_view &other);
    void insert(const iterator &pos, const string_slice_view &other);
    iterator earse(iterator start, iterator end);
    void replace(const iterator &start, const iterator &end,
                 const iterator &new_start, const iterator &new_end);

    iterator begin() const;
    iterator end() const;
};
std::ostream &operator<<(std::ostream &os, const string_slice_view &view);
