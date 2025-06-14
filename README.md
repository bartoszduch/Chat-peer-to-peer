# Chat Peer-to-Peer with TLS (C / OpenSSL)

A simple peer-to-peer chat written in C using TCP sockets and TLS (SSL) encryption via the OpenSSL library. Each user can act as a server or a client and communicate directly — no central server required.

---

## Requirements

- Linux / WSL / Ubuntu
- GCC compiler
- OpenSSL development libraries

### Install dependencies:

```bash
sudo apt update
sudo apt install gcc make libssl-dev
