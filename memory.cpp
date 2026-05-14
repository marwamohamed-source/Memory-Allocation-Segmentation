#include "memory.h"

memory::memory(int memSize) : memSize(memSize) {
    segment s;
    s.name = "hole";
    s.processName = "";
    s.size = memSize;
    s.start = 0;
    s.type = memType::hole;
    m.push_front(s);
}

memory::memory(int memSize, bool manual) : memSize(memSize) {}

void memory::addHole(int start, int size) {
    segment hole;
    hole.name = "hole";
    hole.processName = "";
    hole.type = memType::hole;
    hole.start = start;
    hole.size = size;
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it->start == start) {
            it->size = size;
            it->type = memType::hole;
            it->name = "hole";
            it->processName = "";
            return;
        }
        if (it->start > start) {
            m.insert(it, hole);
            return;
        }
    }
    m.push_back(hole);
}

void memory::allocation(process& p) {
    if (p.s.size() == 0) return;
    int choice = 0;
    while (choice != 1 && choice != 2) {
        cout << "Choose allocation strategy for process " << p.name << ":\n";
        cout << "1. First Fit\n";
        cout << "2. Best Fit\n";
        cin >> choice;
        if (choice != 1 && choice != 2)
            cout << "Invalid choice, try again.\n";
    }
    switch (choice) {
        case 1: firstFit(p); break;
        case 2: bestFit(p); break;
    }
}

void memory::firstFit(process& p) {
    list<segment> temp = m;
    for (auto& seg : p.s) {
        bool found = false;
        for (auto it = temp.begin(); it != temp.end(); ++it) {
            if (it->type == memType::hole && it->size >= seg.size) {
                segment newSeg;
                newSeg.name = seg.name;
                newSeg.processName = p.name;
                newSeg.size = seg.size;
                newSeg.start = it->start;
                newSeg.type = memType::segment;
                seg.start = it->start;
                it->start += seg.size;
                it->size -= seg.size;
                if (it->size == 0)
                    *it = newSeg;
                else
                    temp.insert(it, newSeg);
                found = true;
                break;
            }
        }
        if (!found) {
            cerr << "Process " << p.name << " does not fit, Allocation failed\n";
            return;
        }
    }
    m = temp;
    p.allocated = true;
}

void memory::bestFit(process& p) {
    list<segment> temp = m;
    for (auto& seg : p.s) {
        auto bestIt = temp.end();
        for (auto it = temp.begin(); it != temp.end(); ++it) {
            if (it->type == memType::hole && it->size >= seg.size) {
                if (bestIt == temp.end() || it->size < bestIt->size)
                    bestIt = it;
            }
        }
        if (bestIt == temp.end()) {
            cerr << "Process " << p.name << " does not fit, Allocation failed\n";
            return;
        }
        segment newSeg;
        newSeg.name = seg.name;
        newSeg.processName = p.name;
        newSeg.size = seg.size;
        newSeg.start = bestIt->start;
        newSeg.type = memType::segment;
        seg.start = bestIt->start;
        bestIt->start += seg.size;
        bestIt->size -= seg.size;
        if (bestIt->size == 0)
            *bestIt = newSeg;
        else
            temp.insert(bestIt, newSeg);
    }
    m = temp;
    p.allocated = true;
}

void memory::mergeHoles() {
    auto it = m.begin();
    while (it != m.end()) {
        auto next = it;
        ++next;
        if (next == m.end()) break;
        if (it->type == memType::hole && next->type == memType::hole
            && it->start + it->size == next->start) {
            it->size += next->size;
            m.erase(next);
        } else {
            ++it;
        }
    }
}

void memory::deallocation(string processName) {
    bool found = false;
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it->type == memType::segment && it->processName == processName) {
            it->type = memType::hole;
            it->name = "hole";
            it->processName = "";
            found = true;
        }
    }
    if (!found) {
        cout << "Process " << processName << " not found.\n";
        return;
    }
    mergeHoles();
}

void memory::printLayout() {
    cout << "\n Memory Layout \n";
    cout << left << setw(10) << "Start" << setw(10) << "End"
         << setw(15) << "Process" << setw(15) << "Segment" << setw(10) << "Size" << "\n";
    cout << string(60, '-') << "\n";
    for (auto& seg : m) {
        if (seg.type == memType::hole) {
            cout << left << setw(10) << seg.start << setw(10) << seg.start + seg.size - 1
                 << setw(15) << "[ HOLE ]" << setw(15) << "--------" << setw(10) << seg.size << "\n";
        } else {
            cout << left << setw(10) << seg.start << setw(10) << seg.start + seg.size - 1
                 << setw(15) << seg.processName << setw(15) << seg.name << setw(10) << seg.size << "\n";
        }
    }
    cout << string(60, '-') << "\n";
}

void memory::printSegmentTable(process& p) {
    cout << "\n Segment Table for " << p.name << " \n";
    cout << left << setw(15) << "Segment" << setw(10) << "Base" << setw(10) << "Size\n";
    cout << string(35, '-') << "\n";
    for (auto& seg : p.s) {
        if (seg.type == memType::segment)
            cout << left << setw(15) << seg.name << setw(10) << seg.start << setw(10) << seg.size << "\n";
    }
}