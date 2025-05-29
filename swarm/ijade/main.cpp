#include "multi_ijade.h"
#include <sstream>

AlgPrint SwarmManager::show;

int main(int argc, char *argv[]) {
    int numSwarms = 3;
    int totalRun = 30;
    int func = 1;
    int dim = 30;
    int np = 50;
    int arch = 1;
    double p = 0.1;
    double c = 0.1;
    int fess;

    if (argc >= 5) {
        std::stringstream ss;
        ss << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4];
        ss >> totalRun;
        ss >> numSwarms;
        ss >> func;
        ss >> dim;
    } else {
        std::cerr << "Usage: a.exe <totalRun> <func> <dim>" << std::endl;
        return 1;
    }

    fess = dim * 10000;

    // --- Run multiple times ---
    for (int run = 0; run < totalRun; ++run) {
        // std::cout << "=================== Run " << run << " ===================" << std::endl;
        SwarmManager mgr(numSwarms, run, func, np, fess, dim, arch, p, c, totalRun);
        mgr.Run();

        if (run == totalRun - 1) {
            mgr.SaveToFile(totalRun);  // 儲存所有 run 的 global best
        }
    }

    return 0;
}