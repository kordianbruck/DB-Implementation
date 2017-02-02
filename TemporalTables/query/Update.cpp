
#include "Update.h"


using namespace std;

string QueryUpdate::toString() const {
    stringstream out;
    out << "UPDATE: ";
    out << relation << endl;
    out << "Fields: \n\t ";
    for (auto& e : fields) {
        out << get<0>(e) << "=" << get<1>(e) << " ";
    }
    out << endl << "WHERE: \n\t ";
    for (auto& e : this->selection) {
        out << get<0>(e) << "=" << get<1>(e) << " ";
    }

    return out.str();
}


vector<fieldType> QueryUpdate::getFields(Operator* sel) {
    vector<fieldType> conditions;

    for (auto& condition : fields) {
        for (auto iu : sel->getProduced()) {
            if (iu->attr->name == condition.first) {
                conditions.push_back(make_pair(iu, condition.second));
            }
        }
    }
    return conditions;
}

string QueryUpdate::generateQueryCode() {
    ostringstream out;
    //Find item
    Operator* finder;
    auto ts = new TableScan(schema->findRelation(relation));
    auto selectionConditions = getSelections(ts);

    //If we got matching selections, why not directly add them with a selection
    if (selectionConditions.size() > 0) {
        finder = new Selection(shared_ptr<TableScan>(ts), selectionConditions);
    } else {
        finder = ts;
    }
    vector<fieldType> fields = getFields(finder);
    Update* u = new Update(*finder, fields);
    out << u->produce();

    delete u;
    delete finder;
    return out.str();
}
