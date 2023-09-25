/*
	Note: This source code is written by beginners and can be terrible.
	This program does not handle all exceptions, and there are many security vulnerabilities.
*/

#include "userdata.h"
#include "KISA_SHA256.h"

#include <io.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable:4996)

extern char dataPath[2048];
extern char userDataPath[2048];

int createUser(const char* id, const char* password, const char* username) {
	int idLen = strlen(id);
	int nameLen = strlen(username);
	int pwLen = strlen(password);

	if (idLen == 0 || 20 < idLen)
	    return 0;

	if (nameLen == 0 || 20 < nameLen)
		return 0;

	if (pwLen < 8 || 20 < pwLen)
		return 0;

	for (int i = 0; i < idLen; i++) {
		if (!(('0' <= id[i] && id[i] <= '9') || ('A' <= id[i] && id[i] <= 'Z') || ('a' <= id[i] && id[i] <= 'z') || id[i] == '_'))
			return 0;
	}

	for (int i = 0; i < pwLen; i++) {
		if (!(('0' <= password[i] && password[i] <= '9') || ('A' <= password[i] && password[i] <= 'Z') || ('a' <= password[i] && password[i] <= 'z')))
			return 0;
	}

	char userFilePath[2048] = { 0, };
	sprintf(userFilePath, "%s\\%s", userDataPath, id);

	if (exists(userFilePath))
		return 0;

	FILE* userFp = fopen(userFilePath, "w");

	fprintf(userFp, "%s\n", username);

	char encrpytedPassword[32];
	SHA256_Encrpyt((const BYTE*)password, strlen(password), (BYTE*)encrpytedPassword);

	fwrite(encrpytedPassword, 1, 32, userFp);
	fclose(userFp);

	return 1;
}

USER* getUser(const char* id) {
	char userFilePath[2048] = { 0, };
	sprintf(userFilePath, "%s\\%s", userDataPath, id);

	if (!exists(userFilePath) || isDirExists(userFilePath))
		return 0;

	FILE* userFp = fopen(userFilePath, "r");

	USER* user = (USER*)malloc(sizeof(USER));

	strcpy(user->id, id);

	fgets(user->name, 32, userFp);
	user->name[strlen(user->name) - 1] = 0;

	fread(user->password, 1, 32, userFp);

	char userFriendsFilePath[2048] = { 0, };
	sprintf(userFriendsFilePath, "%s\\%s_friends", userDataPath, id);

	user->friendCount = 0;

	if (exists(userFriendsFilePath) && !isDirExists(userFriendsFilePath)) {
		FILE* userFriendsFp = fopen(userFriendsFilePath, "r");

		while (!feof(userFriendsFp)) {
			char temp[32] = { 0, };
			fgets(temp, 31, userFriendsFp);

			if (temp[0] == '\n')
				break;

			if (temp[strlen(temp) - 1] == '\n')
				temp[strlen(temp) - 1] = '\0';

			if (strlen(temp) == 0)
				break;

			strcpy(user->friends[user->friendCount++], temp);
		}

		fclose(userFriendsFp);
	}

	fclose(userFp);

	return user;
}

int addFriend(const char* id, const char* opponentId) {
	if (strcmp(id, opponentId) == 0)
		return 0;

	USER* user = getUser(id);

	if (user == 0)
		return 0;

	USER* opponent = getUser(opponentId);

	if (opponent == 0)
		return 0;

	for (int i = 0; i < user->friendCount; i++) {
		if (strcmp(user->friends[i], opponentId) == 0)
			return 0;
	}

	for (int i = 0; i < opponent->friendCount; i++) {
		if (strcmp(opponent->friends[i], id) == 0)
			return 0;
	}

	strcpy(user->friends[user->friendCount++], opponentId);
	strcpy(opponent->friends[opponent->friendCount++], id);

	char userFriendsFilePath[2048] = { 0, };
	sprintf(userFriendsFilePath, "%s\\%s_friends", userDataPath, id);

	char opponentFriendsFilePath[2048] = { 0, };
	sprintf(opponentFriendsFilePath, "%s\\%s_friends", userDataPath, opponentId);

	FILE* userFriendsFp = fopen(userFriendsFilePath, "w");
	FILE* opponentFriendsFp = fopen(opponentFriendsFilePath, "w");

	for (int i = 0; i < user->friendCount; i++)
		fprintf(userFriendsFp, "%s\n", user->friends[i]);

	for (int i = 0; i < opponent->friendCount; i++)
		fprintf(opponentFriendsFp, "%s\n", opponent->friends[i]);

	fclose(userFriendsFp);
	fclose(opponentFriendsFp);

	return 1;
}

int removeFriend(const char* id, const char* opponentId) {
	USER* user = getUser(id);

	if (user == 0)
		return 0;

	USER* opponent = getUser(opponentId);

	if (opponent == 0)
		return 0;

	int found = 0;

	for (int i = 0; i < user->friendCount; i++) {
		if (strcmp(user->friends[i], opponentId) == 0) {
			found = 1;
			strcpy(user->friends[i], "");
			break;
		}
	}

	for (int i = 0; i < opponent->friendCount; i++) {
		if (strcmp(opponent->friends[i], id) == 0) {
			found = 1;
			strcpy(opponent->friends[i], "");
			break;
		}
	}

	char userFriendsFilePath[2048] = { 0, };
	sprintf(userFriendsFilePath, "%s\\%s_friends", userDataPath, id);

	char opponentFriendsFilePath[2048] = { 0, };
	sprintf(opponentFriendsFilePath, "%s\\%s_friends", userDataPath, opponentId);

	FILE* userFriendsFp = fopen(userFriendsFilePath, "w");
	FILE* opponentFriendsFp = fopen(opponentFriendsFilePath, "w");

	for (int i = 0; i < user->friendCount; i++) {
		if (user->friends[i][0] != '\0') {
			fprintf(userFriendsFp, "%s\n", user->friends[i]);
		}
	}

	for (int i = 0; i < opponent->friendCount; i++) {
		if (opponent->friends[i][0] != '\0') {
			fprintf(opponentFriendsFp, "%s\n", opponent->friends[i]);
		}
	}

	fclose(userFriendsFp);
	fclose(opponentFriendsFp);

	return found;
}

int passwordMatch(char* id, char* password) {
	USER* user = getUser(id);

	if (user == 0)
		return 0;

	char encrpytedPassword[32];
	SHA256_Encrpyt((const BYTE*)password, strlen(password), (BYTE*)encrpytedPassword);

	int match = 1;
	for (int i = 0; i < 32; i++) {
		if (encrpytedPassword[i] != user->password[i]) {
			match = 0;
			break;
		}
	}

	return match;
}