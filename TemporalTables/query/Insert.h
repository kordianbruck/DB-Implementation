
#ifndef QUERYINSERT_H
#define QUERYINSERT_H

#include "Query.h"

using namespace std;

class QueryInsert : public Query {
    friend class SQLParser;

    string relation;
    vector<conditionType> fields;

    Schema* schema;

public:
    QueryInsert(Schema* s) : schema(s) { }
    ~QueryInsert() { }
    virtual string toString() const;

    virtual string generateQueryCode();
};


#endif //TASK4_QUERY_H
