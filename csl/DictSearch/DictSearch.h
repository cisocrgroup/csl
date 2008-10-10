#ifndef CSL_DICTSEARCH_H
#define CSL_DICTSEARCH_H CSL_DICTSEARCH_H

#include<ostream>
#include<csl/MinDic/MinDic.h>
#include<csl/FBDic/FBDic.h>
#include<csl/MSMatch/MSMatch.h>
#include<csl/LevFilter/LevFilter.h>
#include<csl/Vaam/Vaam.h>

namespace csl {
    
    /**
     * @brief csl::DictSearch is a combined interface for approximate dictionary lookup in the context of historical language.
     *
     * A special @link dictSearch_manual DictSearch Manual@endlink page was prepared for this module. You might want
     * to have a look at it before reading this class reference.
     *
     * csl::DictSearch is a combined interface for a number of modules of the package. The scenario is that the users
     * specify a modern as well as a historical dictionary to perform exact or approximate dictionary lookups.
     * A third component is what we call the "hypothetic dictionary", containing all orthographical variants that
     * can possibly be derived from some word of the modern dictionary and the application of some "orthographical
     * variant patterns". These patterns are simple rewrite rules and can be specified by the user.
     * 
     * For a query word @c w the users receive as answer a set of words, containing exact and approximate matches for 
     * either of the three dictionaries. In the usual application this answer set is understood as set of correction
     * candidates for a (probably garbled) token @c w. The task of ranking these candidates is explicitly out of the scope of 
     * csl::DictSearch: To decide for a correction candidate, various other techniques for channel and language modelling have 
     * to be taken into account.
     *
     * @author Ulrich Reffle<uli@cis.uni-muenchen.de>, 2008
     */
    class DictSearch {
    public:
	typedef FBDic<> Dict_t;
	typedef MinDic<> VaamDict_t;

	enum DictID { UNDEF, MODERN, HISTORIC };

	static const size_t INFINITE = (size_t)-1;


	class Interpretation : public csl::Interpretation {
	public:
	    Interpretation() : dictID_( UNDEF ) {} 
	    
	    Interpretation( csl::Interpretation const& interpretation, DictID id ) : 
		csl::Interpretation( interpretation ),
		dictID_( UNDEF )
		{} 

	    bool operator<( Interpretation const& other ) const {
		// set up a comparison bases on the sum of levenshtein or pattern edits.
		// Give the leevenshtein operations a marginally higher punishment, so that
		// if the sums are equal, the one with less lev. operations and more pattern 
		// operations will win.
		float compareSumOfOperations = 
		    ( getInstruction().size() + getLevDistance() * 1.01 ) - 
		    ( other.getInstruction().size() + other.getLevDistance() * 1.01 );

		if     ( compareSumOfOperations < 0 ) return true;
		else if( compareSumOfOperations > 0 ) return false;
		else if( dictID_ != other.getDictID() ) return ( dictID_ < other.getDictID() );
		else return ( getWord() < other.getWord() );
	    }

	    void setDictID( DictID dictID ) { dictID_ = dictID; }
	    DictID getDictID() const { return dictID_; }
	    std::wstring getDictID_string() const {
		if( getDictID() == UNDEF ) return L"undef";
		else if( getDictID() == MODERN ) return L"modern";
		else if( getDictID() == HISTORIC ) return L"historic";
		else return L"unknown";
	    }

	    void print( std::wostream& os = std::wcout ) const {
		csl::Interpretation::print( os );
		os << "(" << getDictID_string() << ")";
	    }

	private:
	    DictID dictID_;
	}; // class Interpretation
	


	/**
	 * 
	 */
	class CandidateSet : public csl::LevFilter::CandidateReceiver,
			     public csl::Vaam< VaamDict_t >::iCandidateReceiver,
			     std::vector< csl::DictSearch::Interpretation > {
	public:
	    typedef std::vector< csl::DictSearch::Interpretation >::iterator iterator;
	    typedef std::vector< csl::DictSearch::Interpretation >::const_iterator const_iterator;

	    void receive( const wchar_t* str, int levDistance, int annotation ) {
		csl::Interpretation cslInt;
		cslInt.setWord( str );
		cslInt.setBaseWord( str );
		cslInt.setBaseWordScore( annotation );
		cslInt.setLevDistance( levDistance );
		receive( cslInt );
	    }
	    
	    virtual void receive( csl::Interpretation const& vaam_interpretation ) {
		push_back( Interpretation( vaam_interpretation, currentDictID_ ) );
		back().setDictID( currentDictID_ );
	    }

	    void setCurrentDictID( DictID id ) { currentDictID_ = id; }

	    void reset() {
		std::vector< csl::DictSearch::Interpretation >::clear();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    void clear() {
		std::vector< csl::DictSearch::Interpretation >::clear();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    iterator begin() {
		return std::vector< csl::DictSearch::Interpretation >::begin();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    const_iterator begin() const {
		return std::vector< csl::DictSearch::Interpretation >::begin();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    iterator end() {
		return std::vector< csl::DictSearch::Interpretation >::end();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    const_iterator end() const {
		return std::vector< csl::DictSearch::Interpretation >::end();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    size_t size() const {
		return std::vector< csl::DictSearch::Interpretation >::size();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    csl::DictSearch::Interpretation const& at( size_t i ) const {
		return std::vector< csl::DictSearch::Interpretation >::at( i );
	    }


	private:
	    DictID currentDictID_;
	}; // class CandidateSet


	/**
	 * @brief This class is designed to manage all configuration issues in connection with the dictionary lookups.
	 *
	 * class DictSearch creates two such objects, one for the lookup in the modern and one for the historic dictionary.
	 * The hypothetical dictionary is configured by class ConfigHypothetic, see below.
	 */
	class ConfigLookup {
	public:
	    /**
	     * @brief A standard constructor
	     */
	    ConfigLookup() :
		dict_( 0 ),
		disposeDict_( false )
		{
		    setDLev( 0 );
		}

	    ~ConfigLookup() {
		if( dict_ && disposeDict_ ) delete( dict_ );
	    }

	    /**
	     * @brief returns a pointer to the connected dictionary. (May be 0)
	     */
	    Dict_t const* getDict() const {
		return dict_;
	    }

	    /**
	     * @brief connects the configuration to a certain dictionary
	     * 
	     * @param dict a const reference to an existing dictionary.
	     */
	    void setDict( Dict_t const& dict ) {
		if( dict_ && disposeDict_ ) delete dict_;
		dict_ = &dict;
		disposeDict_ = false;
	    }

	    /**
	     * @brief Loads a dictionary from the hard disk and connects it to the configuration 
	     * 
	     * @param dictFile a path to a file storing a dictionary of type Dict_t
	     */
	    void setDict( char const* dictFile ) {
		setDict( *( new Dict_t( dictFile ) ) );
		disposeDict_ = true;
	    }

	    /**
	     * @brief Sets a static Levenshtein distance threshold for approximate lookup.
	     * 
	     * If you set a value other than 0 here (which is the default value), the lookup algorithm
	     * will, for a query word w, return all words w' of the dictionary where the levenshtein distance
	     * between w and w' does not exceed the given boundary.
	     */
	    void setDLev( size_t dLev ) {
		if( dLev > 3 ) throw exceptions::LogicalError( "csl::DictSearch::ConfigLookup::setDLevStatic: choose a threshold in the range [0..3]" );
		// set minWordLengths to 0 for all levDistances smaller or equal to dLev
		for( size_t i = 1; i <= 3; ++i ) {
		    minWordlengths_[i] = ( i <= dLev )? 0 : INFINITE;
		}
	    }

	    /**
	     * @brief configures the approximate lookup in a way that the lev-distance bound (see setDLev() ) depends on 
	     * the word length of the query word.
	     *
	     * @param wordlength_1 minimal word length for a query to be searched for with distance bound 1
	     * @param wordlength_2 minimal word length for a query to be searched for with distance bound 2
	     * @param wordlength_3 minimal word length for a query to be searched for with distance bound 3
	     * 
	     * A search with distance bound n naturally also returns candidates with distances smaller than n. For example, 
	     * searching with distance 2 will in general return candidates with distance 0,1,2.
	     * That's why it makes no sense to assign, e.g. wordlength_2=8 and wordlength_1=6. Whenever 
	     * wordlength_i is smaller than wordlength_i-1 an exception is thrown.
	     */
	    void setDLevWordlengths( size_t wordlength_1, size_t wordlength_2, size_t wordlength_3 ) {
		if( wordlength_2 < wordlength_1 || wordlength_3 < wordlength_2 ) 
		    throw exceptions::LogicalError( "csl::DictSearch::ConfigLookup::setDLevWordlengths: minimal wordlength for distance i must be greater than for distance i-1" );

		minWordlengths_[1] = wordlength_1;
		minWordlengths_[2] = wordlength_2;
		minWordlengths_[3] = wordlength_3;
	    }
	    
	    /**
	     * @brief Sets sensible standard values for word-length dependent distance bounds for approximate search
	     *
	     * - lev. distance 1 for wordslengths 1-6
	     * - lev. distance 2 for wordslengths 7-12
	     * - lev. distance 3 for wordslengths > 13
	     *
	     * @see setDLevWordlengths( size_t wordlength_1, size_t wordlength_2, size_t wordlength_3 ), setDLev
	     */
	    void setDLevWordlengths() {
		minWordlengths_[1] = 1;
		minWordlengths_[2] = 7;
		minWordlengths_[3] = 13;
	    }

	    /**
	     * @brief get the lev-distance threshold given a word length
	     */
	    size_t getDLevByWordlength( size_t wordLength ) const {
		for( int i = 3; i >= 1; --i ) {
		    if( ( minWordlengths_[i] != INFINITE ) && ( minWordlengths_[i] <= wordLength ) ) return i;
		}
		return 0;
	    }

	private:
	    Dict_t const* dict_;
	    bool disposeDict_;
	    size_t minWordlengths_[4];

	}; // class ConfigLookup

	class ConfigHypothetic : public ConfigLookup {
	public:
	    ConfigHypothetic() :
		maxNrOfPatterns_( Vaam<>::INFINITE )
		{
	    }

	    /**
	     * @brief returns the current setting for the upper bound of applied variant patterns
	     */
	    size_t getMaxNrOfPatterns() const {
		return maxNrOfPatterns_;
	    }

	    /**
	     * @brief sets a new value for the upper bound of applied variant patterns
	     */
	    void setMaxNrOfPatterns( size_t max ) {
		maxNrOfPatterns_ = max;
	    }
	private:
	    size_t maxNrOfPatterns_;
	}; 


	/**
	 * @name Constructor/ Destructor
	 */
	//@{
	DictSearch();
	~DictSearch();
	//@}

	/**
	 * @name Configuration
	 */
	//@{


	/**
	 * @brief returns the configuration object responsible for lookup in the modern dictionary
	 */
	ConfigLookup& getConfigModern() {
	    return configModern_;
	}

	/**
	 * @brief returns the configuration object responsible for lookup in the historical dictionary
	 */
	ConfigLookup& getConfigHistoric() {
	    return configHistoric_;
	}

	/**
	 * @brief returns the configuration object responsible for lookup in the hypothetic dictionary
	 */
	ConfigHypothetic& getConfigHypothetic() {
	    return configHypothetic_;
	}

	/**
	 * @brief initialise the hypothetic dictionary.
	 * @param patternFile A file containing a list of rewrite patterns. Consult csl::PatternSet for details
	 *
	 * At the moment you can NOT change the pattern set afterwards.
	 * Please notify the developers if you need such a feature.
	 */
	void initHypothetic( char const* patternFile );

	
	//@} // END Configuration methods

	/**
	 * @name Lookup
	 */
	//@{

	/**
	 *
	 * @param[in] query
	 * @param[out] answers
	 */
	void query( std::wstring const& query, CandidateSet* answers );
	
	//@} // END Lookup methods

    private:

	ConfigLookup configModern_;
	ConfigLookup configHistoric_;
	ConfigHypothetic configHypothetic_;
	
	Vaam< VaamDict_t >* vaam_;
	size_t dlev_hypothetic_;
	size_t dlev_maxNrOfPatterns_;

	MSMatch< FW_BW > msMatch_;

    }; // class DictSearch


} // namespace csl

namespace std {
    inline wostream& operator<<( wostream& os, csl::DictSearch::Interpretation const& obj ) {
	obj.print( os );
	return os;
    }
}


#endif
