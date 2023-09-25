#pragma once
#include "file.h"

typedef struct USER {
	// ���̵�
	char id[32];

	// �̸�
	char name[32];

	// SHA256���� ��ȣȭ�� ��й�ȣ
	char password[32];

	// ģ�� ���(id)
	char friends[256][32];

	int friendCount;
} USER;

int createUser(const char* id, const char* password, const char* username);
USER* getUser(const char* id);

int addFriend(const char* id, const char* opponentId);
int removeFriend(const char* id, const char* opponentId);
int passwordMatch(char* id, char* password);
//create user, exists user, get user