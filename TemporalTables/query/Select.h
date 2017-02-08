#ifndef QUERYSELECT_H
#define QUERYSELECT_H

#include "Query.h"
#include "../utils/Types.hpp"

using namespace std;


class QuerySelect : public Query {
    friend class SQLParser;

    vector<string> projection;
    vector<string> relations;

    bool projectAll = false;
    Timestamp sysTimeStart = Timestamp::null();
    Timestamp sysTimeEnd = Timestamp::null();

    Schema* schema;

public:
    QuerySelect(Schema* s) : schema(s) { }

    ~QuerySelect() { }

    virtual string toString() const;

    virtual string generateQueryCode();
};


#endif //TASK4_QUERY_H
