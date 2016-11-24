#include "Schema.hpp"

#include "SchemaParser.hpp"
#include <iostream>

using namespace std;

string Schema::type(const Schema::Relation::Attribute& attr, bool cpp) {
    Types::Tag type = attr.type;
    switch (type) {
        case Types::Tag::Integer:
            return "Integer";
        case Types::Tag::Timestamp:
            return "Timestamp";
        case Types::Tag::Numeric: {
            stringstream ss;
            if (!cpp) {
                ss << "Numeric(" << attr.len1 << ", " << attr.len2 << ")";
            } else {
                ss << "Numeric<" << attr.len1 << ", " << attr.len2 << ">";
            }
            return ss.str();
        }
        case Types::Tag::Char:
        case Types::Tag::Varchar: {
            stringstream ss;
            if (!cpp) {
                ss << "Char(" << attr.len1 << ")";
            } else {
                ss << "Char<" << attr.len1 << ">";
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
        out << rel.name << endl;
        out << "\tPrimary Key:";
        for (unsigned keyId : rel.primaryKey) {
            out << ' ' << rel.attributes[keyId].name;
        }
        out << endl;
        out << "\tColumns: " << endl;
        for (const auto& attr : rel.attributes) {
            out << "\t\t" << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << endl;
        }
    }
    return out.str();
}

string Schema::generateDatabaseCode() const {
    stringstream out;
    out << "#pragma once" << endl
        << "#include <iostream>" << endl
        << "#include <cstdint>" << endl
        << "#include <fstream>" << endl
        << "#include <vector>" << endl
        << "#include <utility>" << endl
        << "#include <ctime>" << endl
        << "#include <unordered_map>" << endl
        << "#include <tuple>" << endl
        << "#include <map>" << endl
        << "#include \"../Types.hpp\"" << endl
        << "#include \"../tupel_hash.h\"" << endl;


    out << "struct Database {" << endl;
    out << "private: " << endl;
    for (const Schema::Relation& rel : relations) {
        bool hasPK = rel.primaryKey.size() > 0;
        out << "    struct " << rel.name << "{" << endl;

        //Output the primary key type
        if (hasPK) {
            out << "        using pkType = std::tuple<" << pkListType(rel) << ">;" << endl;
        }

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

        //Add the most important table vars
        out << "        std::vector<Row> table{};" << endl;
        if (hasPK) {
            out << "        std::unordered_map<pkType, u_int32_t> pk{};" << endl;
            out << "        std::map<pkType, u_int32_t> pkTree{};" << endl;
        }
        if (rel.indexes.size() > 0) {
            for (auto e : rel.indexes) {
                out << "        //TODO ADD INDEXES AS MULTIMAP" << endl;
                //    out << "        std::unordered_multimap<..."
            }
        }

        //Some table functions that are useful
        out << "        size_t size() { return table.size(); }" << endl;
        if (hasPK) {
            out << "        Row& row(pkType k) { return table[pk[k]]; }" << endl;
        }
        out << "        Row& row(size_t i) { return table[i]; }" << endl;

        if (hasPK) { //Don't allow updating rows, if the table does not have a PK
            out << "        void update(Row& element) { table[pk[element.key()]] = element; }" << endl;
        }

        //Removing elements
        out << "        void remove(size_t i) {" << endl;
        if (hasPK) {
            //out << "            const auto key = row(i).key();" << endl;
            //out << "            pk.erase(key);" << endl;
            //out << "            pkTree.erase(key);" << endl;
            out << "            const auto key = row(i).key();" << endl;
            out << "            pk.erase(key);" << endl;
            out << "            pkTree.erase(key);" << endl;

        }
        //out << "            table[i] = row(table.size()-1);" << endl;
        out << "            std::iter_swap(table.begin() +i, table.end()-1);\n";
        out << "            table.pop_back();" << endl;
        if (hasPK) {
            out << "            pk[table[i].key()] = i;" << endl;
            out << "            pkTree[table[i].key()] = i;" << endl;
        }
        out << "        }" << endl;

        //Inserting
        out << "        void insert(const Row& element) { " << endl;
        out << "            table.push_back(element); " << endl;
        if (hasPK) {
            out << "            pk[element.key()] = table.size() - 1;" << endl;
            out << "            pkTree[element.key()] = table.size() - 1;" << endl;
        }
        out << "        }" << endl;
        if (hasPK) {
            out << "        void buildIndex() {" << endl;
            out << "            size_t size = table.size();" << endl;
            out << "            pk.reserve(size);" << endl;
            out << "            for (size_t i = 0; i < size; i++) {" << endl;
            out << "                pk[table[i].key()] = i;" << endl;
            out << "                pkTree[table[i].key()] = i;" << endl;
            out << "            }" << endl;
            out << "        }" << endl;
        }

        //End the schema struct
        out << "    };" << endl;
    }

    //The split function for parsing the table data
    out << "    void split(const std::string& str, std::vector<std::string>& lineChunks) {\n"
            "        std::stringstream in(str);\n"
            "        std::string segment;\n"
            "        lineChunks.clear();\n"
            "        while (std::getline(in, segment, '|')) {\n"
            "           lineChunks.push_back(segment);\n"
            "        }\n"
            "    }" << endl;

    //Template for loading each table into ram
    out << "    template<typename T>\n"
            "    void loadTableFromFile(T& tbl, const std::string& file) {\n"
            "        std::ifstream myfile(file);\n"
            "        if (!myfile.is_open()) {\n"
            "            return;\n"
            "        }\n"
            "\n"
            "        std::string line;\n"
            "        std::vector<std::string> lineChunks;\n"
            "        while (getline(myfile, line)) {\n"
            "            split(line, lineChunks);\n"
            "            auto tmp = T::parse(lineChunks);\n"
            "            tbl.table.push_back(tmp);\n"
            "        }\n"
            "        myfile.close();\n"
            "    }" << endl;

    out << "public: " << endl;
    out << "    Database(const Database&) = delete;\n"
            "    Database () {}" << endl;
    for (const Schema::Relation& rel : relations) {
        out << "    " << rel.name << " " << rel.name << ";" << endl;
    }

    //Import: import any data into our database
    out << "    void import(const std::string &path) {" << endl;
    for (const Schema::Relation& rel : relations) {
        out << "       loadTableFromFile(" << rel.name << ", path + \"tpcc_" << rel.name << ".tbl\");\n" << endl;
        out << "       std::cout << \"\\t" << rel.name << ": \" << " << rel.name << ".size() << std::endl;" << endl;
    }
    out << "    }" << endl; // End import()

    out << "};" << endl; // End struct Database
    return out.str();
}


Schema::Relation& Schema::findRelation(const string& name) {
    for (auto& e : this->relations) {
        if (e.name == name) {
            return e;
        }
    }
    throw ParserError(0, "No such table for index: '" + name + "'");
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


