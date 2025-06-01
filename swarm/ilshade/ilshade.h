#ifndef ILSHADE_H
#define ILSHADE_H

#include <vector>
#include <algorithm>
#include "AlgPrint.h"
#include "Tool.h"
#include "test_function.h"
using namespace std;

class iLshade
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
    double _p;
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
        double _inCR, _inF;
        // double _inP;
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
void iLshade::RunALG(int Run, int Func, int NP, int Fess, int Dim, int Arch, int H, int NPmin)
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
    
    show = AlgPrint(_Run, "./result", "iLshade");
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
        case 7:
            FileName = "Schwefel";
            break;
        case 8:
            FileName = "BentCigar";
            break;
        case 9:
            FileName = "DropWave";
            break;
        case 10:
            FileName = "Step";
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

void iLshade::Init()
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
        _HS[i]._MCR = 0.8;
        _HS[i]._MF = 0.8;
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
        _FessNow++;
        _X[i]._index = i;
        _X[i]._inCR = _X[i]._inF = 0;

        
        if (_X[i]._fitness < _Best)
            _Best = _X[i]._fitness;

    }
    // init var
    _U._position.assign(dim, 0);
    _V._position.assign(dim, 0);
    _U._fitness = _V._fitness = DBL_MAX;
}

void iLshade::Evaluation()
{
    while (_FessNow < _Fess)
    {
        _p = ((0.2 - 0.1) / _Fess) * _FessNow + 0.1;;
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
            // if (index == _H - 1) {
            //     _HS[index]._MCR = 0.9;
            //     _HS[index]._MF = 0.9;
            // }
            double mcr = (index == _H - 1) ? 0.9 : _HS[index]._MCR;
            double mf  = (index == _H - 1) ? 0.9 : _HS[index]._MF;


            if (_HS[index].EndFlag) // new: EndFlag For CRi
            {
                _X[i]._inCR = 0;
            }
            else
            {
                _X[i]._inCR = tool.rand_normal(mcr, 0.1);
                if (_X[i]._inCR > 1)
                {
                    _X[i]._inCR = 1;
                }
                else if (_X[i]._inCR < 0)
                {
                    _X[i]._inCR = 0;
                }
            }

            double genRatio = (double)_FessNow / _Fess;
            if (genRatio < 0.25)
                _X[i]._inCR = max(_X[i]._inCR, 0.5);
            else if (genRatio < 0.5)
                _X[i]._inCR = max(_X[i]._inCR, 0.25);


            do
            {
                _X[i]._inF = tool.rand_cauchy(mf, 0.1);
                if (_X[i]._inF >= 1)
                {
                    _X[i]._inF = 1;
                }
            } while (_X[i]._inF <= 0);

            if (genRatio < 0.25)
                _X[i]._inF = min(_X[i]._inF, 0.7);
            else if (genRatio < 0.5)
                _X[i]._inF = min(_X[i]._inF, 0.8);
            else if (genRatio < 0.75)
                _X[i]._inF = min(_X[i]._inF, 0.9);


            
            // if (2.0/_NPnow > 0.2){
            //     _X[i]._inP = 0.2;
            // }
            // else {
            //     _X[i]._inP = tool.rand_double(2.0/_NPnow, 0.2);
            // }
            
            // Random choose three place to mutation
            int best, r1, r2, flag = 0;
            // best = selectTopPBest(_X, _X[i]._inP);
            best = selectTopPBest(_X, _p);
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

            if (r2 >= _NPnow && flag == 0) {
                r2 -= _NPnow;
                flag = 1;
            }

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
            _FessNow++;
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
            bool allZeroCR = true;
            for (double cr : _SCR)
            {
                if (cr > 1e-8) {
                    allZeroCR = false;
                    break;
                }
            }
            if (_HS[_k].EndFlag || allZeroCR)
            {
                _HS[_k].EndFlag = true;
            }
            else
            {
                for (int t = 0; t < _SCR.size(); t++)
                {
                    // mean weight Scr
                    mCR += (deltaF[t] / WKdenominator) * _SCR[t];
                }
                // _HS[_k]._MCR = mCR;
                _HS[_k]._MCR = (mCR + _HS[_k]._MCR) / 2;
            }

            // Update MF
            for (int t = 0; t < _SCR.size(); t++)
            {
                // Lehmer mean
                numerator += (deltaF[t] / WKdenominator) * _SF[t] * _SF[t];
                denominator += (deltaF[t] / WKdenominator) * _SF[t];
            }
            mF = numerator / denominator;
            // _HS[_k]._MF = mF;
            _HS[_k]._MF = (mF + _HS[_k]._MF) / 2;

            _k++;
            if (_k == _H)
                _k = 0;
        }

        // new: Population Reduction, Update NPnow
        int _NPnext = (int) round((((_NPmin - _NP) / (double)_Fess) *  (double)_FessNow) + _NP );

        if (_NPnext != _NPnow){
            if (_NPnext < _NPmin)
                _NPnext = _NPmin;
            _NPnow = _NPnext;
            _Arch = _NPnow;
            sort(_X.begin(), _X.end(), compareFitness);
            _X.assign(_X.begin(), _X.begin() + _NPnow);
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
        
        // cout << "_NPnow: " << _NPnow << " _FessNow: " << _FessNow << " Best: " << _Best << endl;
    }
}

void iLshade::Reset()
{
    _Best = DBL_MAX;
    _FessNow = 0;
    _NPnow = _NP;
    _k = 0;

    _X.clear();
    _A.clear();
    _HS.clear();
    _SF.clear();
    _SCR.clear();
    _U._position.clear();
    _V._position.clear();
}

void iLshade::CheckBorder(_Particle &check, _Particle &old)
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

bool iLshade::compareFitness(const _Particle &a, const _Particle &b)
{
    return a._fitness < b._fitness;
}

// int iLshade::selectTopPBest(vector<_Particle> X, double p)
// {
//     vector<_Particle> tmp = X;
//     sort(tmp.begin(), tmp.end(), compareFitness);
//     int place;
//     place = p * _NPnow;
//     place = tool.rand_int(0, place);

//     return tmp[place]._index;
// }

int iLshade::selectTopPBest(vector<_Particle> X, double p)
{
    vector<_Particle> tmp = X;
    sort(tmp.begin(), tmp.end(), compareFitness);

    int p_size = (int)round(p * _NPnow);
    if (p_size < 2) p_size = 2;  // 至少保留前2名以避免只選到第一名

    int place = tool.rand_int(0, p_size - 1);
    return tmp[place]._index;
}


#endif