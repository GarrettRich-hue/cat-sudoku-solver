#include "Formula.h"
#include "CNF.h"
#include <sstream>
#include <algorithm>
#include <cassert>
using namespace std;
And::And(Formula* pLeft, Formula* pRight){
    left = pLeft;
    right = pRight;
}
Or::Or(Formula* pLeft, Formula* pRight){
    left = pLeft;
    right = pRight;
}
Not::Not(Formula* pBody){
    body = pBody;
}
Variable::Variable(int pVar){
    variable = pVar;
}
void And::accept(FormulaVisitor* v){
    v->visit(this);
}
void Or::accept(FormulaVisitor* v){
    v->visit(this);
}

void Not::accept(FormulaVisitor* v){
    v->visit(this);
}

void Variable::accept(FormulaVisitor* v){
    v->visit(this);
}

Formula* NNFVisitor::toNNF(Formula* form){
    NNFVisitor nnf;
    form->accept(&nnf);
    return nnf.getResult();
}
void NNFVisitor::visit(And* land) {
    if(negating){
        Or* lor = new Or(land->left, land->right);
        negating = true;
        lor->left->accept(this);
        lor->left = formula;
        negating = true;
        lor->right->accept(this);
        lor->right = formula;
        delete land;
        formula= lor;
    }
    else{
        land->left->accept(this);
        land->left = formula;
        land->right->accept(this);
        land->right = formula;
        formula = land;
    }
}
void NNFVisitor::visit(Or* lor) {
    if(negating){
        And* land = new And(lor->left, lor->right);
        negating = true;
        land->left->accept(this);
        land->left = formula;
        negating = true;
        land->right->accept(this);
        land->right = formula;
        delete lor;
        formula = land;
    }
    else{
        lor->left->accept(this);
        lor->left = formula;
        lor->right->accept(this);
        lor->right = formula;
        formula= lor;
    }
}
void NNFVisitor::visit(Not* lnot)  {
    Formula* body = lnot->body;
    bool newNegating =!negating; 
    negating = newNegating;
    body->accept(this);
    Formula* form = formula;
    delete lnot;
    formula= form;
    negating = !newNegating;
}
void NNFVisitor::visit(Variable* variable)  {
    if(negating){
        Not* lnot = new Not(variable);
        formula = lnot;
    }
    else{
        formula= variable;
    }
}
Formula* NNFVisitor::getResult(){
    return formula;
}
NNFVisitor::NNFVisitor(){
    negating = false;
}
FormulaStringifyVisitor::FormulaStringifyVisitor(){
}
void FormulaStringifyVisitor::visit(And* land){
    ss << "(";
    land->left->accept(this);
    ss << " && ";
    land->right->accept(this);
    ss << ")";
}

void FormulaStringifyVisitor::visit(Or* lor){
    ss << "(";
    lor->left->accept(this);
    ss << " || ";
    lor->right->accept(this);
    ss << ")";
}
void FormulaStringifyVisitor::visit(Not* lnot){
    ss << "¬(";
    lnot->body->accept(this);
    ss << ")";
}
void FormulaStringifyVisitor::visit(Variable* variable){
    ss << "" << findVariableName(variable->variable) << "";
}
string FormulaStringifyVisitor::getResult(){
    return ss.str();
}
ToCNFVisitor::ToCNFVisitor(){
    expanding = false;
}
void ToCNFVisitor::visit(Not* lnot){
    if(expanding){ // everything is already in CNF, do not need to worry
        formula = lnot;
        return;
    }
    // The formula might not be in negation normal form! So try converting it.
    NNFVisitor nnf;
    lnot->accept(&nnf);
    Formula* result = nnf.getResult();
    formula = result;
    class NotVisitor: public FormulaVisitor{
        private:
            bool isNot;
        public:
            NotVisitor(){
                isNot= false;
            }
            void visit(And* f) override{isNot = false;}
            void visit(Or* f) override{isNot= false;}
            void visit(Not* f) override{isNot= true;}
            void visit(Variable* f) override{isNot= false;}
            bool getResult(){return isNot;}
    };
    NotVisitor nv;
    result -> accept(&nv);
    if(!nv.getResult()){ // if not originally in negation normal form
        result->accept(this); //convert to cnf
        formula = this -> formula;
    }
}
void ToCNFVisitor::visit(Variable* variable){
    //already in cnf
    formula = variable;
}
void ToCNFVisitor::visit(And* land){
    if(expanding){ //children are already in CNF, do not worry
        formula = land;
    }
    else{
        // convert children to CNF
        land->left->accept(this);
        land->left = formula;
        expanding = false;
        land->right->accept(this);
        land->right = formula;
        formula = land;
    }
}
void ToCNFVisitor::visit(Or* lor){
    if(!expanding){ // children are not in cnf, will need to convert them
        expanding = false;
        lor->left->accept(this);
        lor->left = formula;
        expanding = false;
        lor->right->accept(this);
        lor->right = formula;
    }
    class ConjunctVisitor: public FormulaVisitor{
        private:
            bool isConj;
            And* conjunct;
        public:
            ConjunctVisitor(){
                isConj= false;
            }
            void visit(And* f) override{isConj= true; conjunct = f;}
            void visit(Or* f) override{isConj= false;}
            void visit(Not* f) override{isConj= false;}
            void visit(Variable* f) override{isConj= false;}
            bool isConjunct(){return isConj;}
            And* getConjunct(){return conjunct;}
    };
    ConjunctVisitor cv;
    lor->left->accept(&cv);
    if(cv.isConjunct()){
        // lor = (X + Y) * Z
        And* leftConj = cv.getConjunct(); // (X+Y)
        Or* lorLeft = new Or(leftConj->left, lor->right); // X * Z
        Or* lorRight = new Or(leftConj->right, copyFormula(lor->right));// Y * Z 
        leftConj->left = lorLeft;
        leftConj->right = lorRight;
        //  X*Z+Y*Z
        // delete lor;
        expanding = true;
        leftConj -> left -> accept(this);
        leftConj -> left = formula;
        expanding = true;
        leftConj -> right -> accept(this);
        leftConj -> right = formula;
        formula = leftConj;
    }
    else{
        lor->right->accept(&cv);
        if(cv.isConjunct()){
            // lor = X *(Y+Z)
            And* rightConj = cv.getConjunct();// (Y+Z)
            Or* lorLeft = new Or(lor->left, rightConj->left); // X*Y
            Or* lorRight = new Or(copyFormula(lor->left), rightConj->right); // X*Z
            rightConj->left= lorLeft;
            rightConj->right= lorRight;
            // rightConj X*Y + X*Z
            // delete lor;
            expanding = true;
            rightConj -> left -> accept(this);
            rightConj -> left = formula;
            expanding = true;
            rightConj -> right -> accept(this);
            rightConj -> right = formula;
            formula = rightConj;
        }
        else{
            formula = lor;
        }
    }
}
Formula* ToCNFVisitor::getResult(){
    return formula;
}
Formula* ToCNFVisitor::toCNF(Formula* form){
    form = NNFVisitor::toNNF(form);
    ToCNFVisitor tcnf;
    form->accept(&tcnf);
    return tcnf.getResult();
}
void printFormula(Formula* formula){
    FormulaStringifyVisitor fsv;
    formula->accept(&fsv);
    cout << fsv.getResult() << "\n";
}
FormulaCopyVisitor::FormulaCopyVisitor(){}
void FormulaCopyVisitor::visit(And* land){
    land->left->accept(this);
    Formula* leftCopy = formula;
    land->right->accept(this);
    Formula* rightCopy = formula;
    formula = new And(leftCopy, rightCopy);
}
void FormulaCopyVisitor::visit(Or* lor){
    lor->left->accept(this);
    Formula* leftCopy = formula;
    lor->right->accept(this);
    Formula* rightCopy = formula;
    formula = new Or(leftCopy, rightCopy);
}
void FormulaCopyVisitor::visit(Not* lnot){
    lnot->body->accept(this);
    formula = new Not(formula);
}
void FormulaCopyVisitor::visit(Variable* variable){
    formula = new Variable(variable->variable);
}
Formula* FormulaCopyVisitor::getResult(){
    return formula;
}
Formula* copyFormula(Formula* formula){
    FormulaCopyVisitor fcv;
    formula->accept(&fcv);
    return fcv.getResult();
}
CNF CNFBuildVisitor::buildCNF(int maxVariables, Formula* form){
    Formula* formCnf = ToCNFVisitor::toCNF(form);
    CNFFactory fact(maxVariables);
    CNFBuildVisitor cbv(fact);
    formCnf->accept(&cbv);
    return fact.makeCNF();
}
CNFBuildVisitor::CNFBuildVisitor(CNFFactory& cnffacp):
    cnffac(cnffacp),
    clause(0,NONE),
    trueClause(true),
    inOr(false),
    inNot(false)
{
    clause.resize(cnffac.variableCount());
    fill(clause.begin(), clause.end(), NONE);
}
void CNFBuildVisitor::visit(And* land){
    assert(!inOr && !inNot);

    // explore left
    inOr = false;
    inNot= false;
    trueClause = false;
    fill(clause.begin(), clause.end(), NONE);
    land->left->accept(this);

    //explore right
    inOr = false;
    inNot = false;
    trueClause = false;
    fill(clause.begin(), clause.end(), NONE);
    land->right->accept(this);
}
void CNFBuildVisitor::visit(Or* lor){
    bool topOr = !inOr;
    assert(!inNot);
    inNot = false;
    inOr = true;
    if(!trueClause) lor->left->accept(this);

    inNot = false;
    inOr = true;
    if(!trueClause) lor->right->accept(this);

    if(topOr && !trueClause){
        cnffac.addClause(clause);
    }
}
void CNFBuildVisitor::visit(Not* lnot){
    inNot = true;
    lnot->body->accept(this);
}
void CNFBuildVisitor::visit(Variable* variable){
    Trit assignLiteral = boolToTrit(!inNot);
    trueClause = trueClause || (clause[variable->variable] != NONE && clause[variable->variable] != assignLiteral);
    clause[variable->variable] = assignLiteral;
}
FormulaFreeVisitor::FormulaFreeVisitor(){} 

void FormulaFreeVisitor::visit(And* land){
    land->left->accept(this);
    land->right->accept(this);
    delete land;
}
void FormulaFreeVisitor::visit(Or* lor){
    lor->left->accept(this);
    lor->right->accept(this);
    delete lor;
}
void FormulaFreeVisitor::visit(Not* lnot){
    lnot->body->accept(this);
    delete lnot;
}
void FormulaFreeVisitor::visit(Variable* variable){
    delete variable;
}
ConjunctionFactory::ConjunctionFactory(){
    buildingEmpty = true;
}
void ConjunctionFactory::add(Formula* form){
    if(buildingEmpty){
        building = form;
        buildingEmpty = false;
    }
    else{
        building = new And(building, form);
    }
}
Formula* ConjunctionFactory::result(){
    assert(!buildingEmpty);
    return building;
}
