#include "SQLParser.hpp"
#include "../query/Update.h"

using namespace std;

SQLParser::ParserException::ParserException(const std::string& msg) : runtime_error(msg) {
}

SQLParser::SQLParser(SQLLexer& lexer) : lexer(lexer) {
}

SQLParser::~SQLParser() {
}

void SQLParser::parseSelect(QuerySelect* query) {
    SQLLexer::Token token;
    bool emptySelect = true;

    while (true) {
        token = lexer.getNext();
        if (token == SQLLexer::Identifier) {
            if (lexer.isKeyword("from")) {
                lexer.unget(token);
                if (emptySelect) {
                    throw ParserException("Projection should not be empty");
                }
                break;
            }
            query->projection.push_back(lexer.getTokenValue());
        } else if (token == SQLLexer::Star) {
            query->projectAll = true;
        } else if (token == SQLLexer::Comma) {
            if (emptySelect) {
                throw ParserException("Syntax error: Comma after empty expression");
            }
        } else {
            if (emptySelect) {
                throw ParserException("Projection should not be empty");
            }
            lexer.unget(token);
            break;
        }
        emptySelect = false;
    }
}

void SQLParser::parseFrom(QuerySelect* query) {
    SQLLexer::Token token = lexer.getNext();
    if (token != SQLLexer::Identifier || !lexer.isKeyword("from")) {
        throw ParserException("Missing FROM clause");
    }

    bool isRelationParsed = false;
    while (true) {
        token = lexer.getNext();
        if (lexer.isKeyword("where")) {
            lexer.unget(token);
            break;
        }
        if (token == SQLLexer::Identifier) {
            query->relations.push_back(lexer.getTokenValue());
            isRelationParsed = true;
        } else if (token == SQLLexer::Comma) {
            if (!isRelationParsed) {
                throw ParserException("Unexpected Comma in the FROM clause");
            }
            isRelationParsed = false;
        } else if (token == SQLLexer::Eof) {
            if (query->relations.size() > 0) {
                break;
            }
            throw ParserException("Missing relations in FROM clause");
        } else {
            throw ParserException("Unexpected token in the FROM clause");
        }
    }
}

/// Warning: only handles expressions of a form attr1=attr2, or attr = constant
void SQLParser::parseWhere(Query* query) {
    SQLLexer::Token token = lexer.getNext();

    if (token == SQLLexer::Eof) {
        return;
    }

    if (token != SQLLexer::Identifier || !lexer.isKeyword("where")) {
        throw ParserException("Missing WHERE clause");
    }

    bool isLeftSideReady = false;
    bool isExpressionReady = false;
    bool isJoin = false;
    string attrLeft, attrRight;
    string constant;

    while (true) {
        token = lexer.getNext();
        if (token == SQLLexer::Identifier && lexer.isKeyword("and")) {
            if (!isExpressionReady) {
                throw ParserException("Unexpected AND");
            }
            isExpressionReady = false;
            isLeftSideReady = false;
        } else if (token == SQLLexer::Identifier) {
            if (isLeftSideReady) {
                attrRight = lexer.getTokenValue();
                isExpressionReady = true;
                isJoin = true;
            } else if (!isExpressionReady) {
                attrLeft = lexer.getTokenValue();
                isLeftSideReady = true;
            } else {
                throw ParserException("Error in WHERE clause @1");
            }
        } else if (token == SQLLexer::Equal) {
            if (!isLeftSideReady || isExpressionReady) {
                throw ParserException("Error in WHERE clause @2");
            }
            continue;
        } else if (token == SQLLexer::String || token == SQLLexer::Integer) {
            if (!isLeftSideReady || isExpressionReady) {
                throw ParserException("Unexpected String constant in WHERE clause");
            }
            constant = lexer.getTokenValue();
            isExpressionReady = true;
            isJoin = false;
        } else if (token == SQLLexer::Eof) {
            break;
        } else {
            throw ParserException("Unexpected token: " + lexer.getTokenValue());
        }

        if (isExpressionReady) {
            if (isJoin) {
                query->joinConditions.push_back(make_pair(attrLeft, attrRight));
            } else {
                query->selection.push_back(make_pair(attrLeft, constant));
            }
        }
    }

}

void SQLParser::parseInsertColumns(QueryInsert* query) {
    SQLLexer::Token token = lexer.getNext();

    //Check the correct syntax
    if (token != SQLLexer::Identifier || !lexer.isKeyword("into")) {
        throw ParserException("Expected 'INTO' after 'INSERT'");
    }

    while (true) {
        token = lexer.getNext();
        if (token == SQLLexer::Identifier && lexer.isKeyword("values")) {
            lexer.unget(token);
            return;
        } else if (token == SQLLexer::Identifier) {
            query->fields.push_back(make_pair(lexer.getTokenValue(), ""));
        } else {
            throw ParserException("Unexpected token: " + lexer.getTokenValue());
        }
    }

}

void SQLParser::parseInsertValues(QueryInsert* query) {
    SQLLexer::Token token = lexer.getNext();

    //Check the correct syntax
    if (token != SQLLexer::Identifier || !lexer.isKeyword("values")) {
        throw ParserException("Expected 'VALUES' with a 'INSERT' query");
    }

    int currentColumn = 0;
    while (true) {
        token = lexer.getNext();
        if (token == SQLLexer::Eof) {
            break;
        } else if (token == SQLLexer::String || token == SQLLexer::Integer) {
            if (currentColumn >= query->fields.size()) {
                throw ParserException("Columns and Values count mismatch");
            }
            query->fields[currentColumn].second = lexer.getTokenValue();
            currentColumn++;
        } else {
            throw ParserException("Unexpected token: " + lexer.getTokenValue());
        }
    }

    if (currentColumn != query->fields.size() - 1) {
        throw ParserException("Columns and Values count mismatch");
    }
}

void SQLParser::parseSet(QueryUpdate* query) {
    SQLLexer::Token token = lexer.getNext();

    if (token == SQLLexer::Eof || token != SQLLexer::Identifier || !lexer.isKeyword("set")) {
        throw ParserException("Missing 'SET' clause");
    }

    bool isLeftSideReady = false;
    bool isExpressionReady = false;
    string attr;
    string constant;

    while (true) {
        token = lexer.getNext();
        if (token == SQLLexer::Identifier && lexer.isKeyword("where")) {
            lexer.unget(token);
            return;
        } else if (token == SQLLexer::Comma) {
            if (!isExpressionReady) {
                throw ParserException("Unexpected comment ");
            }
            isExpressionReady = false;
            isLeftSideReady = false;
        } else if (token == SQLLexer::Identifier) {
            if (!isExpressionReady) {
                attr = lexer.getTokenValue();
                isLeftSideReady = true;
            } else {
                throw ParserException("Error in SET clause @1");
            }
        } else if (token == SQLLexer::Equal) {
            if (!isLeftSideReady || isExpressionReady) {
                throw ParserException("Error in SET clause @2");
            }
            continue;
        } else if (token == SQLLexer::String || token == SQLLexer::Integer) {
            if (!isLeftSideReady || isExpressionReady) {
                throw ParserException("Unexpected String constant in SET clause");
            }
            constant = lexer.getTokenValue();
            isExpressionReady = true;
        } else if (token == SQLLexer::Eof) {
            break;
        } else {
            throw ParserException("Unexpected token: " + lexer.getTokenValue());
        }
        if (isExpressionReady) {
            query->fields.push_back(make_pair(attr, constant));
        }
    }

}

Query* SQLParser::parse(Schema* schema) {
    SQLLexer::Token token = lexer.getNext();

    //Create a magic ptr for simplicity sake
    Query* query;
    //We need to retain this in a tmp var as we can't yet assign it to the query
    bool queryExplain = false;

    //Catch if the user wants the code to be shown instead of executed
    if (token == SQLLexer::Identifier && lexer.isKeyword("explain")) {
        queryExplain = true;
        token = lexer.getNext(); //Skip to the next identifier
    }

    // Make sure we have a identifier
    if (token != SQLLexer::Identifier) {
        throw ParserException("Invalid expression found at query start");
    }

    // Determine what kind query we have
    if (lexer.isKeyword("select")) {
        query = new QuerySelect(schema);

        parseSelect((QuerySelect*) query);
        parseFrom((QuerySelect*) query);
        parseWhere(query);
    } else if (lexer.isKeyword("update")) {
        //UPDATE table_name SET column1=value1,column2=value2,... WHERE some_column=some_value;
        query = new QueryUpdate(schema);
        if (lexer.getNext() == SQLLexer::Identifier) {
            dynamic_cast<QueryUpdate&>(*query).relation = lexer.getTokenValue();
        } else {
            throw ParserException("Expected relation after 'UPDATE'");
        }
        parseSet((QueryUpdate*) query);
        parseWhere(query);
    } else if (lexer.isKeyword("insert")) {
        //INSERT INTO table_name VALUES (value1,value2,value3,...);
        //INSERT INTO table_name (column1,column2,column3,...) VALUES (value1,value2,value3,...);
        query = new QueryInsert(schema);

        parseInsertColumns((QueryInsert*) query);
        parseInsertValues((QueryInsert*) query);
    } else if (lexer.isKeyword("delete")) {
        //DELETE FROM table_name WHERE some_column=some_value;
        query = new QueryDelete(schema);

        //Check that we have the FROM in there
        if (lexer.getNext() != SQLLexer::Identifier || !lexer.isKeyword("from")) {
            throw ParserException("Expected 'FROM' after 'DELETE'");
        }

        //We need a relation we want to delete from
        if (lexer.getNext() == SQLLexer::Identifier) {
            ((QueryDelete*) query)->relation = lexer.getTokenValue();
        } else {
            throw ParserException("Expected relation after 'DELETE FROM'");
        }

        //We should have some conditions in there
        parseWhere(query);
    } else {
        throw ParserException("Unkown query type");
    }

    //We can only assign the explain flag after the object has been created
    if (queryExplain) {
        query->explain = true;
    }

    //Check that there is nothing left
    if (lexer.getNext() != SQLLexer::Eof) {
        throw ParserException("Syntax error in the query");
    }


    return query;
}



