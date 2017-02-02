#ifndef TASK4_SELECTION_H
#define TASK4_SELECTION_H


#include "Operator.h"
#include "TableScan.h"
#include "../query/Query.h"

class Selection:public  Operator  {
    shared_ptr<Operator> input;
    selectionType conditions;

public:
    Selection(shared_ptr<Operator> input, selectionType condition);

    string produce() override;
    string consume(Operator&) override;
};


#endif //TASK4_SELECTION_H
