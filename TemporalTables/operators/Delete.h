#ifndef OP_DELETE_H
#define OP_DELETE_H

#include "Operator.h"
#include <sstream>
#include <iostream>
#include "../parser/IU.h"
#include "../query/Query.h"

using fieldType = pair<IU*, string>;

class Delete : Operator {
    Operator& input;
    selectionType selections;
    Schema::Relation rel;

public:
    Delete(Operator&, selectionType selections, Schema::Relation);

    string produce() override;

    string consume(Operator&) override;
};


#endif //TASK4_PRINT_H
