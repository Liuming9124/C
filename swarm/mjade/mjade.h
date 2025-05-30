#ifndef MJADE_H
#define MJADE_H

#include <queue>
#include <vector>
#include <random>
#include <algorithm>
#include <cfloat>
#include "Tool.h"
#include "test_function.h"
using namespace std;

class mJade
{
public:
    typedef struct Particle
    {
        vector<double> _position;
        double _inCR, _inF;
        double _fitness;
        int _index;
    } _Particle;
    // new
    void InitSwarm(int, int, int, int, int, int, double, double);
    void Step();
    double GetBestFitness() const { return _Best; }
    Particle GetBestParticle();
    void Inject(const Particle& p);
    int GetFEs() const { return _nFess; }
    bool IsFinished() const {
        return _nFess >= _mFess;
    }
    std::vector<Particle> GetTopKParticles(int k) const;
    double DistanceToSwarm(const Particle& p) const;

private:
    int _Run;
    int _NP;
    int _Arch;
    double _mCR;
    double _mF;
    vector<double> _SF, _SCR;
    int _Dim;
    double _P;
    double _C;
    int _nFess, _mFess;
    int _Func;
    double _upperBound, _lowerBound;
    double _Best;

    _Particle _U, _V;
    vector<_Particle> _X, _A;

    int selectTopPBest(vector<_Particle>, double);
    void CheckBorder(_Particle &, _Particle &);
    static bool compareFitness(const _Particle &, const _Particle &);

    Tool tool;
};

void mJade::CheckBorder(_Particle &check, _Particle &old)
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

bool mJade::compareFitness(const _Particle &a, const _Particle &b)
{
    return a._fitness < b._fitness;
}

int mJade::selectTopPBest(vector<_Particle> X, double p)
{
    vector<_Particle> tmp = X;
    sort(tmp.begin(), tmp.end(), compareFitness);
    int place;
    place = p * _NP;
    place = tool.rand_int(0, place);
    return tmp[place]._index;
}

void mJade::InitSwarm(int Run, int Func, int NP, int Fess, int Dim, int Arch, double P, double C) {
    _Run = Run;
    _NP = NP;
    _Func = Func;
    _mFess = Fess;
    _Dim = Dim;
    _P = P;
    _C = C;
    _Arch = Arch != 0 ? Arch : NP;
    _Best = DBL_MAX;
    _nFess = 0;
    _mCR = _mF = 0.5;
    _A.clear();
    _X.resize(_NP, _Particle());

    _U._position.assign(_Dim, 0.0);
    _V._position.assign(_Dim, 0.0);
    _U._fitness = _V._fitness = DBL_MAX;


    set_search_bound(&_upperBound, &_lowerBound, _Func);

    for (int i = 0; i < _NP; i++) {
        _X[i]._position.resize(_Dim);
        for (int j = 0; j < _Dim; j++)
            _X[i]._position[j] = tool.rand_double(_lowerBound, _upperBound);

        _X[i]._fitness = calculate_test_function(&_X[i]._position[0], _Dim, _Func);
        _nFess++;
        _X[i]._index = i;
        _X[i]._inCR = _X[i]._inF = 0;

        _Best = min(_Best, _X[i]._fitness);
    }
}

void mJade::Inject(const Particle& p) {
    // 隨機替換一個最差的
    auto worstIt = max_element(_X.begin(), _X.end(), compareFitness);
    *worstIt = p;
}

mJade::Particle mJade::GetBestParticle() {
    auto bestIt = min_element(_X.begin(), _X.end(), compareFitness);
    return *bestIt;
}

void mJade::Step() {
    // if (_nFess % 1000 == 0) {
    //     cout << "[Swarm] FEs: " << _nFess << ", Best: " << _Best << endl;
    // }

    if (_nFess >= _mFess) return;

    _SCR.clear();
    _SF.clear();

    for (int i = 0; i < _NP && _nFess < _mFess; i++) {
        // 產生 CR, F
        _X[i]._inCR = tool.rand_normal(_mCR, 0.1);
        _X[i]._inCR = min(max(_X[i]._inCR, 0.0), 1.0);

        do {
            _X[i]._inF = tool.rand_cauchy(_mF, 0.1);
        } while (_X[i]._inF <= 0.0);
        if (_X[i]._inF > 1.0) _X[i]._inF = 1.0;

        // mutation parents
        int best = selectTopPBest(_X, _P);
        int r1, r2, flag;

        do { r1 = tool.rand_int(0, _NP - 1); } while (r1 == i);

        while (true) {
            int randIndex = tool.rand_int(0, _NP + _A.size() - 1);
            if (randIndex < _NP) {
                if (randIndex == i || randIndex == r1) continue;
                r2 = randIndex;
                flag = 0;
                break;
            } else {
                int archiveIndex = randIndex - _NP;
                bool similar = true;
                for (int j = 0; j < _Dim; j++) {
                    if (_A[archiveIndex]._position[j] != _X[i]._position[j] &&
                        _A[archiveIndex]._position[j] != _X[r1]._position[j]) {
                        similar = false;
                        break;
                    }
                }
                if (!similar) {
                    r2 = archiveIndex;
                    flag = 1;
                    break;
                }
            }
        }

        // mutation
        for (int j = 0; j < _Dim; j++) {
            double F = _X[i]._inF;
            if (flag == 0) {
                _V._position[j] = _X[i]._position[j]
                    + F * (_X[best]._position[j] - _X[i]._position[j])
                    + F * (_X[r1]._position[j] - _X[r2]._position[j]);
            } else {
                _V._position[j] = _X[i]._position[j]
                    + F * (_X[best]._position[j] - _X[i]._position[j])
                    + F * (_X[r1]._position[j] - _A[r2]._position[j]);
            }
        }
        CheckBorder(_V, _X[i]);

        // crossover
        int jrand = tool.rand_int(0, _Dim - 1);
        for (int j = 0; j < _Dim; j++) {
            if (j == jrand || tool.rand_double(0, 1) < _X[i]._inCR) {
                _U._position[j] = _V._position[j];
            } else {
                _U._position[j] = _X[i]._position[j];
            }
        }

        // selection
        _U._fitness = calculate_test_function(&_U._position[0], _Dim, _Func);
        _nFess++;

        if (_X[i]._fitness > _U._fitness) {
            _A.push_back(_X[i]);
            _X[i]._position = _U._position;
            _X[i]._fitness = _U._fitness;
            _SCR.push_back(_X[i]._inCR);
            _SF.push_back(_X[i]._inF);
            if (_X[i]._fitness < _Best) _Best = _X[i]._fitness;
        }
    }

    while (_A.size() > _Arch) {
        int remove = tool.rand_int(0, _A.size() - 1);
        _A.erase(_A.begin() + remove);
    }

    if (!_SCR.empty() && !_SF.empty()) {
        double meanScr = accumulate(_SCR.begin(), _SCR.end(), 0.0) / _SCR.size();
        double numerator = 0, denominator = 0;
        for (double f : _SF) {
            numerator += f * f;
            denominator += f;
        }
        double meanF = numerator / denominator;
        _mCR = (1 - _C) * _mCR + _C * meanScr;
        _mF = (1 - _C) * _mF + _C * meanF;
    }
}

std::vector<mJade::Particle> mJade::GetTopKParticles(int k) const {
    vector<Particle> sorted = _X;
    sort(sorted.begin(), sorted.end(), compareFitness);
    if (k > sorted.size()) k = sorted.size();
    return vector<Particle>(sorted.begin(), sorted.begin() + k);
}

double mJade::DistanceToSwarm(const Particle& p) const {
    double totalDist = 0.0;
    for (const auto& member : _X) {
        double dist = 0.0;
        for (int i = 0; i < _Dim; ++i) {
            double diff = p._position[i] - member._position[i];
            dist += diff * diff;
        }
        totalDist += sqrt(dist);
    }
    return totalDist / _X.size();
}


#endif