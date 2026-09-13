#ifndef FORMULA_H
#define FORMULA_H
#include "CNF.h"
#include <sstream>
#include <string>
#include <iostream>

class FormulaVisitor;
class Formula;
class And;
class Or;
class Not;
class Variable;

class Formula{
    public:
        virtual void accept(FormulaVisitor* v){};
};

class And: public Formula{
    public:
        And(Formula*, Formula*);
        void accept(FormulaVisitor* v) override;
        Formula* left;
        Formula* right;
};
class Or: public Formula{
    public:
        Or(Formula*, Formula*);
        void accept(FormulaVisitor* v) override;
        Formula* left;
        Formula* right;
};
class Not: public Formula{
    public:
        Not(Formula*);
        void accept(FormulaVisitor* v) override;
        Formula* body;
};
class Variable: public Formula{
    public:
        Variable(int);
        void accept(FormulaVisitor* v) override;
        int variable;
};
class FormulaVisitor{
    public:
        //never to be used directly except by Formula.accept
        virtual void visit(And*){}; 
        virtual void visit(Or*){}; 
        virtual void visit(Not*){}; 
        virtual void visit(Variable*){}; 

};
class NNFVisitor: public FormulaVisitor{
    private:
        bool negating;
        Formula* formula;
    public:
        NNFVisitor();
        void visit(And*) override; 
        void visit(Or*) override; 
        void visit(Not*) override; 
        void visit(Variable*) override; 
        Formula* getResult();

};
class FormulaStringifyVisitor: public FormulaVisitor{
    private:
        stringstream ss;
    public:
        FormulaStringifyVisitor();
        void visit(And*) override; 
        void visit(Or*) override; 
        void visit(Not*) override; 
        void visit(Variable*) override; 
        string getResult();
};
class ToCNFVisitor: public FormulaVisitor{
    private:
        Formula* formula;
        bool expanding; //true if children of the formula just about to be visited are already in cnf
    public:
        ToCNFVisitor();
        void visit(And*) override; 
        void visit(Or*) override; 
        void visit(Not*) override; 
        void visit(Variable*) override; 
        Formula* getResult();
};
void printFormula(Formula*);
class FormulaCopyVisitor: public FormulaVisitor{
    private:
        Formula* formula;
    public:
        FormulaCopyVisitor();
        void visit(And*) override; 
        void visit(Or*) override; 
        void visit(Not*) override; 
        void visit(Variable*) override; 
        Formula* getResult();
};
Formula* copyFormula(Formula*);
#endif
