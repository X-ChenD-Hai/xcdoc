#pragma once
#include <assert.h>

#include <iostream>
#include <vector>
class string_slice_view {
   public:
    struct StrRef {
        const char *str;
        size_t len;
    };
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

   private:
    std::vector<StrRef> str_queue;

   public:
    string_slice_view() {}
    string_slice_view(iterator start, iterator end) { push(start, end); }
    string_slice_view(const char *str, size_t len) { push(str, len); }
    string_slice_view(const char *start, const char *end) { push(start, end); }
    string_slice_view(std::string *str) { push(str); }
    void push(const char *str, size_t len);
    inline void push(const char *start, const char *end) {
        push(start, end - start);
    }
    void push(std::string *str);
    void push(iterator start, iterator end);
    void push(const string_slice_view &other);
    void pop_back();
    void insert(const iterator &pos, iterator start, const iterator &end);
    void insert(size_t pos, iterator start, const iterator &end);
    void insert(size_t pos, const string_slice_view &other);
    void insert(const iterator &pos, const string_slice_view &other);
    iterator earse(iterator start, iterator end);
    void replace(const iterator &start, const iterator &end,
                 const iterator &new_start, const iterator &new_end);
    inline const std::vector<StrRef> &string_refs() const { return str_queue; }
    iterator begin() const;
    iterator end() const;
    inline std::string to_string() const { return std::string(begin(), end()); }
    inline operator std::string() { return to_string(); }
};
std::ostream &operator<<(std::ostream &os, const string_slice_view &view);
