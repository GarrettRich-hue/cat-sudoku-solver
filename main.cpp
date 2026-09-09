#include "CNF.h"
#include "Solver.h"
#include <vector>
#include <iostream>
using namespace std;
int main(){
    CNFFactory cf = CNFFactory(3);
    cf.addClause({TRUE, FALSE, NONE});
    cf.addClause({NONE, FALSE, FALSE});
    cf.addClause({FALSE, NONE, TRUE});
    cf.addClause({FALSE, TRUE, NONE});
    cf.addClause({TRUE, NONE, FALSE});
    //cf.addClause({TRUE, NONE, TRUE});
    CNF cnf = cf.makeCNF();
    cout << cnf.variableCount() << "\n";
    AssignedCNF acnf = AssignedCNF(cnf);
    acnf.assignValue(1, true);
    acnf.unassignValue();
    acnf.printAssignedCNF();
    bool solvable = ddpl(acnf);
    cout << "Solved: "<< solvable << "\n";
    acnf.printAssignedCNF();
    acnf.printAssignment();
    return 0;
}
