void unitResolution(AssignedCNF &acnf){
    while(acnf.getUnitCount() > 0 && acnf.getEmptyCount() == 0){
        acnf.satisfyUnit();
        //acnf.printAssignedCNF();
    }
}
bool ddpl(AssignedCNF &acnf){
    //TODO: implement ddpl
    return false; 
}
