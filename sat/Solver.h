#ifndef SOLVER_H
#define SOLVER_H

#include "CNF.h"
#include <vector>
using namespace std;

bool ddpl(AssignedCNF &acnf);
void unitResolution(AssignedCNF &acnf);

#endif

