
#ifndef QUERYDELETE_H
#define QUERYDELETE_H

#include "Query.h"
#include "../operators/Selection.h"
#include "../operators/Delete.h"

using namespace std;

class QueryDelete : public Query {
    friend class SQLParser;

    string relation;
    Schema* schema;

public:
    QueryDelete(Schema* s) : schema(s) { }
    ~QueryDelete() { }
    virtual string toString() const;

    virtual string generateQueryCode();
};


#endif //TASK4_QUERY_H
