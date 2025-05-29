#ifndef MULTI_IJADE_H
#define MULTI_IJADE_H

#include "ijade.h"
#include <vector>
#include <iostream>
#include <limits>
#include "AlgPrint.h"

using namespace std;

class SwarmManager {
public:
    static AlgPrint show;


    SwarmManager(int numSwarms, int run, int func, int np, int fess, int dim, int arch, double p, double c, int totalRun)
        : _numSwarms(numSwarms), _Run(run), _Func(func), _NP(np), _Fess(fess), _Dim(dim), _Arch(arch), _P(p), _C(c), _TotalRun(totalRun)
    {
        if (_Run == 0) {
            show = AlgPrint(_TotalRun, "./result", "multi_ijade");
            show.NewShowDataDouble(1);
        }

        swarms.assign(numSwarms, iJade());
        for (int i = 0; i < numSwarms; i++) {
            swarms[i].InitSwarm(run, _Func, np, fess / numSwarms, dim, arch, p, c);
        }
    }


    void Run() {
        cout << scientific << setprecision(8);
        int migrationInterval = 1000;
        int lastMigrated = 0;

        while (true) {
            // Step all unfinished swarms
            for (int i = 0; i < _numSwarms; ++i) {
                if (!swarms[i].IsFinished())
                    swarms[i].Step();
            }

            // Check if all are finished
            bool allFinished = true;
            for (int i = 0; i < _numSwarms; ++i) {
                if (!swarms[i].IsFinished()) {
                    allFinished = false;
                    break;
                }
            }
            if (allFinished) break;

            // Recompute totalFEs
            int totalFEs = 0;
            for (int i = 0; i < _numSwarms; ++i)
                totalFEs += swarms[i].GetFEs();

            // Migration
            if (totalFEs - lastMigrated >= migrationInterval) {
                Migrate();
                lastMigrated = totalFEs;
            }

            // Optional: Logging
            if (totalFEs % 1000 < _numSwarms) {
                // cout << "[Manager] Total FEs: " << totalFEs << endl;
            }
        }

        // Final best
        double globalBest = numeric_limits<double>::max();
        for (auto &swarm : swarms)
            globalBest = min(globalBest, swarm.GetBestFitness());

        // cout << "[MultiSwarm] Run " << _Run << " Global Best: " << globalBest << endl;
        show.SetDataDouble(_Run, globalBest, 0);
    }

    void SaveToFile(int totalRun) {
        string FileName;
        switch (_Func)
        {
            case 1: FileName = "Ackley"; break;
            case 2: FileName = "Rastrigin"; break;
            case 3: FileName = "HappyCat"; break;
            case 4: FileName = "Rosenbrock"; break;
            case 5: FileName = "Zakharov"; break;
            case 6: FileName = "Michalewicz"; break;
            case 7: FileName = "Schwefel"; break;
            case 8: FileName = "BentCigar"; break;
            case 9: FileName = "DropWave"; break;
            case 10: FileName = "Step"; break;
            default: FileName = "UnknownFunc"; break;
        }

        string path = "./result/" + FileName + to_string(_Dim) + "D.txt";
        show.PrintToFileDouble(path, totalRun);
    }

private:
    int _numSwarms;
    int _Run, _Func, _NP, _Fess, _Dim, _Arch, _TotalRun;
    double _P, _C;
    vector<iJade> swarms;

    void Migrate() {
        // Best-to-Worst migration
        for (int i = 0; i < _numSwarms; ++i) {
            int next = (i + 1) % _numSwarms;
            iJade::Particle best = swarms[i].GetBestParticle();
            swarms[next].Inject(best);
        }
    }
};

#endif
