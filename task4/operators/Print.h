//
// Created by hansi on 11/12/16.
//

#ifndef TASK4_PRINT_H
#define TASK4_PRINT_H


#include "Operator.h"
#include "HashJoin.h"

class Print : Operator {
    Operator& input;
    vector<string>& outVars;
public:
    Print(Operator&, vector<string>&);

    ~Print() override {};

    string produce() override;

    string consume(Operator&) override;
};


#endif //TASK4_PRINT_H
