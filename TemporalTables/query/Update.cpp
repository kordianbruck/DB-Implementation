
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


string QueryUpdate::generateQueryCode() {
    string ret = "";
    return ret;
}
