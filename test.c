/* ignore this. testing purposes */

#include "socket.h"
#include "irc.h"
#include <string.h>
#include <time.h>

int calc(double x, double y, char sign){

    switch (sign){
        case '+':
        return (x+y);
        case '-':
        return (x-y);
        case '/':
        return (x/y);
        case '*':
        return (x*y);
    }
}

int irc_connect(irc_t *irc, const char* server, const char* port) {
    if ((irc->s =  get_socket(server, port)) < 0) {
       return -1;
    }
    irc->bufptr = 0;
    irc_set_output(irc, "/dev/null");
    return 0;
}

int irc_set_output(irc_t *irc, const char* file) {
    irc->file = fopen(file, "w");
    if (irc->file == NULL) {
        return -1;
    }
    return 0;
}

int irc_login(irc_t *irc, const char* nick) {
    return sck_sendf(irc->s, "NICK %s\r\nUSER %s %s 0 :%s\r\n", nick, nick, nick, nick);
}

int irc_join_channel(irc_t *irc, const char* channel) {
    strncpy(irc->channel, channel, 254);
    irc->channel[254] = '\0';
    return sck_sendf(irc->s, "JOIN %s\r\n", channel);
}

int irc_handle_data(irc_t *irc) {
    char tempbuffer[512];
    int rc, i;
    if ((rc = sck_recv(irc->s, tempbuffer, sizeof(tempbuffer) - 2 ) ) <= 0) {
        fprintf(stderr, ":v\n");
        return -1;
    }
    tempbuffer[rc] = '\0';
    for (i = 0; i < rc; ++i) {
        switch (tempbuffer[i]) {
            case '\r':
            case '\n':
                irc->servbuf[irc->bufptr] = '\0';
                irc->bufptr = 0;
                if (irc_parse_action(irc) < 0) {
                   return -1;
                }
                break;
            default:
                irc->servbuf[irc->bufptr] = tempbuffer[i];
                if (irc->bufptr < (sizeof (irc->servbuf) - 1)) {
                    irc->bufptr++;
                }
        }
    }
    return 0;
}

int irc_parse_action(irc_t *irc) {
    char irc_nick[128];
    char irc_msg[512];

    if (strncmp(irc->servbuf, "PING :", 6) == 0) {
        return sck_sendf(irc->s, "PONG :%s\r\n", &irc->servbuf[6]);
    } else if (strncmp(irc->servbuf, "NOTICE AUTH :", 13) == 0 || strncmp(irc->servbuf, "ERROR :", 7) == 0) {
        return 0;
    } else {
        char *ptr;
        int privmsg = 0;
        char irc_nick[128];
        char irc_msg[512];
        *irc_nick = '\0';
        *irc_msg = '\0';
        // checks if we have non-message string
        if (strchr(irc->servbuf, 1) != NULL) {
            return 0;
        }
        if (irc->servbuf[0] == ':') {
            ptr = strtok(irc->servbuf, ".");
            if (ptr == NULL) {
                printf("ptr == NULL\n");
                return 0;
            } else {
                strncpy(irc_nick, &ptr[1], 127);
                irc_nick[127] = '\0';
            }
            while ((ptr = strtok(NULL, " ")) != NULL) {
                if (strcmp(ptr, "PRIVMSG") == 0) {
                    privmsg = 1;
                    break;
                }
            }
            if (privmsg) {
                if ((ptr = strtok(NULL, ":")) != NULL && (ptr = strtok(NULL, "")) != NULL) {
                    strncpy(irc_msg, ptr, 511);
                    irc_msg[511] = '\0';
                }
            }
            if (privmsg == 1 && strlen(irc_nick) > 0 && strlen(irc_msg) > 0) {
                irc_log_message(irc, irc_nick, irc_msg);
                return irc_reply_message(irc, irc_nick, irc_msg);
            }
        }
    }
    return 0;
}

int irc_reply_message(irc_t *irc, char *irc_nick, char *msg) {
    // trigger char
    if (*msg != '.') {
       return 0;
    }
    char *command;
    char *arg;
    // gets command
    command = strtok(&msg[1], " ");
    arg = strtok(NULL, "");
    if (arg != NULL) {
        while ( *arg == ' ') {
            arg++;
        }
    }
    if (command == NULL) {
        return 0;
    }
    if (strcmp(command, "snake") == 0) {
        if (irc_msg(irc->s, irc->channel, "ssSSsSssSssSsssssSSsSSSSsSssSSSSsssSSSSsssSSsssSssssSssssSSssSSsSssssSSsSSssSsssSssSSSS") < 0) {
            return -1;
        }
    }

    if (strcmp(command, "snoke") == 0) {
        if (irc_msg(irc->s, irc->channel, "snoke again? gtfo.") < 0) {
            return -1;
        }
    }

    if (strcmp(command, "snare") == 0){
        if (irc_msg(irc->s, irc->channel, "not snare again ?! NOO! PLZ!") < 0 ){
            return -1;
        }
    }

    if (strcmp(command, "mad") == 0){
        if (irc_msg(irc->s, irc->channel, "We have a MAD SCIENTIST ONBOARD :>") < 0 ){
            return -1;
        }
    }
    
    if (strcmp(command, "holy") == 0){
        if (irc_msg(irc->s, irc->channel, "Holy shit this guy is totally NUTS!!!") < 0 ){
            return -1;
        }
    }

    if (strcmp(command, "calc") == 0){
        double nrx = calc(4,4,'+');
        charr output[sizeof(nrx)];
        memcpy(&output,&nrx,sizeof(nrx));
        if (irc_msg(irc->s, irc->channel, output) < 0 ){
            return -1;
        }
    }

    return 0;
}

int irc_log_message(irc_t *irc, const char* nick, const char* message) {
    char timestring[128];
    time_t curtime;
    time(&curtime);
    strftime(timestring, 127, "%F - %H:%M:%S", localtime(&curtime));
    timestring[127] = '\0';
    fprintf(irc->file, "%s - [%s] <%s> %s\n", irc->channel, timestring, nick, message);
    fflush(irc->file);
}

int irc_msg(int s, const char *channel, const char *data) {
   return sck_sendf(s, "PRIVMSG %s :%s\r\n", channel, data);
}
