#ifndef DE_H
#define DE_H

#include <vector>
#include "AlgPrint.h"
#include "Tool.h"
#include "test_function.h"
using namespace std;

class De
{
public:
    typedef struct Particle
    {
        vector<double> _position;
        double _fitness;
    } _Particle;

    void RunALG(int, int, int, int, int, double, double);

private:
    int _Pop;
    int _Fess;
    int _Run;
    int _Iter;
    int _Dim;
    int _Func;
    double _Cr;
    double _F;
    double _upperBound, _lowerBound;

    _Particle _Offspring;
    vector<_Particle> _Swarm;

    void Init();
    void Evaluation();
    void Reset();

    void CheckBorder(_Particle &);

    AlgPrint show;
    Tool tool;
};

void De::RunALG(int Run, int Func, int Pop, int Fess, int Dim, double Cr, double F)
{

    _Run = Run;
    _Func = Func;
    _Pop = Pop;
    _Fess = Fess;
    _Iter = _Fess / _Pop;
    _Dim = Dim;
    _Cr = Cr;
    _F = F;

    cout << scientific << setprecision(8);

    show = AlgPrint(_Run, "./result", "de");
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
    show.PrintToFileDouble("./result/" + FileName + to_string(_Dim) + "D.txt", _Iter);
    cout << "end" << endl;
}

void De::Init()
{
    show.init();
    _Swarm.resize(_Pop);
    _Offspring._position.resize(_Dim);
    for (int i = 0; i < _Dim; i++)
    {
        _Offspring._position[i] = 0;
    }
    for (int i = 0; i < _Pop; i++)
    {
        _Swarm[i]._position.resize(_Dim);
        for (int j = 0; j < _Dim; j++)
        {
            _Swarm[i]._position[j] = tool.rand_double(_lowerBound, _upperBound);
        }
        _Swarm[i]._fitness = calculate_test_function(&_Swarm[i]._position[0], _Dim, _Func);
    }
}

void De::Evaluation()
{
    double best = 0.0;
    for (int iter = 0; iter < _Iter; iter++)
    {
        for (int i = 0; i < _Pop; i++)
        {
            // get random three place to mutaion
            int a, b, c;
            do
            {
                a = tool.rand_int(0, _Pop - 1);
            } while (a == i);
            do
            {
                b = tool.rand_int(0, _Pop - 1);
            } while (b == i || b == a);
            do
            {
                c = tool.rand_int(0, _Pop - 1);
            } while (c == b || c == a || c == i);

            // mutation & check boundary & crossover
            int Jrand = tool.rand_int(0, _Dim - 1);
            for (int j = 0; j < _Dim; j++)
            {
                if (tool.rand_double(0, 1) < _Cr || j == Jrand)
                {
                    _Offspring._position[j] = _Swarm[c]._position[j] + _F * (_Swarm[a]._position[j] - _Swarm[b]._position[j]);
                }
                else
                {
                    _Offspring._position[j] = _Swarm[i]._position[j];
                }
                CheckBorder(_Offspring);
            }
            // selection
            _Offspring._fitness = calculate_test_function(&_Offspring._position[0], _Dim, _Func);
            if (_Offspring._fitness < _Swarm[i]._fitness)
            {
                _Swarm[i] = _Offspring;
            }
        }
        // show data
        best = _Swarm[0]._fitness;
        for (int p = 1; p < _Pop; p++)
        {
            if (best > _Swarm[p]._fitness)
                best = _Swarm[p]._fitness;
        }
    }
    show.SetDataDouble(_Run, best, 0);
    cout << best << endl;
}

void De::Reset()
{
    _Offspring._fitness = 0;
}

void De::CheckBorder(_Particle & check)
{
    for (int i = 0; i < _Dim; i++)
    {
        if (check._position[i] < _lowerBound || check._position[i] > _upperBound)
        {
            check._position[i] = tool.rand_double(_lowerBound, _upperBound);
        }
    }
}


#endif