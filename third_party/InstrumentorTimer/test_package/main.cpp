#include <InstrumentorTimer/InstrumentorTimer.h>
#include <InstrumentorTimer/InstrumentorMacro.h>

int main() {
    Instrumentor::BeginSession("TestSession", "test_output.json");
    {
        PROFILE_SCOPE("main_scope");
    }
    Instrumentor::EndSession();
    return 0;
}