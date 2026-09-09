#include "CNF.h"
#include "Solver.h"
#include <vector>
#include <iostream>
using namespace std;
int main(){
    CNFFactory cf = CNFFactory(3);
    cf.addClause({TRUE, FALSE, NONE});
    cf.addClause({NONE, FALSE, FALSE});
    cf.addClause({NONE, TRUE, NONE});
    CNF cnf = cf.makeCNF();
    cout << cnf.variableCount() << "\n";
    AssignedCNF acnf = AssignedCNF(cnf);
    //acnf.assignValue(1, true);
    //acnf.unassignValue();
    unitResolution(acnf);
    acnf.printAssignedCNF();
    return 0;
}
