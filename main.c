#include "socket.h"
#include "irc.h"

int main(int argc, char **argv) {
    irc_t irc;

    if (irc_connect(&irc, "185.30.166.38", "6667") < 0) { // this is orwell.freenode.net right here !
        fprintf(stderr, "Connection failed.\n");
        goto error;
    }
    if (irc_login(&irc, "maister_") < 0) {
        fprintf(stderr, "Couldn't log in.\n");
        goto error;
    }
    if (irc_join_channel(&irc, "#snake") < 0) {
        fprintf(stderr, "Couldn't join channel.\n");
        goto error;
    }

    while (irc_handle_data(&irc) >= 0);

error:
    close(irc->s);
    fclose(irc->file);
    return 0;
}
