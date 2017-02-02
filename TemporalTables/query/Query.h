
#ifndef TASK5_QUERY_H
#define TASK5_QUERY_H

#include <vector>
#include <string>
#include <tuple>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stack>
#include <memory>

#include "../parser/Schema.hpp"
#include "../parser/IU.h"

using conditionType = pair<string, string>;

class Query {
    friend class SQLParser;

    bool explain = false;

protected:
    vector<conditionType> selection;
    vector<conditionType> joinConditions;

    vector<tuple<IU*, string>> getSelections(Operator* op) {
        vector<tuple<IU*, string>> conditions;
        auto operatorIUs = op->getProduced();

        //Iterate through all selections
        for (auto& s : selection) {
            //Check all produced operators
            for (auto iu : operatorIUs) {
                if (iu->attr->name == get<0>(s)) { // if they match, add the iu and selection value to the list
                    conditions.emplace_back(make_tuple(iu, get<1>(s)));
                }
            }
        }

        return conditions;
    }

    vector<tuple<IU*, IU*>> getJoinConditions(Operator* left, Operator* right) {
        vector<tuple<IU*, IU*>> conditions;
        auto leftIUs = left->getProduced();
        auto rightIUs = right->getProduced();

        for (auto& condition : joinConditions) { //Check all the predicates we got
            for (auto lIU : leftIUs) { //Check if the condition is in the left
                if (lIU->attr->name == get<0>(condition) || lIU->attr->name == get<1>(condition)) { //Account for any order in the condition
                    for (auto rIU : rightIUs) { //And also in the right
                        if (rIU->attr->name == get<1>(condition) || rIU->attr->name == get<0>(condition)) {
                            conditions.emplace_back(make_tuple(lIU, rIU));
                        }
                    }
                }
            }
        }
        return conditions;
    }

public:
    virtual ~Query() { }

    virtual string toString() const = 0;

    virtual string generateQueryCode() = 0;

    bool shouldExplain() {
        return explain;
    }
};


#endif //TASK5_QUERY_H
