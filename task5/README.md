Task 5

1. Implement a simple parser for the following subset of SQL:
* The select clause contains one or more attribute names.
*The from clause contains one or more relation names.
* The where clause is optional and can contain one or more selections (connected by "and"). You only need to support selections of the form "attribute = attribute" and "attribute = constant".
* You can assume that each relation is referenced only once, and that all attribute names are unique.
2. Implement semantic analysis, which takes the parsed SQL statement and the schema as input and computes an algebra tree that can be used as input to your code generator. Report errors when non-existing attributes or tables are referenced. Also, you should report an error if a table has no join condition (i.e., a cross product would be necessary). Build left-deep join trees based on the order of the relations in the from clause.
3. Implement a "read-eval-print-loop" for your database system:
* read one line from standard input
* analyze it (report any errors and abort if necessary)
* generate c++ code and store it in a temporary file
* call a c++ compiler and generate a temporary shared library (.so file)
* using dlopen, dlsym, and dlclose you can now load the shared library and execute the query in it (have a look at the example), you man need to compile with -fPIC and -rdynamic
* Measure and report the compilation and the execution times separately.
* hint: have a look at rlwrap tool
4. Test the following queries:
select w_id from warehouse;
* select c_id, c_first, c_middle, c_last from customer where c_last = 'BARBARBAR';
* select d_name from district, order where d_w_id = o_w_id and o_d_id = d_id and o_id = 7;
* select o_id, ol_dist_info from order, orderline where o_id = ol_o_id and ol_d_id = o_d_id and o_w_id = ol_w_id and ol_* number = 1 and ol_o_id = 100;
* select o_id, ol_dist_info from orderline, order where o_id = ol_o_id and ol_d_id = o_d_id and o_w_id = ol_w_id and ol_number = 1 and ol_o_id = 100;
* select c_last, o_id, i_id, ol_dist_info from customer, order, orderline, item where c_id = o_c_id and c_d_id = o_d_id and c_w_id = o_w_id and o_id = ol_o_id and ol_d_id = o_d_id and o_w_id = ol_w_id and ol_number = 1 and ol_o_id = 100 and ol_i_id = i_id;
* select c_last, o_id, ol_dist_info from customer, order, orderline where c_id = o_c_id and o_id = ol_o_id and ol_d_id = o_d_id and o_w_id = ol_w_id and ol_number = 1 and ol_o_id = 100;

Send your solution to Viktor Leis until 24 Nov 2016, 2pm.