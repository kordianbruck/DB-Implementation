
#include <sstream>
#include <algorithm>
#include "TableScan.h"
#include "../parser/SchemaParser.hpp"
#include "../parser/IU.h"

TableScan::TableScan(Schema::Relation& rel) : relation(rel) {
    for (auto& c : rel.attributes) {
        this->produced.insert(new IU{this, &c});
    }
}

TableScan::~TableScan() {
    for (auto e : this->produced) {
        delete e;
    }
}

string TableScan::produce() {
    stringstream out;
    out << "for(auto& r: db->" << relation.name << ".table) { //Start for: " << relation.name << endl;
    for (const auto e : consumer->getRequired()) {
        if (e->rel == this) {
            out << "auto& " << e->attr->name << " = " << "r." << e->attr->name << ";" << endl;
        }
    }
    out << consumer->consume(*this) << endl;
    out << "} //End for: " << relation.name << endl;
    return out.str();
}

string TableScan::consume(Operator&) {
    throw ParserError(0, "Cannot tableScan cannot consume an operator!");
}

IU* TableScan::getIU(const std::string& name) {
    for (auto& e : this->produced) {
        if (e->attr->name == name) {
            return e;
        }
    }
    throw ParserError(5, "Attribute not found: '" + name + "'");
}




