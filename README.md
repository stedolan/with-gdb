`with-gdb` gives you a backtrace and an interactive `gdb` if your
program segfaults:

```
$ ./segfaulty
1
2
3
Segmentation fault

$ eval `/path/to/with-gdb/with-gdb`
$ ./segfaulty
1
2
3
[... some gdb spam ...]
Program received signal SIGSEGV, Segmentation fault.
0x0000000000400591 in bad_function () at segfaulty.c:3
3	int bad_function() { return *p; }
#0  0x0000000000400591 in bad_function () at segfaulty.c:3
#1  0x00000000004005bc in main () at segfaulty.c:5
(gdb)
```

it's entirely useless, because your programs never segfault.

`with-gdb` doesn't run programs under `gdb` - that affects performance
and sometimes semantics. instead, it adds a signal handler to every
program: if they receive `SIGSEGV` or `SIGABRT`, a `gdb` instance is
started and attached to the failing process.
