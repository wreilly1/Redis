# Mini Redis in C++

## Overview

This project is a simplified implementation of Redis written in C++. It demonstrates core concepts of systems programming including networking, multithreading, and persistence. The mini Redis clone supports a subset of Redis commands and includes these features:

- **Persistence:** Uses an Append-Only File (AOF) mechanism to log every state-changing command to `appendonly.aof`. On startup, the log is replayed to reconstruct the in‑memory state.
- **Concurrency:** Implements a thread pool to handle multiple client connections concurrently.

## Features

- **Core Commands:** `SET`, `GET`, `DEL`
- **Numeric Commands:** `INCR`, `DECR`
- **Expiration Commands:** `EXPIRE`, `TTL`, `PERSIST`
- **Multiple Key Operations:** `MSET`, `MGET`
- **List Operations:** `LPUSH`, `RPUSH`, `LPOP`, `RPOP`, `LRANGE`
- **Server Commands:** `PING`, `QUIT`, `KILL`
- **Persistence:** AOF persistence with command replay on startup
- **Concurrency:** Thread pool for managing client connections

## Requirements

- A C++17 compliant compiler (e.g., `g++` 7 or later)
- Make (GNU Make)
- Linux environment (the project has been tested on Linux)
