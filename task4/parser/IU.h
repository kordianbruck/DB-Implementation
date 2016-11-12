//
// Created by hansi on 11/12/16.
//

#ifndef TASK4_IU_H
#define TASK4_IU_H

#include <string>
#include "../operators/TableScan.h"

using namespace std;

struct IU {
    IU(TableScan* rel, Schema::Relation::Attribute* attr) : rel(rel), attr(attr) {

    }

    TableScan* rel;
    Schema::Relation::Attribute* attr;
};

#endif //TASK4_IU_H
