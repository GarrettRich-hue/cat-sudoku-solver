#ifndef CNF_H
#define CNF_H
#include <vector>
using namespace std;

enum Trit {
    TRUE,
    FALSE,
    NONE
};
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
        vector<bool> clauseTrue;
        vector<int> clauseSize;
        bool containsEmpty;
        int activeCount;
    public:
        AssignedCNF(CNF &pncf);
        void assignValue(int variable, Trit value);
        bool getContainsEmpty();
        int getActiveCount();
        void printAssignedCNF();
};
#include "CNF.cpp"
#endif
