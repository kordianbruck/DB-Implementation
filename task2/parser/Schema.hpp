#ifndef H_Schema_hpp
#define H_Schema_hpp

#include <vector>
#include <string>
#include "Types.hpp"

struct Schema {
    struct Relation {
        struct Attribute {
            std::string name;
            Types::Tag type;
            unsigned len1;
            unsigned len2;
            bool notNull;

            Attribute() : len1(~0), len2(~0), notNull(true) {}
        };

        struct Index {
            std::string name;
            std::vector<unsigned> keys;

            Index(const std::string &name) : name(name) {}
        };

        std::string name;
        std::vector<Schema::Relation::Attribute> attributes;
        std::vector<unsigned> primaryKey;
        std::vector<Index> indexes;

        int findAttributeIndex(const std::string &name);

        Relation(const std::string &name) : name(name) {}
    };

    std::vector<Schema::Relation> relations;

    std::string toString() const;

    std::string generateDatabaseCode() const;

    Schema::Relation &findRelation(const std::string &name);
};

#endif
