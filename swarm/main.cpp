#include "de.h"
#include <sstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
    int Run, Func, Pop, Fess, Dim;
    double Cr, F;

    // [Run] [Func] [Pop] [FESS] [Dim] [Cr] [Mutation Factor]
    stringstream ss;
    for (int i = 1; i < argc; i++)
        ss << argv[i] << " ";
    ss >> Run;
    ss >> Func;
    ss >> Pop;
    ss >> Dim;
    ss >> Cr;
    ss >> F;
    
    Fess = Dim * 10000;

    De de;
    de.RunALG( Run, Func, Pop, Fess, Dim, Cr,  F);
    return 0;
}