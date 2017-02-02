
#include "Delete.h"

using namespace std;


string QueryDelete::toString() const {
    stringstream out;
    out << "DELETE: ";
    out << relation << endl;
    out << "WHERE: \n\t ";
    for (auto& e : this->selection) {
        out << get<0>(e) << "=" << get<1>(e) << " ";
    }

    return out.str();
}


string QueryDelete::generateQueryCode() {
    ostringstream out;

    Operator* finder;
    auto& rel = schema->findRelation(relation);
    auto ts = new TableScan(rel);
    auto selectionConditions = getSelections(ts);

    if (selectionConditions.size() > 0) {
        finder = new Selection(shared_ptr<TableScan>(ts), selectionConditions);
    } else {
        finder = ts;
    }

    Delete* u = new Delete(*finder, selectionConditions, rel);
    out << u->produce();

    delete u;
    delete finder;
    return out.str();
}
