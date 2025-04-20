
#include <pluto_rw/prw_message_queue.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


struct PLUTO_RW_CmdArgs
{
    char *name;
    char *data;
    bool write;
};

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
    PLUTO_RW_MessageQueue_t queue = PLUTO_RW_CreateMessageQueue(args.name);
    if(!queue)
    {
        fprintf(stderr, "Error, unable to open Messagae Queue.\n");
        return -1;
    }
    struct PLUTO_RW_Message msg; 
    if(args.write)
    {
        msg.buffer.msgtype = 1;
        memcpy(msg.buffer.text, args.data, strlen(args.data));
        msg.size = strlen(args.data);
        return PLUTO_RW_MessageQueueWrite(queue, &msg) ? 0 : -1;
    }
    else
    {
        if(PLUTO_RW_MessageQueueRead(queue, &msg))
        {
            fprintf(stderr, "%s\n", msg.buffer.text);
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
