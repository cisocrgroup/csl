#include<iostream>
#include "TransTable.cxx"

int main() {
    typedef csl::TransTable< csl::BASIC > TransTable_t;
    TransTable_t t(10);

    TransTable_t::State_t state(10);

    state.addEdge(2,5);
    state.addAnnotation(25);
    state.addAnnotation(42);
    state.addAnnotation(43);
    t.storeState(state);

    state.reset();
    state.addEdge(2,7);
    state.addAnnotation(60);
    state.addAnnotation(61);
    state.setFinal(true);
    t.storeState(state);




}
