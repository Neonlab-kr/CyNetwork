/*
    Note: This source code is written by beginners and can be terrible.
    This program does not handle all exceptions, and there are many security vulnerabilities.
*/

#include "server.h"

#include "userdata.h"
#include "postdata.h"

#include <stdio.h>
#include <string.h>
#include <io.h>

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")

int recvn(SOCKET sock, char* buf, int n) {
    int idx = 0;
    while (TRUE) {
        if (recv(sock, &buf[idx++], 1, 0) <= 0)
            return 0;

        if (idx == n)
            return n;
    }    
}

extern char postDataPath[2048];

DWORD WINAPI clientThreadFunc(void* data) {
    SOCKET sock = (SOCKET)data;

    puts("client connected");

    int loggedIn = 0;
    char login_id[32] = { 0 };

    while (TRUE) {
        char command;
        if (recvn(sock, &command, 1) == 0) {
            return 0;
        }

        if (command == 0) {
            char id_len,  password_len, name_len;

            char id[32] = { 0, };
            char password[32] = { 0, };
            char name[32] = { 0, };

            if (recvn(sock, &id_len, 1) == 0) break;
            if (recvn(sock, &password_len, 1) == 0)  break;
            if (recvn(sock, &name_len, 1) == 0) break;

            if (id_len <= 0 || name_len <= 0 || password_len <= 0) {
                break;
            }

            if (recvn(sock, id, id_len) == 0) break;
            if (recvn(sock, password, (int)password_len) == 0) break;
            if (recvn(sock, name, (int)name_len) == 0) break;

            char result = 0;
            if (createUser(id, password, name)) result = 1;

            send(sock, &result, 1, 0);
        }

        if (command == 1) {
            char id_len, password_len;

            char id[32] = { 0, };
            char password[32] = { 0, };

            if (recvn(sock, &id_len, 1) == 0) break;
            if (recvn(sock, &password_len, 1) == 0)  break;

            if (id_len <= 0 || password_len <= 0) break;

            if (recvn(sock, id, (int)id_len) == 0) break;
            if (recvn(sock, password, (int)password_len) == 0) break;

            if (loggedIn) {
                send(sock, "\0", 1, 0);
                continue;
            }

            USER* user = getUser(id);
            if (user == 0) {
                send(sock, "\0", 1, 0);
                continue;
            }

            if (!passwordMatch(id, password)) {
                send(sock, "\0", 1, 0);
                continue;
            }

            loggedIn = 1;
            strcpy(login_id, id);
            send(sock, "\x01", 1, 0);
        }

        if (command == 2) {
            if (!loggedIn) {
                send(sock, "\0", 1, 0);
                continue;
            }

            USER* user = getUser(login_id);
            char name_len = (char)strlen(user->name);

            if ( name_len <= 0 || 20 < name_len)
                break;

            send(sock, &name_len, 1, 0);
            send(sock, user->name, strlen(user->name), 0);

            for (int i = 0; i < user->friendCount; i++) {
                send(sock, user->friends[i], strlen(user->friends[i]), 0);
                send(sock, "\n", 1, 0);
            }

            send(sock, "\0", 1, 0);
        }

        if (command == 3) {
            if (!loggedIn) {
                send(sock, "\0", 1, 0);
                continue;
            }

            USER* user = getUser(login_id);

            char id_len;
            char id[32] = { 0 };

            if (recvn(sock, &id_len, 1) == 0) break;

            if (id_len <= 0)
                break;

            if (recvn(sock, id, (int)id_len) == 0) break;

            if (addFriend(login_id, id)) send(sock, "\x01", 1, 0);
            else send(sock, "\0", 1, 0);
        }

        if (command == 4) {
            if (!loggedIn) {
                send(sock, "\0", 1, 0);
                continue;
            }

            USER* user = getUser(login_id);

            char id_len;
            char id[32] = { 0 };

            if (recvn(sock, &id_len, 1) == 0) break;

            if (id_len <= 0)
                break;

            if (recvn(sock, id, (int)id_len) == 0) break;

            if (removeFriend(login_id, id)) send(sock, "\x01", 1, 0);
            else send(sock, "\0", 1, 0);
        }

        if (command == 5) {
            if (!loggedIn) {
                send(sock, "\0", 1, 0);
                continue;
            }

            char title_len;
            char title[64] = { 0 };

            if (recvn(sock, &title_len, 1) == 0) break;

            if (title_len <= 0) break;

            if (recvn(sock, title, (int)title_len) == 0) break;
            
            char body[512] = { 0, };
            int idx = 0;
            while (TRUE) {
                if (recvn(sock, &body[idx], 1) == 0) break;
                if (body[idx] == '\0') break;
                if (500 == idx + 1) {
                    body[idx + 1] = '\0';
                    break;
                }
                idx++;
            }

            if (createPost(login_id, title, body) == 0) send(sock, "\x00", 1, 0);
            else send(sock, "\x01", 1, 0);
        }

        if (command == 6) {
            if (!loggedIn) {
                send(sock, "\0", 1, 0);
                continue;
            }

            char res[16384] = { 0 };

            _finddata_t fd;
            long handle;
            int result = 1;
            char findPath[2048] = { 0 };
            sprintf(findPath, "%s\\*", postDataPath);
            handle = _findfirst(findPath, &fd);

            if (handle == -1) {
                send(sock, res, strlen(res) + 1, 0);
                continue;
            }

            while (result != -1)
            {
                if (!(strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0)) {
                    long long id;
                    sscanf(fd.name, "%lld", &id);
                    POST* post = getPost(id);
                    if (post != 0) {
                        char append[2048] = { 0 };
                        sprintf(append, "author: %s, id:%lld\n[%s]\n%s\n\n", post->authorId, post->id, post->title, post->body);
                        strcat(res, append);
                    }
                }
                result = _findnext(handle, &fd);
            }

            send(sock, res, strlen(res) + 1, 0);

            _findclose(handle);
        }
    }

    closesocket(sock);

    return 0;
}

void server() {
    WSADATA wsdata;

    int iRes = WSAStartup(MAKEWORD(0x02, 0x02), &wsdata);
    if (ERROR_SUCCESS != iRes) abort();

    SOCKET sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sock) abort();

    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serverAddr.sin_port = htons(3000);

    iRes = bind(sock, (LPSOCKADDR)&serverAddr, sizeof(serverAddr));
    if (ERROR_SUCCESS != iRes) abort();

    iRes = listen(sock, SOMAXCONN);
    if (ERROR_SUCCESS != iRes) abort();

    sockaddr accept_addr;
    int iLen = sizeof(accept_addr);

    puts("Server Started!");

    while (TRUE) {
        SOCKET sockAccept = accept(sock, &accept_addr, &iLen);
        CreateThread(NULL, 0, clientThreadFunc, (LPVOID)sockAccept, 0, NULL);
    }
    
    closesocket(sock);
    WSACleanup();
}