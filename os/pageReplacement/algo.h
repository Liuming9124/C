#ifndef algo_h
#define algo_h

#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
#include <algorithm>

using namespace std;

typedef struct frame
{
    int _pageSize;
    int _dataSize;
    vector<int> _data;
    queue<int> _qdata, _qdirty, _qrefer;
    vector<int> _dirtyBit, _referBit;
    int _pageFaults;
    int _diskWrites;
    int _interrupts;
    vector<int> _interruptIndex;
} T_Frame;

void Init_frame(T_Frame &frame, int pageSize){
    frame._pageSize = pageSize;
    frame._data.resize(pageSize);
    frame._qdata = queue<int>();
    frame._qdirty = queue<int>();
    frame._qrefer = queue<int>();
    frame._dirtyBit.resize(pageSize);
    frame._referBit.resize(pageSize);
    frame._dataSize = 0;
    frame._pageFaults = 0;
    frame._diskWrites = 0;
    frame._interrupts = 0;
}

void Init_frames(vector<T_Frame> &frames)
{
    frames.resize(10);
    for (int i = 0  ; i < 10; i++)
        Init_frame(frames[i], (i+1)*10);
}

// FIFO Page Replacement Algorithm
void fifoPageReplacement(vector<int>& referenceString, T_Frame& frame) {
    int num_string = referenceString.size();
    for (int i=0; i<num_string; i++){
        // 如果當前頁面不在frame中，發生 Page Fault
        if (find(frame._data.begin(), frame._data.end(), referenceString[i]) == frame._data.end()) {
            // Page Fault
            frame._pageFaults++;
            // 如果frame已經滿了，移除最早加入的頁面
            if (frame._dataSize == frame._pageSize) {
                int page_to_remove = frame._qdata.front();
                frame._qdata.pop();
                // // 模擬磁碟寫入，如果需要
                // frame._diskWrites++;
            }
            else
                frame._dataSize++;
            // 將新頁面加入frame
            frame._qdata.push(referenceString[i]);         // 將新頁面加入佇列（FIFO）

            // // 觸發中斷
            // frame._interrupts++;
            // frame._interruptIndex.push_back(i);  // 記錄中斷發生的位置
        }
        // else
        //     cout << "Page Hit" << endl;
    }
}

// Optimal Algorithm
void optimalPageReplacement(vector<int>& referenceString, T_Frame& frame){
    
    int num_string = referenceString.size();
    for (int i=0; i<num_string; i++){
        // 如果當前頁面不在frame中，發生 Page Fault
        if (find(frame._data.begin(), frame._data.end(), referenceString[i]) == frame._data.end()) {
            // Page Fault
            frame._pageFaults++;
            // 如果frame已經滿了，移除未來最少用的頁面
            if (frame._dataSize == frame._pageSize) {
                int farthest = -1;
                int indexToRemove = -1;
                // 查找最遲再被使用的頁面
                for (int j = 0; j < frame._pageSize; j++) {
                    int nextUse = -1;
                    // 從i+1開始查找該頁面何時會被再次使用
                    for (int k = i + 1; k < num_string; k++) {
                        if (frame._data[j] == referenceString[k]) {
                            nextUse = k;  // 紀錄這個頁面的下次使用時間
                            break;
                        }
                    }
                    // 如果該頁面將來不會被使用，直接選中這個頁面來移除
                    if (nextUse == -1) {
                        indexToRemove = j;  // 選擇此頁面進行移除
                        break;  // 找到未來不再使用的頁面，立即結束查找
                    }
                    // 找到未來使用最晚的頁面
                    if (nextUse > farthest) {
                        farthest = nextUse;
                        indexToRemove = j;  // 更新最遲被使用的頁面索引
                    }
                }
                // 確保找到了一個可以移除的頁面
                if (indexToRemove != -1) {
                    // 移除最遲會被使用的頁面
                    frame._data.erase(frame._data.begin() + indexToRemove);
                }
            }
            else
                frame._dataSize++;
            // 將新頁面加入frame
            frame._data.push_back(referenceString[i]);         // 將新頁面加入佇列（FIFO）

            // // 觸發中斷
            // frame._interrupts++;
            // frame._interruptIndex.push_back(i);  // 記錄中斷發生的位置
        }
    }
}

// Enhanced Second Chance Algorithm
void enSecChancePageReplacement(vector<int>& referenceString, T_Frame& frame) {
    int num_string = referenceString.size();
    
    for (int i = 0; i < num_string; i++) {
        // 如果當前頁面不在frame中，發生 Page Fault
        if (find(frame._data.begin(), frame._data.end(), referenceString[i]) == frame._data.end()) {
            // Page Fault
            frame._pageFaults++;
            
            // 如果frame已經滿了，移除最早進來的資料
            if (frame._dataSize == frame._pageSize) {
                // 更新 qdirty 和 qrefer，並使用第二機會算法
                while (true) {
                    int page_to_remove = frame._qdata.front();  // 取得隊列前面的頁面
                    frame._qdata.pop();                          // 移除該頁面

                    int dirty = frame._qdirty.front();          // 取得該頁面的髒位
                    frame._qdirty.pop();                         // 移除髒位

                    int refer = frame._qrefer.front();          // 取得該頁面的參考位
                    frame._qrefer.pop();                         // 移除參考位

                    // 檢查該頁面的參考位和髒位
                    if (refer == 0 && dirty == 0) {
                        // 移除最近沒有被使用也沒有被修改的頁面
                        auto it = find(frame._data.begin(), frame._data.end(), page_to_remove);
                        if (it != frame._data.end()) {
                            frame._data.erase(it); // 從數據中移除該頁面
                        }
                        break;  // 離開循環
                    } else if (refer == 0 && dirty == 1) {
                        // 最近沒有被使用，但有被修改
                        // 此時，將該頁面寫回磁碟（這裡可加入寫回磁碟的邏輯）
                        auto it = find(frame._data.begin(), frame._data.end(), page_to_remove);
                        if (it != frame._data.end()) {
                            frame._data.erase(it); // 從數據中移除該頁面
                        }
                        break;  // 離開循環
                    } else {
                        // 將這個頁面重新加入佇列，並且更新 qdirty and qrefer
                        frame._qdata.push(page_to_remove);
                        frame._qdirty.push(dirty);
                        frame._qrefer.push(refer);
                    }
                }
            } else {
                frame._dataSize++;  // 增加已用的frame大小
            }

            // 將新頁面加入frame 並確保 qdirty and qrefer 的值是0
            frame._data.push_back(referenceString[i]);  // 新頁面加入數據
            frame._qdata.push(referenceString[i]);       // 將頁面加入佇列
            frame._qdirty.push(0);                        // 髒位設置為0
            frame._qrefer.push(0);                        // 參考位設置為0

            // 觸發中斷
            // frame._interrupts++;
            // frame._interruptIndex.push_back(i);  // 記錄中斷發生的位置
        }
    }
}


#endif