#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;
using std::setw;

class AlgPrint{
public:
    AlgPrint(){}
    AlgPrint( int run, const string& folder, const string& alg) : _run(run), _folder(folder), _alg(alg){
        _lastPrint = -1;
        _maxPrint = _lastPrint;
    }
    
    void clearResult(const std::string& filename) {
        std::ofstream ofs(filename, std::ofstream::out | std::ofstream::trunc);
    }

    double calculateMean(const std::vector<double>& numbers) {
        if (numbers.empty()) return 0.0;
        double sum = 0.0;
        for(double num : numbers) {
            sum += num;
        }
        return sum / numbers.size();
    }

    template<typename T>
    void NewShowData(vector<vector<T>>& data, int amount){
        data.resize(_run);
        for (int i = 0; i < _run; i++){
            data[i].resize(amount);
        }
        cout << "Data size: " << data.size() << " " << data[0].size() << endl;
    }

    template<typename T>
    void SetData(int run, vector<vector<T>>& data, T num, int iter){
        if (_lastPrint==-1){
            for (int i=0; i <= iter; i++){
                data[run][i] = num;
            }
            _lastPrint = iter;
        }
        else {
            if (_lastPrint==iter){
                data[run][iter] = num;
            }
            else{
                for (int i = _lastPrint+1; i <= iter; i++){
                    data[run][i] = num;
                }
                _lastPrint = iter;
            }
        }
        if (_lastPrint != _maxPrint)
            _maxPrint = _lastPrint;
    }

    template<typename T>
    void PrintToFile(string fileName, vector<vector<T>>& data, int iter){
        // clear old file
        clearResult(fileName);
        // write to file
        ofstream file(fileName, ios_base::app);
        file << scientific << setprecision(8);
        if (file.is_open()) {
            double mean = 0.0;
            for (int i=0; i<_run; i++){
                mean += data[i][0];
            }
            mean /= _run;
            for (int i=0; i<_run; i++){
                file << data[i][0] << endl;
            }
            
            // std dev
            if (_run!=1){
                vector <double> StdDev(_run, 0);
                for (int i=0; i<_run; i++){
                    StdDev[i] = (double)data[i][0];
                }
                file << "Run " << _run << ", std dev: " << (double)calculateStandardDeviation(StdDev) << ", mean: " << mean << endl;
            }

            // close file
            file.close();
        }
        else {
            cerr << "Unable to open file!\n";
        }
    }


    double calculateStandardDeviation(const std::vector<double>& numbers) {
        double mean = calculateMean(numbers);
        double variance = 0.0;
        
        for(double num : numbers) {
            variance += (num - mean) * (num - mean);
        }
        
        variance /= numbers.size();
        return std::sqrt(variance);
    }

    void NewShowDataInt(int amount)    { NewShowData(_dataInt, amount); }
    void NewShowDataDouble(int amount) { NewShowData(_dataDouble, amount); }
    void SetDataInt(int run, long int num, int iter)       { SetData(run, _dataInt, num, iter); }
    void SetDataDouble(int run, long double num, int iter) { SetData(run, _dataDouble, num, iter); }
    void PrintToFileInt(const std::string& fileName, int iter)       { PrintToFile(fileName, _dataInt, iter); }
    void PrintToFileDouble(const std::string& fileName, int iter)    { PrintToFile(fileName, _dataDouble, iter); }

    void init() {
        _lastPrint = -1;
        _maxPrint = _lastPrint;
    }

private:
    string _folder;
    string _alg;
    int _run;
    int _lastPrint;
    int _maxPrint;
    vector<vector<long int>> _dataInt;
    vector<vector<long double>> _dataDouble;
};