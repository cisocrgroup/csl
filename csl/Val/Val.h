
#include<vector>

#include<csl/Pattern/PatternGraph.h>
#include<csl/Vaam/Vaam.h>

namespace csl {


    class Val {
    public:

	typedef MinDic<> MinDic_t;

	typedef Vaam< MinDic_t >::CandidateReceiver CandidateReceiver;
	
	static const size_t INFINITE = (size_t)-1;

	/**
	 * @brief A trivial construtor, taking the input ressources as arguments
	 *
	 * @param baseDic a reference to a MinDic_t that serves as @c baseDic
	 * @param patternFile path to a file containing the spelling variant patterns 
	 (see class description for some more details). 
	*/
	Val( MinDic_t const& baseDic, const char* patternFile );
	

	/**
	 * @name Configuration
	 */
	//@{

	/**
	 * @brief restrict allowed number of pattern applications to greater or equal than @c n
	 */
	inline void setMinNrOfPatterns( size_t n );

	/**
	 * @brief restrict allowed number of pattern applications to less or equal than @c n
	 */
	inline void setMaxNrOfPatterns( size_t n );

	/**
	 * @brief set the base dictionary. This restricts output words to words NOT present in this filterDic.
	 */
	inline void setBaseDic( MinDic_t const& baseDic );

	// @}

	/**
	 * @name Usage/ Queries
	 */
	//@{
	/**
	 * @brief query a @c word to get possible interpretations as a variant.
	 *
	 * You can write your own class that implements the interface iCandidateReceiver to receive all answers,
	 * but you can also use Vaam's subclass CandidateReceiver.
	 * 
	 */
	inline bool query( std::wstring const& word, iVaamResultReceiver* interpretations ) const;

	//@}
	
	
    private:

	MinDic_t const* baseDic_;

	PatternGraph patternGraph_; 

	size_t minNrOfPatterns_;
	size_t maxNrOfPatterns_;

    }; // class Val



} // eon

#include "./Val.tcc"
