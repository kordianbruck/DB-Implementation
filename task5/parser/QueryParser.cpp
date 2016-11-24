#include "QueryParser.hpp"


namespace keyword {
    const string Select = "select";
    const string From = "from";
    const string Where = "where";
    const string And = "and";
}

namespace literal {
    const char ParenthesisLeft = '(';
    const char ParenthesisRight = ')';
    const char Comma = ',';
    const char Semicolon = ';';
    const char Equals = '=';
}

unique_ptr<Query> QueryParser::parse(const string& query, Schema* schema) {
    unique_ptr<Query> q(new Query(schema));
    string token;
    std::stringstream queryStream(query);
    unsigned line = 1;

    while (queryStream >> token) {
        string::size_type pos;
        string::size_type prevPos = 0;

        while ((pos = token.find_first_of(",;", prevPos)) != string::npos) {
            nextToken(token.substr(prevPos, pos - prevPos), *q);
            nextToken(token.substr(pos, 1), *q);
            prevPos = pos + 1;
        }

        nextToken(token.substr(prevPos), *q);
        if (token.find("\n") != string::npos) {
            ++line;
        }
    }

    return move(q);
}

static bool isIdentifier(const string& str) {
    if (str == keyword::Select || str == keyword::From || str == keyword::Where || str == keyword::And) {
        return false;
    }
    return str.find_first_not_of("abcdefghijklmnopqrstuvwxyz_1234567890") == string::npos;
}

static bool isInt(const string& str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

static bool isValue(const string& str) {
    return isInt(str) || (str[0] == '\'' && str.back() == '\'');
}

static string leftPredicate = "";

void QueryParser::nextToken(const string& token, Query& query) {
    if (getenv("DEBUG")) {
        cerr << token << endl;
    }
    if (token.empty()) {
        return;
    }
    string tok;
    transform(token.begin(), token.end(), back_inserter(tok), ::tolower);
    /*
        Init,
        SelectItem, SelectEnd,
        FromItem, FromEnd,
        WhereLeft, WhereEquals, WhereRight, WhereAnd,
        Semicolon
     */
    switch (state) {
        case State::Semicolon:
            throw ParserError(0, "Cannot continue query after semicolon '" + token + "'");
        case State::Init:
            if (tok == keyword::Select) {
                state = State::SelectItem;
            } else {
                throw ParserError(0, "Expected 'SELECT', found '" + token + "'");
            }
            break;
        case State::SelectItem:
            if (isIdentifier(tok)) {
                query.projection.push_back(tok);
                state = State::SelectEnd;
            } else {
                throw ParserError(0, "Expected 'SELECT' identifier predicate, found '" + token + "'");
            }
            break;
        case State::SelectEnd:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::SelectItem;
            } else if (tok == keyword::From) {
                state = State::FromItem;
            } else {
                throw ParserError(0, "Expected ',' or 'FROM' , found '" + token + "'");
            }
            break;
        case State::FromItem:
            if (isIdentifier(tok)) {
                query.relation.push_back(tok);
                state = State::FromEnd;
            } else {
                throw ParserError(0, "Expected 'FROM' identifier predicate, found '" + token + "'");
            }
            break;
        case State::FromEnd:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::FromItem;
            } else if (tok.size() == 1 && tok[0] == literal::Semicolon) {
                state = State::Semicolon;
            } else if (tok == keyword::Where) {
                state = State::WhereLeft;
            } else {
                throw ParserError(0, "Expected ',', ';' or 'WHERE' , found '" + token + "'");
            }
            break;
        case State::WhereAnd:
            if (tok.size() == 1 && tok[0] == literal::Semicolon) {
                state = State::Semicolon;
            } else if (tok == keyword::And) {
                state = State::WhereLeft;
            } else {
                throw ParserError(0, "Expected ';' or 'AND' , found '" + token + "'");
            }
            break;
        case State::WhereLeft:
            if (isIdentifier(tok)) {
                leftPredicate = tok;
                state = State::WhereEquals;
            } else {
                throw ParserError(0, "Expected left condition predicate, found '" + token + "'");
            }
            break;
        case State::WhereEquals:
            if (tok.size() == 1 && tok[0] == literal::Equals) {
                state = State::WhereRight;
            } else {
                throw ParserError(0, "Expected '=', found '" + token + "'");
            }
            break;
        case State::WhereRight:
            if (isValue(tok)) {
                tok = token;
                replace(tok.begin(), tok.end(), '\'', '"');
                query.selection.push_back(make_tuple(leftPredicate, tok));
                leftPredicate = "";
                state = State::WhereAnd;
            } else if (isIdentifier(tok)) {
                query.joinConditions.push_back(make_tuple(leftPredicate, tok));
                leftPredicate = "";
                state = State::WhereAnd;
            } else {
                throw ParserError(0, "Expected constant or right predicate, found '" + token + "'");
            }
            break;
        default:
            throw ParserError(0, "Missing state in query parser for token '" + token + "'");
            break;
    }
}

void QueryParser::reset() {
    state = State::Init;

}
