#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#endif

#include "sqlite/sqlite3.h"
#include <string>
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <vector>

using std::cout;
using std::cin;
using std::string;
using std::endl;

///
/// DATA STRUCTURES
///

struct loginRequestData {
    int userID; //maybe change to string
    string passwordHash;
    string passwordSalt;
    string emailAddress;
    string userName;
};

struct passwordUpdate {
    int userID;
    string passwordHash;
    string passwordSalt;
};

struct miscUpdate {
    int userID;
    string record;
};

///
/// CALLBACKS
///

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    cout << "Success!" << endl;
    return 0;
}

/// 
/// INITIAL REQUEST HANDLERS
///

void handleRequest(string request) { // refer to loginDataStructure.txt to understand this parsing data
    switch (request[0]) {
        case 'þ': //signup request
        request.erase(0);
        handleRequest_signup(request);
        break;
        case 'ü': //update request
        request.erase(0);
        switch (request[0]) {
            case '0': //password change
            request.erase(0);
            handleRequest_password_Change(request);
            break;
            case '1': //email address change
            request.erase(0);
            handleRequest_emailAddress_Change(request);
            break;
            case '2': //username change
            request.erase(0);
            handleRequest_username_Change(request);
            break;
        }
        break;
        case 'ø': //delete account request
        request.erase(0);
        handleRequest_account_delete(stoi(request));
        break;
    }
}

void handleRequest_signup(string rq) { // probs not nessesary but makes code cleaner
    writeRecord_account_Create(parseData_login(rq));
}

void handleRequest_password_Change(string rq) {
    //stuff
}

void handleRequest_username_Change(string rq) {
    //stuff
}

void handleRequest_emailAddress_Change(string rq) {
    //stuff
}

void handleRequest_account_delete(int userID) {
    //stuff
}

///
/// REQUEST PARSERS
///

loginRequestData parseData_login(string ULD) { //unparsedLoginData
    loginRequestData PLD; //parsedLoginData

    std::vector<string> splitULD;
    string temp;

    for (int i = 0; i < ULD.size(); i++) {
        if (ULD[i] == 'ÿ') { // "" is a c-string and '' is a char for some reason
            splitULD.push_back(temp);
            temp = "";
        } else {
            temp += ULD[i];
        }
    };

    PLD.userID = stoi(splitULD[0]); // stoi() parses int from string
    PLD.passwordHash = splitULD[1];
    PLD.passwordSalt = splitULD[2];
    PLD.emailAddress = splitULD[3];
    PLD.userName = splitULD[4];

    return PLD;
}

///
/// RECORD AND DATABASE WRITERS
///

void writeRecord_password_update(passwordUpdate pwu) {
    //stuff
}

void writeRecord_account_Create(int uid, string pwh, string pws, string ema, string usn) { //needs to check uniqueness of each param //need to find a way to parse all this info from a string. maybe use a struct
    sqlite3 *db;
    int dbc;
    char *zErrMsg = 0;
    string sql;
    string uids = std::to_string(uid); 



    dbc = sqlite3_open("example.db", &db);

    if (dbc) {
        cout << "Error opening database" << endl;
        return;
    } else {
        cout << "Successfully opened database" << endl;
    }

    sql = 
    "INSERT INTO users(userID, PWHash, PWSalt, emailAddress, userName) " \
    "VALUES (" + uids + ", " + pwh + ", " + pws + ", " + ema + ", " + usn + "); ";

    const char * sqlc = sql.c_str();

    dbc = sqlite3_exec(db, sqlc, callback, 0, &zErrMsg);

    sqlite3_close(db);
}

void writeRecord_account_Create(loginRequestData lrd) { // overloaded function which takes a struct instead of individual data
    sqlite3 *db;
    int dbc;
    char *zErrMsg = 0;
    string sql;
    string uids = std::to_string(lrd.userID);

    dbc = sqlite3_open("example.db", &db);

    if (dbc) {
        cout << "Error opening database" << endl;
        return;
    } else {
        cout << "Successfully opened database" << endl;
    }

    sql = 
    "INSERT INTO users(userID, PWHash, PWSalt, emailAddress, userName) " \
    "VALUES (" + uids + ", " + lrd.passwordHash + ", " + lrd.passwordSalt + ", " + lrd.emailAddress + ", " + lrd.userName + "); ";

    const char * sqlc = sql.c_str();

    dbc = sqlite3_exec(db, sqlc, callback, 0, &zErrMsg);

    sqlite3_close(db);
}