
#ifndef QUERYUPDATE_H
#define QUERYUPDATE_H

#include "Query.h"

using namespace std;

class QueryUpdate : public Query {
    friend class SQLParser;

    string relation;
    vector<conditionType> fields;

    Schema* schema;

public:
    QueryUpdate(Schema* s) : schema(s) { }
    ~QueryUpdate() { }

    virtual string toString() const;

    virtual string generateQueryCode();
};


#endif //TASK4_QUERY_H
