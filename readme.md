## QtDao2.0
#### TODO List:
- **support**
  - [x] sqlite
  - [ ] mysql
  - [ ] sqlserver
  - [ ] Confusion

- **loader**
  - [x] configure loader
  - [ ] driver loader
  - [x] connect test
  - [x] auto create database/table (use config)
  - [ ] truncate/drop table
  - [ ] drop database
  - [ ] database switch
- **upgrader**
  - [x] version check
  - [x] table upgrade
- **log**
  - [ ] sql statement print 
  - [ ] value list print
- **entity generator**
  - [x] field getter/setter
  - [x] table field
  - [x] transient field
  - [x] default value
  - [x] function query result getter
- **query condition**
  - [x] operator field <-> value
  - [ ] operator field <-> field
  - [ ] ~~operator field <-> function~~
  - [ ] ~~operator function <-> function~~
  - [ ] operator field <-> query results
- **query executor**
  - insert operator
    - [x] object(s) insert
    - [x] part field value insert
    - [ ] insert in query results
  - query operator
    - [x] unique/list query
    - [ ] multi-table query
    - [ ] union query
    - [ ] inner/left/right/full join
    - [ ] recursive query
    - [ ] query in query/join results
    - [x] original query
    - [x] custom function query
    - [ ] custom statement query
  - count operator
    - [x] condition count
    - [ ] multi-table count
    - [ ] count in query result
  - update operator
    - [ ] set by where update
    - [ ] object(s) update
    - [ ] update with query results
  - delete operator
    - [ ] condition delete
    - [ ] object(s) delete
    - [ ] delete by query results
  - [ ] transaction
- **connection pool**
  - [x] connection reuse
  - [x] work thread connection separate
  - [x] work thread connection reuse
- **auto close connection**
  - [ ] auto close connection when work thread end
- **exception**
  - [x] load configure exception
  - [x] upgrade exception
  - [x] query exception
  - [x] connection pool exception
