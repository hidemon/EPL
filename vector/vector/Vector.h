// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <assert.h>

namespace epl{

template <typename T>
    
class vector {
private:
    T * data;
    uint64_t capacity;
    uint64_t length;
    uint64_t head;
    const static uint64_t DEFAULT_CAPACITY = 8;
public:
    /* Default Constructor */
    vector(void) {
        capacity = DEFAULT_CAPACITY;
        data = (T*) ::operator new(capacity * sizeof(T));
        length = 0;
        head = 0;
        
    }
    /* Constructor with an argument of size */
    vector(uint64_t n) {
        if (n == 0) {
            capacity = DEFAULT_CAPACITY;
            data = (T*) ::operator new(capacity * sizeof(T));
            length = 0;
            head = 0;
        } else {
            this -> capacity = n;
            data = (T*) ::operator new(capacity * sizeof(T));
            length = n;
            head = 0;
            for (uint64_t i = 0; i < n; i++)
                new (data + i) T{};
        }
    }
    ~vector(void) { destroy(data, capacity, length, head); }
    
    /* Copy constructor */
    vector(vector const& that) { copy(that); }
    /* Copy assignment */
    vector& operator=(vector const& rhs) {
        if (this != &rhs) {
            destroy(data, capacity, length, head);
            copy(rhs);
        }
        return *this;
    }
    
    /*Move constructor */
    vector(vector && tmp) { move(std::move(tmp)); }
    /*Move assignment */
    vector& operator=(vector&& rhs) {
        if (this != & rhs) {
            destroy(data, capacity, length, head);
            move(std::move(rhs));
        }
        return *this;
    }
    
    T& operator[](uint64_t k) {
        if (k >= length)
            throw std::out_of_range{"subscript out of range"};
        return data[(head + k + capacity) % capacity];
        
    }
    
    const T& operator[](uint64_t const k) const {
        if (k >= length)
            throw std::out_of_range{"subscript out of range"};
        return data[(head + k + capacity) % capacity];
    }
    
    /* return the number of constructed objects in the vector */
    uint64_t size(void) const { return length; }
    
    /* add a new value to the end of the array */
    void push_back(const T& element) {
        if (length == capacity) resize();
        new (data + ((head + length + capacity) % capacity)) T{element};
        length++;
    }
    /* add a new value to the end of the array */
    void push_back(T&& element) {
        if (length == capacity) resize();
        new (data + ((head + length + capacity) % capacity)) T{std::move(element)};
        length++;
    }
    /* add a new value to the front of the array */
    void push_front(const T& element) {
        if (length == capacity) resize();
        head = (head + capacity - 1) % capacity;
        new (data + head) T{element};
        length++;
    }
    /* add a new value to the front of the array */
    void push_front(T&& element) {
        if (length == capacity) resize();
        head = (head + capacity - 1) % capacity;
        new (data + head) T{std::move(element)};
        length++;
    }
    
    void pop_front() {
        if (length == 0)
            throw std::out_of_range{"subscript out of range"};
        else {
            data[head].~T();
            head = (head + capacity + 1) % capacity;
            length--;
        }
    }
    
    void pop_back() {
        if (length == 0)
            throw std::out_of_range{"subscript out of range"};
        else {
            data[(head + length - 1) % capacity].~T();
            length--;
        }
    }
    
    void print() {
        for (int k = 0; k < length; k++)
            std::cout << (*this)[k] << " ";
//        std::cout<< "dd";
    }
    
    
private:

    /*Copy function is used to implement copy constructor and assignment*/
    void copy(vector<T> const & that) {
        capacity = that.capacity;
        data = (T*) ::operator new(capacity * sizeof(T));
        length = that.length;
        head = that.head;
        for (uint64_t i = 0; i < length; i++)
            new (data + i) T{that.data[i]};
    }
    
    void move(vector&& tmp) {
        capacity = tmp.capacity;
        data = tmp.data;
        length = tmp.length;
        head = tmp.head;
        tmp.data = nullptr;
        tmp.length = 0;
    }
    
    void resize() {
        capacity = capacity * 2;
        T * old = data;
        data = (T*) ::operator new(capacity * sizeof(T));
        for (uint64_t i = 0; i < length; i++) {
            new (data + i) T{std::move(old[(head + i + (capacity/2)) % (capacity/2)])};
        }
        ::operator delete (old);
        head = 0;
    }
        
    void destroy(T * data, uint64_t capacity, uint64_t length, uint64_t head) {
        for (int k = 0; k < length; k++) {
//            assert(*(data[(head + k + capacity) % capacity]) == k);
            data[(head + k + capacity) % capacity].~T();
        }
        ::operator delete (data);
    }
 
    
};

} //namespace epl

#endif /* _Vector_h */
