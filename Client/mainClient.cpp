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
#include <json.hpp>

using json = nlohmann::json;

#define BUFFSIZE 512

using namespace std;

LPDWORD bytesRead = 0;
LPDWORD bytesWritten = 0;
LPCTSTR myPipe = TEXT("\\\\.\\pipe\\pipeline");
LPCSTR pipeName = "\\\\.\\pipe\\pipeline";


string createMessage(){
    cout << "aqui5" <<endl;
    json message;
    string main[3];
    main[0] = "type";
    main[1] = "methodCalled";
    main[2] = "address";
    main[3] = "object";
    
    
    string object[4];
    
    object[0] = "shape";
    object[1] = "base";
    object[2] = "height";
    object[3] = "radius";
    
    string type;
    cout << "aqui6" <<endl;
    
    for(int i = 0; i < 4; i++ ){
        if(main[i] == "type")
        {
            cout << "Type(i - int/f - float/s - string/o - object): "<< endl;
            cin >> type;
            message[main[i]] = type;
           
        }
        else if(main[i] == "address")
        {
            int a;
            cout << "Object address in the server: "<< endl;
            cin >> a;
            message[main[i]] = a;
        }
        else if(main[i] == "methodCalled")
        {
            int x;
            cout << "Method called(1 - create/2 - update/3 - retrieve/4 - delete): "<< endl;
            cin >> x;
            message[main[i]] = x;
        }
        else if(main[i] == "object")
        {
            string shape;
            for(int j = 0; j < 4; j++){
                if(type == "o") {
                    if(shape != "circle"){
                        if(object[j] == "shape")
                        {
                            cout << "Type(square/circle): "<< endl;
                            cin >> shape;
                            message[main[i]][object[j]] = shape;
                        }
                        else if(object[j] == "base") 
                        {
                            int b;
                            cout << "Base: "<< endl;
                            cin >> b;
                            message[main[i]][object[j]] = b ;
                        }
                        else if(object[j] == "height") 
                        {
                            int h;
                            cout << "Height: "<< endl;
                            cin >> h;
                            message[main[i]][object[j]] = h ;
                        }
                        message[main[i]]["radius"]  = -1;
                    }
                    else if(object[j] == "radius") 
                    {
                        string r;
                        cout << "Radius: "<< endl;
                        cin >> r;
                        message[main[i]]["base"]  = -1;
                        message[main[i]]["height"]  = -1;
                        message[main[i]][object[j]] = r ;
                    }
                }
                else {
                    if(type == "i") {
                        int x;
                        cout << "Int: "<< endl;
                        cin >> x;
                        message[main[i]] = x ;
                        break;
                    }
                    else if(type == "f") {
                        float f;
                        cout << "Float: "<< endl;
                        cin >> f;
                        message[main[i]] = f ;
                        break;
                    } 
                    else if(type == "s"){
                        string s;
                        cout << "String: "<< endl;
                        cin >> s;
                        message[main[i]] = s ;
                        break;
                    }
                }
            }
        }
    }
    cout << message.dump(4);
    return message.dump(4);
}

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
    //string message = "{ \nthis \nis \ngoing to be a json! \n}";
    //string message = "";
    
    int size;
    const char * pm = nullptr;
    
    
    //message = createMessage();
    cout << message << endl;
    
    

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
        cout << "\n" ;
        if(message != "no")
            usePipe(message);
        else break;
    }
    return 0;
}
