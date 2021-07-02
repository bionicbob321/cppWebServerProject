# Databse Interface

This folder contains the database interface application. This is the only thread in the entire project that writes to the SQLite database.
This is to make sure that concurrent processes do not attempt to write to the database, which is unsupported in SQLite. WAL is also enabled
to prevent read locking wherever possible.


This process acts as a local winsock server, and recieves write requests from other threads in the main web server, parses the request and
writes to the database.


To compile this code yourself you must download the sqlite3 repository, compile sqlite3.c as a .o file, and like that file to the rest of the
code at compile time. You also need to include the sqlite3.h file for function definitions
