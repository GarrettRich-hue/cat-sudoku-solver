#ifndef CNF_H
#define CNF_H
#include <vector>
using namespace std;

enum Trit {
    TRUE,
    FALSE,
    NONE
};
Trit boolToTrit(bool b);
bool tritToBool(Trit t);
class CNF{
    private:
        int varCount;
        vector<vector<Trit>> clauses;
    public:
        CNF(vector<vector<Trit>> initClauses);
        Trit literalInClause(int clause, int variable);
        int clauseCount();
        int variableCount();
};
class CNFFactory{
    private:
        int varCount;
        vector<vector<Trit>> clauses = {};
    public:
        CNFFactory(int pVarCount);
        void addClause(vector<Trit> clause);
        int variableCount();
        CNF makeCNF();
};
class AssignedCNF{
    private:
        CNF &cnf;
        vector<Trit> assignment;
        vector<int> assignmentStack;
        int stackTop;
        vector<int> clauseSatisfiedBy;
        vector<int> clauseSize;
        int emptyCount;
        int activeCount;
    public:
        AssignedCNF(CNF &pncf);
        void assignValue(int variable, bool value);
        void unassignValue();
        int getEmptyCount();
        bool getContainsEmpty();
        int getActiveCount();
        bool isTrueClause(int clause);
        void printAssignedCNF();
};
#include "CNF.cpp"
#endif
