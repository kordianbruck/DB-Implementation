#ifndef TASK4_SELECTION_H
#define TASK4_SELECTION_H


#include "Operator.h"
#include "TableScan.h"

class Selection:public  Operator  {
    shared_ptr<Operator> input;
    vector<tuple<IU*, string>> conditions;

public:
    Selection(shared_ptr<Operator> input, vector<tuple<IU*, string>> condition);

    string produce() override;
    string consume(Operator&) override;
};


#endif //TASK4_SELECTION_H
