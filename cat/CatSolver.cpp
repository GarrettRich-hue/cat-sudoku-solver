#include "CatSolver.h"
#include "Formula.h"
#include "CNF.h"
#include <vector>
#include <cassert>
#include <initializer_list>
using namespace std;

CatInstance::CatInstance(int n, initializer_list<initializer_list<int>> initColours):
    size(n),
    regionColours(n, vector<int>(n, 0)),
    catPlaced(n, vector<Trit>(n, NONE))
{
    assert(n>=2);
    int i = 0;
    for(initializer_list<int> row: initColours){
        int j = 0;
        for(int col: row){
            assert(0 <= i && i<n && 0<=j && j<n);
            regionColours[i][j] = col;
            j++;
        }
        i++;
    }
}
Formula* CatInstance::asFormula(){
    int n = size;
    vector<vector<int>> x(n, vector<int>(n, 0));
    for(int row=0;row<n;row++){
        for(int col=0;col<n;col++){
            x[row][col] = col + row*n;
        }
    }
    // enforce at least 1 per row
    Formula* rowLower;
    for(int row=0;row<n;row++){
        Formula* rowFormula = new Variable(x[row][0]);
        for(int col = 1;col<n;col++){
            rowFormula = new Or(new Variable(x[row][col]), rowFormula);
        }
        if(row == 0){
            rowLower = rowFormula;
        }
        else{
            rowLower = new And(rowLower, rowFormula);
        }
    }

    // enforce at least 1 per column
    Formula* colLower;
    for(int col=0;col<n;col++){
        Formula* colFormula = new Variable(x[0][col]);
        for(int row = 1;row<n;row++){
            colFormula = new Or(new Variable(x[row][col]), colFormula);
        }
        if(col == 0){
            colLower = colFormula;
        }
        else{
            colLower = new And(colLower, colFormula);
        }
    }
    
    //enforce at most 1 per row
    ConjunctionFactory rowHigherFact;
    for(int row=0;row<n;row++){
        ConjunctionFactory rowFormulaFact;
        for(int col1=0;col1<n;col1++){
            for(int col2=0;col2<n;col2++){
                if(col1 == col2) continue;
                Formula* distinct = new Not(new And(
                            new Variable(x[row][col1]),
                            new Variable(x[row][col2])));
                rowFormulaFact.add(distinct);
            }
        }
        rowHigherFact.add(rowFormulaFact.result());
    }

    //enforce at most 1 per col
    ConjunctionFactory colHigherFact;
    for(int col=0;col<n;col++){
        ConjunctionFactory colFormulaFact;
        for(int row1=0;row1<n;row1++){
            for(int row2=0;row2<n;row2++){
                if(row1 == row2) continue;
                Formula* distinct = new Not(new And(
                            new Variable(x[row1][col]),
                            new Variable(x[row2][col])));
                colFormulaFact.add(distinct);
            }
        }
        colHigherFact.add(colFormulaFact.result());
    }

    //enforce no touching along major diagonal
    ConjunctionFactory majorFact;
    for(int row=0;row<n-1;row++){
        for(int col=0;col<n-1;col++){
            Formula* distinct = new Not(new And(
                            new Variable(x[row][col]),
                            new Variable(x[row+1][col+1])
                        ));
            majorFact.add(distinct);
        }
    }
    // enforce no touching along minor diagonal
    ConjunctionFactory minorFact;
    for(int row=1;row<n;row++){
        for(int col=0;col<n-1;col++){
            Formula* distinct = new Not(new And(
                            new Variable(x[row][col]),
                            new Variable(x[row-1][col+1])
                        ));
            minorFact.add(distinct);
        }
    }

    // enforce at most 1 per region
    ConjunctionFactory regionFact;
    vector<vector<int>> regionVariables(n, vector<int>(0, 0));
    for(int row = 0;row<n;row++){
        for(int col=0;col<n;col++){
            int current = regionColours[row][col]; // current region
            // add to list of regions until current is included
            while(regionVariables.size()<= current){
                regionVariables.push_back(vector<int>(0, 0));
            }
            vector<int> region = regionVariables[current];//all prior variables in the current region
            // make sure current cell is not true at the same time as a variable from another region
            for(int i=0;i<region.size();i++){
                regionFact.add(new Not(new And(
                                new Variable(x[row][col]),
                                new Variable(region[i])
                                )));
            }
            regionVariables[current].push_back(x[row][col]);
        }
    }

    ConjunctionFactory finalFact;
    finalFact.add(rowLower);
    finalFact.add(colLower); 
    finalFact.add(rowHigherFact.result());
    finalFact.add(colHigherFact.result());
    finalFact.add(majorFact.result());
    finalFact.add(minorFact.result());
    finalFact.add(regionFact.result());
    return finalFact.result();
}

