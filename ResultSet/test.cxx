#include<iostream>

#include "./ResultSet.cxx"
using namespace csl;

int main() {
    std::cout<<"ResultSet test"<<std::endl;
    ResultSet s;

    s.push((uchar*)"uli",25);
    s.push((uchar*)"felix",5);
    s.push((uchar*)"maxi",1);


    for(ResultSet::Iterator it = s.begin(); it != s.end(); ++it) {
	std::cout<<(*it)<<std::endl;
    }
    
}
