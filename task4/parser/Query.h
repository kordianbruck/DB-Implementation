//
// Created by hansi on 11/12/16.
//

#ifndef TASK4_QUERY_H
#define TASK4_QUERY_H

#include <vector>
#include <string>
#include "Schema.hpp"

using namespace std;

class Query {

    std::vector<Schema::Relation> relations;

    //ToDo

    std::string toString() const;

    std::string generateQueryCode() const;
};


#endif //TASK4_QUERY_H
