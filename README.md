# Rewind
Rewind is a command log library. Similar to a WAL (write-ahead-log).

Command logging provides durability of database operations. Command logging keeps a record of every transaction. If the server fails for any reason, the database can restore the last snapshot and "replay" the log entries to recover the database.

# Differences between a Command Log and a Write-Ahead-Log
The difference between a `command log` and `write-ahead-log` is that a `command log` records the invocations, whereas a `WAL` records the consequences of the transactions.

This means the timestamp of the replay time may be different than the time the transaction was recorded. Recovering changes read from a command log that use random number generators or date/time based operations need to reproduce the original seed or timestamp. 

Rewind gets around this by recording the timestamp in each log entry so that replaying the command log can source the original time the transaction occured.

# Log entry format
Each log entry in Rewind has the following byte layout.

Total 12 bytes  

```
---------------------------------------
Bytes | Description
---------------------------------------
1       Flags
4       Seconds since the Unix epoch
3       Machine identifier
2       Process id
3       Counter, starting with a random value
4       Size in bytes of log data (uint32, max value of 4,294,967,295)
N       Log entry data
``` 
