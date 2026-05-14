#ifndef PROCESS_H
#define PROCESS_H
#include <vector>
#include "segment.h"
using namespace std;

struct process {
    string name;
    vector<segment> s;
    bool allocated = false;
};
#endif