#include "CNF.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <string>
#include <sstream> // for stringstream, to format strings
using namespace std;
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
    assert(0<= variable && variable < varCount);
    return clauses[clause][variable];
}
AssignedCNF::AssignedCNF(CNF &pcnf):
    assignment(pcnf.variableCount(), NONE),
    clauseTrue(pcnf.variableCount(), false),
    clauseSize(pcnf.variableCount(), 0),
    cnf(pcnf)
{
    containsEmpty = false;
    for(int i = 0;i<cnf.clauseCount();i++){
       for(int j = 0; j< cnf.variableCount(); j++){
           if(cnf.literalInClause(i,j) != NONE){
               clauseSize[i] += 1;
           }
       }
       if(clauseSize[i] == 0){
           containsEmpty = true;
       }
    }
    activeCount = cnf.clauseCount();
}
void AssignedCNF::assignValue(int variable, Trit value){
    assignment[variable] = value;
    //TODO: maintain DTI
}
bool AssignedCNF::getContainsEmpty(){
    return containsEmpty;
}
int AssignedCNF::getActiveCount(){
    return activeCount;
}
void AssignedCNF::printAssignedCNF(){
    const string variableNames = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(activeCount == 0){
        cout << "true\n";
        return;
    }
    for(int i=0;i<cnf.clauseCount();i++){
        if(!clauseTrue[i]){
            cout << "{";
            bool firstLiteral = true;
            for(int j=0;j<cnf.variableCount();j++){
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


