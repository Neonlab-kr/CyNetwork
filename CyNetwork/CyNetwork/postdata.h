#pragma once

typedef struct POST {
	long long id;
	char authorId[32];

	char title[64];
	char* body;
} POST;

long long createPost(const char* authorId, const char* title, const char* body);
int removePost(long long id);
POST* getPost(long long id);

//create user, exists user, get user