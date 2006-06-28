#ifndef LEVFILTER_H
#define LEVFILTER_H LEVFILTER_H

#include "../Global.h"

namespace csl {
    /**
     * @author Uli Reffle
     * @date 2006
     *
     * @description
     * LevFilter - an interface for classes providing approximate search in
     * dictionaries.
     */
    class LevFilter {
    private:
    public:
	// documentation for ResultSet_if: see below
	class ResultSet_if;

	/**
	   @
	*/
	virtual int query(const uchar* query, ResultSet_if& resultSet) = 0;

	virtual ~LevFilter() {
	    // compiler wants a virtual destructor for abstract classes
	}
    };


    class LevFilter::ResultSet_if {
    public:
	virtual bool push(const uchar* str, int annotation) = 0;
	virtual void reset() = 0;
    };

} // eon

#endif
