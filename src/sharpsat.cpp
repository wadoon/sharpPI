//
// Created by weigl on 11.11.15.
//

#include "sharpsat.h"


#include <iostream>

#include "dsharp/shared/SomeTime.h"
#include "dsharp/shared/Interface/AnalyzerData.h"
#include "dsharp/src_sharpSAT/MainSolver/MainSolver.h"
#include "dsharp/src_sharpSAT/Basics.h"

using namespace std;



/*
void finalcSATEvaluation()
{
    const AnalyzerData &rAda = theRunAn.getData();

    if (rAda.theExitState == TIMEOUT)
    {
        toSTDOUT(endl << " TIMEOUT !"<<endl);
        return;
    }

    toSTDOUT(endl<<endl);
    toSTDOUT("#Variables:\t\t"<< rAda.nVars<<endl);

    if (rAda.nVars != rAda.nUsedVars)
        toSTDOUT("#used Variables:\t"<< rAda.nUsedVars<<endl);
    toSTDOUT("#Clauses:\t\t"<< rAda.nOriginalClauses<<endl);
    toSTDOUT("#Clauses removed:\t"<< rAda.nRemovedClauses<<endl);
    toSTDOUT("\n#added Clauses: \t"<< rAda.nAddedClauses<<endl);

    bool projection = rememberedVarNums.size() > 0;
    if (!projection)
    {
        toSTDOUT("\n# of all assignments:\t" << rAda.getAllAssignments()
                 << " = 2^(" << rAda.nVars<<")" <<endl);

        toSTDOUT("Pr[satisfaction]:\t" << rAda.rnProbOfSat <<endl);

        toSTDOUT("# of solutions:\t\t" << rAda.getNumSatAssignments() <<endl);
        toSTDOUT("#SAT (full):   \t\t");
        if (!CSolverConf::quiet)
            rAda.printNumSatAss_whole();
        toSTDOUT(endl);
    }

    toDEBUGOUT(".. found in:\t\t" << rAda.nReceivedSatAssignments << " units"<<endl);

    toSTDOUT(endl);

    toSTDOUT("Num. conflicts:\t\t" << rAda.nConflicts<<endl);
    toSTDOUT("Num. implications:\t" << rAda.nImplications<<endl);
    toSTDOUT("Num. decisions:\t\t" << rAda.nDecisions<<endl);
    toSTDOUT("max decision level:\t" << rAda.maxDecLevel<<"\t\t");
    toSTDOUT("avg decision level:\t"<< rAda.get(AVG_DEC_LEV)<<endl);
    toSTDOUT("avg conflict level:\t"<< rAda.get(AVG_CONFLICT_LEV)<<endl);
    toSTDOUT("avg solution level:\t"<< rAda.get(AVG_SOLUTION_LEV)<<endl);

    toSTDOUT("CCLLen 1stUIP - max:\t"<< rAda.get(LONGEST_CCL_1stUIP));
    toSTDOUT("\t avg:\t"<< rAda.get(AVG_CCL_1stUIP)<<endl);
    toSTDOUT("CCLLen lastUIP - max:\t"<< rAda.get(LONGEST_CCL_lastUIP));
    toSTDOUT("\t avg:\t"<< rAda.get(AVG_CCL_lastUIP)<<endl);

    toSTDOUT(endl);
    toSTDOUT("FormulaCache stats:"<<endl);
    toSTDOUT("memUse:\t\t\t"<<rAda.get(FCACHE_MEMUSE) <<endl);
    toSTDOUT("cached:\t\t\t"<<rAda.get(FCACHE_CACHEDCOMPS)<<endl);
    toSTDOUT("used Buckets:\t\t"<<rAda.get(FCACHE_USEDBUCKETS)<<endl);
    toSTDOUT("cache retrievals:\t"<<rAda.get(FCACHE_RETRIEVALS)<<endl);
    toSTDOUT("cache tries:\t\t"<<rAda.get(FCACHE_INCLUDETRIES)<<endl);

    toSTDOUT("\n\nTime: "<<rAda.elapsedTime<<"s\n\n");

    cout << "Runtime:" << rAda.elapsedTime << endl;

}
*/

uint64_t sharpSAT_dsharp(const char* filename) {
    CMainSolver theSolver;

    auto dataFile = "data.txt";
    bool fileout = false;
    auto graphFile = "bdg.txt";
    bool graphFileout = false;
    auto nnfFile = "nnf.txt";
    bool nnfFileout = false;

    //Dimitar Shterionov:
    bool smoothNNF = false;

    CSolverConf::analyzeConflicts = true;
    CSolverConf::doNonChronBackTracking = true;
    CSolverConf::nodeCount = 0;

    /*
        cout << "Usage: dsharp [options] [CNF_File]" << endl;
        cout << "Options: " << endl;
        cout << "\t -noPP  \t\t turn off preprocessing" << endl;
        cout << "\t -noCA  \t\t turn off conflict analysis" << endl;
        cout << "\t -noCC  \t\t turn off component caching" << endl;
        cout << "\t -noNCB \t\t turn off nonchronological backtracking" << endl;
        cout << "\t -noIBCP\t\t turn off implicit BCP" << endl;
        cout << "\t -noDynDecomp\t\t turn off dynamic decomposition" << endl;
        cout << "\t -q     \t\t quiet mode" << endl;
        cout << "\t -t [s] \t\t set time bound to s seconds" << endl;
        cout << "\t -cs [n]\t\t set max cache size to n MB" << endl;
        cout << "\t -FrA [file] \t\t file to output the run statistics" << endl;
        cout << "\t -Fgraph [file] \t file to output the backdoor or d-DNNF graph" << endl;
        cout << "\t -Fnnf [file] \t\t file to output the nnf graph to" << endl;

        //Dimitar Shterionov:
        cout << "\t -smoothNNF \t\t post processing to smoothed d-DNNF" << endl;

        cout << "\t -disableAllLits \t when producing a smooth d-DNNF, don't bother enforcing every literal" << endl;
    }
    */

    // "-noNCB"
    CSolverConf::doNonChronBackTracking = false;
    // "-noCC"
    CSolverConf::allowComponentCaching = false;
    // "-noIBCP"
    CSolverConf::allowImplicitBCP = true;

    //Dimitar Shterionov:
    // "-smoothNNF"
    CSolverConf::smoothNNF = true;

    // "-disableAllLits"
    CSolverConf::ensureAllLits = false;

    // "-noDynDecomp"
    CSolverConf::disableDynamicDecomp = true;

    // "-noPP"
    CSolverConf::allowPreProcessing = false;

    // "-noCA"
    CSolverConf::analyzeConflicts = false;

    // "-q"
    CSolverConf::quietMode = true;

    // -FrA
    /*        memset(dataFile, 0, 1024);
        fileout = true;
        if (argc <= i + 1)
        {
            toSTDOUT("wrong parameters"<<endl);
            return -1;
        }
        strcpy(dataFile, argv[i + 1]);
    */

    // "-Fgraph")
    /*{
        memset(graphFile, 0, 1024);
        graphFileout = true;
        if (argc <= i + 1)
        {
            toSTDOUT("wrong parameters"<<endl);
            return -1;
        }
        strcpy(graphFile, argv[i + 1]);
    }*/
    //"-Fnnf"
    /*
    {
        memset(nnfFile, 0, 1024);
        nnfFileout = true;
        if (argc <= i + 1)
        {
            toSTDOUT("wrong parameters"<<endl);
            return -1;
        }
        strcpy(nnfFile, argv[i + 1]);
    }*/
    // "-t"
    /*{
        if (argc <= i + 1)
        {
            toSTDOUT("wrong parameters"<<endl);
            return -1;
        }
        CSolverConf::secsTimeBound = atoi(argv[i + 1]);
        toSTDOUT("time bound:" <<CSolverConf::secsTimeBound<<"s\n");
        theSolver.setTimeBound(CSolverConf::secsTimeBound);
    }*/
    //"-cs"
    /*{
        if (argc <= i + 1)
        {
            toSTDOUT("wrong parameters"<<endl);
            return -1;
        }
        CSolverConf::maxCacheSize = atoi(argv[i + 1]) * 1024 * 1024;
        //cout <<"maxCacheSize:" <<CSolverConf::maxCacheSize<<"bytes\n";
    }*/

    toSTDOUT("cachesize Max:\t" << CSolverConf::maxCacheSize / 1024 << " kbytes" << endl);

    // first: delete all data in the output
    if (fileout)
        theRunAn.getData().writeToFile(dataFile);

    theRunAn = CRunAnalyzer();
    auto model_count = theSolver.solve(filename);

    const AnalyzerData &rAda = theRunAn.getData();

    //theRunAn.finishcountSATAnalysis();
    //finalcSATEvaluation();

    if (fileout)
        theRunAn.getData().writeToFile(dataFile);

    if (graphFileout)
        theSolver.writeBDG(graphFile);

    if (nnfFileout)
        theSolver.writeNNF(nnfFile);

    return model_count;
}
