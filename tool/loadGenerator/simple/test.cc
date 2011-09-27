#include "RandomPulseFunction.h"
#include "BRInputGenerator.cc"
#include "NetworkTrafficQueryParams.h"
using namespace std;
using namespace Borealis;


int main(int argc, const char* argv[])
{
    BRIOParams::get_singleton()->use_tcp_trace = true;
    BRIOParams::get_singleton()->rate_multiplier = 0.001;

    BRInputGenerator input_generator("io.xml");
    input_generator.run();
}
