#include <iostream>
#include <queue>
#include <vector>
#include "./tool.h"
#include "./algo.h"
using namespace std;

Tool tool;

void gen_random_string(vector<int> &xx, int string_length){
    xx.clear();
    xx.reserve(string_length);
    for (int i = 0; i < string_length; i++)
        xx.push_back(tool.rand_int(0, 1200));
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
void show_data(T_Frame &frame, string show){
    cout << show << frame._pageSize << ", Page Fault: " << frame._pageFaults << endl;
}

int main() {
    vector<vector<T_Frame>> frames, framesLocal, framesMine;
    frames.resize(3);
    framesLocal.resize(3);
    framesMine.resize(3);
    Init_frames(frames[0]);
    Init_frames(frames[1]);
    Init_frames(frames[2]);
    Init_frames(framesLocal[0]);
    Init_frames(framesLocal[1]);
    Init_frames(framesLocal[2]);
    Init_frames(framesMine[0]);
    Init_frames(framesMine[1]);
    Init_frames(framesMine[2]);
    
// Block1 : Random page replacement
    vector<int> randomString;
    int str_len = 1200;
    randomString.resize(str_len);
    gen_random_string(randomString, str_len);
    cout << "Random String" << endl;
    for (int i=0; i< 10; i++) {
        fifoPageReplacement(randomString, frames[0][i]);
        optimalPageReplacement(randomString, frames[1][i]);
        enSecChancePageReplacement(randomString, frames[2][i]);
    }

    // show data
    for (int i=0; i<10; i++)
        show_data(frames[0][i], "FIFO    Random Frame size: ");
    cout << endl;
    for (int i=0; i<10; i++)
        show_data(frames[1][i], "Optimal Random Frame size: ");
    cout << endl;
    for (int i=0; i<10; i++)
        show_data(frames[2][i], "Second  Random Frame size: ");
    cout << endl << "---------------" << endl;

// Block2 : Local page replacement
    vector<int> localString;
    gen_local_string(localString, str_len);

    for (int i=0; i< 10; i++) {
        fifoPageReplacement(randomString, framesLocal[0][i]);
        optimalPageReplacement(randomString, framesLocal[1][i]);
        enSecChancePageReplacement(randomString, framesLocal[2][i]);
    }

    
    // show data
    for (int i=0; i<10; i++)
        show_data(framesLocal[0][i], "FIFO    Local Frame size: ");
    cout << endl;
    for (int i=0; i<10; i++)
        show_data(framesLocal[1][i], "Optimal Local Frame size: ");
    cout << endl;
    for (int i=0; i<10; i++)
        show_data(framesLocal[2][i], "Second  Local Frame size: ");
    cout << endl << "---------------" << endl;

    return 0;
}
