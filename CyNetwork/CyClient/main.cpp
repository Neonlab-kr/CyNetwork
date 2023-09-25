#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

#pragma warning(disable:4996)
#pragma comment( lib, "ws2_32.lib")

void clear() {
    system("cls");
}

int recvn(SOCKET sock, char* buf, int n) {
    int idx = 0;
    while (TRUE) {
        if (recv(sock, &buf[idx++], 1, 0) <= 0)
            return 0;

        if (idx == n)
            return n;
    }
}

void disconnected() {
    puts("서버와 연결이 끊겼습니다");
    system("pause");
    exit(0);
}

int main() {
    WSADATA wsdata;

    int iRes = ::WSAStartup(MAKEWORD(0x02, 0x02), &wsdata);
    if (ERROR_SUCCESS != iRes) return 0;

    SOCKET sock;
    sock = ::socket(PF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sock) return 0;

    SOCKADDR_IN servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAddr.sin_port = htons(3000);

    if (connect(sock, (LPSOCKADDR)&servAddr, sizeof(servAddr)) != 0) {
        puts("서버 연결에 실패했습니다..!");
        system("pause");
        exit(0);
    }

    puts("서버에 연결되었습니다!");

    int mode = 0;
    int n;
    while (TRUE) {
        if (mode == 0) {
            puts("1.가입\n2.로그인");
            printf("번호를 입력하세요 : ");
            scanf("%d", &n);

            if (n == 1) {
                char id[32] = { 0, };
                char pw[32] = { 0, };
                char name[32] = { 0, };
                printf("아이디(영어/숫자/언더바 1~20) : ");
                scanf_s("%s", id, sizeof(id));
                printf("비밀번호(영어/숫자 8~20) : ");
                scanf_s("%s", pw, sizeof(pw));
                printf("이름(1~20) : ");
                scanf_s("%s", name, sizeof(name));

                char id_len = (char)strlen(id);
                char pw_len = (char)strlen(pw);
                char name_len = (char)strlen(name);

                send(sock, "\x00", 1, 0);
                send(sock, &id_len, 1, 0);
                send(sock, &pw_len, 1, 0);
                send(sock, &name_len, 1, 0);
                send(sock, id, strlen(id), 0);
                send(sock, pw, strlen(pw), 0);
                send(sock, name, strlen(name), 0);

                char result;
                if (recvn(sock, &result, 1) == 0) disconnected();

                if (result == 1) {
                    puts("\n가입 되었습니다\n");
                }
                else {
                    puts("\n아이디/비밀번호/이름이 올바르지 않거나, 아이디가 이미 존재합니다\n");
                }
            }
            else if (n == 2) {
                char id[32] = { 0, };
                char pw[32] = { 0, };
                printf("아이디 : ");
                scanf_s("%s", id, sizeof(id));
                printf("비밀번호 : ");
                scanf_s("%s", pw, sizeof(pw));

                char id_len = (char)strlen(id);
                char pw_len = (char)strlen(pw);

                send(sock, "\x01", 1, 0);
                send(sock, &id_len, 1, 0);
                send(sock, &pw_len, 1, 0);
                send(sock, id, strlen(id), 0);
                send(sock, pw, strlen(pw), 0);

                char result;
                if (recvn(sock, &result, 1) == 0) disconnected();

                if (result == 1) {
                    clear();
                    mode = 1;
                }
                else {
                    puts("\n아이디 또는 비밀번호를 확인해주세요\n");
                }
            }
            else {
                puts("\n없는 번호입니다\n");
            }
        }
        else if (mode == 1) {
            char name[32] = { 0 };
            char name_len;
            char friendList[8192] = { 0 };
            int idx = 0;

            send(sock, "\x02", 1, 0);

            if (recvn(sock, &name_len, 1) == 0) disconnected();
            if (recvn(sock, name, name_len) == 0) disconnected();

            while (TRUE) {
                if (recvn(sock, &friendList[idx], 1) == 0) disconnected();
                if (friendList[idx] == '\0') break;
                idx++;
            }

            int friendCount = 0;
            for (int i = 0; friendList[i] != '\0'; i++) {
                if (friendList[i] == '\n') friendCount++;
            }

            clear();

            printf("[ ==== CY NETWORK ==== ]\n내 이름 : %s\n친구 수 : %d\n\n[친구 목록]\n%s\n\n", name, friendCount, strlen(friendList) <= 1 ? "친구 없음" : friendList);

            puts("1.친구 추가\n2.친구 삭제\n3.글 목록\n4.글 등록\n5.새로 고침");
            printf("번호를 입력하세요 : ");
            scanf("%d", &n);

            if (n == 1) {
                char id[32] = { 0, };
                printf("친구 아이디 : ");
                scanf_s("%s", id, sizeof(id));

                char id_len = (char)strlen(id);

                send(sock, "\x03", 1, 0);
                send(sock, &id_len, 1, 0);
                send(sock, id, strlen(id), 0);

                char result;
                if (recvn(sock, &result, 1) == 0) disconnected();

                if (result == 1) {
                    puts("\n친구 추가 완료\n");
                    Sleep(1000);
                }
                else {
                    puts("\n이미 친구이거나, 존재하지 않는 아이디입니다\n");
                    Sleep(1400);
                }
            }
            else if (n == 2) {
                char id[32] = { 0, };
                printf("친구 아이디 : ");
                scanf_s("%s", id, sizeof(id));

                char id_len = (char)strlen(id);

                send(sock, "\x04", 1, 0);
                send(sock, &id_len, 1, 0);
                send(sock, id, strlen(id), 0);

                char result;
                if (recvn(sock, &result, 1) == 0) disconnected();

                if (result == 1) {
                    puts("\n친구 삭제 완료\n");
                    Sleep(1000);
                }
                else {
                    puts("\n친구가 아닙니다\n");
                    Sleep(1000);
                }
            }
            else if (n == 3) {
                char posts[16384] = { 0 };
                int idx = 0;
                send(sock, "\x06", 1, 0);
                while (TRUE) {
                    if (recvn(sock, &posts[idx], 1) == 0) disconnected();
                    if (posts[idx] == '\0') break;
                    idx++;
                }
                clear();
                printf("[글 목록]\n%s\n\n", posts);
                system("pause");
            }
            else if (n == 4) {
                char title[64] = { 0, };
                char body[512] = { 0, };

                getchar();
                
                printf("제목을 입력하세요 : ");
                gets_s(title, 60);

                printf("내용을 입력하세요 : ");
                gets_s(body, 500);

                char title_len = (char)strlen(title);

                send(sock, "\x05", 1, 0);
                send(sock, &title_len, 1, 0);
                send(sock, title, strlen(title), 0);

                send(sock, body, strlen(body) + 1, 0);
                
                char result;
                if (recvn(sock, &result, 1) == 0) disconnected();
                if (result == 0) {
                    puts("\n글 등록에 실패했습니다\n");
                    Sleep(1000);
                }
                else {
                    puts("\n글이 등록되었습니다\n");
                    Sleep(1000);
                }
            }
            else if (n != 5) {
                puts("\n없는 번호입니다!\n");
                Sleep(1000);
            }
        }
    }
    
    closesocket(sock);
    WSACleanup();

    return 0;
}