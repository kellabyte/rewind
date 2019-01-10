# Rewind
Rewind is a command log library. Similar to a WAL (write-ahead-log).

Command logging provides durability of database operations. Command logging keeps a record of every transaction. If the server fails for any reason, the database can restore the last snapshot and "replay" the log entries to recover the database.

# Differences between a Command Log and a Write-Ahead-Log
The difference between a `command log` and `write-ahead-log` is that a `command log` records the invocations, whereas a `WAL` records the consequences of the transactions.

This means the timestamp of the replay time may be different than the time the transaction was recorded. Recovering changes read from a command log that use random number generators or date/time based operations need to reproduce the original seed or timestamp. 

Rewind gets around this by recording the timestamp in each log entry so that replaying the command log can source the original time the transaction occured.

# Goals
The goal for Rewind are to implement an optional logical command log for LMDB. LMDB is a very fast storage engine that performs very fast for many types of workloads but there are a few workloads that could potentially benefit from having a commit log in place.

This also becomes the foundation for replication. It's a goal to layer on capabilities that can be enabled as desired as optional features.

# Log entry format
TODO.

# Compiling
```
make
```

# Running tests
```
./build/tests

===============================================================================
test cases: 1 | 1 passed
assertions: - none -
```