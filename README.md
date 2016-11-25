# DB-Implementation

In this course we implement a simple database system that uses g++ to compile queries into executeable code and load this code at run time through dlsym. 
We've learned how to implement the princples that larger systems build upon like:
* Column / Row store
* Parsing SQL: CREATE and SELECT statements with indexes and conditions
* Building an abstract syntax tree with a produce/consume pattern to simplify the generation of code for a query
* Optimizing certain parts for performance within the contraints of the TPC-H benchmark
