create table warehouse (
   w_id integer not null,
   w_name varchar(10) not null,
   w_street_1 varchar(20) not null,
   w_street_2 varchar(20) not null,
   w_city varchar(20) not null,
   w_state char(2) not null,
   w_zip char(9) not null,
   w_tax numeric(4,4) not null,
   w_ytd numeric(12,2) not null,
   sys_start datetime GENERATED ALWAYS AS ROW START NOT NULL,
   sys_end datetime GENERATED ALWAYS AS ROW END NOT NULL,
   PERIOD FOR SYSTEM_TIME (sys_start,sys_end),
   primary key (w_id)
)
WITH (SYSTEM_VERSIONING = ON);
