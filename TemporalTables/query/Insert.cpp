
#include "Insert.h"


using namespace std;


string QueryInsert::toString() const {
    stringstream out;
    out << "INSERT: ";
    out << relation << endl;
    out << "Fields: \n\t ";
    for (auto& e : fields) {
        out << e.first << "=" << e.second << " ";
    }

    return out.str();
}


string QueryInsert::generateQueryCode() {
    stringstream out;
    Schema::Relation r = schema->findRelation(relation);

    out << "auto r = Database::";
    out << r.getTypeRelationName() << "::Row{};\n" << endl;
    out << "try {" << endl;
    for (auto& field : r.attributes) {
        string val = "";
        for (auto& findVal : fields) {
            if (field.name == findVal.first) {
                val = findVal.second;
            }
        }
        if (val.length() > 0) {
            out << "r." << field.name << " = r." << field.name << ".castString(\"" << val << "\", " << val.size() << ");" << endl;
        } else if (field.type == Types::Tag::Date) {
            out << "r." << field.name << " = r." << field.name << ".castString(\"0000-01-01\", 10);" << endl;
        } else if (field.type == Types::Tag::Datetime) {
            out << "r." << field.name << " = r." << field.name << ".castString(\"0000-01-01\", 10);" << endl;
        }
    }
    out << "} catch (const char * r) { cout << r << endl; }";
    out << "    db->warehouse.insert(r);";
    return out.str();
}
