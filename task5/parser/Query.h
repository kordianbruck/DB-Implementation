//
// Created by hansi on 11/12/16.
//

#ifndef TASK4_QUERY_H
#define TASK4_QUERY_H

#include <vector>
#include <string>
#include <tuple>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stack>
#include <memory>

#include "Schema.hpp"
#include "IU.h"

using namespace std;
using conditionType = tuple<string, string>;

class Query {
    friend class QueryParser;

    vector<string> projection;
    vector<string> relation;
    vector<conditionType> selection;
    vector<conditionType> joinConditions;
    Schema* schema;

    vector<tuple<IU*, string>> getSelections(Operator*);

    vector<tuple<IU*, IU*>> getJoinConditions(Operator*, Operator*);

public:
    Query(Schema* s) : schema(s) { }

    string toString() const;

    string generateQueryCode();
};


#endif //TASK4_QUERY_H
