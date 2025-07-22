#include "./string_slice_view.h"
string_slice_view::iterator::iterator()
    : str(&__EOF), str_queue(nullptr), offset(0) {}
string_slice_view::iterator::iterator(const std::vector<StrRef> *_q)
    : str_queue(_q), str(_q->size() ? _q->front().str : &__EOF) {}
string_slice_view::iterator::iterator(const iterator &other)
    : str(other.str), str_queue(other.str_queue), offset(other.offset) {}
string_slice_view::iterator &string_slice_view::iterator::operator=(
    const iterator &other) {
    str = other.str;
    str_queue = other.str_queue;
    offset = other.offset;
    return *this;
}
const char &string_slice_view::iterator::operator*() const { return *str; }
string_slice_view::iterator &string_slice_view::iterator::operator++() {
    if (offset == str_queue->size()) return *this;
    auto it = str_queue->begin() + offset;
    if (++str >= it->str + it->len) {
        ++offset;
        if (offset < str_queue->size())
            str = str_queue->operator[](offset).str;
        else {
            str = &__EOF;
        }
    }
    return *this;
}
string_slice_view::iterator string_slice_view::iterator::operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
}
string_slice_view::iterator string_slice_view::iterator::operator+(
    int n) const {
    iterator res = *this;
    auto it = res.str_queue->begin() + res.offset;
    while ((res.str + n) >= it->str + it->len) {
        n -= (it->str + it->len - res.str);
        ++res.offset;
        if (res.offset < res.str_queue->size())
            res.str = (++it)->str;
        else {
            res.str = &__EOF;
            return res;
        }
    }
    res.str += n;
    return res;
}
string_slice_view::iterator &string_slice_view::iterator::operator+=(int n) {
    while ((str += n) >= str_queue->operator[](offset).str +
                             str_queue->operator[](offset).len) {
        n -= (str_queue->operator[](offset).str +
              str_queue->operator[](offset).len - str);
        ++offset;
        if (offset < str_queue->size())
            str = str_queue->operator[](offset).str;
        else {
            str = &__EOF;
            return *this;
        }
    }
    return *this;
}
bool string_slice_view::iterator::operator==(const iterator &other) const {
    return other.str == &__EOF ? str == &__EOF
                               : (str == other.str && offset == other.offset &&
                                  str_queue == other.str_queue);
}
void string_slice_view::push(const char *str, size_t len) {
    if (len == 0) return;
    str_queue.push_back({str, static_cast<unsigned int>(len)});
    if (str_queue.size() == 1) {
        str = str_queue.front().str;
    }
}
void string_slice_view::push(const std::string &str) {
    push(str.data(), str.size());
}
void string_slice_view::push(iterator start, iterator end) {
    assert(!end.str_queue || start.str_queue == end.str_queue);
    while (start.offset < end.offset) {
        size_t len =
            start.str_queue->operator[](start.offset).len -
            (start.str - start.str_queue->operator[](start.offset).str);
        str_queue.emplace_back(start.str, len);
        start += len;
    }
    if (start == end) return;
    str_queue.emplace_back(start.str, size_t(end.str - start.str));
}
void string_slice_view::push(const string_slice_view &other) {
    for (const auto &ref : other.str_queue) {
        push(ref.str, ref.len);
    }
}
void string_slice_view::insert(const iterator &pos, iterator start,
                               const iterator &end) {
    string_slice_view tmp;
    auto it = str_queue.begin() + pos.offset;
    tmp.push(start, end);
    if (pos.str != it->str) {
        tmp.str_queue.emplace_back(pos.str, it->len - (pos.str - it->str));
        it->len = pos.str - it->str - 1;
        ++it;
    }
    str_queue.insert_range(it, tmp.str_queue);
}
void string_slice_view::insert(size_t pos, iterator start,
                               const iterator &end) {
    insert(iterator(&str_queue) + pos, start, end);
}
void string_slice_view::insert(size_t pos, const string_slice_view &other) {
    insert(iterator(&str_queue) + pos, other.begin(), other.end());
}
void string_slice_view::insert(const iterator &pos,
                               const string_slice_view &other) {
    insert(pos, other.begin(), other.end());
}
string_slice_view::iterator string_slice_view::earse(iterator start,
                                                     iterator end) {
    auto it = str_queue.begin() + start.offset;

    if (auto l = end.offset - start.offset; l == 0) {
        if (start.str == it->str) {
            it->len = it->len - (end.str - start.str);
            it->str = end.str;
        } else {
            size_t pre_len = start.str - it->str;
            auto pre_s = it->str;
            it->len = it->str + it->len - end.str + 1;
            it->str = end.str;
            it = str_queue.insert(it, {pre_s, pre_len});
            ++end.offset;
        }
    } else {
        it->len = start.str - it->str;
        if (it->len)
            ++it;
        else {
            it = str_queue.erase(it);
            --end.offset;
        }
        it = str_queue.erase(it, it + l - 1);
        end.offset -= l - 1;
        it->len = it->len - (end.str - it->str);
        it->str = end.str;
    }
    return end;
}
void string_slice_view::replace(const iterator &start, const iterator &end,
                                const iterator &new_start,
                                const iterator &new_end) {
    insert(earse(start, end), new_start, new_end);
}
string_slice_view::iterator string_slice_view::begin() const {
    return iterator(&str_queue);
}
string_slice_view::iterator string_slice_view::end() const {
    iterator e{&str_queue};
    e.offset = str_queue.size();
    e.str = &iterator::__EOF;
    return e;
}
std::ostream &operator<<(std::ostream &os, const string_slice_view &view) {
    for (auto it : view) {
        os << it;
    }
    return os;
}
