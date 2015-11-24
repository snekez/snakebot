#ifndef __IRC_H
#define __IRC_H

#include <stdio.h>

typedef struct
{
   int s;
   FILE *file;
   char channel[256];
   char *nick;
   char servbuf[512];
   int bufptr;
} irc_t;

int irc_connect(irc_t *irc, const char* server, const char* port);
int irc_login(irc_t *irc, const char* nick);
int irc_join_channel(irc_t *irc, const char* channel);
int irc_handle_data(irc_t *irc);
int irc_set_output(irc_t *irc, const char* file);
int irc_parse_action(irc_t *irc);
int irc_log_message(irc_t *irc, const char *nick, const char* msg);
int irc_reply_message(irc_t *irc, char *nick, char* msg);
int irc_msg(int s, const char *channel, const char *data);

#endif
