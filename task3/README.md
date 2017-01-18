#Task 3
1. Implement the following SQL query as a C++ program using STL data structures for the join implementation. Do not use any existing index structures (primary key indexes):

```
select sum(ol_quantity*ol_amount-c_balance*o_ol_cnt)
from customer, "order", orderline
where o_w_id = c_w_id
and o_d_id = c_d_id
and o_c_id = c_id
and o_w_id = ol_w_id
and o_d_id = ol_d_id
and o_id = ol_o_id
and c_last like 'B%'
```

2. Run the query 10 times in sequence. For each run, print the result of the query (should be 1367872001.25 initially) and measure the execution times.
3. Execute 1 million transactions (the newOrder and delivery mix). Using the fork system call, run the query concurrently. Whenever a query is finished, create a new snapshot using fork, so that exactly one snapshot and query is active at any given time. This example for using fork may be helpful.
4. Measure how many transactions per second you can process using this snapshotting model, how long the queries take on average, and how long the fork takes.

Send your solution to Viktor Leis until 10 Nov 2016, 2pm.