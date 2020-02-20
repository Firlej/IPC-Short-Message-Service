#include "functions.h"

struct user USERS[256];
int users_count = 0;

struct group GROUPS[256];
int groups_count = 0;

struct msg_a in;
struct msg_b out;

void init();

char *get_login_by_pid(int pid) {
    for (int i = 0; i < users_count; i++) {
        if (pid == USERS[i].pid) {
            return USERS[i].login;
        }
    }
}

int get_pid_by_user(char *user) {
    for (int i = 0; i < users_count; i++) {
        if (strcmp(user, USERS[i].login) == 0) {
            return USERS[i].pid;
        }
    }
}

int main() {
    printf("\e[1;1H\e[2J");

    mid = msgget(1235, 0600 | IPC_CREAT);
    printf("Server running at %d\n", mid);

    init();

    while (true) {
        msgrcv(mid, &in, sizeof(in), 0, 0);

        bool found_user;
        bool found_group;
        char s[1024];

        switch (in.type) {
            case LOGIN: {
                found_user = false;
                for (int i = 0; i < users_count; i++) {
                    if (strcmp(in.s, USERS[i].login) == 0) {
                        if (!USERS[i].online) {
                            out.success = 1;
                            USERS[i].pid = in.pid;
                            USERS[i].online = true;
                            strcpy(out.s, "Login succesful.");
                            printf("USER %s LOGGED IN\n", USERS[i].login);
                        } else {
                            out.success = 0;
                            strcpy(out.s, "Already logged in somewhere else.");
                            printf("USER %s TRIED LOGGING IN A SECOND LOCATION\n", USERS[i].login);
                        }
                        found_user = true;
                        break;
                    }
                }
                if (!found_user) {
                    out.success = 0;
                    strcpy(out.s, "User not found.");
                    printf("USER %s TRIED LOGGING IN (doesnt exist)\n", in.s);
                }
                out.pid = in.pid;
                msgsnd(mid, &out, b_size, 0);
                break;
            }
            case LOGOUT: {
                for (int i = 0; i < users_count; i++) {
                    if (in.pid == USERS[i].pid) {
                        if (USERS[i].online) {
                            out.success = 1;
                            USERS[i].pid = -1;
                            USERS[i].online = false;
                            strcpy(out.s, "Succesfully logged out.");
                            printf("USER %s LOGGED OUT\n", USERS[i].login);
                        } else {
                            // not reachable
                            strcpy(out.s, "You are not logged in.");
                            printf("USER %s TRIED LOGGING OUT WHILE NOT OGGED IN\n", USERS[i].login);
                        }
                        break;
                    }
                }
                out.pid = in.pid;
                msgsnd(mid, &out, b_size, 0);
                break;
            }
            case LIST_USERS: {
                strcpy(s, "Users online: ");
                char s[] = "Users online: ";
                for (int i = 0; i < users_count; i++) {
                    if (USERS[i].online) {
                        strcat(s, USERS[i].login);
                        strcat(s, " ");
                    }
                }
                out.pid = in.pid;
                strcpy(out.s, s);
                printf("USER %s LISTS ONLINE USERS %s\n", get_login_by_pid(in.pid), s);
                msgsnd(mid, &out, b_size, 0);
                break;
            }
            case SEND_MSG_TO_USER: {
                found_user = false;
                for (int i = 0; i < users_count; i++) {
                    if (strcmp(in.name, USERS[i].login) == 0) {
                        if (USERS[i].online) {
                            out.pid = USERS[i].pid;
                            printf("USER %s SENDS MSG TO USER %s\n", get_login_by_pid(in.pid), USERS[i].login);
                            found_user = true;
                        }
                        break;
                    }
                }
                if (found_user) {
                    sprintf(out.s, "User %s sent you a message: %s", get_login_by_pid(in.pid), in.s);
                } else {
                    out.pid = in.pid;
                    strcpy(out.s, "User not found, or is offline.");
                    printf("USER %s SENDS MSG TO INVALID USER %s\n", get_login_by_pid(in.pid), in.name);
                }
                msgsnd(mid, &out, b_size, 0);
                break;
            }
            case LIST_GROUPS: {
                strcpy(s, "Available groups and users in these groups:");
                for (int i = 0; i < groups_count; i++) {
                    strcat(s, "\n");
                    strcat(s, GROUPS[i].name);
                    strcat(s, ": ");
                    for (int j = 0; j < GROUPS[i].n; ++j) {
                        strcat(s, GROUPS[i].users[j]);
                        strcat(s, " ");
                    }
                }
                out.pid = in.pid;
                strcpy(out.s, s);
                printf("USER %s LISTS GROUPS\n", get_login_by_pid(in.pid));
                msgsnd(mid, &out, b_size, 0);
                break;
            }
            case JOIN_GROUP: {
                found_group = false;
                char user[256];
                strcpy(user, get_login_by_pid(in.pid));
                for (int i = 0; i < groups_count; i++) {
                    if (strcmp(in.name, GROUPS[i].name) == 0) {
                        bool user_in_group = false;
                        for (int j = 0; j < GROUPS[i].n; ++j) {
                            if (strcmp(user, GROUPS[i].users[j]) == 0) {
                                user_in_group = true;
                                break;
                            }
                        }
                        if (user_in_group) {
                            strcpy(out.s, "You are already in this group.");
                            printf("USER %s TRIES JOINING A GROUP HES ALREADY IN %s\n", user, GROUPS[i].name);
                        } else {
                            strcpy(GROUPS[i].users[GROUPS[i].n], user);
                            GROUPS[i].n += 1;
                            strcpy(out.s, "You have succesfully joined the group.");
                            printf("USER %s JOINS GROUP %s\n", user, GROUPS[i].name);
                        }
                        found_group = true;
                        break;
                    }
                }
                if (!found_group) {
                    strcpy(out.s, "Group not found.");
                    printf("USER %s TRIES JOINING INVALID GROUP %s\n", user, in.name);
                }
                out.pid = in.pid;
                msgsnd(mid, &out, b_size, 0);
                break;
            }
            case LEAVE_GROUP: {
                found_group = false;
                char user[256];
                strcpy(user, get_login_by_pid(in.pid));
                for (int i = 0; i < groups_count; i++) {
                    if (strcmp(in.name, GROUPS[i].name) == 0) {
                        bool user_in_group = false;
                        int position = -1;
                        for (int j = 0; j < GROUPS[i].n; ++j) {
                            if (strcmp(user, GROUPS[i].users[j]) == 0) {
                                position = j;
                                user_in_group = true;
                                break;
                            }
                        }
                        if (user_in_group) {
                            for (int j = position; j < GROUPS[i].n - 1; ++j) {
                                strcpy(GROUPS[i].users[j], GROUPS[i].users[j + 1]);
                            }
                            GROUPS[i].n -= 1;
                            strcpy(out.s, "You have succefully left this group.");
                            printf("USER %s LEFT GROUP %s\n", user, GROUPS[i].name);
                        } else {
                            strcpy(out.s, "You cant leave a group you are not in.");
                            printf("USER %s TRIES LEAVING A GROUP HE IS NOT IN %s\n", user, GROUPS[i].name);
                        }
                        found_group = true;
                        break;
                    }
                }
                if (!found_group) {
                    strcpy(out.s, "Group not found.");
                    printf("USER %s TRIES LEAVING INVALID GROUP %s\n", user, in.name);
                }
                out.pid = in.pid;
                msgsnd(mid, &out, b_size, 0);
                break;
            }
            case SEND_MSG_TO_GROUP: {
                found_group = false;
                char user[256];
                strcpy(user, get_login_by_pid(in.pid));
                for (int i = 0; i < groups_count; i++) {
                    if (strcmp(in.name, GROUPS[i].name) == 0) {
                        bool user_in_group = false;
                        for (int j = 0; j < GROUPS[i].n; ++j) {
                            if (strcmp(user, GROUPS[i].users[j]) == 0) {
                                user_in_group = true;
                                break;
                            }
                        }
                        if (user_in_group) {
                            sprintf(out.s, "[%s] %s: %s", GROUPS[i].name, user, in.s);
                            for (int j = 0; j < GROUPS[i].n; ++j) {
                                out.pid = get_pid_by_user(GROUPS[i].users[j]);
                                msgsnd(mid, &out, b_size, 0);
                            }
                            printf("USER %s SENT A MEESAGE TO GROUP %s: %s\n", user, GROUPS[i].name, out.s);
                        } else {
                            strcpy(out.s, "You cant send a message to a group you are not in.");
                            printf("USER %s TRIES SENDING A MESSAGE TO A GROUP HE IS NOT IN %s\n", user, GROUPS[i].name);
                            out.pid = in.pid;
                            msgsnd(mid, &out, b_size, 0);
                        }
                        found_group = true;
                        break;
                    }
                }
                if (!found_group) {
                    strcpy(out.s, "Group not found.");
                    printf("USER %s TRIES SENDING A MESSAGE TO AN INVALID GROUP %s\n", user, in.name);
                    out.pid = in.pid;
                    msgsnd(mid, &out, b_size, 0);
                }
                break;
            }
        }
    }

    return 0;
}

void init() {

    FILE *fp = fopen("config.txt", "r");
    if (fp == NULL) {
        printf("No config file!\n");
        exit(1);
    }

    char *s = NULL;
    size_t len = 0;
    ssize_t read;

    bool reading_groups = false;
    while ((read = getline(&s, &len, fp)) != -1) {
        if (strlen(s) == 1 && s[0] == '\n') {
            reading_groups = true;
        } else if (!reading_groups) {
            if (s[strlen(s) - 1] == '\n')
                s[strlen(s) - 1] = 0;
            strcpy(USERS[users_count].login, s);
            USERS[users_count].online = false;
            users_count += 1;
        } else {
            if (s[strlen(s) - 1] == '\n')
                s[strlen(s) - 1] = 0;
            strcpy(GROUPS[groups_count].name, s);
            GROUPS[groups_count].n = 0;
            groups_count += 1;
        }
    }
}