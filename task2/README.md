# Task 2
1. Write a simple parser for SQL schema files. It should handle the TPC-C schema file. You can assume that all attributes are declared NOT NULL, and you must support additional non-primary (non-unique) key indexes. This code may be helpful, but it does not support all needed features.
2. Write a generator that generates C++ code from the schema. It must be possible to store, insert, delete, access attributes, and find (using the primary key) tuples.
3. (Manually) re-implement the newOrder transaction using the generated code.
4. Add the delivery transaction to your existing TPC-C implementation.
5. Execute 1 million transactions composed of a mix of newOrder (90%) and delivery (10%) transactions (see oltp.cpp); measure your performance.

Send your solution to Viktor Leis until November 3 2016, 2pm.