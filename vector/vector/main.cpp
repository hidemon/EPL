//
//  main.cpp
//  vector
//
//  Created by mackbook on 1/28/16.
//  Copyright © 2016 utaustin. All rights reserved.
//

#include <iostream>
#include "Vector.h"
#include <assert.h>
using namespace epl;

int main(void) {
    epl::vector<int> v;    
    assert(v.size() == 0);
    for(int i=0; i<10000; i++){
        v.push_back(i);
        assert(v[i] == i);
        assert(v.size() == i+1);
    }
    for(int i=0; i<10000; i++){
        v.push_front(i);
        assert(v[0] == i);
        assert(v.size() == 10001+i);
    }
    for(int i=9999; i>=0; i--){
        assert(v[v.size()-1] == i);
        v.pop_back();
        assert(v.size() == 10000+i);
    }
    for(int i=9999; i>=0; i--){
        assert(v[0] == i);
        v.pop_front();
        assert(v.size() == i);
    }
    assert(v.size() == 0);
    
    for(int i=0; i<10000; i++){
        v.push_back(std::move(i));
        assert(v[v.size()-1] == i);
        v.push_front(std::move(i));
        assert(v[0] == i);
        assert(v.size() == (i+1)*2);
    }
    while(v.size() > 0) v.pop_front();
    assert(v.size() == 0);
    try{
        v[-1] = 0;
        assert(false);
    }catch(std::exception e){}
    try{
        v[0] = 0;
        assert(false);
    }catch(std::exception e){}
    try{
        v.pop_back();
        assert(false);
    }catch(std::exception e){}
    try{
        v.pop_front();
        assert(false);
    }catch(std::exception e){}
    epl::vector<int> w{10000};
    assert(w.size() == 10000);
    for(int i=0; i<w.size(); i++){
        w[i] = i;
        assert(w[i] == i);
    }
    v = w;
    assert(v.size() == w.size());
    for(int i=0; i<v.size(); i++)
        assert(v[i] == w[i]);
    const epl::vector<int> p{v};
    assert(p.size() == v.size());
    for(int i=0; i<p.size(); i++)
        assert(p[i] == v[i]);
    w = epl::vector<int>(p);
    assert(w.size() == p.size());
    for(int i=0; i<w.size(); i++)
        assert(w[i] == p[i]);
    epl::vector<int> q{std::move(w)};
    assert(q.size() == p.size());
    for(int i=0; i<q.size(); i++)
        assert(q[i] == p[i]);
    q = q;
    for(int i=0; i<q.size(); i++)
        assert(q[i] == p[i]);
    std::cout<<"pass!"<<std::endl;
    return 0;
}
