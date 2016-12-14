//
// Created by hansi on 11/12/16.
//

#ifndef TASK4_TABLESCAN_H
#define TASK4_TABLESCAN_H


#include "Operator.h"
#include "../parser/Schema.hpp"


class TableScan: public Operator  {
    Schema::Relation& relation;

public:
    TableScan(Schema::Relation&);
    ~TableScan() override;

    string produce() override;
    string consume(Operator&) override;

    IU* getIU(const string& name);
};


#endif //TASK4_TABLESCAN_H
