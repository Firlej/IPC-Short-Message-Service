#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LOGIN 1
#define LOGOUT 2
#define SEND_MSG_TO_USER 3
#define JOIN_GROUP 4
#define LEAVE_GROUP 5
#define SEND_MSG_TO_GROUP 6

#define LIST_USERS 11
#define LIST_GROUPS 12

#define FOR(n) for(int i=0;i<n;i++)

struct msg_a {
    long type;
    int pid;
    char name[256];
    char s[256];
};

struct msg_b {
    long pid;
    bool success;
    char s[1024];
};

size_t a_size = sizeof(struct msg_a) - sizeof(long);
size_t b_size = sizeof(struct msg_b) - sizeof(long);

int mid;

void clean_stdin() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

struct user {
    int pid;
    char login[256];
    bool online;
};

struct group {
    char name[256];
    char n;
    char users[256][256];
};