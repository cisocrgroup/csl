#ifndef CSL_SIMPLEFSA_SIMPLE2TRANSTABLE
#define CSL_SIMPLEFSA_SIMPLE2TRANSTABLE CSL_SIMPLEFSA_SIMPLE2TRANSTABLE

#include "./SimpleFSA.h"
#include<csl/TransTable/TransTable.h>
#include<csl/Stopwatch.h>


namespace csl {
    namespace SimpleFSA {


	/**
	 * @brief Simple2TransTable is a brute-force solution to store a SimpleFSA into a TransTable.
	 *
	 */
	class Simple2TransTable {
	public:
	    void translate( Automaton const& sfsa, TransTable< TOKDIC >* transTable  );

	private:
	    typedef TransTable< TOKDIC > TransTable_t;
	    void translateState( State* simpleState, TempState* tempState );
	    std::map< State const*, size_t > stateMap_;

	};


	void Simple2TransTable::translate( Automaton const& sfsa, TransTable< TOKDIC >* transTable ) {
	    transTable->initConstruction();

	    TempState tempState;

	    Stopwatch watch; watch.start();
	    std::wcerr <<"Insert states to TransTable ... "<<std::endl;
	    size_t count = 0;
	    for( Automaton::const_state_iterator state = sfsa.states_begin(); state != sfsa.states_end(); ++state ) {
		translateState( *state, &tempState );
		stateMap_[*state] = transTable->storeTempState( tempState );
		if( ++count % 100000 == 0 ) std::wcerr<<count/1000<<"k states processed"<<std::endl;
	    }
	    std::wcerr <<"done. "<< watch.readSeconds() << " seconds" << std::endl;
	    transTable->setRoot( stateMap_[sfsa.getRoot()] );

	    TransTable_t::Cell_t* cells = transTable->getCells();
	    size_t nrOfCells = transTable->getNrOfCells();

	    watch.start();
	    std::wcerr <<"Rewrite TransTable ... "<<std::flush;
	    for( size_t i = 0; i < nrOfCells; ++i ) {
		cells[i].setValue( stateMap_[(State const*)cells[i].getValue()] );
	    }
	    std::wcerr <<"done. "<< watch.readSeconds() << " seconds" << std::endl;

	    transTable->finishConstruction();

	    

	}

	void Simple2TransTable::translateState( State* simpleState, TempState* tempState ) {
	    tempState->reset();

	    tempState->setFinal( simpleState->isFinal() );
	    for( State::const_trans_iterator trans = simpleState->trans_begin();
		 trans != simpleState->trans_end(); 
		 ++trans ) {

		tempState->addTransition( trans->first, (size_t)trans->second, 0 ); // 3rd arg ist the phValue ...
	    }
	}

    }
}

#endif
