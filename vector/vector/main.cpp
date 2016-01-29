//
//  main.cpp
//  vector
//
//  Created by mackbook on 1/28/16.
//  Copyright © 2016 utaustin. All rights reserved.
//

#include <iostream>
#include "Vector.h"
using namespace epl;

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    vector <int> v;
    v.push_back(5);
    v.push_back(4);
    v.print();
    return 0;
}
