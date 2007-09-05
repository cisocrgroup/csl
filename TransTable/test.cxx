#include<iostream>
#include "TransTable.cxx"

int main() {
    typedef csl::TransTable< csl::TOKDIC > TransTable_t;
    TransTable_t t;

    csl::TempState state;

    state.addTransition( 2, 5, 0 );
    t.storeTempState( state );

    state.reset();
    state.addTransition( 2, 7, 0 );
    state.setFinal( true );
    t.storeTempState( state );




}
