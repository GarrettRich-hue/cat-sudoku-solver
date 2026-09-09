#include "CNF.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <string>
#include <sstream> // for stringstream, to format strings
using namespace std;
Trit boolToTrit(bool b){
    if(b){
        return TRUE;
    }
    else{
        return FALSE;
    }
}
bool tritToBool(Trit t){
    assert(t!=NONE);
    if(t == TRUE){
        return true;
    }
    else{
        return false;
    }
}
CNFFactory::CNFFactory(int pVarCount){
        varCount = pVarCount;
}
void CNFFactory::addClause(vector<Trit> clause){
    assert(clause.size() == varCount);
    clauses.push_back(clause);
}
CNF CNFFactory::makeCNF(){
    return CNF(clauses);
}
int CNFFactory::variableCount(){
    return varCount;
}
CNF::CNF(vector<vector<Trit>> initClauses)
{
    if(initClauses.size() == 0){
        varCount = 0;
        return;
    }
    varCount = initClauses[0].size();
    for(int i=1; i< initClauses.size(); i++){
        assert(initClauses[i].size() == varCount);
    }
    clauses = initClauses;
}
int CNF::variableCount(){
    return varCount;
}
int CNF::clauseCount(){
    return clauses.size();
}
Trit CNF::literalInClause(int clause, int variable){
    assert(0<= variable && variable < varCount && 0 <= clause && clause < clauses.size());
    return clauses[clause][variable];
}
AssignedCNF::AssignedCNF(CNF &pcnf):
    assignment(pcnf.variableCount(), NONE),
    assignmentStack(pcnf.variableCount(), 0),
    clauseSatisfiedBy(pcnf.variableCount(), -1),
    clauseSize(pcnf.variableCount(), 0),
    units(0,0),
    cnf(pcnf)
{
    stackTop = 0;
    emptyCount = 0;
    activeCount = cnf.clauseCount();
    for(int i = 0;i<cnf.clauseCount();i++){
       for(int j = 0; j< cnf.variableCount(); j++){
           if(cnf.literalInClause(i,j) != NONE){
               clauseSize[i] += 1;
           }
       }
       if(clauseSize[i] == 0){
           emptyCount += 1;
       }
       else if(clauseSize[i] == 1){
           units.push_back(i);
       }
    }
    activeCount = cnf.clauseCount();
}
void AssignedCNF::assignValue(int variable, bool value){
    Trit t = boolToTrit(value);
    assignment[variable] = t;
    assignmentStack[stackTop] = variable;
    stackTop++;
    for(int j = 0;j<cnf.clauseCount(); j++){
        if(clauseSatisfiedBy[j] >= 0){
            continue;
        }
        Trit l = cnf.literalInClause(j, variable);
        if(l == NONE){
            continue;
        }
        if(t == l){
            clauseSatisfiedBy[j] = variable;
            activeCount -= 1;
        }
        else{
            clauseSize[j] -= 1;
            if(clauseSize[j] == 0){
                emptyCount += 1;
            }
            else if(clauseSize[j] == 1){
                units.push_back(j);
            }
        }
    }
}
void AssignedCNF::unassignValue(){
    assert(stackTop >0);
    stackTop--;
    int variable = assignmentStack[stackTop]; 
    Trit assigned = assignment[variable];
    assignment[variable] = NONE;
    for(int i =0;i<cnf.clauseCount(); i++){
        if(clauseSatisfiedBy[i] == variable){ //if clause i was first satisfied by the variable we are now unassigning
            clauseSatisfiedBy[i] = -1; // unsatisfy the clause
            activeCount += 1; // add the new active clause to count
        }
        else if(cnf.literalInClause(i, variable) != NONE){ //if the clause was not satisfied by the assignment, but its size got smaller by the assignment
            if(clauseSize[i] == 0){ // if the clause will become no longer empty by the reintroduction of the variable
                emptyCount -= 1; // reduce the empty clause count
            }
            else if(clauseSize[i] == 1){
                int unitInVec = 0;
                while(units[unitInVec] != i){
                    unitInVec++;
                }
                units.erase(units.begin() + unitInVec);
            }
            clauseSize[i] += 1; // increase the size as the variable has been reintroduced
        }
    }
}
bool AssignedCNF::getContainsEmpty(){
    return emptyCount >0;
}
int AssignedCNF::getActiveCount(){
    return activeCount;
}
bool AssignedCNF::isTrueClause(int clause){
    assert(0 <= clause && clause < cnf.clauseCount());
    return clauseSatisfiedBy[clause] >= 0;
}
void AssignedCNF::printAssignedCNF(){
    const string variableNames = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(activeCount == 0){
        cout << "true\n";
        return;
    }
    for(int i=0;i<cnf.clauseCount();i++){
        if(!isTrueClause(i)){
            cout << "{";
            bool firstLiteral = true;
            for(int j=0;j<cnf.variableCount();j++){
                if(assignment[j] != NONE){
                    continue;
                }
                Trit lit = cnf.literalInClause(i,j);
                if(lit == NONE){
                    continue;
                }
                if(!firstLiteral){
                    cout << ", ";
                }
                firstLiteral = false;
                stringstream variableNameStream;
                if(j < variableNames.length()){
                    variableNameStream << variableNames[j];
                }
                else{
                    variableNameStream << "p"<<j;
                }
                string variableName = variableNameStream.str();
                if(lit == TRUE){ // if variable j appears in clause i as a positive literal
                    cout << variableName; 
                }
                else if(lit == FALSE){
                    cout << "¬" << variableName;
                }
            }
            cout << "}, ";
        }
    }
    cout << "\n";
}
Trit AssignedCNF::getAssignment(int variable){
    return assignment[variable];
}
int AssignedCNF::getClauseSize(int clause){
    return clauseSize[clause];
}
int AssignedCNF::clauseCount(){
    return cnf.clauseCount();
}
int AssignedCNF::getEmptyCount(){
    return emptyCount;
}
int AssignedCNF::getUnitCount(){
    return units.size();
}
Trit AssignedCNF::literalInClause(int clause, int variable){
    return cnf.literalInClause(clause, variable);
}
void AssignedCNF::satisfyUnit(){
    assert(units.size() > 0);
    int unit = units[units.size()-1];
    units.pop_back();
    int variable = -1;
    Trit sign = NONE;
    while(sign == NONE ||assignment[variable] != NONE){
        variable += 1;
        sign = cnf.literalInClause(unit, variable);
    }
    assignValue(variable, tritToBool(sign));
}
