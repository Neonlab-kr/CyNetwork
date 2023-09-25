/*
	Note: This source code is written by beginners and can be terrible.
	This program does not handle all exceptions, and there are many security vulnerabilities.
*/

#include "postdata.h"
#include "userdata.h"

#include <io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#pragma warning(disable:4996)

extern char dataPath[2048];
extern char postDataPath[2048];

long long createPost(const char* authorId, const char* title, const char* body) {
	long long postId = time(NULL);

	if (getUser(authorId) == 0)
		return 0;

	int titleLen = strlen(title);
	if (titleLen == 0 || 60 < titleLen)
		return 0;

	int bodyLen = strlen(body);
	if (bodyLen == 0 || 10000 < bodyLen)
		return 0;

	char postPath[2048] = { 0 };
	sprintf(postPath, "%s//%lld", postDataPath, postId);

	if (exists(postPath))
		return 0;

	FILE* postFp = fopen(postPath, "w");

	fprintf(postFp, "%s\n%s\n%s", authorId, title, body);

	fclose(postFp);

	return postId;
}

POST* getPost(long long id) {
	char postPath[2048] = { 0 };
	sprintf(postPath, "%s//%lld", postDataPath, id);

	if (!exists(postPath))
		return 0;

	FILE* postFp = fopen(postPath, "r");

	POST* post = (POST*)malloc(sizeof(POST));
    post->id = id;

	fgets(post->authorId, 32, postFp);
	post->authorId[strlen(post->authorId) - 1] = '\0';

	fgets(post->title, 64, postFp);
	post->title[strlen(post->title) - 1] = '\0';

	post->body = (char*)malloc(sizeof(char) * 10001);
	memset(post->body, 0, 10001);

	int bodyIdx = 0;
	while (!feof(postFp)) {
		fread(&post->body[bodyIdx++], 1, 1, postFp);
	}

	fclose(postFp);

	return post;
}

int removePost(long long id) {
	char postPath[2048] = { 0 };
	sprintf(postPath, "%s//%lld", postDataPath, id);

	if (!exists(postPath))
		return 0;

	remove(postPath);

	return 1;
}