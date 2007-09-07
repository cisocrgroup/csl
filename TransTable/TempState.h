#ifndef CSL_TRANSTABLE_STATE
#define CSL_TRANSTABLE_STATE CSL_TRANSTABLE_STATE

#include <vector>
#include <cassert>
#include <iostream>
#include "../Alphabet/Alphabet.h"
#include "../Global.h"
namespace csl {

    /**
       TempState is to be used to construct a state of the automaton bit by bit. Once it is certain that no more outgoing edges need to be added, it can be stored into the Automaton. Edges can point to already-stored edges only.

       @author Uli Reffle <uli@reffle.de>
       @date 2005
    */

    class TempState {
    private:
	/**
	 * Represents one transitions of the TempState
	 */
	class Transition {
	public:
	    Transition( wchar_t label, uint_t target, size_t phNumber) {
		label_ = label;
		target_ = target;
		phNumber_ = phNumber;
	    }
	    wchar_t getLabel() const { return label_; }
	    uint_t getTarget() const { return target_; }
	    size_t getPhNumber() const { return phNumber_; }

	    // we need this method whenever the state is marked as final
	    void incPhNumber() { ++phNumber_; }
	private:
	    wchar_t label_;
	    uint_t target_;
	    size_t phNumber_;
	}; // class Transition

	std::vector<Transition> transitions_;

	std::vector<wchar_t> susoString_;

	int phSum_;

	bool isFinal_;

	std::vector<int> annotations_;

	class Iterator {
	public:
	    Iterator( TempState& myTempState ) : myTempState_( myTempState ),
						       index_( 0 ) {
	    }
 
	    bool isValid() const {
		return ( index_ < ( myTempState_.transitions_.size() ) );
	    }
	    Transition& operator*() {
		return myTempState_.transitions_.at( index_ );
	    }

	    Transition* operator->() {
		return &( myTempState_.transitions_.at( index_ ) );
	    }

	    Transition& operator++() {
		++index_;
		return myTempState_.transitions_.at( index_ );
	    }
	    

	private:
	    TempState& myTempState_;
	    size_t index_;
	}; // class Iterator

	Iterator getIterator() {
	    return Iterator( *this );
	}

    public:

	class ConstIterator {
	public:
	    ConstIterator( const TempState& myTempState ) : myTempState_( myTempState ),
						       index_( 0 ) {
	    }
 
	    bool isValid() const {
		return ( ( index_ ) < ( myTempState_.transitions_.size() ) );
	    }
	    const Transition& operator*() {
		return myTempState_.transitions_[index_];
	    }

	    const Transition* operator->() {
		return &( myTempState_.transitions_[index_] );
	    }

	    const Transition& operator++() {
		++index_;
		return myTempState_.transitions_[index_];
	    }

	private:
	    const TempState& myTempState_;
	    size_t index_;
	}; // class ConstIterator

	ConstIterator getConstIterator() const {
	    return ConstIterator( *this );
	}


	TempState() : phSum_( 0 ),
		      isFinal_( false )
	    {

	    reset();
	}

	~TempState() {
	}
	
	inline int getPhValue() const {
	    return phSum_ + ( isFinal() ? 1 : 0 );
	}

	/**
	 * add an outgoing edge
	 *
	 * @param label
	 * @param target
	 */
	inline void addTransition( wchar_t label, uint_t target, size_t targetPhNumber ) {
	    // assert that new labels are coming in alphabetical order
	    // that's important for transStr_
	    if( ( transitions_.size() > 0 ) &&
		( transitions_.at( transitions_.size() - 1 ).getLabel() >= label ) ) {

		throw exceptions::cslException("TempState: new transition violating alphabetical order");
	    }
	    
	    transitions_.push_back( Transition( label, target, getPhValue() ) );
	    susoString_.at( susoString_.size() - 1 ) = label;
	    susoString_.push_back( 0 );

	    phSum_ += targetPhNumber;
	}

	/**
	   reset the state for re-use
	*/
	inline void reset() {
	    transitions_.clear();

	    susoString_.clear();
	    susoString_.push_back( 0 );
	    
	    annotations_.clear();
	    isFinal_ = false;
	    phSum_ = 0;
	}

	inline void addAnnotation( int newAnn ) {
	    annotations_.push_back( newAnn );
	}

	inline int getNrOfAnnotations() const {
	    return annotations_.size();
	}

	inline int getAnnotation( size_t index ) const {
	    return annotations_.at( index );
	}

	inline const wchar_t* getSusoString() {
	    susoString_.push_back( 0 );  // make susoString_ a \0-terminated string
	    return &( susoString_.at( 0 ) );
	}

	/**
	 * mark the state as final/ not final
	 */
	void setFinal( bool b ) {
	    if( ! isFinal_ ) {
		for( Iterator it = getIterator(); it.isValid(); ++it ) {
		    (*it).incPhNumber();
		}
	    }
	    isFinal_ = b;
	}

	/**
	 * returns true iff state is marked as final
	 * @returns true iff state is marked as final
	 */
	bool isFinal() const {
	    return isFinal_;
	}

    };

} //eon

#endif
