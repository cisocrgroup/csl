#ifndef CSL_TRANSTABLE_STATE
#define CSL_TRANSTABLE_STATE CSL_TRANSTABLE_STATE

#include <vector>
#include <cassert>
#include <iostream>
#include "../Global.h"
namespace csl {

/**
   TempState is to be used to construct a state of the automaton bit by bit. Once it is certain that no more outgoing edges need to be added, it can be stored into the Automaton. Edges can point to already-stored edges only.

   @author Uli Reffle <uli@reffle.de>
   @date 2005
*/

    template< CellType CellTypeValue >
    class TempState {
	// dummy. Use specializations only
    };

    template<>
    class TempState< BASIC > {
    private:
	uint_t* transitions_;
	const int alphSize_;

	std::vector< int > annotations_; 
    
	bool isFinal_;
    
    public:
	TempState( int alphSize ) : alphSize_( alphSize ) {
	    transitions_ = new uint_t[alphSize_ + 1];
	    reset();
	}
    
	~TempState() {
	    delete(transitions_);
	}

	inline uint_t getTransTarget(int c) const {
	    assert(c<(alphSize_ + 1));
	    return transitions_[c];
	}

	/**
	   add an outgoing transition
	   @param label
	   @param target
	*/
	inline void addTransition(uchar label, uint_t target) {
	    assert(label<(alphSize_ + 1));
	    transitions_[label] = target;
	}

	/**
	   reset the state for re-use
	*/
	inline void reset() {
	    memset(transitions_, 0, (alphSize_ + 1)*sizeof(int));
	    annotations_.clear();
	    isFinal_ = false;
	}

	inline void addAnnotation(int newAnn) {annotations_.push_back(newAnn);}

	inline const std::vector<int>& getAnnotations() const {
	    return annotations_;
	}

	inline size_t getNrOfAnnotations() const {
	    return annotations_.size();
	}

	inline int getAnnotation(int n) const {
	    return annotations_.at(n);
	}

	/**
	   mark the state as final/ not final
	*/
	void setFinal(bool b) {
	    isFinal_ = b;
	}

	bool isFinal() const {
	    return isFinal_;
	}

    };


    
    template<>
    class TempState< TOKDIC > {
    private:
	typedef std::pair<int, int> Transition;
	Transition* transitions_;
	uchar transStr_[Global::maxNrOfChars + 1]; // +1 for a terminating \0
	const int alphSize_;

	int phValue_;
	int phSum_;

	int sizeOfLabelStr_;

	bool isFinal_;
	int annotation_; 
	

    public:
	TempState(int alphSize) : alphSize_(alphSize),
			      phValue_(0),
			      phSum_(0),
			      sizeOfLabelStr_(0),
			      isFinal_(false),
			      annotation_(0) {
	    transitions_ = new Transition[alphSize_ + 1];
	    memset(transitions_, 0, alphSize_+1 * sizeof(Transition));
	    reset();
	}
    
	~TempState() {
	    delete[](transitions_);
	}
	
	inline int getTransTarget(int c) const {
	    assert(c<(alphSize_ + 1));
	    return transitions_[c].first;
	}

	inline int getTransPhValue(int c) const {
	    assert(c<(alphSize_ + 1));
	    return transitions_[c].second + (isFinal()? 1: 0);
	}

	inline int getPhValue() const {
	    return phSum_ + (isFinal()? 1: 0);
	}

	/**
	   add an outgoing edge
	   @param label
	   @param target
	*/
	inline void addTransition(uchar label, int target, int targetPhValue) {
	    assert(label<(alphSize_ + 1));
	    // assert that new labels are coming in alphabetical order
	    // that's important for transStr_
 	    assert(sizeOfLabelStr_==0 || transStr_[sizeOfLabelStr_-1] < label);

 	    transStr_[sizeOfLabelStr_++] = label;
	    
	    transitions_[label].first = target;
	    transitions_[label].second = phSum_;

	    phSum_ += targetPhValue;
	}

	/**
	   reset the state for re-use
	*/
	inline void reset() {
	    memset(transitions_, 0, (alphSize_ + 1)*sizeof(Transition));
	    memset(transStr_,0,(Global::maxNrOfChars + 1) * sizeof(uchar));
	    sizeOfLabelStr_ = 0;
	    annotation_ = 0;
	    isFinal_ = false;
	    phSum_ = 0;
	}

	inline void addAnnotation(int newAnn) {
	    annotation_ = newAnn;
	}

	inline int getNrOfAnnotations() const {
	    return 0; // CAUTiON HERE!
	}

	inline int getAnnotation() const {
	    return annotation_;
	}

	inline const uchar* getTransString() const {
	    return transStr_;
	}

	/**
	   mark the state as final/ not final
	*/
	void setFinal(bool b) {
	    isFinal_ = b;
	}

	bool isFinal() const {
	    return isFinal_;
	}

    };

} //eon

#endif
