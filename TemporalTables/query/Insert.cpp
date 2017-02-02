
#include "Insert.h"


using namespace std;


string QueryInsert::toString() const {
    stringstream out;
    out << "INSERT: ";
    out << relation << endl;
    out << "Fields: \n\t ";
    for (auto& e : fields) {
        out << get<0>(e) << "=" << get<1>(e) << " ";
    }

    return out.str();
}


string QueryInsert::generateQueryCode() {
    string ret = "";
    return ret;
}
