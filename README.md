# Reading and writing memory of other processes using fasttrap
The `/dev/fasttrap` device for creating trap nodes in user-space processes for 
the `pid` and `objc` providers has the permissions `666`. In contrast to 
`/dev/dtrace`, which is also `666`, fasttrap lacks a permission check. This 
allows any process to issue a `FASTTRAPIOC_MAKEPROBE` or `FASTTRAPIOC_GETINSTR`
ioctl.  
An attacker can create probes in any other process for any memory 
location using `FASTTRAPIOC_MAKEPROBE`. On activation of these probes, the 
values at the specified memory location is replaced with a trap instruction 
(`0xCC` on x86) and the original value is placed in a shadow memory. Retrieving 
the original value is possible with the `FASTTRAPIOC_GETINSTR` ioctl. This 
allows the attacker to read arbitrary memory of another process by placing 
multiple traps.

This vulnerability is tracked as CVE-2020-27949, [fixed in macOS Big Sur 11.1,
Security Update 2020-001 Catalina, Security Update 2020-007 Mojave
](https://support.apple.com/HT212011).

## Disclosure Timeline
* 2020-07-07 -> Initial disclosure to Apple Product Security.
* 2020-07-08 <- Investigation confirmation.
* 2020-10-28 -> Request for status update.
* 2020-12-07 <- Notification about upcoming update that addresses issue.
* 2020-12-14 <- Update released.

## Requirements
* Attacker can execute code with regular user permissions (non-elevated)
* Probes in the victim process must be enabled (i.e. the victim must be run 
under DTrace)

## Tested system configurations
- SIP is on (no exceptions)
- 10.15.5 on iMac Late 2012
- 10.15.5 on MacBook Pro 2020

## Impact
* Attacker can read memory of other processes executed under DTrace without 
elevated permissions. Even reading memory from processes running as root is 
possible.
* Attacker can write the trap instruction value (`0xCC` on x86) to any memory 
location of other processes executed under DTrace without elevated permissions. 
Even writing memory to processes running as root is possible. As part of other
instructions or jump addresses, the victim's control flow can be altered,
allowing escalation of privileges.

## Expectations
As a user, I expect that enabling DTrace only allows processes with root 
permissions to trace, instrument, or modify other processes. DTrace is a 
valuable tool for administrative tasks offering tools like `dtruss`, `iotop`, 
etc.

## Exploit details
Our victim (`target`) is an executable running as root. The 
attacker (`memaccess`) is an executable running with regular user 
permissions. The tracing script `libc_monitor.d` records the number of calls to 
functions in the `libsystem_c.dylib` shared object (this is just a 
simple example of an administrative script a machine owner might run).  

1. Victim is executed
2. The attacker creates a probe with victim's PID and a memory location they 
are interested in.
3. The attacker waits for DTrace to trace the victim process. This could 
happen as part of an administrative operation where the machine owner wants to 
trace functions in the victim process (e.g. `libc_monitor.d`).
4. When DTrace starts tracing the victim, the previously created probes will be
enabled, thus, replacing the original memory values.
5. The attacker uses `FASTTRAPIOC_GETINSTR` to read the memory values of the 
created probes.  
  
Step 4 replaces memory in the victim process with the trap instruction value. 
Depending on the application, an attacker can use this to write a previously 
known value to a critical memory location. For example, the attacker could 
control the seed of an RNG or replace cryptographic keys in a process.  
  
Note: The attacker never requires elevated or root permissions.  
Note 2: Self-written DTrace scripts are not required. For example, `dapptrace` 
also uses the `pid` provider to trace any function.

## Proposed Fix
Implement privilege checks in `/dev/fasttrap` like in `/dev/dtrace` or remove 
the world-writable permission.  

Note: We did not check the released fix.

## Instructions
1. Ensure, that DTrace is available and working correctly
2. Run `target` and follow the instructions on-screen
