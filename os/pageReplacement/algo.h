#ifndef algo_h
#define algo_h

#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include "./tool.h"
Tool tool;
using namespace std;

typedef struct frame
{
    int _pageSize;
    int _dataSize;
    vector<int> _data, _dirtydata, _referdata;
    queue<int> _qdata, _qdirty, _qrefer;
    int _pageFaults;
    int _diskWrites;
    int _interrupts;
} T_Frame;

void Init_frame(T_Frame &frame, int pageSize){
    frame._pageSize = pageSize;
    frame._data.resize(pageSize);
    frame._qdata = queue<int>();
    frame._qdirty = queue<int>();
    frame._qrefer = queue<int>();
    frame._dirtydata.resize(pageSize);
    frame._referdata.resize(pageSize);
    frame._dataSize = 0;
    frame._pageFaults = 0;
    frame._diskWrites = 0;
    frame._interrupts = 0;
}

void Init_frames(vector<vector<T_Frame>> &frames)
{
    frames.resize(4);
    for (int i = 0; i < 4; i++)
    {
        frames[i].resize(10);
        for (int j = 0; j < 10; j++)
            Init_frame(frames[i][j], (j + 1) * 10);
    }
}

/*
異常中斷：可能在某些特定的時候，有一定機率會遇到異常情況，從而觸發中斷。
隨機中斷行為：利用隨機數模擬一些不確定的情況，這些隨機中斷可能來自於硬體故障、計時器中斷、或者其它不可預測的事件。
*/

void Interrupt(T_Frame &frame)
{
    frame._interrupts++;
    if (tool.rand_double(0, 1) < 0.1)
        frame._interrupts++;
}

/*
當某頁面的訪問次數達到某個閾值時，觸發磁碟寫入，模擬這種頻繁訪問和修改的行為。

讀取次數（寫入頻繁修改的頁面）
原因：某些頁面可能會被頻繁訪問和修改。當這樣的頁面被反覆使用且髒位為 1 時，它們最終可能會被替換並寫回磁碟，以保證數據一致性。

真實情境：例如，數據庫查詢或更新操作會反覆讀寫某些頁面，這些頁面可能在一段時間內被頻繁修改。最終，這些頁面可能會因為內存管理策略被替換，並因此觸發磁碟寫入。

*/
void DiskWrite(T_Frame &frame) {
    int dirty = frame._qdirty.front();  // 取得隊列中的髒位
    frame._qdirty.pop();  // 移除隊列中的這個髒位
    
    // 如果髒位為1，進行磁碟寫入操作
    if (dirty == 1) {
        // 增加磁碟寫入次數
        frame._diskWrites++;
        // 重置髒位並放回佇列，表示這個頁面已經寫回磁碟
        frame._qdirty.push(0);  // 髒位重置為0
    } else {
        // 如果髒位為0，無需磁碟寫入，只將該髒位放回佇列
        frame._qdirty.push(dirty);  // 保持原樣
    }
}


// Optimal Algorithm
void optimalPageReplacement(vector<int>& referenceString, vector<int>& dirtyString, T_Frame& frame){
    int num_string = referenceString.size();
    for (int i=0; i<num_string; i++){
        // 如果當前頁面不在frame中，發生 Page Fault
        if (find(frame._data.begin(), frame._data.end(), referenceString[i]) == frame._data.end()) {
            // Page Fault
            frame._pageFaults++;
            // 如果frame已經滿了，移除未來最少用的頁面
            if (frame._data.size() == frame._pageSize) {
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
                    // 模擬磁碟寫入
                    if (frame._dirtydata[indexToRemove] == 1) {
                        frame._diskWrites++;
                    }
                    // 移除最遲會被使用的頁面和對應的髒位
                    frame._dirtydata.erase(frame._dirtydata.begin() + indexToRemove);
                }
            }
            
            // 將新頁面加入frame
            frame._data.push_back(referenceString[i]);         // 將新頁面加入佇列（FIFO）
            frame._dirtydata.push_back(dirtyString[i]);        // 將新頁面的髒位加入佇列

            // 觸發中斷
            Interrupt(frame);
        }
    }
}

// FIFO Page Replacement Algorithm
void fifoPageReplacement(vector<int>& referenceString, vector<int>& dirtyString, T_Frame& frame) {
    int num_string = referenceString.size();
    
    for (int i = 0; i < num_string; i++) {
        // 檢查當前頁面是否在frame._qdata中，使用 find 方法來判斷
        queue<int> tempQueue = frame._qdata;
        bool pageFound = false;

        while (!tempQueue.empty()) {
            if (tempQueue.front() == referenceString[i]) {
                pageFound = true;
                break;
            }
            tempQueue.pop();
        }

        // 如果當前頁面不在frame中，發生 Page Fault
        if (!pageFound) {
            // Page Fault
            frame._pageFaults++;

            // 如果frame已經滿了，移除最早加入的頁面
            if (frame._qdata.size() == frame._pageSize) {
                // 移除最早加入的頁面
                frame._qdata.pop();
                // 取得髒位並判斷是否需要進行磁碟寫入
                int dirty_to_remove = frame._qdirty.front();
                frame._qdirty.pop();
                
                if (dirty_to_remove == 1) {
                    frame._diskWrites++;
                }
            }

            // 將新頁面加入frame佇列
            frame._qdata.push(referenceString[i]);  // 將新頁面加入佇列（FIFO）
            frame._qdirty.push(dirtyString[i]);  // 將新頁面的髒位加入佇列

            // 觸發中斷
            Interrupt(frame);
        }
    }
}


// Enhanced Second Chance Algorithm
void enSecChancePageReplacement(vector<int>& referenceString, vector<int>& dirtyString, T_Frame& frame) {
    frame._data.clear();
    frame._referdata.clear();
    frame._dirtydata.clear();
    frame._data.reserve(frame._pageSize);
    frame._referdata.reserve(frame._pageSize);
    frame._dirtydata.reserve(frame._pageSize);

    int num_string = referenceString.size();
    int pointer=0;

    for (int i = 0; i < num_string; i++) {
        // 檢查當前頁面是否在frame中，若不在，發生 Page Fault
        int findIndex = -1;
        for (int j = 0; j < frame._data.size(); j++) {
            if (frame._data[j] == referenceString[i]) {
                findIndex = j;
                break;
            }
        }

        if ( findIndex == -1) {
            // Page Fault
            frame._pageFaults++;
            // 如果frame已經滿了，開始進行替換
            if (frame._data.size() == frame._pageSize) {
                bool pageReplaced = false;
                int checkedPages = 2;  // 計算檢查的頁面數量，用來判斷是否需要強制替換

                int page_to_remove=0;
                int order = 2;
                int tmpPtr;
                for (int  j = 0; j < frame._data.size() * 2 && !pageReplaced; j++)
                {
                    int index = (pointer + j) % frame._data.size();
                    if (frame._referdata[index] == 0 && frame._dirtydata[index] == 0 && order!=0) {
                        page_to_remove = index;
                        order = 0;
                        pageReplaced = true;
                        tmpPtr = index;
                        break;
                    }
                    else if (frame._referdata[index] == 0 && frame._dirtydata[index] == 1 && order > 1) {
                        order = 1;
                        page_to_remove = index;
                        pageReplaced = true;
                        tmpPtr = index;
                    }
                    else {
                        if (j >= frame._data.size()) {
                            frame._referdata[index] = 0;
                        }
                    }
                }
                pointer = tmpPtr;
                // 把第page_to_remove筆資料從frame 的vector中移除
                frame._data.erase(frame._data.begin() + page_to_remove);
                frame._referdata.erase(frame._referdata.begin() + page_to_remove);
                if (frame._dirtydata[page_to_remove] == 1) {
                    frame._diskWrites++;
                }
                frame._dirtydata.erase(frame._dirtydata.begin() + page_to_remove);
            }

            // 新頁面加入frame，設置對應的髒位和參考位
            frame._data.push_back(referenceString[i]);  // 新頁面加入數據
            frame._dirtydata.push_back(dirtyString[i]);  // 新頁面的髒位加入佇列
            frame._referdata.push_back(1);  // 參考位設置為1

            // 觸發中斷
            Interrupt(frame);
        } else {
            // 如果頁面命中，設置參考位為1
            int pageIndex = find(frame._data.begin(), frame._data.end(), referenceString[i]) - frame._data.begin();
            frame._referdata[pageIndex] = 1;
        }
    }
}

// mine Algorithm : tripleChancePageReplacement
void minePageReplacement(vector<int>& referenceString, vector<int>& dirtyString, T_Frame& frame) {
    int num_string = referenceString.size();

    for (int i = 0; i < num_string; i++) {
        // 檢查當前頁面是否在frame中，若不在，發生 Page Fault
        if (find(frame._data.begin(), frame._data.end(), referenceString[i]) == frame._data.end()) {
            // Page Fault 發生
            frame._pageFaults++;

            // 如果frame已經滿了，開始頁面置換
            if (frame._qdata.size() == frame._pageSize) {
                bool pageReplaced = false;

                while (!pageReplaced) {
                    int page_to_remove = frame._qdata.front();  // 取得FIFO中的第一個頁面
                    frame._qdata.pop();  // 從佇列中移除

                    auto it = find(frame._data.begin(), frame._data.end(), page_to_remove);
                    int pageIndex = distance(frame._data.begin(), it);

                    // 檢查參考位：將二進制參考位解讀
                    int referBit = frame._referdata[pageIndex];

                    if ((referBit & 0b10) == 0 && (referBit & 0b01) == 0) {
                        // 參考位為 00 -> 最近沒有被使用，可以被替換
                        frame._data.erase(frame._data.begin() + pageIndex);
                        frame._referdata.erase(frame._referdata.begin() + pageIndex);
                        pageReplaced = true;
                    } else if ((referBit & 0b10) == 0b10) {
                        // 參考位為 10 -> 將其設置為 01
                        frame._referdata[pageIndex] = 0b01;  // 降級參考位
                        frame._qdata.push(page_to_remove);  // 給該頁面第二次機會
                    } else if ((referBit & 0b01) == 0b01) {
                        // 參考位為 01 -> 將其設置為 00
                        frame._referdata[pageIndex] = 0b00;  // 降級參考位
                        frame._qdata.push(page_to_remove);  // 給該頁面第三次機會
                    }
                }
                // 模擬磁碟寫入
                DiskWrite(frame);
            }

            // 新頁面加入frame，設置二進制參考位為 10（最高優先級）
            frame._data.push_back(referenceString[i]);  // 新頁面加入數據
            frame._qdata.push(referenceString[i]);      // 新頁面加入佇列
            frame._referdata.push_back(0b10);            // 將參考位設為 10
            frame._qdirty.push(dirtyString[i]);         // 新頁面的髒位加入佇列
            
            // 觸發中斷
            Interrupt(frame);
        }
        else {
            // 如果頁面命中，將二進制參考位設為 10
            int pageIndex = find(frame._data.begin(), frame._data.end(), referenceString[i]) - frame._data.begin();
            frame._referdata[pageIndex] = 0b10;  // 命中時，將參考位設為 10
        }
    }
}

#endif