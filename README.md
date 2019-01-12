# Rewind
Rewind is a command log library. Similar to a WAL (write-ahead-log).

Command logging provides durability of database operations. Command logging keeps a record of every transaction. If the server fails for any reason, the database can restore the last snapshot and "replay" the log entries to recover the database.

# Differences between a Command Log and a Write-Ahead-Log
The difference between a `command log` and `write-ahead-log` is that a `command log` records the invocations, whereas a `WAL` records the consequences of the transactions.

# Goals
The goal for Rewind is to implement an optional logical command log for LMDB. LMDB is a very fast storage engine that performs very fast for many types of workloads but there are a few workloads that could potentially benefit from having a commit log in place. It's my long standing theory that a command log or WAL can help LMDB catch up to RocksDB in benchmarks RocksDB excels against LMDB without all the LSM complexity and LSM drawbacks. Rewind is an attempt to implement my ideas without adding a bunch of internal complexity to LMDB while enabling most LMDB consumers to use Rewind with very little or no code change.

# Log entry format
TODO.

# Compiling
```
make
```

# Running tests
```
make test

===============================================================================
test cases: 1 | 1 passed
assertions: - none -
```