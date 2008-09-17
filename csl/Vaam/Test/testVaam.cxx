#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "./TestVaam.h"


int main() {
//     // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();



    // Adds the test to the list of tests to run
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( suite );
    
    // Change the default outputter to a compiler error format outputter
    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
							 std::cerr ) );
    // Run the tests.
    bool wasSucessful = runner.run();
    
    // Return error code 1 if one of the tests failed.
    return wasSucessful ? 0 : 1;    

}
