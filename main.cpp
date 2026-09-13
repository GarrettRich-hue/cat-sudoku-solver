#include "CNF.h"
#include "Solver.h"
#include "Formula.h"
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
    Formula* demorgans = new Not(new And(new Variable(0), new Not(new Variable(1))));
    printFormula(demorgans);
    {
        NNFVisitor nnf;
        demorgans->accept(&nnf);
        demorgans = nnf.getResult();
    }
    printFormula(demorgans);
    // ¬(A+B*(C+B+¬A)+D*B)
    Formula* thingToExpand = new Not(
            new And(new Variable(0), 
                new And(
                    new Or(new Variable(1),
                        new And(
                            new Variable(2),
                        new And( 
                            new Variable(1),
                            new Not(new Variable(0))
                            )
                        )
                        
                    ),
                    new Or(new Variable(3), new Variable(1))
                )));
    Formula* thingToExpandTest = copyFormula(thingToExpand);
    cout << "Going to convert to CNF: \n";
    printFormula(thingToExpand);
    cout << "A copy of it: \n";
    printFormula(thingToExpandTest);
    {
        NNFVisitor nnf;
        thingToExpandTest->accept(&nnf);
        thingToExpandTest = nnf.getResult();
    }
    cout << "Copy in negational normal form: \n";
    printFormula(thingToExpandTest);
    
    ToCNFVisitor tcnfv;
    thingToExpand->accept(&tcnfv);
    thingToExpand = tcnfv.getResult();
    cout << "As CNF: \n";
    printFormula(thingToExpand);
    return 0;
}
