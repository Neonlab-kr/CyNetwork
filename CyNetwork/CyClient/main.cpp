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
    puts("������ ������ ������ϴ�");
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
        puts("���� ���ῡ �����߽��ϴ�..!");
        system("pause");
        exit(0);
    }

    puts("������ ����Ǿ����ϴ�!");

    int mode = 0;
    int n;
    while (TRUE) {
        if (mode == 0) {
            puts("1.����\n2.�α���");
            printf("��ȣ�� �Է��ϼ��� : ");
            scanf("%d", &n);

            if (n == 1) {
                char id[32] = { 0, };
                char pw[32] = { 0, };
                char name[32] = { 0, };
                printf("���̵�(����/����/����� 1~20) : ");
                scanf_s("%s", id, sizeof(id));
                printf("��й�ȣ(����/���� 8~20) : ");
                scanf_s("%s", pw, sizeof(pw));
                printf("�̸�(1~20) : ");
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
                    puts("\n���� �Ǿ����ϴ�\n");
                }
                else {
                    puts("\n���̵�/��й�ȣ/�̸��� �ùٸ��� �ʰų�, ���̵� �̹� �����մϴ�\n");
                }
            }
            else if (n == 2) {
                char id[32] = { 0, };
                char pw[32] = { 0, };
                printf("���̵� : ");
                scanf_s("%s", id, sizeof(id));
                printf("��й�ȣ : ");
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
                    puts("\n���̵� �Ǵ� ��й�ȣ�� Ȯ�����ּ���\n");
                }
            }
            else {
                puts("\n���� ��ȣ�Դϴ�\n");
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

            printf("[ ==== CY NETWORK ==== ]\n�� �̸� : %s\nģ�� �� : %d\n\n[ģ�� ���]\n%s\n\n", name, friendCount, strlen(friendList) <= 1 ? "ģ�� ����" : friendList);

            puts("1.ģ�� �߰�\n2.ģ�� ����\n3.�� ���\n4.�� ���\n5.���� ��ħ");
            printf("��ȣ�� �Է��ϼ��� : ");
            scanf("%d", &n);

            if (n == 1) {
                char id[32] = { 0, };
                printf("ģ�� ���̵� : ");
                scanf_s("%s", id, sizeof(id));

                char id_len = (char)strlen(id);

                send(sock, "\x03", 1, 0);
                send(sock, &id_len, 1, 0);
                send(sock, id, strlen(id), 0);

                char result;
                if (recvn(sock, &result, 1) == 0) disconnected();

                if (result == 1) {
                    puts("\nģ�� �߰� �Ϸ�\n");
                    Sleep(1000);
                }
                else {
                    puts("\n�̹� ģ���̰ų�, �������� �ʴ� ���̵��Դϴ�\n");
                    Sleep(1400);
                }
            }
            else if (n == 2) {
                char id[32] = { 0, };
                printf("ģ�� ���̵� : ");
                scanf_s("%s", id, sizeof(id));

                char id_len = (char)strlen(id);

                send(sock, "\x04", 1, 0);
                send(sock, &id_len, 1, 0);
                send(sock, id, strlen(id), 0);

                char result;
                if (recvn(sock, &result, 1) == 0) disconnected();

                if (result == 1) {
                    puts("\nģ�� ���� �Ϸ�\n");
                    Sleep(1000);
                }
                else {
                    puts("\nģ���� �ƴմϴ�\n");
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
                printf("[�� ���]\n%s\n\n", posts);
                system("pause");
            }
            else if (n == 4) {
                char title[64] = { 0, };
                char body[512] = { 0, };

                getchar();
                
                printf("������ �Է��ϼ��� : ");
                gets_s(title, 60);

                printf("������ �Է��ϼ��� : ");
                gets_s(body, 500);

                char title_len = (char)strlen(title);

                send(sock, "\x05", 1, 0);
                send(sock, &title_len, 1, 0);
                send(sock, title, strlen(title), 0);

                send(sock, body, strlen(body) + 1, 0);
                
                char result;
                if (recvn(sock, &result, 1) == 0) disconnected();
                if (result == 0) {
                    puts("\n�� ��Ͽ� �����߽��ϴ�\n");
                    Sleep(1000);
                }
                else {
                    puts("\n���� ��ϵǾ����ϴ�\n");
                    Sleep(1000);
                }
            }
            else if (n != 5) {
                puts("\n���� ��ȣ�Դϴ�!\n");
                Sleep(1000);
            }
        }
    }
    
    closesocket(sock);
    WSACleanup();

    return 0;
}