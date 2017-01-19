#include "SchemaParser.hpp"

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <stdlib.h>
#include <algorithm>

namespace keyword {
    const std::string Primary = "primary";
    const std::string Key = "key";
    const std::string Create = "create";
    const std::string Table = "table";
    const std::string Index = "index";
    const std::string Not = "not";
    const std::string Null = "null";

    const std::string Integer = "integer";
    const std::string Numeric = "numeric";
    const std::string Char = "char";
    const std::string Varchar = "varchar";
    const std::string Date = "date";
    const std::string Timestamp = "timestamp";
    const std::string Datetime = "datetime";

    //System Versioning
    const std::string With = "with";
    const std::string System_Versioning = "system_versioning";
    const std::string On = "on";
    const std::string Off = "off";
    const std::string Period = "period";
    const std::string For = "for";
    const std::string System_Time = "system_time";
    const std::string Generated = "generated";
    const std::string Always = "always";
    const std::string As = "as";
    const std::string Row = "row";
    const std::string Start = "start";
    const std::string End = "end";
}

namespace literal {
    const char ParenthesisLeft = '(';
    const char ParenthesisRight = ')';
    const char Comma = ',';
    const char Semicolon = ';';
    const char Equals = '=';
}

static Schema::Relation::Index* lastIndex = 0;
static Schema::Relation* lastIndexRelation = 0;

SchemaParser::~SchemaParser() {
    if (lastIndex != NULL) {
        delete lastIndex;
    }
}

Schema* SchemaParser::parse() {
    std::string token;
    unsigned line = 1;
    Schema* s = new Schema();
    in.open(fileName.c_str());
    if (!in.is_open()) {
        throw ParserError(line, "cannot open file '" + fileName + "'");
    }
    while (in >> token) {
        std::string::size_type pos;
        std::string::size_type prevPos = 0;

        while ((pos = token.find_first_of(",)(;", prevPos)) != std::string::npos) {
            nextToken(line, token.substr(prevPos, pos - prevPos), *s);
            nextToken(line, token.substr(pos, 1), *s);
            prevPos = pos + 1;
        }
        nextToken(line, token.substr(prevPos), *s);
        if (token.find("\n") != std::string::npos) {
            ++line;
        }
    }
    in.close();


    return s;
}

static bool isIdentifier(const std::string& str) {
    if (
            str == keyword::Primary ||
            str == keyword::Key ||
            str == keyword::Table ||
            str == keyword::Create ||
            str == keyword::Index ||
            str == keyword::Not ||
            str == keyword::Null ||
            str == keyword::Integer ||
            str == keyword::Varchar ||
            str == keyword::Timestamp ||
            str == keyword::Date ||
            str == keyword::Numeric ||
            str == keyword::Char ||

            //Sys Versioning
            str == keyword::With ||
            str == keyword::System_Versioning ||
            str == keyword::On ||
            str == keyword::Off ||
            str == keyword::Period ||
            str == keyword::For ||
            str == keyword::System_Time ||
            str == keyword::Generated ||
            str == keyword::Always ||
            str == keyword::As ||
            str == keyword::Row ||
            str == keyword::Start ||
            str == keyword::End
            ) {
        return false;
    }
    return str.find_first_not_of("abcdefghijklmnopqrstuvwxyz_1234567890") == std::string::npos;
}

static bool isInt(const std::string& str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

void SchemaParser::nextToken(unsigned line, const std::string& token, Schema& schema) {
    //std::cerr << line << ": " << token << std::endl;

    if (token.empty()) {
        return;
    }
    std::string tok;
    std::transform(token.begin(), token.end(), std::back_inserter(tok), ::tolower);

    switch (state) {
        case State::Semicolon: /* fallthrough */
        case State::Init:
            if (tok == keyword::Create) {
                state = State::Create;
            } else {
                throw ParserError(line, "Expected 'CREATE', found '" + token + "'");
            }
            break;
        case State::Create:
            if (tok == keyword::Table) {
                state = State::Table;
            } else if (tok == keyword::Index) {
                state = State::Index;
            } else {
                throw ParserError(line, "Expected 'TABLE' or 'INDEX', found '" + token + "'");
            }
            break;
        case State::Table:
            if (isIdentifier(tok)) {
                state = State::TableName;
                schema.relations.push_back(Schema::Relation(token));
            } else {
                throw ParserError(line, "Expected TableName, found '" + token + "'");
            }
            break;
        case State::TableName:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisLeft) {
                state = State::CreateTableBegin;
            } else {
                throw ParserError(line, "Expected '(', found '" + token + "'");
            }
            break;
        case State::Index:
            if (isIdentifier(tok)) {
                state = State::IndexName;
                if (lastIndex != NULL) {
                    delete lastIndex;
                }
                lastIndex = new Schema::Relation::Index(token);
            } else {
                throw ParserError(line, "Expected IndexName, found '" + token + "'");
            }
            break;
        case State::IndexName:
            if (tok == keyword::On) {
                state = State::IndexOn;
            } else {
                throw ParserError(line, "Expected 'on', found '" + token + "'");
            }
            break;
        case State::IndexOn:
            if (isIdentifier(tok)) {
                if (!lastIndex) {
                    throw ParserError(line, "No index assigned at character '" + token + "'");
                }
                lastIndexRelation = &schema.findRelation(tok);
                lastIndexRelation->indexes.push_back(*lastIndex);
                state = State::IndexTableName;
            } else {
                throw ParserError(line, "Expected IndexName, found '" + token + "'");
            }
            break;
        case State::IndexTableName:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisLeft) {
                state = State::IndexBegin;
            } else {
                throw ParserError(line, "Expected '(', found '" + token + "'");
            }
            break;
        case State::IndexBegin:
            if (isIdentifier(tok)) {
                lastIndexRelation->indexes.back().keys.push_back(lastIndexRelation->findAttributeIndex(token));
                state = State::IndexKey;
            } else {
                throw ParserError(line, "Expected a column name for the index, found '" + token + "'");
            }
            break;
        case State::IndexKey:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::IndexBegin;
            } else if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::IndexEnd;
            } else {
                throw ParserError(line, "Expected ',' or ')', found '" + token + "'");
            }
            break;
        case State::IndexEnd:
            if (tok.size() == 1 && tok[0] == literal::Semicolon) {
                state = State::Semicolon;
            } else {
                throw ParserError(line, "Expected ';', found '" + token + "'");
            }
            break;
        case State::Separator: /* fallthrough */
        case State::CreateTableBegin:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::CreateTableEnd;
            } else if (tok == keyword::Primary) {
                state = State::Primary;
            } else if (tok == keyword::Period) {
                state = State::PeriodStart;
            } else if (isIdentifier(tok)) {
                schema.relations.back().attributes.push_back(Schema::Relation::Attribute());
                schema.relations.back().attributes.back().name = token;
                state = State::AttributeName;
            } else {
                throw ParserError(line, "Expected attribute definition, primary key definition or ')', found '" + token + "'");
            }
            break;
        case State::CreateTableEnd:
            if (tok.size() == 1 && tok[0] == literal::Semicolon) {
                state = State::Semicolon;
            } else if (tok == keyword::With) {
                state = State::WithStart;
            } else {
                throw ParserError(line, "Expected ';' OR WITH, found '" + token + "'");
            }
            break;
        case State::Primary:
            if (tok == keyword::Key) {
                state = State::Key;
            } else {
                throw ParserError(line, "Expected 'KEY', found '" + token + "'");
            }
            break;
        case State::Key:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisLeft) {
                state = State::KeyListBegin;
            } else {
                throw ParserError(line, "Expected list of key attributes, found '" + token + "'");
            }
            break;
        case State::KeyListBegin:
            if (isIdentifier(tok)) {
                struct AttributeNamePredicate {
                    const std::string& name;

                    AttributeNamePredicate(const std::string& name) : name(name) { }

                    bool operator()(const Schema::Relation::Attribute& attr) const {
                        return attr.name == name;
                    }
                };
                const auto& attributes = schema.relations.back().attributes;
                AttributeNamePredicate p(token);
                auto it = std::find_if(attributes.begin(), attributes.end(), p);
                if (it == attributes.end()) {
                    throw ParserError(line, "'" + token + "' is not an attribute of '" + schema.relations.back().name + "'");
                }
                schema.relations.back().primaryKey.push_back(std::distance(attributes.begin(), it));
                state = State::KeyName;
            } else {
                throw ParserError(line, "Expected key attribute, found '" + token + "'");
            }
            break;
        case State::KeyName:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::KeyListBegin;
            } else if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::KeyListEnd;
            } else {
                throw ParserError(line, "Expected ',' or ')', found '" + token + "'");
            }
            break;
        case State::KeyListEnd:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::Separator;
            } else if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::CreateTableEnd;
            } else {
                throw ParserError(line, "Expected ',' or ')', found '" + token + "'");
            }
            break;
        case State::AttributeName:
            if (tok == keyword::Integer) {
                schema.relations.back().attributes.back().type = Types::Tag::Integer;
                state = State::AttributeTypeInt;
            } else if (tok == keyword::Char) {
                schema.relations.back().attributes.back().type = Types::Tag::Char;
                state = State::AttributeTypeChar;
            } else if (tok == keyword::Numeric) {
                schema.relations.back().attributes.back().type = Types::Tag::Numeric;
                state = State::AttributeTypeNumeric;
            } else if (tok == keyword::Varchar) {
                schema.relations.back().attributes.back().type = Types::Tag::Varchar;
                state = State::AttributeTypeChar;
            } else if (tok == keyword::Timestamp) {
                schema.relations.back().attributes.back().type = Types::Tag::Timestamp;
                state = State::AttributeTypeInt;
            } else if (tok == keyword::Date) {
                schema.relations.back().attributes.back().type = Types::Tag::Date;
                state = State::AttributeTypeInt;
            } else if (tok == keyword::Datetime) {
                schema.relations.back().attributes.back().type = Types::Tag::Datetime;
                state = State::AttributeTypeDatetime;
            } else { throw ParserError(line, "Expected type after attribute name, found '" + token + "'"); }
            break;
        case State::AttributeTypeChar:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisLeft) {
                state = State::CharBegin;
            } else {
                throw ParserError(line, "Expected '(' after 'CHAR', found'" + token + "'");
            }
            break;
        case State::CharBegin:
            if (isInt(tok)) {
                schema.relations.back().attributes.back().len1 = std::atoi(tok.c_str());
                state = State::CharValue;
            } else {
                throw ParserError(line, "Expected integer after 'CHAR(', found'" + token + "'");
            }
            break;
        case State::CharValue:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::CharEnd;
            } else {
                throw ParserError(line, "Expected ')' after length of CHAR, found'" + token + "'");
            }
            break;
        case State::AttributeTypeNumeric:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisLeft) {
                state = State::NumericBegin;
            } else {
                throw ParserError(line, "Expected '(' after 'NUMERIC', found'" + token + "'");
            }
            break;
        case State::NumericBegin:
            if (isInt(tok)) {
                schema.relations.back().attributes.back().len1 = std::atoi(tok.c_str());
                state = State::NumericValue1;
            } else {
                throw ParserError(line, "Expected integer after 'NUMERIC(', found'" + token + "'");
            }
            break;
        case State::NumericValue1:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::NumericSeparator;
            } else {
                throw ParserError(line, "Expected ',' after first length of NUMERIC, found'" + token + "'");
            }
            break;
        case State::NumericValue2:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::NumericEnd;
            } else {
                throw ParserError(line, "Expected ')' after second length of NUMERIC, found'" + token + "'");
            }
            break;
        case State::NumericSeparator:
            if (isInt(tok)) {
                schema.relations.back().attributes.back().len2 = std::atoi(tok.c_str());
                state = State::NumericValue2;
            } else {
                throw ParserError(line, "Expected second length for NUMERIC type, found'" + token + "'");
            }
            break;

        case State::AttributeTypeDatetime:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::Separator;
            } else if (tok == keyword::Not) {
                state = State::Not;
            } else if (tok == keyword::Generated) {
                schema.relations.back().attributes.back().generated = true;
                state = State::SysTime1;
            } else { throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '" + token + "'"); }
            break;
        case State::SysTime1:
            if (tok == keyword::Always) {
                state = State::SysTime2;
            } else { throw ParserError(line, "Expected ALWAYS after GENERATED, found '" + token + "'"); }
            break;
        case State::SysTime2:
            if (tok == keyword::As) {
                state = State::SysTime3;
            } else { throw ParserError(line, "Expected ALWAYS after GENERATED, found '" + token + "'"); }
            break;
        case State::SysTime3:
            if (tok == keyword::Row) {
                state = State::SysTime4;
            } else { throw ParserError(line, "Expected ALWAYS after GENERATED, found '" + token + "'"); }
            break;
        case State::SysTime4:
            if (tok == keyword::Start) {
                schema.relations.back().attributes.back().generatedStart = true;
                state = State::SysTimeEnd;
            } else if (tok == keyword::End) {
                schema.relations.back().attributes.back().generatedEnd = true;
                state = State::SysTimeEnd;
            } else { throw ParserError(line, "Expected START or END after ROW, found '" + token + "'"); }
            break;
        case State::PeriodStart:
            if (tok == keyword::For) {
                state = State::PeriodStart1;
            } else { throw ParserError(line, "Expected FOR, found '" + token + "'"); }
            break;
        case State::PeriodStart1:
            if (tok == keyword::System_Time) {
                state = State::PeriodStart2;
            } else { throw ParserError(line, "Expected SYSTEM_TIME, found '" + token + "'"); }
            break;
        case State::PeriodStart2:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisLeft) {
                state = State::PeriodStart3;
            } else {
                throw ParserError(line, "Expected '(', found '" + token + "'");
            }
            break;
        case State::PeriodStart3:
            if (isIdentifier(tok)) {
                schema.relations.back().systemVersioningPeriod.first = schema.relations.back().findAttributeIndex(token);
                state = State::PeriodStart4;
            } else { throw ParserError(line, "Expected first Column Name, found '" + token + "'"); }
            break;
        case State::PeriodStart4:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::PeriodStart5;
            } else {
                throw ParserError(line, "Expected ',', found '" + token + "'");
            }
            break;
        case State::PeriodStart5:
            if (isIdentifier(tok)) {
                auto rel = schema.relations.back();
                schema.relations.back().systemVersioningPeriod.second = schema.relations.back().findAttributeIndex(token);
                state = State::PeriodStart6;
            } else { throw ParserError(line, "Expected second Column Name, found '" + token + "'"); }
            break;
        case State::PeriodStart6:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::SysTimeEnd;
            } else {
                throw ParserError(line, "Expected ')', found '" + token + "'");
            }
            break;
        case State::WithStart:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisLeft) {
                state = State::WithParLeft;
            } else {
                throw ParserError(line, "Expected '(', found '" + token + "'");
            }
            break;
        case State::WithParLeft:
            if (tok == keyword::System_Versioning) {
                state = State::WithSysVersioning;
            } else {
                throw ParserError(line, "Expected SYSTEM_VERSIONING, found '" + token + "'");
            }
            break;
        case State::WithSysVersioning:
            if (tok.size() == 1 && tok[0] == literal::Equals) {
                state = State::WithSysVersioningEquals;
            } else {
                throw ParserError(line, "Expected '=', found '" + token + "'");
            }
            break;
        case State::WithSysVersioningEquals:
            if (tok == keyword::On) {
                state = State::WithEnd;
                schema.relations.back().systemVersioning = true;
            } else if (tok == keyword::Off) {
                state = State::WithEnd;
                schema.relations.back().systemVersioning = false;
            } else {
                throw ParserError(line, "Expected ON or OFF, found '" + token + "'");
            }
            break;
        case State::WithEnd:
            if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::AfterWith;
            } else {
                throw ParserError(line, "Expected ')', found '" + token + "'");
            }
            break;
        case State::AfterWith:
            if (tok.size() == 1 && tok[0] == literal::Semicolon) {
                state = State::Semicolon;
            } else {
                throw ParserError(line, "Expected ')', found '" + token + "'");
            }
            break;
        case State::SysTimeEnd:
        case State::CharEnd: /* fallthrough */
        case State::NumericEnd: /* fallthrough */
        case State::AttributeTypeInt:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::Separator;
            } else if (tok == keyword::Not) {
                state = State::Not;
            } else if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::CreateTableEnd;
            } else { throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '" + token + "'"); }
            break;
        case State::Not:
            if (tok == keyword::Null) {
                schema.relations.back().attributes.back().notNull = true;
                state = State::Null;
            } else { throw ParserError(line, "Expected 'NULL' after 'NOT' name, found '" + token + "'"); }
            break;
        case State::Null:
            if (tok.size() == 1 && tok[0] == literal::Comma) {
                state = State::Separator;
            } else if (tok.size() == 1 && tok[0] == literal::ParenthesisRight) {
                state = State::CreateTableEnd;
            } else { throw ParserError(line, "Expected ',' or ')' after attribute definition, found '" + token + "'"); }
            break;
        default:throw;
    }
}
