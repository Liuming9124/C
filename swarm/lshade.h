#ifndef LSHADE_H
#define LSHADE_H

#include <vector>
#include <algorithm>
#include "AlgPrint.h"
#include "Tool.h"
#include "test_function.h"
using namespace std;

class Lshade
{
public:
    void RunALG(int, int, int, int, int, int, int, int);

private:
    int _Run;
    int _NP;
    int _Arch;
    int _Dim;
    int _Fess;
    int _H;
    int _NPmin;
    int _NPnow;
    int _k;
    int _FessNow;
    int _Func;
    vector<double> _SF, _SCR;
    double _upperBound, _lowerBound;
    double _Best;


    typedef struct History
    {
        double _MCR, _MF;
        bool EndFlag;
    } _History;
    vector<_History> _HS;

    typedef struct Particle
    {
        vector<double> _position;
        double _inCR, _inF, _inP;
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
void Lshade::RunALG(int Run, int Func, int NP, int Fess, int Dim, int Arch, int H, int NPmin)
{
    _Run = Run;
    _Func = Func;
    _NP = NP;
    _Dim = Dim;
    _Arch = 0;
    _Fess = Fess;
    _H = H;
    _NPmin = NPmin;
    _NPnow = _NP;
    if (Arch != 0)
        _Arch = Arch;
    
    show = AlgPrint(_Run, "./result", "Lshade");
    show.NewShowDataDouble(1);

    set_search_bound(&_upperBound, &_lowerBound, Func);
    string FileName;
    switch (Func)
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

void Lshade::Init()
{
    show.init();
    _A.assign(0, _Particle());
    _X.assign(_NP, _Particle());
    _k = 0;
    _HS.assign(_H, _History());
    _FessNow = 0;
    _Best = DBL_MAX;
    for (int i = 0; i < _H; i++)
    {
        _HS[i]._MCR = 0.5;
        _HS[i]._MF = 0.5;
        _HS[i].EndFlag = 0;
    }

    int dim = _Dim;
    // random init _X
    for (int i = 0; i < _NP; i++)
    {
        _X[i]._position.assign(dim, 0);
        for (int j = 0; j < dim; j++)
        {
            _X[i]._position[j] = tool.rand_double(_lowerBound, _upperBound);
        }
        _X[i]._fitness = calculate_test_function(&_X[i]._position[0], _Dim, _Func);
        _X[i]._index = i;
        _X[i]._inCR = _X[i]._inF = 0;

        
        if (_X[i]._fitness < _Best)
            _Best = _X[i]._fitness;

    }
    // init var
    _U._position.assign(dim, 0);
    _V._position.assign(dim, 0);
    _U._fitness = _V._fitness = 0;
    _SCR.assign(_NP, 0);
    _SF.assign(_NP, 0);
}

void Lshade::Evaluation()
{
    while (_FessNow < _Fess)
    {
        // check in Evaluation times
        if (_FessNow+_NPnow > _Fess)
            break;

        vector<double> deltaF; // to store fitness to calculate mean
        deltaF.clear();
        _SCR.clear();
        _SF.clear();
        for (int i = 0; i < _NPnow; i++)
        {
            // init CR & F & P
            int index = tool.rand_int(0, _H - 1);
            if (_HS[index].EndFlag) // new: EndFlag For CRi
            {
                _X[i]._inCR = 0;
            }
            else
            {
                _X[i]._inCR = tool.rand_normal(_HS[index]._MCR, 0.1);
                if (_X[i]._inCR > 1)
                {
                    _X[i]._inCR = 1;
                }
                else if (_X[i]._inCR < 0)
                {
                    _X[i]._inCR = 0;
                }
            }
            do
            {
                _X[i]._inF = tool.rand_cauchy(_HS[index]._MF, 0.1);
                if (_X[i]._inF >= 1)
                {
                    _X[i]._inF = 1;
                }
            } while (_X[i]._inF <= 0);
            
            if (2.0/_NPnow > 0.2){
                _X[i]._inP = 0.2;
            }
            else {
                _X[i]._inP = tool.rand_double(2.0/_NPnow, 0.2);
            }
            
            // Random choose three place to mutation
            int best, r1, r2, flag = 0;
            best = selectTopPBest(_X, _X[i]._inP);
            do
            {
                r1 = tool.rand_int(0, _NPnow - 1);
            } while (r1 == i);
            do
            {
                r2 = tool.rand_int(0, _NPnow + _A.size() - 1);
                if (r2 >= _NPnow)
                {
                    bool checkFlag = 0;
                    for (int j = 0; j < _Dim; j++)
                    {
                        if (_A[r2 - _NPnow]._position[j] != _X[i]._position[j])
                        {
                            checkFlag = 1;
                            break;
                        }
                    }
                    for (int j = 0; j < _Dim; j++)
                    {
                        if (_A[r2 - _NPnow]._position[j] != _X[r1]._position[j])
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
                        r2 -= _NPnow;
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
            if (_X[i]._fitness >= _U._fitness)
            {
                if (_X[i]._fitness > _U._fitness)
                {
                    _A.push_back(_X[i]);
                    _SCR.push_back(_X[i]._inCR);
                    _SF.push_back(_X[i]._inF);
                    deltaF.push_back(_X[i]._fitness - _U._fitness);
                }
                _X[i]._position = _U._position;
                _X[i]._fitness = _U._fitness;
                if (_X[i]._fitness < _Best)
                    _Best = _X[i]._fitness;
            }
        }

        // Update HS
        if (_SCR.size() != 0 && _SF.size() != 0)
        {
            // prepare param
            double mCR, mF, WKdenominator, numerator, denominator;
            mCR = mF = WKdenominator = numerator = denominator = 0;

            for (int t = 0; t < _SCR.size(); t++)
            {
                WKdenominator += deltaF[t];
            }

            // Update MCR
            double maxSCR = *max_element(_SCR.begin(), _SCR.end());
            if (_HS[_k].EndFlag || maxSCR == 0)
            {
                _HS[_k].EndFlag = 1;
            }
            else
            {
                for (int t = 0; t < _SCR.size(); t++)
                {
                    // mean weight Scr
                    mCR += (deltaF[t] / WKdenominator) * _SCR[t];
                }
                _HS[_k]._MCR = mCR;
            }

            // Update MF
            for (int t = 0; t < _SCR.size(); t++)
            {
                // Lehmer mean
                numerator += (deltaF[t] / WKdenominator) * _SF[t] * _SF[t];
                denominator += (deltaF[t] / WKdenominator) * _SF[t];
            }
            mF = numerator / denominator;
            _HS[_k]._MF = mF;

            _k++;
            if (_k == _H)
                _k = 0;
        }

        // new: Population Reduction, Update NPnow
        _FessNow += _NPnow;
        int _NPnext = (int) round((((_NPmin - _NP) / (double)_Fess) *  (double)_FessNow) + _NP );
        
        if (_NPnext > _NPmin){
            if (_NPnow > _NPnext) {
                _NPnow = _NPnext;
                _Arch = _NPnext;
                sort(_X.begin(), _X.end(), compareFitness);
                _X.assign(_X.begin(), _X.begin() + _NPnow);
            }
        }

        // Resize Archive size
        while (_A.size() > _Arch)
        {
            // randomly remove one element from A
            int remove = tool.rand_int(0, _A.size() - 1);
            _A.erase(_A.begin() + remove);
        }

        // show data
        for (int p = 1; p < _NPnow; p++)
        {
            if (_Best > _X[p]._fitness)
                _Best = _X[p]._fitness;
        }
        
        if (_FessNow+_NPnow <= _Fess)
            show.SetDataDouble(_Run, _Best, 0);
    }
}

void Lshade::Reset()
{
    _X.clear();
    _A.clear();
    _U._position.clear();
    _V._position.clear();
    _Best = DBL_MAX;
    _FessNow = 0;
    _NPnow = _NP;
    _k = 0;
    _SF.clear();
    _SCR.clear();
    _HS.clear();
}

void Lshade::CheckBorder(_Particle &check, _Particle &old)
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

bool Lshade::compareFitness(const _Particle &a, const _Particle &b)
{
    return a._fitness < b._fitness;
}

int Lshade::selectTopPBest(vector<_Particle> X, double p)
{
    vector<_Particle> tmp = X;
    sort(tmp.begin(), tmp.end(), compareFitness);
    int place;
    place = p * _NPnow;
    place = tool.rand_int(0, place);
    return tmp[place]._index;
}

#endif