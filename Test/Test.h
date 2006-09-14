

namespace csl {

    class Test {
    public:
	Test( const std::string& name ) : name_( name ) {}
	virtual ~Test() {}
	virtual bool run() = 0;
	
    private:
	std::string name_;
    };


} // namespace csl
