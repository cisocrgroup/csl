#ifndef RESULTSET_CXX
#define RESULTSET_CXX RESULTSET_CXX

#include "ResultSet.h"

namespace csl {

    std::ostream& operator<<(std::ostream& os, const csl::ResultSet::Item& item) {
	os<<item.getStr()<<"("<<item.getAnn()<<")";
	return os;
    }
}

#endif
