// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h
#include <iostream>
#include <stdexcept>

namespace epl{

template <typename T>
    
class vector {
private:
    T ** data;
    uint64_t capacity;
    unit64_t length;
    unit64_t head, tail;
    const static DEFAULT_CAPACITY = 8;
public:
    /* Default Constructor */
    vector(void) {
        data = new T* [DEFAULT_CAPACITY];
        capacity = DEFAULT_CAPACITY;
        length = 0;
    }
    /* Constructor with an argument of size */
    vector(unit64_t n) {
        if (n == 0) {
            data = new T* [DEFAULT_CAPACITY];
            capacity = DEFAULT_CAPACITY;
            length = 0;
            head = 0, tail = 0;
        } else {
            data = new T* [n];
            this -> capacity = n;
            length = n;
            head = 0, tail = length - 1;
            for (unit64_t i = 0; i < n; i++)
                data[i] = new T;
        }
    }
    ~vector(void) { destroy(); }
    
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
    
    T& operator[](unit64_t k) {
        if (k >= size)
            throw std::out_of_range{"subscript out of range"};
        return data[(head + k + length) % length];
        
    }
    
    const T& operator[](unit64_t k) {
        if (k >= size)
            throw std::out_of_range{"subscript out of range"};
        return data[(head + k + length) % length];
    }
    
    /* return the number of constructed objects in the vector */
    unit64_t(void) const { return size; }
    
    /* add a new value to the end of the array */
    void push_back(const T& element) {
        length++;
        if (length == capacity) resize();
        tail++;
        data[tail] = new T{element};
        tail = length - 1;
    }
    /* add a new value to the end of the array */
    void push_back(T&& element) {
        length++;
        if (length == capacity) resize();
        tail++;
        data[tail] = new T{std::move(element)};
        tail = length - 1;
    }
    /* add a new value to the front of the array */
    void push_front(const T& element) {
        length++;
        if (length == capacity) resize();
        head--;
        data[(head + length) % length] = new T{element};
        head = length - 1;
    }
    /* add a new value to the front of the array */
    void push_front(T&& element) {
        length++;
        if (length == capacity) resize();
        data[(head + length) % length] = new T{std::move(element)};
        head = length - 1;
    }
    
    
private:

    /*Copy function is used to implement copy constructor and assignment*/
    void copy(const vector* that) {
        capacity = that.capacity;
        data = new T* [capacity];
        length = that.length;
        head = that.head;
        tail = that.tail;
        for (unit64_t i = 0; i < length; i++)
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
        for (unit64_t i = 0; i < length; i++) {
            data[i] = old[i];
        }
        destroy(old, capacity, length, head);
        head = 0;
    }
        
    void destroy() {
        delete[] data;
    }
 
    
};

} //namespace epl

#endif /* _Vector_h */
