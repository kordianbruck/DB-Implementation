#include "Schema.hpp"

#include "Parser.hpp"
#include <sstream>
#include <iostream>

static std::string type(const Schema::Relation::Attribute &attr) {
    Types::Tag type = attr.type;
    switch (type) {
        case Types::Tag::Integer:return "Integer";
        case Types::Tag::Timestamp:return "Integer";
        case Types::Tag::Numeric: {
            std::stringstream ss;
            ss << "Numeric(" << attr.len1 << ", " << attr.len2 << ")";
            return ss.str();
        }
        case Types::Tag::Char:
        case Types::Tag::Varchar: {
            std::stringstream ss;
            ss << "Char(" << attr.len1 << ")";
            return ss.str();
        }
    }
    throw "type not found";
}

std::string Schema::toString() const {
    std::stringstream out;
    for (const Schema::Relation &rel : relations) {
        out << rel.name << std::endl;
        out << "\tPrimary Key:";
        for (unsigned keyId : rel.primaryKey) {
            out << ' ' << rel.attributes[keyId].name;
        }
        out << std::endl;
        out << "\tColumns: " << std::endl;
        for (const auto &attr : rel.attributes) {
            out << "\t\t" << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << std::endl;
        }
    }
    return out.str();
}

std::string Schema::generateDatabaseCode() const {
    std::stringstream out;
    out << "#pragma once" << std::endl
        << "#include <iostream>" << std::endl
        << "#include <cstdint>" << std::endl
        << "#include <fstream>" << std::endl
        << "#include <vector>" << std::endl
        << "#include <utility>" << std::endl
        << "#include <ctime>" << std::endl
        << "#include <unordered_map>" << std::endl
        << "#include <tuple>" << std::endl
        << "#include \"Types.hpp\"" << std::endl
        << "#include \"table_types.hpp\"" << std::endl
        << "#include \"tupel_hash.h\"" << std::endl
            << "class Database {" << std::endl
            << "public:" << std::endl
            << "class Database {" << std::endl
            << "class Database {" << std::endl
            << "class Database {" << std::endl
            << "class Database {" << std::endl
            << "class Database {" << std::endl;

        //void import(const std::string &path);
        //Table<Warehouse> warehouses;

    for (const Schema::Relation &rel : relations) {
        out << rel.name << std::endl;
        out << "\tPrimary Key:";
        for (unsigned keyId : rel.primaryKey) {
            out << ' ' << rel.attributes[keyId].name;
        }
        out << std::endl;
        out << "\tColumns: " << std::endl;
        for (const auto &attr : rel.attributes) {
            out << "\t\t" << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << std::endl;
        }
    }
    return out.str();
}


Schema::Relation &Schema::findRelation(const std::string &name) {
    for (auto &e : this->relations) {
        if (e.name == name) {
            return e;
        }
    }
    throw ParserError(0, "No such table for index: '" + name + "'");
}

int Schema::Relation::findAttributeIndex(const std::string &name) {
    int i = 0;
    for (auto &e : this->attributes) {
        if (e.name == name) {
            return i;
        }
        i++;
    }
    throw ParserError(0, "No such table for index: '" + name + "'");
}


