
#ifndef QUERYDELETE_H
#define QUERYDELETE_H

#include "Query.h"

using namespace std;

class QueryDelete : public Query {
    friend class SQLParser;

    string relation;
    vector<conditionType> selection;

    Schema* schema;

public:
    QueryDelete(Schema* s) : schema(s) { }
    ~QueryDelete() { }
    virtual string toString() const;

    virtual string generateQueryCode();
};


#endif //TASK4_QUERY_H
