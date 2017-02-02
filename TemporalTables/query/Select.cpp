


#include "Select.h"
#include "../operators/TableScan.h"
#include "../operators/Selection.h"
#include "../operators/HashJoin.h"
#include "../parser/ParserError.h"
#include "../operators/Print.h"

using namespace std;


string QuerySelect::toString() const {
    stringstream out;
    out << "SELECT: ";
    for (auto& e : this->projection) {
        out << e << " ";
    }
    out << endl << "FROM: ";
    for (auto& e : this->relations) {
        out << e << " ";
    }
    out << endl << "WHERE: \n\t SEL: ";
    for (auto& e : this->selection) {
        out << get<0>(e) << "=" << get<1>(e) << " ";
    }
    out << "\n\t JOI: ";
    for (auto& e : this->joinConditions) {
        out << get<0>(e) << "=" << get<1>(e) << " ";
    }

    return out.str();
}

string QuerySelect::generateQueryCode() {

    //Generate all tablescans with selections
    stack<Operator*> ops;
    for (auto r : relations) {
        //for (auto i = relations.size(); i > 0; i--) {
        auto ts = new TableScan(schema->findRelation(r));
        auto selectionConditions = getSelections(ts);

        //If we got matching selections, why not directly add them with a selection
        if (selectionConditions.size() > 0) {
            ops.push(new Selection(shared_ptr<TableScan>(ts), selectionConditions));
        } else {
            ops.push(ts);
        }
    }

    //Create some hashjoins
    while (ops.size() >= 2) {
        auto left = ops.top();
        ops.pop();
        auto right = ops.top();
        ops.pop();

        //Check that there are join conditions and we are not doing a cross join
        auto conditions = getJoinConditions(left, right);
        if (conditions.size() == 0) {
            throw ParserError(0, "No join condition, hash join not possible.");
        }

        //Add a new hashjoin of the two elements
        ops.push(new HashJoin(*left, *right, conditions));
    }

    //All the vars we want to output need to be passed to the printer
    auto projections = vector<IU*>();
    if (projectAll) { // Show all columns
        for (auto& iu : ops.top()->getProduced()) {
            projections.push_back(iu);
        }
    } else { // Only show a few columns
        projections.reserve(projection.size());
        bool found;
        for (auto& p : projection) {
            found = false;
            for (auto& iu : ops.top()->getProduced()) {
                if (iu->attr->name == p || projectAll) {
                    projections.emplace_back(iu);
                    found = true;
                }
            }

            //Check that we actually have this predicate available for output and fail if not
            if (!found) {
                throw ParserError(0, "The predicate " + p + " was not found to be output.");
            }
        }
    }


    //Create a printer that will output our projections
    Print result = Print(*ops.top(), projections);
    string ret = result.produce();

    //Free memory
    while (ops.size() > 0) {
        auto e = ops.top();
        delete e;
        ops.pop();
    }

    return ret;
}
