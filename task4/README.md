#Task 4
1. Implement query compilation of relational algebra trees to C++ code. Use the produce/consume (push) model. You need to support the table scan, selection, print, and inner join operators.
2. Manually build an algebra tree for the following SQL query:
```
select c_first, c_last, o_all_local, ol_amount 
from customer, "order", orderline
where o_w_id = c_w_id
and o_d_id = c_d_id
and o_c_id = c_id
and o_w_id = ol_w_id
and o_d_id = ol_d_id
and o_id = ol_o_id
and c_id = 322
and c_w_id = 1
and c_d_id = 1
```
3. Compile the expression to C++ code and execute the resulting code (including a print operator at the root). For reading the generated code it may be useful to indent it using clang-format
4. Send your solution to Viktor Leis until 17 Nov 2016, 2pm.