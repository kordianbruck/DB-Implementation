
#include <iostream>
#include "parser/Parser.hpp"
#include "parser/Query.h"
#include "operators/TableScan.h"
#include "operators/HashJoin.h"
#include "operators/Selection.h"
#include "operators/Print.h"

using namespace std;

int main(int argc, char** argv) {
    std::cout << "TPC-C Compile" << std::endl;
    std::cout << "--------------------------" << std::endl;

    if (argc != 2) {
        argv[1] = (char*) "schema.sql";
    }

    Parser p(argv[1]);
    std::unique_ptr<Schema> schema;
    try {
        schema = p.parse();
        std::cout << "Loaded " << schema->relations.size() << " relations into our schema." << std::endl;
        //std::cout << schema->toString() << std::endl;

        //Write to file the database
        std::cout << "Generating database code..." << std::endl;
        std::ofstream myfile;
        myfile.open("db.h");
        myfile << schema->generateDatabaseCode();
        myfile.close();
    } catch (ParserError& e) {
        std::cerr << e.what() << " on line " << e.where() << std::endl;
    }
    std::cout << "All done" << std::endl << endl << endl;

    /*
   vector<Schema::Relation::Attribute&> columnsCust{};
   columnsCust.push_back(customer.findAttribute("c_id"));
   columnsCust.push_back(customer.findAttribute("c_w_id"));
   columnsCust.push_back(customer.findAttribute("c_d_id"));
   vector<Schema::Relation::Attribute&> columnsOrd{};
     columnsOrd.push_back(order.findAttribute("c_id"));
   columnsOrd.push_back(order.findAttribute("c_w_id"));
   columnsOrd.push_back(order.findAttribute("c_d_id"));
     */
    //Parse our DB query
    try {
        //Get all the data from customers
        auto tblCustomer = TableScan(schema->findRelation("customer"));

        //Select a one exact customer
        vector<tuple<IU*, string>> selecCond{
                make_tuple(tblCustomer.getIU("c_w_id"), "1"),
                make_tuple(tblCustomer.getIU("c_d_id"), "1"),
                make_tuple(tblCustomer.getIU("c_id"), "322")};
        auto tblCustomerSelection = Selection(tblCustomer, selecCond);

        //Get our join table
        auto tblOrder = TableScan(schema->findRelation("order"));

        //Do the join
        vector<tuple<IU*, IU*>> joinCond1{
                make_tuple(tblCustomer.getIU("c_w_id"), tblOrder.getIU("o_w_id")),
                make_tuple(tblCustomer.getIU("c_d_id"), tblOrder.getIU("o_d_id")),
                make_tuple(tblCustomer.getIU("c_id"), tblOrder.getIU("o_c_id"))};
        auto hashJoin1 = HashJoin(tblCustomerSelection, tblOrder, joinCond1);

        //Do the Second join
        auto tblOrderLine = TableScan(schema->findRelation("orderline"));
        vector<tuple<IU*, IU*>> joinCond2{
                make_tuple(tblOrder.getIU("o_w_id"), tblOrderLine.getIU("ol_w_id")),
                make_tuple(tblOrder.getIU("o_d_id"), tblOrderLine.getIU("ol_d_id")),
                make_tuple(tblOrder.getIU("o_id"), tblOrderLine.getIU("ol_o_id"))};
        auto hashJoin2 = HashJoin(hashJoin1, tblOrderLine, joinCond2);

        //Print the data
        vector<IU*> outVars{tblCustomer.getIU("c_first"), tblCustomer.getIU("c_last"),
                            tblOrder.getIU("o_all_local"), tblOrderLine.getIU("ol_amount")};
        auto printer = Print(hashJoin2, outVars);

        cout << printer.produce();
        std::ofstream myfile;
        myfile.open("query.h");
        myfile << "#include <string>" << endl;
        myfile << "#include <map>" << endl;
        myfile << "#include <iostream>" << endl;
        myfile << "#include <tuple>\n"
                "#include \"Types.hpp\"\n"
                "#include <algorithm>" << endl;
        myfile << "using namespace std;" << endl;
        myfile << "void query(Database* db) {" << printer.produce() << "}";
        myfile.close();
    } catch (ParserError& e) {
        std::cerr << e.what() << " on line " << e.where() << std::endl;
    }
    return 0;
}
