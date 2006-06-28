class List : public LevFilter::ResultSet_if  { // of class PermuteMatch

 public:
    class Item {
    private:
	uchar str_[Global::lengthOfWord];
	int value_;
	bits32 col_;
	bits32 stillPossible_;
	
	
    public:
	void set(const uchar* str, int value, bits32 col) {
	    strcpy((char*)str_, (char*)str);
	    value_ = value;
	    col_ = col;
	}
	const uchar* getStr() const {
	    return str_;
	}
	int getValue() const {
	    return value_;
	}

	bits32 getStillPossible() {
	    return stillPossible_;
	}
	void setStillPossible(bits32 v) {
	    stillPossible_ = v;
	}

	bits32 getCol() {
	    return col_;
	}
    };


    int size_;
    Item list_[Global::Perm::listSize];
    bits32 curCol_; ///< used during filling the list

    void calcStillPossible() {
	bits32 n = 0;
	for(int i = getSize()-1; i>=0; --i) {
	    n |= at(i).getCol();
	    at(i).setStillPossible(n);
	}
    }


 public:
    List() {
	reset();
    }

    Item& operator[](int i) {
	assert(i < size_);
	return list_[i];
    }

    Item& at(int i) {
	assert(i < size_);
	return list_[i];
    }
    const Item& at(int i) const {
	assert(i < size_);
	return list_[i];
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


    void setCurCol(int col) {
	curCol_ = 1<<col;
    }

    bool push(const uchar* str, int value) {
	if(size_ >= Global::Perm::listSize) {
	    return false;
	}
	at(size_++).set(str, value, curCol_);
	return true;
    }

    void reset() {
	size_ = 0;
    }

    void List::sort() {
	sort(0,getSize() - 1); // private method
    }


 private:
    // quicksort: see Cormen, Introduction to algorithms, p.145ff
    void List::sort(int left, int right) {
	int q;
	if(left<right) {
	    q = partition(left,right);
	    sort(left,q-1);
	    sort(q+1,right);
	}
    }

    // for quicksort
    int List::partition(int left,int right) {
	int i = left -1;
	int j;
	for(j=left;j<right;++j) {
	    if((at(j).getValue() < at(right).getValue()) || // let freq() decide
	       ((at(j).getValue() == at(right).getValue())  // or if freqs are equal
		&& (strcmp((char*)at(j).getStr(), (char*)at(right).getStr()) < 0))) { // let alphab. order decide (standard strcmp fubnction, not the one implemented in Alphabet)
		++i;
		swap(i,j);
	    }
	}
	swap(i+1,right);
	return (i+1);
    }

    void List::swap(int i, int j) {
	Item tmp = at(i);
	at(i) = at(j);
	at(j) = tmp;
    }

 public:
    void printList()  {
	printf("str\tcol\tval\tsp\n");
	for(int i=0;i<getSize();++i) {
	    printf("%s\t%d\t%d\t%d\n",at(i).getStr(),at(i).getCol(),at(i).getValue(),at(i).getStillPossible());
	}
    }

};
