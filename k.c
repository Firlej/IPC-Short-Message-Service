#include "functions.h"

struct msg_a out;
struct msg_b in;

char login[256];
int pid;

bool logged_in = false;

int main() {
    printf("\e[1;1H\e[2J");

    mid = msgget(1235, 0600 | IPC_CREAT);
    printf("Succesfully conntected to server at %d\n", mid);

    pid = getpid();
    printf("Your pid: %d\n", pid);

    while (!logged_in) {
        printf("Login: ");
        scanf("%s", login);
        strcpy(out.s, login);

        out.type = LOGIN;
        out.pid = getpid();

        msgsnd(mid, &out, a_size, 0);
        msgrcv(mid, &in, b_size, pid, 0);
        if (in.success) {
            logged_in = true;
        }
        printf("%s\n", in.s);
    }

    if (fork() == 0) {
        while (true) {
            msgrcv(mid, &in, b_size, pid, 0);
            printf("%s\n", in.s);
        }
    } else {

        int n = -1;
        while (logged_in) {
            clean_stdin();
            printf("|| What would you like to do?\n");
            printf("|| [2] LOGOUT\n");
            printf("|| [3] SEND_MSG_TO_USER\n");
            printf("|| [4] JOIN_GROUP\n");
            printf("|| [5] LEAVE_GROUP\n");
            printf("|| [6] SEND_MSG_TO_GROUP\n");

            scanf("%d", &n);
            printf("Accepted [%d] as input.\n", n);

            out.pid = pid;

            switch (n) {
                case LOGOUT: {
                    out.type = LOGOUT;
                    logged_in = false;
                    msgsnd(mid, &out, a_size, 0);
                    break;
                }
                case SEND_MSG_TO_USER: {
                    out.type = LIST_USERS;
                    msgsnd(mid, &out, a_size, 0);
                    usleep(5000);

                    out.type = SEND_MSG_TO_USER;

                    printf("Choose user: ");
                    char user[256];
                    scanf("%s", user);
                    strcpy(out.name, user);

                    printf("Message: ");
                    char message[256];
                    scanf("%s", message);
                    strcpy(out.s, message);

                    msgsnd(mid, &out, a_size, 0);
                    break;
                }
                case JOIN_GROUP: {
                    out.type = LIST_GROUPS;
                    msgsnd(mid, &out, a_size, 0);
                    usleep(5000);

                    out.type = JOIN_GROUP;

                    printf("Choose group: ");
                    char group[256];
                    scanf("%s", group);
                    strcpy(out.name, group);

                    msgsnd(mid, &out, a_size, 0);
                    break;
                }
                case LEAVE_GROUP: {
                    out.type = LIST_GROUPS;
                    msgsnd(mid, &out, a_size, 0);
                    usleep(5000);

                    out.type = LEAVE_GROUP;

                    printf("Choose group: ");
                    char group[256];
                    scanf("%s", group);
                    strcpy(out.name, group);

                    msgsnd(mid, &out, a_size, 0);
                    break;
                }
                case SEND_MSG_TO_GROUP: {
                    out.type = LIST_GROUPS;
                    msgsnd(mid, &out, a_size, 0);
                    usleep(5000);

                    out.type = SEND_MSG_TO_GROUP;

                    printf("Choose group: ");
                    char group[256];
                    scanf("%s", group);
                    strcpy(out.name, group);

                    printf("Message: ");
                    char message[256];
                    scanf("%s", message);
                    strcpy(out.s, message);

                    msgsnd(mid, &out, a_size, 0);
                    break;
                }
            }
            usleep(5000);
        }
    }

    return 0;
}