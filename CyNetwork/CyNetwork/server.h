#pragma once
#include <winsock2.h>
#include <windows.h>

void server();
DWORD WINAPI clientThreadFunc(void* data);