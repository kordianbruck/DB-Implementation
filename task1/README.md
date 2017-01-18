#Task 1

Implement the TPC-C "New Order" transaction and storage for all relations as a standalone C++ program.

1. Write a load procedure to populate the database with the data provided in the *.tbl files.
2. Translate the "New Order" (neworder.script) transaction to C++ using the following mapping from SQL to C data types:
* integer: int32_t
* char,varchar: length plus character array (do not use pointers or std::string)
* timestamp: uint64_t (seconds since 01.01.1970)
* numeric(x,y): int64_t (x is the total number of digits, y is number of digits after the decimal separator, for example numeric(10,2) x=12.3 is represented as 1230)
3. Test your implementation by translating newOrderRandom (from oltp.script) to C++ and executing it 1 million times; how many newOrder transactions per second can your implementation execute?
4. Before and after executing the transactions print the number of tuples in the order, neworder, and orderline tables

You can find the initial data files and a script implementing the newOrder transaction here. Send your solution to Viktor Leis until 27 Oct 2016, 2pm.