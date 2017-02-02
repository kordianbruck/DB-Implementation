#ifndef OP_UPDATE_H
#define OP_UPDATE_H

#include "Operator.h"
#include <sstream>
#include <iostream>
#include "../parser/IU.h"

using fieldType = pair<IU*, string>;

class Update : Operator {
    Operator& input;
    vector<fieldType>& outVars;
public:
    Update(Operator&, vector<fieldType>&);

    string produce() override;

    string consume(Operator&) override;
};


#endif //TASK4_PRINT_H
