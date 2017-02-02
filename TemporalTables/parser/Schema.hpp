#ifndef H_Schema_hpp
#define H_Schema_hpp

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>

using namespace std;

namespace Types {
    enum class Tag : unsigned {
        Integer, Char, Numeric, Varchar, Date, Timestamp, Datetime
    };
}

struct Schema {
    struct Relation {
        struct Attribute {
            std::string name;
            Types::Tag type;
            unsigned len1;
            unsigned len2;

            //Temporal Tables
            bool generated;
            bool generatedStart;
            bool generatedEnd;

            //Default val
            bool notNull;

            //Is this attribute part of the PK?
            bool isPartPK;

            Attribute() : len1(0), len2(0), notNull(true), generated(false), generatedStart(false), generatedEnd(false), isPartPK(false) {}
        };

        struct Index {
            std::string name;
            std::vector<unsigned> keys;

            Index(const std::string& name) : name(name) {}
        };

        std::string name;
        std::vector<Schema::Relation::Attribute> attributes;
        std::vector<unsigned> primaryKey;
        std::vector<Index> indexes;

        bool systemVersioning = false;
        std::pair<int, int> systemVersioningPeriod;

        int findAttributeIndex(const std::string& name);

        Schema::Relation::Attribute& findAttribute(const std::string& name);

        Relation(const std::string& name) : name(name) {}

        string getTypeRelationName() const {
            string ret = name;
            if (!ret.empty()) {
                ret[0] = (char) std::toupper(ret[0]);

                for (std::size_t i = 1; i < name.length(); ++i)
                    ret[i] = (char) std::tolower(ret[i]);
            }
            return ret;
        }
    };

    std::vector<Schema::Relation> relations;

    std::string toString() const;

    std::string generateDatabaseCode() const;

    Schema::Relation& findRelation(const std::string& name);

    static string type(const Relation::Attribute& attr, bool cpp = 0);
};


#endif
