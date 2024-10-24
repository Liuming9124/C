#include <iostream>
#include <queue>
#include <vector>
#include <utility>
#include "./tool.h"
#include "./algo.h"
#include <fstream>
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
        // 設計每個program的locality在1~90之間
        int duration = tool.rand_int(10, 90);
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
    ProgramData.reserve(string_length/10);

    // 生成每個program的長度，所有program的長度總和等於string_length
    int count = 0;
    while (count <= string_length)
    {
        int numData = tool.rand_int(100, 200);
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
        // 設計每個program的locality在150~250之間
        int duration = tool.rand_int(100, 200);
        int startLocality = tool.rand_int(0, 1200-duration);
        int endLocality = startLocality + duration;

        for (int j = 0; j < ProgramData[i]; j++)
            xx.push_back(tool.rand_int(startLocality, endLocality));
    }
}

void gen_dirty_string(vector<int> &xx, int string_length){
    xx.clear();
    xx.reserve(string_length);
    for (int i = 0; i < string_length; i++){
        if (tool.rand_int(0, 100) < 10)
            xx.push_back(1);
        else
            xx.push_back(0);
    }
}

void show_data(T_Frame &frame, string show){
    cout << show << frame._pageSize << ",pf: " << frame._pageFaults << ",intpt: " << frame._interrupts << ",dw: " << frame._diskWrites << endl;
}

void show_datas(vector<vector<T_Frame>> &frames, vector<vector<T_Frame>> &framesLocal, vector<vector<T_Frame>> &framesMine){
    
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

}

void write_data(ofstream& pageFile, ofstream& interruptFile, ofstream& diskFile, vector<T_Frame>& frames) {
    // Write page faults, interrupts, and disk writes for a specific frame
    for (auto& frame : frames) {
        pageFile << frame._pageFaults << endl;
        interruptFile << frame._interrupts << endl;
        diskFile << frame._diskWrites << endl;
    }
}

void print_to_file(vector<vector<T_Frame>> &frames, vector<vector<T_Frame>> &framesLocal, vector<vector<T_Frame>> &framesMine) {
    // write local data
    ofstream l_p_enha("./local/pagefault/enhance.txt"); 
    ofstream l_p_fifo("./local/pagefault/fifo.txt"); 
    ofstream l_p_mine("./local/pagefault/mine.txt"); 
    ofstream l_p_opti("./local/pagefault/optimal.txt"); 
    
    ofstream l_i_enha("./local/interrupt/enhance.txt"); 
    ofstream l_i_fifo("./local/interrupt/fifo.txt"); 
    ofstream l_i_mine("./local/interrupt/mine.txt"); 
    ofstream l_i_opti("./local/interrupt/optimal.txt"); 
    
    ofstream l_d_enha("./local/diskwrite/enhance.txt"); 
    ofstream l_d_fifo("./local/diskwrite/fifo.txt"); 
    ofstream l_d_mine("./local/diskwrite/mine.txt"); 
    ofstream l_d_opti("./local/diskwrite/optimal.txt"); 
    
    // write random data
    ofstream r_p_enha("./random/pagefault/enhance.txt"); 
    ofstream r_p_fifo("./random/pagefault/fifo.txt"); 
    ofstream r_p_mine("./random/pagefault/mine.txt"); 
    ofstream r_p_opti("./random/pagefault/optimal.txt"); 
    
    ofstream r_i_enha("./random/interrupt/enhance.txt"); 
    ofstream r_i_fifo("./random/interrupt/fifo.txt"); 
    ofstream r_i_mine("./random/interrupt/mine.txt"); 
    ofstream r_i_opti("./random/interrupt/optimal.txt"); 
    
    ofstream r_d_enha("./random/diskwrite/enhance.txt"); 
    ofstream r_d_fifo("./random/diskwrite/fifo.txt"); 
    ofstream r_d_mine("./random/diskwrite/mine.txt"); 
    ofstream r_d_opti("./random/diskwrite/optimal.txt"); 

    // write mine data
    ofstream m_p_enha("./mine/pagefault/enhance.txt"); 
    ofstream m_p_fifo("./mine/pagefault/fifo.txt"); 
    ofstream m_p_mine("./mine/pagefault/mine.txt"); 
    ofstream m_p_opti("./mine/pagefault/optimal.txt"); 
    
    ofstream m_i_enha("./mine/interrupt/enhance.txt"); 
    ofstream m_i_fifo("./mine/interrupt/fifo.txt"); 
    ofstream m_i_mine("./mine/interrupt/mine.txt"); 
    ofstream m_i_opti("./mine/interrupt/optimal.txt"); 
    
    ofstream m_d_enha("./mine/diskwrite/enhance.txt"); 
    ofstream m_d_fifo("./mine/diskwrite/fifo.txt"); 
    ofstream m_d_mine("./mine/diskwrite/mine.txt"); 
    ofstream m_d_opti("./mine/diskwrite/optimal.txt"); 

    // Random data output
    write_data(r_p_fifo, r_i_fifo, r_d_fifo, frames[0]); // fifo
    write_data(r_p_opti, r_i_opti, r_d_opti, frames[1]); // optimal
    write_data(r_p_enha, r_i_enha, r_d_enha, frames[2]); // enhance
    write_data(r_p_mine, r_i_mine, r_d_mine, frames[3]); // mine

    // Local data output
    write_data(l_p_fifo, l_i_fifo, l_d_fifo, framesLocal[0]); // fifo
    write_data(l_p_opti, l_i_opti, l_d_opti, framesLocal[1]); // optimal
    write_data(l_p_enha, l_i_enha, l_d_enha, framesLocal[2]); // enhance
    write_data(l_p_mine, l_i_mine, l_d_mine, framesLocal[3]); // mine

    // Mine data output
    write_data(m_p_fifo, m_i_fifo, m_d_fifo, framesMine[0]); // fifo
    write_data(m_p_opti, m_i_opti, m_d_opti, framesMine[1]); // optimal
    write_data(m_p_enha, m_i_enha, m_d_enha, framesMine[2]); // enhance
    write_data(m_p_mine, m_i_mine, m_d_mine, framesMine[3]); // mine

    // Close all files (not necessary since destructors will handle it, but it's good practice)
    l_p_enha.close(); l_p_fifo.close(); l_p_mine.close(); l_p_opti.close();
    l_i_enha.close(); l_i_fifo.close(); l_i_mine.close(); l_i_opti.close();
    l_d_enha.close(); l_d_fifo.close(); l_d_mine.close(); l_d_opti.close();

    r_p_enha.close(); r_p_fifo.close(); r_p_mine.close(); r_p_opti.close();
    r_i_enha.close(); r_i_fifo.close(); r_i_mine.close(); r_i_opti.close();
    r_d_enha.close(); r_d_fifo.close(); r_d_mine.close(); r_d_opti.close();

    m_p_enha.close(); m_p_fifo.close(); m_p_mine.close(); m_p_opti.close();
    m_i_enha.close(); m_i_fifo.close(); m_i_mine.close(); m_i_opti.close();
    m_d_enha.close(); m_d_fifo.close(); m_d_mine.close(); m_d_opti.close();

}


int main() {
    vector<vector<T_Frame>> frames, framesLocal, framesMine;
    frames.resize(4);
    framesLocal.resize(4);
    framesMine.resize(4);
    Init_frames(frames);
    Init_frames(framesLocal);
    Init_frames(framesMine);
    
    int str_len = 120000;

    vector<int> dirty_string;
    dirty_string.resize(str_len);
    gen_dirty_string(dirty_string, str_len);

// Block1 : Random page replacement
    vector<int> randomString;
    randomString.resize(str_len);
    gen_random_string(randomString, str_len);
    cout << "Random String" << endl;
    for (int i=0; i< 10; i++) {
        fifoPageReplacement         (randomString, dirty_string, frames[0][i]);
        optimalPageReplacement      (randomString, dirty_string, frames[1][i]);
        enSecChancePageReplacement  (randomString, dirty_string, frames[2][i]);
        minePageReplacement         (randomString, dirty_string, frames[3][i]);
        cout << "Finish Random: " << i  << endl;
    }

// Block2 : Local page replacement
    vector<int> localString;
    vector<int> localWrite;
    gen_local_string(localString, str_len);

    for (int i=0; i< 10; i++) {
        fifoPageReplacement         (localString, dirty_string, framesLocal[0][i]);
        optimalPageReplacement      (localString, dirty_string, framesLocal[1][i]);
        enSecChancePageReplacement  (localString, dirty_string, framesLocal[2][i]);
        minePageReplacement         (localString, dirty_string, framesLocal[3][i]);
        cout << "Finish  Local: " << i  << endl;
    }

// Block3 : Mine page replacement
    vector<int> mineString;
    vector<int> mineWrite;
    gen_mine_string(mineString, str_len);

    for (int i=0; i< 10; i++) {
        fifoPageReplacement         (mineString, dirty_string, framesMine[0][i]);
        optimalPageReplacement      (mineString, dirty_string, framesMine[1][i]);
        enSecChancePageReplacement  (mineString, dirty_string, framesMine[2][i]);
        minePageReplacement         (mineString, dirty_string, framesMine[3][i]);
        cout << "Finish   Mine: " << i  << endl;
    }

    show_datas(frames, framesLocal, framesMine);

    print_to_file(frames, framesLocal, framesMine);

    return 0;
}
