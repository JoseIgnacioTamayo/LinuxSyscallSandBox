Testing
=======

> This code has been compiled and tested in architectures **i386** and **x86_64**.
>The demo libraries provided work for both architectures.

# Quick Demo

**runExample.sh** : Tests the most common scenarios for Sandbox, giving quick examples of all the Sandbox tool working with child processes.

 * PID Syscalls, library than returns Static PIDs
 * IO Syscalls, local files are created as sniffer in /tmp
 * Catching Syscalls Verbose with custom libraries
 * Catching Syscalls with several libraries, execution path
 * Catching Syscalls with several libraries
 * PID Syscalls of parent and child processes, library returns Static PIDs to both processes
 * Running STRACE on the same commands, to compare

# Demo

As a demonstration, a **multi-client TELNET Echo Server** is monitored and its syscalls interrupted.

The TELNET Echo Server listens on a TCP port and simply replies all the received messages with the inversed string or characters. When receiving `q` the server disconnects the client.
The TELNET Echo Server creates a forked process for each client, allowing for concurrency.

With this server in the Sandbox, the Server can request binding on privileged ports, and its input/output is changed on the fly, altering the messages it receives from the clients and the ones sent to them.

 * If the port that the **TELNET Echo Server** wants to bind is < 1024, then the port value is added 2000 by the Sandbox and binded.

 * On the replied messages, upper and lower case are inverted after the TELNET Echo Server has replied.

 * On the received messages, all numbers are changed to 0 before the message gets to the TELNET Echo Server

To test it:

 * On one terminal, run `sandbox -p -L libs -l tcp tests/ECHOserver 23`

 * On another terminal, run ` telnet <server_ip> 2023`

> To run the TELNET Server running with no library, make sure you bind a port greater than 1024 or the kernel will not let you (unless you have high permissions)

To test the TELNET Echo Server running with libtcp.so and libchatty.so, tracing child processes: `sandbox -p -L libs -l tcp -l chatty tests/ECHOserver 3000 `
