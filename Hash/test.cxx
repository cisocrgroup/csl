#include<iostream>

#include "./Hash.h"

using namespace csl;

void smokeTest() {
    uchar* strings = (uchar*)malloc( 3 );
    Hash h( 5, strings, 3 );

    h.findOrInsert((uchar*)"Uli");
    h.findOrInsert((uchar*)"Anna");
    h.findOrInsert((uchar*)"Uli");
}


int main() {
    std::cout<<"Tests for class Hash"<<std::endl;

    smokeTest();

}


