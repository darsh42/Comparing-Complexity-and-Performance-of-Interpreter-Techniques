#include "pin.H"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

BOOL g_dispatching_enabled = false;
BOOL g_processing_enabled = false;

UINT64 g_dispatching_count = 0;
UINT64 g_processing_count = 0;

VOID CountInstruction() {
    // increment whenever guest instruction logic is not executing
    if (g_dispatching_enabled && 
       !g_processing_enabled) { 
        g_dispatching_count++; 
    }

    // increment when guest instruction logic is executing
    if (g_processing_enabled) { 
        g_processing_count++; 
    }
}

VOID Instruction(INS ins, VOID *v) {
    /* link function to perform instruction count behaviour */
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)CountInstruction, IARG_END);
}

VOID PrintResults(INT32 code, VOID *v) {
    std::cout << "Data" << ":"
              << g_processing_count << ","
              << g_dispatching_count << ","
              << (double) g_processing_count / g_dispatching_count << std::endl;
#if 0
    std::cout << "\n--- Profiling Results ---" << std::endl;
    std::cout << "Processing  Instructions: " << g_processing_count  << std::endl;
    std::cout << "Dispatching Instructions: " << g_dispatching_count << std::endl;
    std::cout << "Efficiency Ratio:         " 
              << (double) g_processing_count / g_dispatching_count << std::endl;
#endif 
}

/* functions that will replace the stubs */
VOID SetProcessingEnabled (BOOL enabled) { g_processing_enabled = enabled; }
VOID SetDispatchingEnabled(BOOL enabled) { g_dispatching_enabled = enabled; }

VOID HookProfilingFunctions(IMG img, VOID *v) {
    if (!IMG_IsMainExecutable(img)) return;

    // Iterate through all symbols in the image
    for (SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym)) {
        std::string name = SYM_Name(sym);

        // Check for our symbols
        if (name == "ENTER_PROCESSING"  || name == "EXIT_PROCESSING" ||
            name == "ENTER_DISPATCHING" || name == "EXIT_DISPATCHING") {

            // Get the routine associated with this symbol's address
            RTN rtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));

            /* check if the found symbol is valid, then reroute any calls to our profiling enablers */
            if (RTN_Valid(rtn)) {
                RTN_Open(rtn);
                     if (name == "ENTER_PROCESSING")  RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)SetProcessingEnabled, IARG_BOOL, TRUE, IARG_END);
                else if (name == "EXIT_PROCESSING")   RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)SetProcessingEnabled, IARG_BOOL, FALSE, IARG_END);
                else if (name == "ENTER_DISPATCHING") RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)SetDispatchingEnabled, IARG_BOOL, TRUE, IARG_END);
                else if (name == "EXIT_DISPATCHING")  RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)SetDispatchingEnabled, IARG_BOOL, FALSE, IARG_END);
                RTN_Close(rtn);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (PIN_Init(argc, argv)) return -1;

    PIN_InitSymbols();

    IMG_AddInstrumentFunction(HookProfilingFunctions, 0);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(PrintResults, 0);

    PIN_StartProgram();
    return 0;
}
