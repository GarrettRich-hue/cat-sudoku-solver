#ifndef CATSOLVE_H
#define CATSOLVE_H

#include <initializer_list>
#include "Formula.h"
#include "CNF.h"
#include <vector>
using namespace std;

class CatInstance{
    private:
        int size;
        vector<vector<int>> regionColours;
        vector<vector<Trit>> catPlaced;
    public:
        CatInstance(int, initializer_list<initializer_list<int>>);
        Formula* asFormula();
        Trit& catAt();
};

#endif
