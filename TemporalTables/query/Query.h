
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

public:
    virtual ~Query() { }

    virtual string toString() const = 0;

    virtual string generateQueryCode() = 0;

    bool shouldExplain() {
        return explain;
    }
};


#endif //TASK5_QUERY_H
