#ifndef SEGMENT_H
#define SEGMENT_H
#include <string>
using namespace std;

enum class memType { hole, segment };

class segment {
public:
    string name;
    string processName;
    int size, start;
    memType type;
};
#endif