// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h
#include <iostream>
#include <cstdint>
#include <stdexcept>

namespace epl{

template <typename T>
    
class vector {
private:
    T ** data;
    uint64_t capacity;
    uint64_t length;
    uint64_t head, tail;
    const static uint64_t DEFAULT_CAPACITY = 8;
public:
    /* Default Constructor */
    vector(void) {
        data = new T* [DEFAULT_CAPACITY];
        capacity = DEFAULT_CAPACITY;
        length = 0;
        head = 0;
        tail = -1;
        
    }
    /* Constructor with an argument of size */
    vector(uint64_t n) {
        if (n == 0) {
            data = new T* [DEFAULT_CAPACITY];
            capacity = DEFAULT_CAPACITY;
            length = 0;
            head = 0, tail = -1;
        } else {
            data = new T* [n];
            this -> capacity = n;
            length = n;
            head = 0, tail = length - 1;
            for (uint64_t i = 0; i < n; i++)
                data[i] = new T;
        }
    }
    ~vector(void) { destroy(data, capacity, length, head); }
    
    /* Copy constructor */
    vector(vector const& that) { copy(that); }
    /* Copy assignment */
    vector& operator=(vector const& rhs) {
        if (this != &rhs) {
            destroy();
            copy(rhs);
        }
        return *this;
    }
    
    /*Move constructor */
    vector(vector && tmp) { move(tmp); }
    /*Move assignment */
    vector& operator=(vector&& rhs) {
        if (this != & rhs) {
            destroy();
            move(rhs);
        }
        return *this;
    }
    
    T& operator[](uint64_t k) {
        if (k >= length)
            throw std::out_of_range{"subscript out of range"};
        return *data[(head + k + capacity) % capacity];
        
    }
    
    const T& operator[](uint64_t const k) const {
        if (k >= length)
            throw std::out_of_range{"subscript out of range"};
        return *data[(head + k + capacity) % capacity];
    }
    
    /* return the number of constructed objects in the vector */
    uint64_t getLength(void) const { return length; }
    
    /* add a new value to the end of the array */
    void push_back(const T& element) {
        length++;
        if (length == capacity) resize();
        tail++;
        data[tail] = new T{element};
    }
    /* add a new value to the end of the array */
    void push_back(T&& element) {
        length++;
        if (length == capacity) resize();
        tail++;
        data[tail] = new T{std::move(element)};

    }
    /* add a new value to the front of the array */
    void push_front(const T& element) {
        length++;
        if (length == capacity) resize();
        head--;
        data[(head + length) % length] = new T{element};
    }
    /* add a new value to the front of the array */
    void push_front(T&& element) {
        length++;
        if (length == capacity) resize();
        head--;
        data[(head + length) % length] = new T{std::move(element)};
    }
    
    void print() {
        for (int k = 0; k < length; k++)
            std::cout << (*this)[k] << " ";
//        std::cout<< "big ass";
    }
    
    
private:

    /*Copy function is used to implement copy constructor and assignment*/
    void copy(vector<T> const & that) {
        capacity = that.capacity;
        data = new T* [capacity];
        length = that.length;
        head = that.head;
        tail = that.tail;
        for (uint64_t i = 0; i < length; i++)
            data[i] = new T(*that.data[i]);
    }
    
    void move(const vector&& tmp) {
        capacity = tmp.capacity;
        data = tmp.data;
        length = tmp.length;
        head = tmp.head;
        tail = tmp.tail;
    }
    
    void resize() {
        capacity = capacity * 2;
        T ** old = data;
        data = new T* [capacity];
        for (uint64_t i = 0; i < length; i++) {
            data[i] = old[i];
        }
        destroy(old, capacity, length, head);
        head = 0;
    }
        
    void destroy(T ** data, uint64_t capacity, uint64_t length, uint64_t head) {
        for (int k = 0; k < length; k++) {
            delete data[(head + k + capacity) % capacity];
        }
    }
 
    
};

} //namespace epl

#endif /* _Vector_h */
