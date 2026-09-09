void unitResolution(AssignedCNF &acnf){
    while(acnf.getUnitCount() > 0 && acnf.getEmptyCount() == 0){
        acnf.satisfyUnit();
        //acnf.printAssignedCNF();
    }
}
bool ddpl(AssignedCNF &acnf){
    unitResolution(acnf);
    if(acnf.getActiveCount() == 0){
        return true;
    }
    else if(acnf.getEmptyCount()>0){
        return false;
    }
    else{
        //choose a literal
        int literal = 0;
        while(acnf.getAssignment(literal) != NONE){
            literal++;
        }
        //branch
        acnf.assignValue(literal, true);
        if(ddpl(acnf)){
            return true;
        }
        acnf.unassignValueUntil(literal);
        acnf.assignValue(literal, false);
        if(ddpl(acnf)){
            return true;
        }
        acnf.unassignValueUntil(literal);
    }
    return false; 
}
