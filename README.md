Force socket bind calls to bind to a different port
===================================================

LD_PRELOAD this library to "rwrite" port values in bind calls. You
must set environment variable `REBIND_PORT` to value `FROM:TO`

In following example `nc` tries to bind to port 9000, but
becaue of librebind it is "rewritten" to bind to port 10000:

```
make
LD_PRELOAD=./librebind.so REBIND_PORT=9000:10000 nc -l 9000 &
lsof -p $! -P -a -i
```

You can force any application (like jprofiler) to bind to 
random port by specifying 0 as "target port".
