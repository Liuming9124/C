#ifndef JADET_H
#define JADET_H

#include "AlgPrint.h"
#include "Tool.h"
#include <queue>
#include <vector>
#include <random>
#include <algorithm>
#include <cfloat>
#include "test_function.h"
using namespace std;

class Jade
{
public:

    void RunALG(int, int, int, int, int, int, double, double);

private:
    int _Run;
    int _NP;
    int _Gen;
    int _Arch;
    double _mCR;
    double _mF;
    vector<double> _SF, _SCR;
    int _Dim;
    int _Func;
    double _P;
    double _C;
    int _nFess, _mFess;
    double _upperBound, _lowerBound;
    double _Best;

    typedef struct Particle
    {
        vector<double> _position;
        double _inCR, _inF;
        double _fitness;
        int _index;
    } _Particle;
    _Particle _U, _V;
    vector<_Particle> _X, _A;

    void Init();
    void Evaluation();
    void Reset();

    int selectTopPBest(vector<_Particle>, double);
    void CheckBorder(_Particle &, _Particle &);
    static bool compareFitness(const _Particle &, const _Particle &);

    AlgPrint show;
    Tool tool;
};

void Jade::RunALG(int Run, int Func, int NP, int Fess, int Dim, int Arch, double P, double C)
{
    _Run = Run;
    _NP = NP;
    _Func = Func;
    _mFess = Fess;
    _Dim = Dim;
    _P = P;
    _C = C;
    _Arch = 0;
    if (Arch!=0){
        _Arch = _NP;
    }
    show = AlgPrint(_Run, "./result", "jade");
    show.NewShowDataDouble(1);

    set_search_bound(&_upperBound, &_lowerBound, _Func);
    string FileName;
    switch (_Func)
    {
        case 1:
            FileName = "Ackley";
            break;
        case 2:
            FileName = "Rastrigin";
            break;

        case 3:
            FileName = "HappyCat";
            break;

        case 4:
            FileName = "Rosenbrock";
            break;

        case 5:
            FileName = "Zakharov";
            break;

        case 6:
            FileName = "Michalewicz";
            break;
        default:
            printf("Invalid function number");
            break;
    }


    while (_Run--)
    {
        cout << "-------------------Run" << Run - _Run << "---------------------" << endl;
        Init();
        Evaluation();
        Reset();
    }
    show.PrintToFileDouble("./result/" + FileName + to_string(_Dim) + "D.txt", Run);
    cout << "end" << endl;
}

void Jade::Init()
{
    show.init();
    _mCR = 0.5;
    _mF = 0.5;
    _A.resize(0);
    _X.resize(_NP);
    _Best = DBL_MAX;
    _nFess = 0;

    int dim = _Dim;
    // random init _X
    for (int i = 0; i < _NP; i++)
    {
        _X[i]._position.resize(dim);
        for (int j = 0; j < dim; j++)
        {
            _X[i]._position[j] = tool.rand_double( _lowerBound, _upperBound);
        }
        _X[i]._fitness = calculate_test_function(&_X[i]._position[0], _Dim, _Func);
        _nFess++;
        _X[i]._index = i;
        _X[i]._inCR = _X[i]._inF = 0;
        
        if (_X[i]._fitness < _Best)
            _Best = _X[i]._fitness;
    }
    // init var
    _U._position.resize(dim);
    _V._position.resize(dim);
    _U._fitness = _V._fitness = DBL_MAX;
    _SCR.resize(_NP);
    _SF.resize(_NP);
}

void Jade::Evaluation()
{
    while(_nFess < _mFess)
    {
        _SCR.clear();
        _SF.clear();
        for (int i = 0; i < _NP; i++)
        {
            // init CR & F
            _X[i]._inCR = tool.rand_normal(_mCR, 0.1);
            if (_X[i]._inCR>1){
                _X[i]._inCR = 1;
            }
            else if (_X[i]._inCR<0){
                _X[i]._inCR = 0;
            }
            do
            {
                _X[i]._inF = tool.rand_cauchy(_mF, 0.1);
                if (_X[i]._inF >=1)
                {
                    _X[i]._inF = 1;
                }
            } while (_X[i]._inF <= 0);
            
            // Random choose three place to mutation
            int best, r1, r2, flag = 0;
            best = selectTopPBest(_X, _P);
            do
            {
                r1 = tool.rand_int(0, _NP - 1);
            } while (r1 == i);
            do
            {
                r2 = tool.rand_int(0, _NP + _A.size() - 1);
                if (r2 >= _NP)
                {
                    bool checkFlag = 0;
                    for (int j = 0; j < _Dim; j++)
                    {
                        if (_A[r2 - _NP]._position[j] != _X[i]._position[j])
                        {
                            checkFlag = 1;
                            break;
                        }
                    }
                    for (int j = 0; j < _Dim; j++)
                    {
                        if (_A[r2 - _NP]._position[j] != _X[r1]._position[j])
                        {
                            checkFlag = 1;
                            break;
                        }
                    }
                    if (checkFlag == 0)
                    {
                        continue;
                    }
                    else
                    {
                        r2 -= _NP;
                        flag = 1;
                        break;
                    }
                }
            } while (r2 == i || r2 == r1);

            // mutation & check boundary
            for (int j = 0; j < _Dim; j++)
            {
                double F = _X[i]._inF;
                if (flag == 0)
                {
                    _V._position[j] = _X[i]._position[j] + F * (_X[best]._position[j] - _X[i]._position[j]) + F * (_X[r1]._position[j] - _X[r2]._position[j]);
                }
                else
                {
                    _V._position[j] = _X[i]._position[j] + F * (_X[best]._position[j] - _X[i]._position[j]) + F * (_X[r1]._position[j] - _A[r2]._position[j]);
                }
                CheckBorder(_V, _X[i]);
            }
            
            // crossover
            int jrand = tool.rand_int(0, _Dim - 1);
            for (int j = 0; j < _Dim; j++)
            {
                if (j == jrand || tool.rand_double(0, 1) < _X[i]._inCR)
                {
                    _U._position[j] = _V._position[j];
                }
                else
                {
                    _U._position[j] = _X[i]._position[j];
                }
            }

            // Selection
            _U._fitness = calculate_test_function(&_U._position[0], _Dim, _Func);
            _nFess++;
            if (_X[i]._fitness > _U._fitness)
            {
                _A.push_back(_X[i]);
                _X[i]._position = _U._position;
                _X[i]._fitness = _U._fitness;
                _SCR.push_back(_X[i]._inCR);
                _SF.push_back(_X[i]._inF);
            }
        }

        while (_A.size() > _Arch)
        {
            // randomly remove one element from A
            int remove = tool.rand_int(0, _A.size() - 1);
            _A.erase(_A.begin() + remove);
        }

        if (_SCR.size() != 0 && _SF.size() != 0){
            // mean Scr
            double meanScr = 0;
            for (int t = 0; t < _SCR.size(); t++)
            {
                meanScr += _SCR[t];
            }
            meanScr /= _SCR.size();

            // Lehmer mean
            double meanF, numerator, denominator;
            meanF = numerator = denominator = 0;
            for (int t = 0; t < _SF.size(); t++) {
                numerator += _SF[t] * _SF[t];
                denominator += _SF[t];
            }
            meanF = numerator / denominator;

            // update mCR & mF
            _mCR = (1 - _C) * _mCR + _C * meanScr;
            _mF = (1 - _C) * _mF + _C * meanF;
        }
        
        // show data
        for (int p=1; p<_NP; p++){
            if (_Best > _X[p]._fitness)
                _Best = _X[p]._fitness;
        }
        show.SetDataDouble(_Run, _Best, 0);;
        cout << "nfess: " << _nFess << ", Best: " << _Best << endl;
    }
}

void Jade::Reset()
{
    _X.clear();
    _A.clear();
    _U._position.clear();
    _V._position.clear();
}

void Jade::CheckBorder(_Particle &check, _Particle &old)
{
    for (int i = 0; i < _Dim; i++)
    {
        if (check._position[i] < _lowerBound)
        {
            check._position[i] = (_lowerBound + old._position[i]) / 2;
        }
        if (check._position[i] > _upperBound)
        {
            check._position[i] = (_upperBound + old._position[i]) / 2;
        }
    }
}

bool Jade::compareFitness(const _Particle &a, const _Particle &b)
{
    return a._fitness < b._fitness;
}

int Jade::selectTopPBest(vector<_Particle> X, double p)
{
    vector<_Particle> tmp = X;
    sort(tmp.begin(), tmp.end(), compareFitness);
    int place;
    place = p * _NP;
    place = tool.rand_int(0, place);
    return tmp[place]._index;
}

#endif