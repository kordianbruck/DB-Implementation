#ifndef TASK4_SELECTION_H
#define TASK4_SELECTION_H


#include "Operator.h"
#include "TableScan.h"
#include "../query/Query.h"
#include "../utils/Types.hpp"
#include "../parser/IU.h"

class Selection : public Operator {
    shared_ptr<Operator> input;
    selectionType conditions;

    Timestamp sysTimeStart = Timestamp::null();
    IU* sysTimeStartIU = nullptr;
    Timestamp sysTimeEnd = Timestamp::null();
    IU* sysTimeEndIU = nullptr;

    int paramOffset;

public:
    Selection(shared_ptr<Operator>, selectionType, Timestamp = Timestamp::null(), Timestamp = Timestamp::null(), int = 0);

    string produce() override;

    string consume(Operator&) override;

};


#endif //TASK4_SELECTION_H
