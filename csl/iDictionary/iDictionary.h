#ifndef CSL_IDICTIONARY_H
#define CSL_IDICTIONARY_H CSL_IDICTIONARY_H

namespace csl {

    class iDictionary {
    public:
	virtual bool lookup( wchar_t const* str, int* annotation = 0 ) const = 0;
	virtual bool lookup( std::wstring const& str, int* annotation = 0 ) const = 0;
	
    }; // class iDictionary
    
} // namespace csl

#endif
