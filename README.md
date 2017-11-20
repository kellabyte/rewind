# Rewind
Rewind is a command log library. Similar to a WAL (write-ahead-log).

Command logging provides durability of database operations. Command logging keeps a record of every transaction. If the server fails for any reason, the database can restore the last snapshot and "replay" the log entries to recover the database.

# Differences between a Command Log and a Write-Ahead-Log
The difference between a `command log` and `write-ahead-log` is that a `command log` records the invocations, whereas a `WAL` records the consequences of the transactions.

Recovering changes read from a write-ahead-log is more straight forward with operations that use random number generators or timestamps and timestamp functions than a command log because it restores the stored values from the past. A command log replays operations which means the timestamp of the replay time may be different than the time the transaction was recorded.

Rewind gets around this by recording the timestamp in each log entry so that replaying database operations can source the original time the transaction occured.

# Log entry format
Each log entry in Rewind has the following byte layout.

Total 12 bytes  

```
---------------------------------------
Bytes | Description
---------------------------------------
4       Seconds since the Unix epoch
3       Machine identifier
2       Process id
3       Counter, starting with a random value
4       Size in bytes of log data
N       Log entry data
``` 
