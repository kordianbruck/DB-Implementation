#ifndef SQLParser_HPP
#define SQLParser_HPP

#include <unordered_map>
#include <string>
#include <exception>
#include <vector>
#include <utility>
#include "SQLLexer.hpp"

class SQLLexer;

class SQLParser {
private:
    SQLLexer& lexer;

    void parseSelect();

    void parseFrom();

    void parseWhere();

public:
    class ParserException : public std::runtime_error {
    public:
        ParserException(const std::string& msg);
    };

    explicit SQLParser(SQLLexer& lexer);

    ~SQLParser();



    // relation in the FROM clause
    struct Relation {
        std::string name; // table name
        std::string binding; // binding
        bool isFullyQualified; // both table name and binding are not empty
        std::string getName() {
            return isFullyQualified ? name + " " + binding : name;
        }
    };

    // attribute of the relations mentioned in SELECT or WHERE
    struct RelationAttribute {
        std::string relation; // relation name
    };

    // constant mentioned in the condition
    enum class Type : unsigned {
        Int, Double, Bool, String
    };

    struct Constant {
        SQLParser::Type type;
        std::string value;
    };

    // result of the parser
    struct Result {
        std::vector<Relation> relations;
        std::vector<RelationAttribute> projections;
        std::vector<std::pair<RelationAttribute, Constant>> selections;
        std::vector<std::pair<RelationAttribute, RelationAttribute>> joinConditions;
    };

    Result parse();

private:
    Result result;
    RelationAttribute parseAttributeName();
    Relation parseRelation();
};

#endif