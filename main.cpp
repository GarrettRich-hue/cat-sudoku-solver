#include "CNF.h"
#include <vector>
#include <iostream>
using namespace std;
int main(){
    CNFFactory cf = CNFFactory(3);
    cf.addClause({TRUE, FALSE, NONE});
    cf.addClause({NONE, FALSE, FALSE});
    CNF cnf = cf.makeCNF();
    cout << cnf.variableCount() << "\n";
    AssignedCNF acnf = AssignedCNF(cnf);
    acnf.assignValue(2, true);
    acnf.unassignValue();
    acnf.printAssignedCNF();
    return 0;
}
