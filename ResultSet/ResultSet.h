#ifndef RESULTSET_H
#define RESULTSET_H RESULTSET_H

#include<cassert>
#include<iostream>
#include "../Global.h"
#include "../LevFilter/LevFilter.h"



namespace csl {

    /**
       \brief a container class for candidate sets extracted by a LevFilter object
       \author Uli Reffle
       \date 2006
     */
    class ResultSet : public LevFilter::ResultSet_if {
    public:
	class Item {
	private:
	    uchar str_[Global::lengthOfWord];
	    int annotation_;
	public:
	    void set(const uchar* str, int ann) {
		strcpy((char*)str_, (char*)str);
		annotation_ = ann;
	    }
	    const uchar* getStr() const {
		return str_;
	    }
	    int getAnn() const {
		return annotation_;
	    }
	};

	int size_;
	Item list_[Global::LevMaxNrOfResults];

    public:
	ResultSet() {
	    reset();
	}

	class Iterator {
	private:
	    const Item *cur_;
	public:

	    Iterator(const Item *init) : cur_(init) {
	    }

	    const Item& operator*() const {
		return *cur_;
	    }

	    Iterator operator++() {
		++cur_;
		return *this;
	    }

	    bool operator==(const Iterator& i2) const {
		return (cur_ == i2.cur_);
	    }

	    bool operator!=(const Iterator& i2) const {
		return (cur_ != i2.cur_);
	    }

	};

	Iterator begin() const {
	    return Iterator(list_);
	}

	Iterator end() const {
	    return Iterator(list_ + size_);
	}

	int getSize() const {
	    return size_;
	}

	const Item& operator[](int i) const {
	    assert(i < size_);
	    return list_[i];
	}

	bool push(const uchar* str, int annotation) {
	    if(size_ >= Global::LevMaxNrOfResults) {
		return false;
	    }
	    list_[size_++].set(str, annotation);
	    return true;
	}

	void reset() {
	    size_ = 0;
	}

    };

    /// provide a nice print for a ResultSet::Item
    std::ostream& operator<<(std::ostream& os, const csl::ResultSet::Item& item);


} // eon


#endif
