#pragma once
#include "file.h"

typedef struct USER {
	// 아이디
	char id[32];

	// 이름
	char name[32];

	// SHA256으로 암호화된 비밀번호
	char password[32];

	// 친구 목록(id)
	char friends[256][32];

	int friendCount;
} USER;

int createUser(const char* id, const char* password, const char* username);
USER* getUser(const char* id);

int addFriend(const char* id, const char* opponentId);
int removeFriend(const char* id, const char* opponentId);
int passwordMatch(char* id, char* password);
//create user, exists user, get user