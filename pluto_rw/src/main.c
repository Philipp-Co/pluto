
#include "pluto/pluto_event/pluto_event.h"
#include "pluto_rw/prw_message_queue.h"
#include <pluto/pluto_edge/pluto_edge.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>


struct PLUTO_RW_CmdArgs
{
    char *name;
    char *data;
    bool write;
};

struct PLUTO_RW_MessageQueueIdentifier
{
    char *name;
    char *path;
};

static struct PLUTO_RW_MessageQueueIdentifier PLUTO_RW_FromName(const char *name);
static void PLUTO_RW_PrintHelp(void);
#ifdef PLUTO_PRINT_ARGS
static void PLUTO_RW_PrintArgs(struct PLUTO_RW_CmdArgs *args);
#endif
static bool PLUTO_RW_ParseArgs(int argc, char **argv, struct PLUTO_RW_CmdArgs *args);

int main(int argc, char **argv)
{
    struct PLUTO_RW_CmdArgs args;
    if(!PLUTO_RW_ParseArgs(argc, argv, &args))
    {
        return -1;
    }
#ifdef PLUTO_PRINT_ARGS
    PLUTO_RW_PrintArgs(&args);
#endif
    
    struct PLUTO_RW_MessageQueueIdentifier mqid = PLUTO_RW_FromName(args.name);
    unsigned int permission = 0777;
    PLUTO_Logger_t logger = PLUTO_CreateLogger("rw");
    PLUTO_EDGE_Edge_t edge = PLUTO_EDGE_CreateEdge(mqid.path, mqid.name, permission, logger);
    if(!edge)
    {
        fprintf(stderr, "Error, unable to open Messagae Queue.\n");
        return -1;
    }
    PLUTO_Event_t event = PLUTO_CreateEvent();
    if(args.write)
    {
        snprintf(
            PLUTO_EventPayload(event),
            PLUTO_EventSizeOfPayloadBuffer(event),
            "%s",
            args.data 
        );
        PLUTO_EventSetSizeOfPayload(event, strlen(args.data));
        return PLUTO_EDGE_EdgeSendEvent(edge, event) ? 0 : -1;
    }
    else
    {
        if(PLUTO_EDGE_EdgeReceiveEvent(edge, event))
        {
            char buffer[1024];
            memset(buffer, '\0', sizeof(buffer));
            memcpy(buffer, PLUTO_EventPayload(event), PLUTO_EventSizeOfPayload(event));
            fprintf(stderr, "%s\n", buffer);
            return 0;
        }
        return -1;
    }
}

static bool PLUTO_RW_ParseArgs(int argc, char **argv, struct PLUTO_RW_CmdArgs *args)
{
    bool rw = false;
    char c;
    opterr = 0;
    optopt = 0;
    while((c = getopt(argc, argv, "n:rwd:")) != -1)
    {
        //
        // TODO: Why is this neccessary on Ubuntu!?
        //       On Macos this is not needed -.-'
        //
        if(((char)-1) == (char)c)
        {
            break;
        }
        //
 
        switch(c)
        {
            case 'n':
                args->name = optarg; 
                break;
            case 'w':
                if(rw)
                {
                    printf("A mode was already specified!\n");
                    PLUTO_RW_PrintHelp();
                    return false;
                }
                args->write = true;
                rw = true;
                break;
            case 'r':
                if(rw)
                {
                    printf("A mode was already specified!\n");
                    PLUTO_RW_PrintHelp();
                    return false;
                }
                args->write = false;
                rw = true;
                break;
            case 'd':
                printf("%s given to program.\n", optarg);
                args->data = optarg;
                break;
            case '?':
                if(optopt == 'n' || optopt == 'd')
                {
                    printf("n / d failed...\n");
                    PLUTO_RW_PrintHelp();
                }
                else if(isprint(optopt))
                {
                    printf("Unknown Argument! %i -> %c\n", optopt, c);
                    PLUTO_RW_PrintHelp();
                }
                else 
                {
                    printf("Unknown Argument! 0x%i\n", (int)optopt);
                    PLUTO_RW_PrintHelp();
                }
                return false;
            default:
                printf("Error, unexpected Opt Char 0x%i, %s.\n", c, optarg);
                PLUTO_RW_PrintHelp();
                return false;
        }
    }
    if(!args->data && args->write)
    {
        printf("No data given to Write!\n");
        PLUTO_RW_PrintHelp();
        return false;
    }
    if(!args->name)
    {
        printf("No Name given!\n");
        PLUTO_RW_PrintHelp();
        return false;
    }
    return true;
}

#ifdef PLUTO_PRINT_ARGS
static void PLUTO_RW_PrintArgs(struct PLUTO_RW_CmdArgs *args)
{
    fprintf(
        stderr,
        "===================================\n"
        " Args:\n"
        "   Name: %s\n"
        "   Mode: %s\n"
        "   Data: %s\n"
        "===================================\n",
        args->name,
        args->write ? "Write" : "Read",
        args->data
    );
}
#endif

static void PLUTO_RW_PrintHelp(void)
{
    fprintf(
        stderr,
        "pluto_rw [-n name] [[-r] [[-w] -d [data]]]\n"
        "\n"
        "  -n, name of message queue.\n"
        "  -r, read message from queue.\n"
        "  -w, write message queue.\n"
        "  -d, data to write into the message queue. Need -w as well.\n"
    );
}

static struct PLUTO_RW_MessageQueueIdentifier PLUTO_RW_FromName(const char *name)
{
    const int len = strlen(name);
    struct PLUTO_RW_MessageQueueIdentifier identifier = {
        .name = PLUTO_Malloc(len + 1),
        .path = PLUTO_Malloc(len + 1)
    };
    memset(identifier.name, '\0', len + 1);
    memset(identifier.path, '\0', len + 1);

    for(int i=len;i > 0; --i)
    {
        if('/' == name[i])
        {
            memcpy(identifier.name, name + i + 1, len - i);
            memcpy(identifier.path, name, i + 1);
            break;
        }
    }

    return identifier;
}
