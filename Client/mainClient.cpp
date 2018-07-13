/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*  
 * File:   main.cpp
 * Author: Alice
 *
 * Created on July 10, 2018, 3:49 PM
 */
//libs used
#include <windows.h>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;

#define BUFFSIZE 512

using namespace std;

LPDWORD bytesRead = 0;
LPDWORD bytesWritten = 0;
LPCTSTR myPipe = TEXT("\\\\.\\pipe\\pipeline");
LPCSTR pipeName = "\\\\.\\pipe\\pipeline";



HANDLE createHandle(){
    HANDLE h = CreateFile(myPipe, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    while(true){
        if (h != INVALID_HANDLE_VALUE) {
            cout << "Valid handle for pipe!" << endl;
            break; 
        }
        if (GetLastError() == ERROR_PIPE_BUSY){
            cout << "Waiting for pipe..." << endl; 
            if (WaitNamedPipe(pipeName, 5000)){
                cout << "Pipe available!" << endl;
                break;
            }
            else{
                cout << "No pipes found, trying again..." << endl;
                continue;
            }
        }
    }
    cout << "Returning handle..." << endl;
    return h;
}


void usePipe(string message){
    HANDLE h = createHandle();
    //message = "{ \nthis \nis \ngoing to be a json! \n}";
    
    int size;
    const char * pm = nullptr;

    size = message.length();
    pm = message.c_str();

    bool writeSize = WriteFile(h, &size, sizeof(int),  bytesWritten, NULL);
    if(writeSize) cout << "Size message written!" << endl;
    else cout << "Failed to write size message :(" << endl;
    
    bool writeMessage = WriteFile(h, pm, size*sizeof(char),  bytesWritten, NULL);
    if(writeMessage) cout << "Message written!" << endl;
    else cout << "Failed to write message :(" << endl;
    
    char res = 'n';
    bool readACK = ReadFile(h, &res, BUFFSIZE, bytesRead, NULL);
    if(readACK) cout << "ACK from server read!" << endl;
    else cout << "Failed to read ACK from server :(" << endl;
    cout << res << endl;
    
    CloseHandle(h);
}

int main(int argc, char** argv) {
    string message = "yes";
    
    while(true) {
        cout << "Message: ";
        cin >> message;
        cout << "\n" << message << endl;
        if(message != "no")
            usePipe(message);
        else break;
    }
    return 0;
}
