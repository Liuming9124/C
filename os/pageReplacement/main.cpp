#include <iostream>
#include <queue>
#include <vector>
#include <utility>
#include "./tool.h"
#include "./algo.h"
using namespace std;


void gen_random_string(vector<int> &xx, int string_length){
    xx.clear();
    xx.reserve(string_length);
    for (int i = 0; i < string_length; i++){
        xx.push_back(tool.rand_int(0, 1200));
    }
}

void gen_local_string(vector<int> &xx, int string_length){
    xx.clear();
    xx.reserve(string_length);

    int minProcedures = string_length/200;
    int maxProcedures = string_length/100;
    
    vector<int> ProgramData;
    ProgramData.reserve(string_length/200);

    // 生成每個program的長度，所有program的長度總和等於string_length
    int count = 0;
    while (count <= string_length)
    {
        int numData = tool.rand_int(minProcedures, maxProcedures);
        bool flag = false;
        if (numData+count > string_length){
            numData -= numData+count - string_length;
            flag = true;
        }
        ProgramData.push_back(numData);
        count += numData;
        if (flag)
            break;
    }
    // 生成每個program的locality
    for (int i = 0; i < ProgramData.size(); i++)
    {
        // 設計每個program的locality在1~50之間
        int duration = tool.rand_int(1, 50);
        int startLocality = tool.rand_int(0, 1200-duration);
        int endLocality = startLocality + duration;

        for (int j = 0; j < ProgramData[i]; j++)
            xx.push_back(tool.rand_int(startLocality, endLocality));
    }
}


void gen_mine_string(vector<int> &xx, int string_length){
    xx.clear();
    xx.reserve(string_length);
    
    vector<int> ProgramData;
    ProgramData.reserve(string_length/150);

    // 生成每個program的長度，所有program的長度總和等於string_length
    int count = 0;
    while (count <= string_length)
    {
        int numData = tool.rand_int(50, 150);
        bool flag = false;
        if (numData+count > string_length){
            numData -= numData+count - string_length;
            flag = true;
        }
        ProgramData.push_back(numData);
        count += numData;
        if (flag)
            break;
    }
    // 生成每個program的locality
    for (int i = 0; i < ProgramData.size(); i++)
    {
        // 設計每個program的locality在1~50之間
        int duration = tool.rand_int(1, 50);
        int startLocality = tool.rand_int(0, 1200-duration);
        int endLocality = startLocality + duration;

        for (int j = 0; j < ProgramData[i]; j++)
            xx.push_back(tool.rand_int(startLocality, endLocality));
    }
}

void show_data(T_Frame &frame, string show){
    cout << show << frame._pageSize << ",pf: " << frame._pageFaults << ",intpt: " << frame._interrupts << ",dw: " << frame._diskWrites << endl;
}

int main() {
    vector<vector<T_Frame>> frames, framesLocal, framesMine;
    frames.resize(4);
    framesLocal.resize(4);
    framesMine.resize(4);
    Init_frames(frames[0]);
    Init_frames(frames[1]);
    Init_frames(frames[2]);
    Init_frames(frames[3]);
    Init_frames(framesLocal[0]);
    Init_frames(framesLocal[1]);
    Init_frames(framesLocal[2]);
    Init_frames(framesLocal[3]);
    Init_frames(framesMine[0]);
    Init_frames(framesMine[1]);
    Init_frames(framesMine[2]);
    Init_frames(framesMine[3]);
    
// Block1 : Random page replacement
    vector<int> randomString;
    int str_len = 120000;
    randomString.resize(str_len);
    gen_random_string(randomString, str_len);
    cout << "Random String" << endl;
    for (int i=0; i< 10; i++) {
        fifoPageReplacement(randomString, frames[0][i]);
        optimalPageReplacement(randomString, frames[1][i]);
        enSecChancePageReplacement(randomString, frames[2][i]);
        minePageReplacement(randomString, frames[3][i]);
        cout << "Finish Random: " << i  << endl;
    }

// Block2 : Local page replacement
    vector<int> localString;
    vector<int> localWrite;
    gen_local_string(localString, str_len);

    for (int i=0; i< 10; i++) {
        fifoPageReplacement(localString, framesLocal[0][i]);
        optimalPageReplacement(localString, framesLocal[1][i]);
        enSecChancePageReplacement(localString, framesLocal[2][i]);
        minePageReplacement(localString, framesLocal[3][i]);
        cout << "Finish Local: " << i  << endl;
    }

// Block3 : Mine page replacement
    vector<int> mineString;
    vector<int> mineWrite;
    gen_mine_string(mineString, str_len);

    for (int i=0; i< 10; i++) {
        fifoPageReplacement(mineString, framesMine[0][i]);
        optimalPageReplacement(mineString, framesMine[1][i]);
        enSecChancePageReplacement(mineString, framesMine[2][i]);
        minePageReplacement(mineString, framesMine[3][i]);
        cout << "Finish mine: " << i  << endl;
    }



    // show random data
    for (int i=0; i<10; i++)
        show_data(frames[0][i], "FIFO    : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(frames[1][i], "Optimal : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(frames[2][i], "Second  : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(frames[3][i], "Mine    : "); cout << endl << "---------------" << endl;


    // show local data
    for (int i=0; i<10; i++)
        show_data(framesLocal[0][i], "FIFO    : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(framesLocal[1][i], "Optimal : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(framesLocal[2][i], "Second  : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(framesLocal[3][i], "Mine    : "); cout << endl << "---------------" << endl;


    // show mine data
    for (int i=0; i<10; i++)
        show_data(framesMine[0][i], "FIFO    : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(framesMine[1][i], "Optimal : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(framesMine[2][i], "Second  : "); cout << endl;
    for (int i=0; i<10; i++)
        show_data(framesMine[3][i], "Mine    : "); cout << endl << "---------------" << endl;


    return 0;
}
