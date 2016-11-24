//
// Created by hansi on 11/12/16.
//

#ifndef TASK4_PRINT_H
#define TASK4_PRINT_H


#include "Operator.h"
#include "HashJoin.h"

class Print : Operator {
    Operator& input;
    vector<IU*>& outVars;
public:
    Print(Operator&, vector<IU*>&);

    ~Print() override {
        delete &input;
    };

    string produce() override;

    string consume(Operator&) override;
};


#endif //TASK4_PRINT_H
