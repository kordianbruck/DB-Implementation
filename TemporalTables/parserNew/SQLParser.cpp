#include "SQLParser.hpp"

using namespace std;

SQLParser::ParserException::ParserException(const std::string& msg) : runtime_error(msg) {
}

SQLParser::SQLParser(SQLLexer& lexer) : lexer(lexer) {
}

SQLParser::~SQLParser() {
}

void SQLParser::parseSelect(Query& query) {
    SQLLexer::Token token = lexer.getNext();

    //Catch if the user wants the code to be shown instead of executed
    if (token == SQLLexer::Identifier && lexer.isKeyword("explain")) {
        query.explain = true;
        token = lexer.getNext(); //Skip this identifier
    }

    //After that we should have a select - if not catch this here
    if (token != SQLLexer::Identifier || !lexer.isKeyword("select")) {
        throw ParserException("Query should start with SELECT");
    }

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
            query.projection.push_back(lexer.getTokenValue());
        } else if (token == SQLLexer::Star) {
            query.projectAll = true;
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

void SQLParser::parseFrom(Query& query) {
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
            query.relation.push_back(lexer.getTokenValue());
            isRelationParsed = true;
        } else if (token == SQLLexer::Comma) {
            if (!isRelationParsed) {
                throw ParserException("Unexpected Comma in the FROM clause");
            }
            isRelationParsed = false;
        } else if (token == SQLLexer::Eof) {
            if (query.relation.size() > 0) {
                break;
            }
            throw ParserException("Missing relation in FROM clause");
        } else {
            throw ParserException("Unexpected token in the FROM clause");
        }
    }
}

/// Warning: only handles expressions of a form attr1=attr2, or attr = constant
void SQLParser::parseWhere(Query& query) {
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
                throw ParserException("Error in WHERE clause");
            }
        } else if (token == SQLLexer::Equal) {
            if (!isLeftSideReady || isExpressionReady) {
                throw ParserException("Error in WHERE clause");
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
                query.joinConditions.push_back(make_pair(attrLeft, attrRight));
            } else {
                query.selection.push_back(make_pair(attrLeft, constant));
            }
        }
    }

}

unique_ptr<Query> SQLParser::parse(Schema* schema) {
    unique_ptr<Query> q(new Query(schema));

    parseSelect(*q);
    parseFrom(*q);
    parseWhere(*q);

    if (lexer.getNext() != SQLLexer::Eof) {
        throw ParserException("Syntax error in the query");
    }

    return move(q);
}

