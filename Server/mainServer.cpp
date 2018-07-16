/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Alice
 *
 * Created on July 10, 2018, 4:13 PM
 */
//libs used
#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include <stack>
#include <vector>

//other libs
//#include <cstdlib>
//#include <fstream>
//#include <assert.h>

#define _CRT_SECURE_NO_WARNINGS
#define BUFFSIZE 512

using namespace std;


LPDWORD bytesRead = 0;
LPDWORD bytesWritten = 0;
LPCTSTR myPipe = TEXT("\\\\.\\pipe\\pipeline");
LPCSTR pipeName = "\\\\.\\pipe\\pipeline";

const int max_instances = 5;
thread pipeThreads[max_instances];
stack<int> threadStack;


void serveClient(HANDLE h, int threadNumber){
    cout << "Serving client on thread " << threadNumber << "..."<< endl;
    string message = "";

    int b;
    bool readSize = ReadFile(h, &b, BUFFSIZE, bytesRead, NULL);
    if(readSize) cout << "Size message read!" << endl;
    else cout << "Failed to read size message :(" << endl;
    
    
    char m[1000];
    char * pm = m;
    bool readMessage = ReadFile(h, pm, b*sizeof(char), bytesRead, NULL);
    if(readMessage) cout << "Message read!" << endl;
    else cout << "Failed to read message :(" << endl;


    message = pm;
    cout << pm << endl;

    char x = 'y';
    bool writeACK = WriteFile(h, &x , sizeof(char), bytesWritten, NULL);
    if(writeACK) cout << "Wrote ACK to client!" << endl;
    else cout << "Failed to write ACK to client  :(" << endl;
    
    cout << "Client served on thread " << threadNumber << "!"<< endl;
    cout << "Closing handle..." << endl;
    FlushFileBuffers(h); 
    DisconnectNamedPipe(h); 
    CloseHandle(h); 
    cout << "Handle closed!" << endl;
    
}

void createHandles(){
    int i = 1;
    //for(int i = 0; i < 3; i++){  
    for(;;){
        HANDLE h = CreateNamedPipe(myPipe, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, max_instances, BUFFSIZE, BUFFSIZE, 0, NULL);
        bool connected = false;

        if (h != INVALID_HANDLE_VALUE){
            cout << "Valid handle!" << endl;
            connected = ConnectNamedPipe(h, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED); 
            cout << "Connected to pipe!" << endl;
        }
        else {
            cout << "Invalid handle :(" << endl;
            continue;
        }
        
        if (connected){
            
            cout << "Creating thread..." << endl;
            //async(launch::async, serveClient, h, i);
            if(!threadStack.empty()){
                int threadNumber = threadStack.top();
                threadStack.pop();
                pipeThreads[threadNumber] = thread(serveClient, h, threadNumber);
            }
        }
        else{
            cout << "Failed to serve client... :(" << endl;
            cout << "Closing handle..." << endl;
            CloseHandle(h); 
            cout << "Handle closed!" << endl;
            continue;
        }
        if(i < max_instances) i++;
        else i  = 1;
    }
    for(int j = 0; j < max_instances; j++){
        pipeThreads[j].join();
    }
    
}

void initThreadStack(){
     for(int i = 0; i < max_instances; i++){
        threadStack.push(i);
    }
}
 
int main(int argc, char** argv) {
    initThreadStack();
    createHandles();
    return 0;
}


