/* Name: Sri Vennala Kandibedala */
/* FSUID: SK22BV */
#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <strings.h>

#define MAXCONN 100

char *gethostname(char *h, int n);

struct ClientInfo {
    struct sockaddr_in addr;
    int sockfd;
};

int main(int argc, char *argv[]) {
    int sockfd, rec_sock, len, i, j;
    struct sockaddr_in addr, cli_addr;
    struct ClientInfo clients[MAXCONN];
    char buf[100], hostname[256]; // Buffer to store the hostname
    fd_set allset, rset;
    int maxfd;

    len = sizeof(cli_addr);

    if (argc < 1) {
        printf("Usage: a.out port.\n");
        exit(0);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(": Can't get socket");
        exit(1);
    }

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    // addr.sin_port = htons((short)atoi(5412));

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror(": bind");
        exit(1);
    }
gethostname(hostname, sizeof(hostname));
    // Get the hostname and store it in the 'hostname' buffer
    // if (gethostname(hostname, sizeof(hostname)) == 0) {
    //     printf("Hostname: %s\n", hostname);
    // } else {
    //     perror("gethostname");
    //     return 1; // Error occurred
    // }

    socklen_t addr_len = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr *)&addr, &addr_len) != 0) {
        perror("getsockname");
        exit(1);
    }

    // printf("admin: started server on '%s' at '%d'\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    printf("admin: started server on '%s' at '%d'\n", hostname, ntohs(addr.sin_port));

    if (listen(sockfd, 5) < 0) {
        perror(": bind");
        exit(1);
    }

    for (i = 0; i < MAXCONN; i++) {
        clients[i].sockfd = -1;
        // clients[i].addr = cli_addr;
    }

    FD_ZERO(&allset);
    FD_SET(sockfd, &allset);
    maxfd = sockfd;
    // i = 0;
    while (1) {
        rset = allset;
        select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &rset)) {
            if ((rec_sock = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &len)) < 0) {
                if (errno == EINTR)
                    continue;
                else {
                    perror(":accept error");
                    exit(1);
                }
            }
            
            for (i = 0; i < MAXCONN; i++) {
                if (clients[i].sockfd < 0) {
                    printf("admin: connect from '%s' at '%d'\n",
					inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
                    clients[i].sockfd = rec_sock;
                    clients[i].addr = cli_addr;
                    FD_SET(rec_sock, &allset);
                    if (rec_sock > maxfd) maxfd = rec_sock;
                    break;
                }
            }

            if (i == MAXCONN) {
                printf("too many connections.\n");
                close(rec_sock);
            } 
        }

        for (i = 0; i < MAXCONN; i++) {
            if (clients[i].sockfd < 0) continue;
            if (FD_ISSET(clients[i].sockfd, &rset)) {
                int num;
                memset(buf, 0, sizeof(buf));
                num = read(clients[i].sockfd, buf, 100);
                if (num == 0) {
                    printf("admin: disconnect from '%s(%d)'\n",inet_ntoa(clients[i].addr.sin_addr), ntohs(clients[i].addr.sin_port));
                    close(clients[i].sockfd);
                    FD_CLR(clients[i].sockfd, &allset);
                    clients[i].sockfd = -1;
                } else {
                    for (j = 0; j < MAXCONN; j++) {
                        if (j != i && clients[j].sockfd >= 0) {
                            write(clients[j].sockfd, buf, num);
                        }
                    }
                    printf("%s(%d): %s", inet_ntoa(clients[i].addr.sin_addr), ntohs(clients[i].addr.sin_port),buf);
                }
            }
        }
    }
    return 0;
}
