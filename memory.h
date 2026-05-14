#ifndef MEMORY_H
#define MEMORY_H
#include <list>
#include <iostream>
#include <iomanip>
#include "segment.h"
#include "process.h"
using namespace std;

class memory {
private:
    int memSize;
    list<segment> m;
public:
    memory(int memSize);
    memory(int memSize, bool manual);
    void addHole(int start, int size);
    void allocation(process& p);
    void firstFit(process& p);
    void bestFit(process& p);
    void deallocation(string processName);
    void printLayout();
    void printSegmentTable(process& p);
    void mergeHoles();
    list<segment> getBlocks() { return m; }
};
#endif