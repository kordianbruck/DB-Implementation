% Temporal Tables with in-memory Databases
% Kordian Bruck <ask@bruck.me>
% 10.03.2017
---

This is a summary of the project written for the database implementation practical course at TUM.

## Abstract
Temporal tables describe a way to organize historical data within normal database tables that allow us to time travel to snapshots of previous timestamps. We can not only view the state of all records at one specific time but also run an analysis on how the data changed.

The key benefit with this approach is that regular SQL syntax can be used to achieve this archiving functionality. With the SQL2011 standard we now have a common approach and behavior definition for temporal tables.

The idea for this project is to create an example implementation within an in-memory database as a proof of concept and to check what obstacles are encountered in terms of performance and optimization.

## Introduction
Usually SQL statements like ```UPDATE```, ```INSERT``` or  ```DELETE``` are destructive operations. The data that is being manipulated will be removed from the persisting medium. The data cannot be recovered or used in the future. This might have been useful when disk space on hard drives was expensive but today we are able to save everything. The idea with temporal tables is that we rewrite destructive operations in a way to make them keep a history of previous versions.

Already in the early nineties a few people recognized the need for temporal aware tables. [TSQL2](https://www2.cs.arizona.edu/~rts/tsql2.html) was worked on in 1993 most prominently by Richard T. Snodgrass. A preliminary language specification was released which made first steps towards embedding temporal tables in the language syntax of SQL.

Unfortunately the approach TSQL2 was not further pursed and it was only revisited as of 2011 when the ISO committee decided that a newer specification proposal made sense in contrast to TSQL2 as it did not introduce hidden columns. SQL2011, as part of ISO/IEC 9075, now officially represents a standardized way on how SQL syntax should behave when tables are system or application versioned.

## Functionality
The standard allows for three ways a table can be versioned:
* system versioning
* application versioning
* system & application versioning

System versioning describes the automated versioning of all rows inserted into a database. For that, one has to define two columns with a timestamp type, that are used as begin and end dates for the validity time range. The two columns responsible for the validity are managed by the system and cannot be updated or inserted. The system will automatically populate them with each operation that is executed on that row. As an example we could use a *website content* table that we want to support a redo feature on. When a user wants to restore a record we simply select the value at that timestamp and run an update with the selected value. This is most useful if we want to have an exact history of any changes that were made.

Application versioning is a bit more complex as there are more corner cases to be thought of. As with system versioning, we again need two columns for beginning and end of the validity of a row. Rather than the system taking care of the timestamps, its now in the users hand to manage them accordingly. Meaning we can also run ```INSERT``` and ```UPDATE``` operations on those columns and modify them. As an example we could have a employee that is assigned to an department for a specific period. We can also insert past events in this case, meaning that maybe someone forgot to set the correct department a couple months ago and we want to correct that. This type of versioning is most useful when we want to save logical assignments that happen for a specific period. Overlaps can happen but can also be prevented with special schema definitions.

The third type of versioning is the combination of the previous two. We can set a table to be system and application versioned to combine the two features. We will then get an accurate history of logical assignments and how they were created or changed. Especially companies that have to keep books on any transactions in the last couple years will find this most useful.

The complexity and dimensions rises with every type of versioning we add. The most simplest is system versioning as the user does not need to take care of it. Application versioning is more complex as it might require teh DBMS to do multiple checks and lookups when doing an ```UPDATE``` to an existing period that might result in two inserts and one update operation. Overlaps and other corner cases make some operations, that usually are simple, complex. Versioning is therefore to be applied with care and with knowledge of what repercussions it might have.

## Usefulness
One was already able to implement a similar behavior in normal application logic and ordinary timestamp fields, as many applications surely did. But this often was tedious work as one cannot simple add a middle ware or driver to abstract the logic, as that would mean we would then have to manipulate the input query. With the standardized way of operation we redefine the elementary operators and abstract that kind of logic into our database. Thus it is simple for developers to use and preserves the many advantages a database management system offers.

This has two benefits over application logic: First we can optimize the query engine to make use of better index structures and speed up the operations. Second we can additionally add analytical instructions to analyze the change of data over time. Together with more advanced group by clauses and aggregations one can easily query statistically relevant data using normal SQL syntax.

## Support
There are only a few database systems that support the full standard. The most complete solution is currently implemented in [IBM's DB2](https://www.ibm.com/developerworks/data/library/techarticle/dm-1204whatsnewdb210/index.html). DB2 supports all three variants of versioning and all ways of accessing it.

Oracle on the other hand decided to use their Flashback technology to mimic the SQL standards proposed way of temporal data. The problem with Oracle's solution is that it uses the transaction log as a method to travel through time. Once the log is full, old entries get deleted and cannot be recovered. Apart from confusing documentation, the feature does seem to work but is not optimized in any way.

In contrast SQL Server 2016 from Microsoft seems to have a sophisticated asynchronous persisting algorithm for their system versioned tables. Application versioning is not yet available but system versioning is well documented and thought through. The current view of the table is kept in memory for fast access. Any updates to the history are asynchronously pushed to the hard drive while keeping the data accessible.

There is a plugin available for PostgreSQL which retrofits similar functionality but it does not follow the standard exactly. One is forced to manually union selects in order to combine historic and current data opposed to queries that make use of the ```BETWEEN``` operator in DB2 or SQL Server 2016.

## Implemented solution
For the implementation I have decided to follow the syntax and modus operandi of SQL Server 2016 as it has shown to be the best documented solution. Although the ISO standard defines the exact syntax, each of the vendors has introduced small nuances that make them incompatible to each other.

The implemented system is able to handle ```UPDATE```, ```INSERT``` and  ```DELETE``` operations in a system versioned table. ```SELECT``` queries support the ```AS OF``` operator as well of any query that uses the ```BETWEEN``` keyword. All temporal queries require a full table scan in order to identify all relevant tuples. Normal queries can use index structures for lookups.

For performance testing purposes we required prepared statements. The database system is using code generation to create C++ code, compile it and load the dynamic library. Obviously without prepared statements we would need to compile the code for each execution which would mean that we cannot compare the results of the actual operation in a meaningful manner. By using prepared statements we can pass an array of parameters to be used for any of the data modifying queries.

In addition the compiled queries were cached on disk in order to prevent multiple compilations during one program execution. As one compilation would often take a couple seconds, this was impacting the user experience when using the implemented command line interface to try out the temporal features. Using a hash to identify the query we would then be able to directly load the shared library and execute the code.

## Evaluation
Of course enabling versioning on a table will have an impact on performance as our table grows in size and nothing is removed. ```INSERT``` and ```UPDATE``` statements slowed down by 30% - 40% as each operation had to insert a new row as well as update the old row. ```DELETE``` operations on the other hand have shown a speed up of nearly 80% as one was omitting deleting entries from the table and index structures in favor of simply updating one entry with the expiration timestamp. One can run the test suite from the CLI using the command ```run performance```.

As there were no optimizations yet done on ```SELECT``` queries and most of them had to do a table scan in any case, there is no point in comparing the performance here.

## Outlook
In order to really make this feature viable we have to consider new index structures that support temporal queries. Interval trees might be useful but there is no standard solution for this problem statement as we need to consider multiple cases that interval trees might not cover. Depending on which data we are asking for and their respective selectivity we might even be faster with a normal table scan.

As this feature is such an underlying game changer that changes the behavior of all basic operators, a lot of previously thought of concepts in query optimization and execution must be revisited and optimized for the use cases of temporal queries. Many systems use a separate history table that would enable them to turn on compression to minimize duplicate data.

Summarizing temporal tables are a really useful addition for any database system that wants their users to analyze historical data. Big data currently is mostly about finding relationships on the current view of data, but adding a time dimension might open a lot of new ways to generate insights.
