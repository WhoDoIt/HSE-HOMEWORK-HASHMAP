#include <bits/stdc++.h>


template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    struct HashCell {
        bool is_free = true;
        size_t hash;
        size_t distance;
        std::pair<const KeyType, ValueType> kv;

        HashCell() : hash(0), distance(0) {};

        HashCell(KeyType key, ValueType value, size_t hash, size_t distance = 0) : is_free(false),
                                                                                   hash(hash),
                                                                                   distance(distance),
                                                                                   kv({key, value}) {};
    };

    size_t _size = 0;
    size_t _capacity = 1;
    std::vector<HashCell *> _bukkit;
    typename std::vector<HashCell *>::iterator first_element;
    Hash _hasher;

    void rebuild(size_t nc) {
        std::vector<HashCell *> new_bukkit(nc, nullptr);
        auto new_first_element = new_bukkit.end();
        _size = 0;
        for (iterator b_it = begin(); b_it != end(); ++b_it) {
            auto el = *b_it;
            size_t hash = _hasher(el.first);
            size_t index = hash % (nc);
            HashCell *insert_element = new HashCell(el.first, el.second, hash);
            for (size_t i = 0; i < nc; i += 1) {
                if (new_bukkit[index] == nullptr || new_bukkit[index]->is_free) {
                    new_bukkit[index] = insert_element;
                    break;
                } else if (new_bukkit[index]->distance < insert_element->distance) {
                    std::swap(new_bukkit[index], insert_element);
                }
                index += 1;
                if (index >= nc) {
                    index -= nc;
                }
                insert_element->distance += 1;
            }
            _size += 1;

            if (new_first_element > new_bukkit.begin() + index) {
                new_first_element = new_bukkit.begin() + index;
            }
        }
        first_element = new_first_element;
        for (auto &p: new_bukkit) {
            if (p == nullptr) {
                p = new HashCell();
            }
        }
        for (auto &p: _bukkit) {
            delete p;
        }
        _capacity = nc;
        std::swap(new_bukkit, _bukkit);
    };
public:
    explicit HashMap(Hash hasher = Hash()) : _hasher(hasher) {
        _bukkit = {new HashCell()};
        first_element = _bukkit.end();
    };

    template<class iterator>
    HashMap(iterator begin, iterator end, Hash hasher = Hash()) {
        _hasher = hasher;
        _bukkit = {new HashCell()};
        first_element = _bukkit.end();
        while (begin != end) {
            insert(*begin);
            begin++;
        }
    };

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, Hash hasher = Hash()) {
        _hasher = hasher;
        _bukkit = {new HashCell()};
        first_element = _bukkit.end();
        for (auto &i: list) {
            insert(i);
        }
    };

    HashMap(const HashMap &rhs) {
        *this = rhs;
    };

    ~HashMap() {
        for (auto &i: _bukkit) {
            delete i;
        }
    }

    size_t size() const {
        return _size;
    };

    bool empty() const {
        return _size == 0;
    }

    Hash hash_function() const {
        return _hasher;
    }

    class iterator {
    private:
        std::vector<HashCell *> *bukkit;
        typename std::vector<HashCell *>::iterator it;
    public:
        iterator() = default;

        iterator(const size_t &index, std::vector<HashCell *> *bukkit) : bukkit(
                bukkit), it(bukkit->begin() + index) {};

        iterator(typename std::vector<HashCell *>::iterator it,
                 std::vector<HashCell *> *bukkit) : bukkit(bukkit),
                                                    it(it) {};

        iterator(const iterator &rhs) {
            it = rhs.it;
            bukkit = rhs.bukkit;
        }

        iterator &operator++() {
            it++;
            while (it != bukkit->end() && (*it)->is_free) {
                it++;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator copy = iterator(it, bukkit);
            it++;
            while (it != bukkit->end() && (*it)->is_free) {
                it++;
            }
            return copy;
        }

        std::pair<const KeyType, ValueType> &operator*() {
            return (*it)->kv;
        }

        std::pair<const KeyType, ValueType> *operator->() {
            return &((*it)->kv);
        }

        bool operator==(const iterator &rhs) {
            return it == rhs.it;
        }

        bool operator!=(const iterator &rhs) {
            return it != rhs.it;
        }
    };

    class const_iterator {
    private:
        const std::vector<HashCell *> *bukkit;
        typename std::vector<HashCell *>::const_iterator it;
    public:
        const_iterator() = default;

        const_iterator(const size_t &index, const std::vector<HashCell *> *bukkit) : bukkit(
                bukkit), it(bukkit->begin() + index) {};

        const_iterator(const typename std::vector<HashCell *>::const_iterator it,
                       const std::vector<HashCell *> *bukkit) : bukkit(bukkit),
                                                                it(it) {};

        const_iterator(const const_iterator &rhs) {
            it = rhs.it;
            bukkit = rhs.bukkit;
        }

        const_iterator &operator++() {
            it++;
            while (it != bukkit->end() && (*it)->is_free) {
                it++;
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator copy = const_iterator(it, bukkit);
            it++;
            while (it != bukkit->end() && (*it)->is_free) {
                it++;
            }
            return copy;
        }

        const std::pair<const KeyType, ValueType> &operator*() {
            return (*it)->kv;
        }

        const std::pair<const KeyType, ValueType> *operator->() {
            return &((*it)->kv);
        }

        bool operator==(const const_iterator &rhs) {
            return it == rhs.it;
        }

        bool operator!=(const const_iterator &rhs) {
            return it != rhs.it;
        }
    };

    void insert(std::pair<KeyType, ValueType> el) {
        iterator it = find(el.first);
        if (it != end()) {
            return;
        }
        if (4 * _capacity < 5 * (_size + 1)) {
            rebuild(2 * _capacity);
        }

        size_t hash = _hasher(el.first);
        size_t index = hash % _capacity;
        HashCell *insert_element = new HashCell(el.first, el.second, hash);
        for (size_t i = 0; i < _capacity; i += 1) {
            if (_bukkit[index]->is_free) {
                delete _bukkit[index];
                _bukkit[index] = insert_element;
                break;
            } else if (_bukkit[index]->distance < insert_element->distance) {
                std::swap(_bukkit[index], insert_element);
            }
            index += 1;
            if (index >= _capacity) {
                index -= _capacity;
            }
            insert_element->distance += 1;
        }
        _size += 1;

        if (first_element > _bukkit.begin() + index) {
            first_element = _bukkit.begin() + index;
        }
    }

    void erase(KeyType key) {
        size_t hash = _hasher(key);
        size_t index = hash % _capacity;
        for (size_t i = 0; i < _capacity; i += 1) {
            if (_bukkit[index]->is_free) {
                return;
            } else if (_bukkit[index]->hash == hash && _bukkit[index]->kv.first == key) {
                if (first_element == _bukkit.begin() + index) {
                    first_element = _bukkit.end();
                }
                _bukkit[index]->is_free = true;
                size_t prev_index = index;
                index += 1;
                if (index >= _capacity) {
                    index -= _capacity;
                }
                while (_bukkit[index]->distance > 0) {
                    _bukkit[index]->distance -= 1;
                    std::swap(_bukkit[index], _bukkit[prev_index]);
                    prev_index = index;
                    index += 1;
                    if (index >= _capacity) {
                        index -= _capacity;
                    }
                }
                _size -= 1;
                return;
            }
            index += 1;
            if (index >= _capacity) {
                index -= _capacity;
            }
        }
    }

    std::vector<HashCell *> get_bukkit() {
        return _bukkit;
    }

    iterator begin() {
        for (size_t i = 0; i < _capacity; i += 1) {
            if (!_bukkit[i]->is_free) {
                return iterator(first_element = _bukkit.begin() + i, &_bukkit);
            }
        }
        return end();
    };

    const_iterator begin() const {
        return const_iterator(first_element, &_bukkit);
    };

    iterator end() {
        return iterator(_capacity, &_bukkit);
    };

    const_iterator end() const {
        return const_iterator(_bukkit.end(), &_bukkit);
    };

    iterator find(KeyType key) {
        size_t hash = _hasher(key);
        size_t index = hash % _capacity;
        for (size_t i = 0; i < _capacity; i += 1) {
            if (_bukkit[index]->is_free) {
                return end();
            } else if (_bukkit[index]->hash == hash && _bukkit[index]->kv.first == key) {
                return iterator(index, &_bukkit);
            }
            index += 1;
            if (index >= _capacity) {
                index -= _capacity;
            }
        }
        return end();
    }

    const_iterator find(KeyType key) const {
        size_t hash = _hasher(key);
        size_t index = hash % _capacity;
        for (size_t i = 0; i < _capacity; i += 1) {
            if (_bukkit[index]->is_free) {
                return end();
            } else if (_bukkit[index]->hash == hash && _bukkit[index]->kv.first == key) {
                return const_iterator(index, &_bukkit);
            }
            index += 1;
            if (index >= _capacity) {
                index -= _capacity;
            }
        }
        return end();
    }

    ValueType &operator[](KeyType key) {
        iterator it = find(key);
        if (it == end()) {
            insert({key, ValueType()});
        }
        return find(key)->second;
    }

    HashMap &operator=(const HashMap &rhs) {
        if (&rhs == this) {
            return *this;
        }
        if (_bukkit.size() != 0) {
            for (auto &i: _bukkit) {
                delete i;
            }
        }
        _hasher = rhs._hasher;
        _size = rhs._size;
        _capacity = rhs._capacity;
        _bukkit.resize(_capacity);
        first_element = (rhs.first_element - rhs._bukkit.begin()) + _bukkit.begin();
        for (size_t i = 0; i < _capacity; i += 1) {
            _bukkit[i] = new HashCell(rhs._bukkit[i]->kv.first, rhs._bukkit[i]->kv.second, rhs._bukkit[i]->hash,
                                      rhs._bukkit[i]->distance);
            _bukkit[i]->is_free = rhs._bukkit[i]->is_free;
        }
        return *this;
    }

    const ValueType &at(KeyType key) const {
        auto it = this->find(key);
        if (it == end()) {
            throw std::out_of_range("U have not that key");
        }
        return (*it).second;
    }

    void clear() {
        _size = 0;
        for (auto &i: _bukkit) {
            delete i;
        }
        _capacity = 1;
        _bukkit = {new HashCell()};
        first_element = _bukkit.end();
    }

};
