
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "./TestPattern.h"
#include "./TestPatternWeights.h"

int main() {
    std::locale::global( std::locale( "" ) );

    csl::TestPattern tester;

    tester.run();

}
