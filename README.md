# shellProject (shellP)
**A high-performance Unix-based command-line interpreter.**

## Overview
`shellP` is a custom systems-level shell built in C. It provides a direct interface to the operating system kernel, handling process lifecycle management, 
environment pathing, and standard stream redirection. It is designed to be lightweight, efficient, and robust against malformed user input.

## Core Features
* **Dual Execution Modes:** * **Interactive:** Real-time command processing with a persistent prompt.
    * **Batch:** Non-interactive execution of scripts for automation.
* **Advanced Process Logic:** Implements execution through a managed `fork-exec-wait` cycle to ensure shell stability during child process failures.
* **Flexible I/O Redirection:** Enables dynamic output routing to files using the `>` operator.
* **Dynamic Path Resolution:** User-definable execution paths via the built-in `path` command, allowing for a customizable environment.

## Architecture
The shell operates on a classic **Read-Eval-Print Loop (REPL)** architecture:
1. **Parser:** Tokenizes raw input strings into executable commands and arguments.
2. **Logic Engine:** Distinguishes between built-in commands (cd, exit, path) and external binaries.
3. **Executor:** Spawns child processes and manages I/O stream redirection.

## Getting Started
### Prerequisites
* GCC (GNU Compiler Collection)
* Make

### Build Instructions
```bash
make
