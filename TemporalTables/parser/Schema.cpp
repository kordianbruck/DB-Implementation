#include "Schema.hpp"

#include "SchemaParser.hpp"
#include <iostream>

using namespace std;

string Schema::type(const Schema::Relation::Attribute& attr, bool cpp) {
    Types::Tag type = attr.type;
    switch (type) {
        case Types::Tag::Integer:return "Integer";
        case Types::Tag::Date:return "Date";
        case Types::Tag::Datetime:
        case Types::Tag::Timestamp:return "Timestamp";
        case Types::Tag::Numeric: {
            stringstream ss;
            if (!cpp) {
                ss << "Numeric(" << attr.len1 << ", " << attr.len2 << ")";
            } else {
                ss << "Numeric<" << attr.len1 << ", " << attr.len2 << ">";
            }
            return ss.str();
        }
        case Types::Tag::Char: {
            stringstream ss;
            if (!cpp) {
                ss << "Char(" << attr.len1 << ")";
            } else {
                ss << "Char<" << attr.len1 << ">";
            }
            return ss.str();
        }
        case Types::Tag::Varchar: {
            stringstream ss;
            if (!cpp) {
                ss << "Varchar(" << attr.len1 << ")";
            } else {
                ss << "Varchar<" << attr.len1 << ">";
            }
            return ss.str();
        }
    }
    throw "type not found";
}

static string pkList(const Schema::Relation& rel) {
    stringstream out;
    for (auto e : rel.primaryKey) {
        out << rel.attributes[e].name;
        if (e != rel.primaryKey.back()) {
            out << ", ";
        }
    }
    return out.str();
}

static string pkListType(const Schema::Relation& rel) {
    stringstream out;
    for (auto e : rel.primaryKey) {
        out << Schema::type(rel.attributes[e], 1);
        if (e != rel.primaryKey.back()) {
            out << ", ";
        }
    }
    return out.str();
}

string Schema::toString() const {
    stringstream out;
    for (const Schema::Relation& rel : relations) {
        out << rel.name;
        if (rel.systemVersioning) {
            out << "(SysVerioned on Period: ";
            out << rel.attributes[rel.systemVersioningPeriod.first].name;
            if (rel.attributes[rel.systemVersioningPeriod.first].generatedStart) {
                out << " START";
            } else {
                out << " END";
            }

            out << ", ";
            out << rel.attributes[rel.systemVersioningPeriod.second].name;
            if (rel.attributes[rel.systemVersioningPeriod.second].generatedStart) {
                out << " START";
            } else {
                out << " END";
            }
            out << ")";
        }
        out << endl;
        out << "\tPrimary:";
        for (unsigned keyId : rel.primaryKey) {
            out << rel.attributes[keyId].name << ' ';
        }
        out << endl;
        out << "\tColumns: " << endl;
        for (const auto& attr : rel.attributes) {
            out << "\t\t";
            out << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << endl;
        }
    }
    return out.str();
}

void Schema::genIncludes(ostream& out) const {
    out << "#include <iostream>" << endl
        << "#include <cstdint>" << endl
        << "#include <fstream>" << endl
        << "#include <vector>" << endl
        << "#include <utility>" << endl
        << "#include <ctime>" << endl
        << "#include <unordered_map>" << endl
        << "#include <tuple>" << endl
        << "#include <map>" << endl
        << "#include <sstream>" << endl
        << "#include \"../utils/Types.hpp\"" << endl
        << "#include \"../utils/DatabaseTools.h\"" << endl
        << "#include \"../utils/TupelHash.h\"" << endl;
    //out << "#include \"btree/btree_map.h\"" << endl;
}

void Schema::genRowDef(ostream& out, const Schema::Relation& rel, bool hasPK) const {
    //Output the Row Type
    out << "        struct Row {" << endl;
    for (auto e : rel.attributes) {
        out << "            " << Schema::type(e, 1) << " " << e.name << ";" << endl;
    }
    if (hasPK) {
        out << "            pkType key() const { return std::make_tuple(" << pkList(rel) << "); }" << endl;
    }
    out << "        };" << endl;

    //Output the parsing algo
    out << "        static Row parse(const std::vector<std::string>& row) {" << endl;
    out << "            auto ret = Row{};" << endl;
    int i = 0;
    for (auto e : rel.attributes) {
        out << "            ret." << e.name << " = ret." << e.name << ".castString(row[" << i << "].c_str(), row[" << i << "].length());" << endl;
        i++;
    }
    out << "            return ret;" << endl;
    out << "        }" << endl;
}

void Schema::genRelation(ostream& out, const Schema::Relation& rel) const {
    bool hasPK = rel.primaryKey.size() > 0;
    string sysTimeColStart = "";
    string sysTimeColEnd = "";
    if (rel.systemVersioning) {
        sysTimeColStart = rel.attributes[rel.systemVersioningPeriod.first].name;
        sysTimeColEnd = rel.attributes[rel.systemVersioningPeriod.second].name;
    }

    out << "    struct " << rel.getTypeRelationName() << "{" << endl;

    //Output the primary key type
    if (hasPK) {
        out << "        using pkType = std::tuple<" << pkListType(rel) << ">;" << endl;
    }

    genRowDef(out, rel, hasPK);

    //Add the most important table vars
    out << "        std::vector<Row> table{};" << endl;
    if (hasPK) {
        out << "        std::unordered_map<pkType, u_int32_t> pk{};" << endl;
    }
    if (rel.systemVersioning) {
        out << "        std::multimap<pkType, u_int32_t> pkHistory{};" << endl;
    }
    /*if (rel.indexes.size() > 0) {
        for (auto& e : rel.indexes) {
            out << "        //TODO ADD INDEXES" << endl;
        }
    }*/

    //Some table functions that are useful
    out << "        size_t size() { return table.size(); }" << endl;
    if (hasPK) {
        out << "        Row& row(pkType k) { return table[pk[k]]; }" << endl;
    }
    out << "        Row& row(size_t i) { return table[i]; }" << endl;

    if (rel.systemVersioning) {
        out << "    void update(Row& element) {" << endl;
        //Move previous element to history pk
        out << "        auto& currIdx = pk[element.key()];" << endl;

        //Expire the current entry (update)
        out << "        table[currIdx].";
        out << sysTimeColEnd << " = Timestamp::now();" << endl;

        //Insert the new element
        out << "        element."
            << sysTimeColStart << " = Timestamp::now();" << endl
            << "        insert(element);" << endl
            << "    }" << endl;
    } else if (hasPK) { //Don't allow updating rows, if the table does not have a PK
        out << "        void update(Row& element) { table[pk[element.key()]] = element; }" << endl;
    }

    //Removing elements
    out << "        void remove(size_t i) {" << endl;
    if (rel.systemVersioning) {
        out << "auto& r = row(i);" << endl;
        out << "r." << sysTimeColEnd << " = Timestamp::now();" << endl;//set the deletion time
        out << "pk.erase(r.key()); " << endl;//Remove from "current" pk
        //Entry already should be in history table through correct insert
    } else {
        if (hasPK) {
            out << "            const auto key = row(i).key();" << endl;
            out << "            pk.erase(key);" << endl;
        }
        out << "            std::iter_swap(table.begin() +i, table.end()-1);\n";
        out << "            table.pop_back();" << endl;
        if (hasPK) {
            out << "            pk[table[i].key()] = i;" << endl;
        }
    }
    out << "        }" << endl;

    //Inserting
    out << "        void insert(Row& element) { " << endl;
    if(hasPK && !rel.systemVersioning) {//TODO need a better index structure for temporal tables
        out << "            auto it = pk.find(element.key());" << endl;
        out << "            if(it != pk.end()) { throw \"Dupe PK!\"; }" << endl;
    }
    if (rel.systemVersioning) {
        out << "if (element." << sysTimeColStart << " == Timestamp::null()) {element." << sysTimeColStart << " = Timestamp::now();}" << endl;
    }
    out << "            table.push_back(element); " << endl;
    if (hasPK) {
        out << "            pk[element.key()] = table.size() - 1;" << endl;
    }
    if (rel.systemVersioning) {
        out << "pkHistory.insert(make_pair(element.key(), table.size() - 1));" << endl;
    }
    out << "        }" << endl;

    //Creating the index as a bulk operation
    /*if (hasPK) {
        out << "        void buildIndex() {" << endl;
        out << "            size_t size = table.size();" << endl;
        out << "            pk.reserve(size);" << endl;
        out << "            for (size_t i = 0; i < size; i++) {" << endl;
        out << "                pk[table[i].key()] = i;" << endl;
        out << "            }" << endl;
        out << "        }" << endl;
    }*/

    //End the schema struct
    out << "    };" << endl;
}

string Schema::generateDatabaseCode() const {
    stringstream out;
    genIncludes(out);

    out << "struct Database {" << endl;
    //out << "private: " << endl;
    for (const Schema::Relation& rel : relations) {
        genRelation(out, rel);
    }

    out << "public: " << endl;
    out << "    Database(const Database&) = delete;\n"
        << "    Database () {}" << endl;

    //Output all relations as members
    for (const Schema::Relation& rel : relations) {
        out << "\t";
        out << rel.getTypeRelationName() << " " << rel.name << ";" << endl;
    }

    // Import any data into our database
    out << "    void import(const std::string &path) {" << endl;
    for (const Schema::Relation& rel : relations) {
        out << "       DatabaseTools::loadTableFromFile(";
        out << rel.name << ", path + \"tpcc_" << rel.name << ".tbl\");\n" << endl;
        out << "       std::cout << \"\\t";
        out << rel.getTypeRelationName() << ": \" << " << rel.name << ".size() << std::endl;" << endl;
    }
    out << "    }" << endl;

    // End struct Database
    out << "};" << endl;

    // Allow db to be loaded dynamically
    out << "extern \"C\" Database* make_database(std::string filename) {\n"
            "    Database* db = new Database();\n"
            "    db->import(filename);\n"
            "    return db;\n"
            "}";
    out << "extern \"C\" size_t getSize(Database* db, std::string dbn) {\n"
            "    if(dbn==\"wh\") {\n"
            "        return db->warehouse.size();\n"
            "    }\n"
            "    return db->warehouseold.size();\n"
            "}";

    return out.str();
}


Schema::Relation& Schema::findRelation(const string& name) {
    for (auto& e : this->relations) {
        if (e.name == name) {
            return e;
        }
    }
    throw ParserError(0, "No such table: '" + name + "'");
}


int Schema::Relation::findAttributeIndex(const string& name) {
    int i = 0;
    for (auto& e : this->attributes) {
        if (e.name == name) {
            return i;
        }
        i++;
    }
    throw ParserError(0, "Attribute not found: '" + name + "'");
}

Schema::Relation::Attribute& Schema::Relation::findAttribute(const std::string& name) {
    for (auto& e : this->attributes) {
        if (e.name == name) {
            return e;
        }
    }
    throw ParserError(0, "Attribute not found: '" + name + "'");
}


