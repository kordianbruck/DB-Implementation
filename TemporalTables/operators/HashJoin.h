//
// Created by hansi on 11/12/16.
//

#ifndef TASK4_HASHJOIN_H
#define TASK4_HASHJOIN_H


#include "Operator.h"
#include "TableScan.h"

using namespace std;

class HashJoin : public  Operator {
    Operator& left;
    Operator& right;
    vector<tuple<IU*, IU*>> conditions;
    string mapName;

    set<IU*> intersectDeps();

public:
    HashJoin(Operator& left, Operator& right, vector<tuple<IU*, IU*>> conditions);

    string produce() override;
    string consume(Operator&) override;

};


#endif //TASK4_HASHJOIN_H
