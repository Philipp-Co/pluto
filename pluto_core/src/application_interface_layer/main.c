//
// --------------------------------------------------------------------------------------------------------------------
//
///
/// \brief  Main Function of this Program.
///
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_core/application_layer/pluto_core.h>
#include <pluto/os_abstraction/pluto_logger.h>

#include <signal.h>
#include <stdatomic.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Structure for parsed Program Arguments.
///
typedef struct
{
    char config_path[4096]; // Store the Path to the Configurationfile.
    bool dry_run;           // Flag.
} PLUTO_CORE_Args_t;

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Flag that tells the Main-Loop that the Program is running or not.
///         0 = run
///         1 = terminate
///
static atomic_int PLUTO_terminate = 0;

///
/// \brief  Core Object.
///
static PLUTO_Core_t PLUTO_core = NULL;

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  A Signalhandler.
///         This Handler is used to handle all registered Signals.
///
static void PLUTO_CoreSignalHandler(int signum, siginfo_t *info, void *args);

///
/// \brief  Setup Programkontext.
///         Register Signals and Handlers.
///         Setup Timer.         
///
static void PLUTO_CoreSetUp(void);

///
/// \brief  Print a Helptext to the User.
///         Usees printf to stderr.
///
static void PLUTO_CORE_PrintHelp(void);

///
/// \brief Parse Programarguments.
/// \return true if parsing was succesfull. false otherwise. 
///
static bool PLUTO_CORE_ParseArgs(PLUTO_CORE_Args_t *args, int argc, char **argv);

//
// --------------------------------------------------------------------------------------------------------------------
//

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    PLUTO_CORE_Args_t args = {0};
    if(!PLUTO_CORE_ParseArgs(&args, argc, argv))
    {
        return -1;
    }

    PLUTO_Logger_t logger = PLUTO_CreateLogger("core");

    PLUTO_CoreSetUp();
    PLUTO_core = PLUTO_CreateCore(
        args.config_path,
        logger
    );
    if(!PLUTO_core) return -1;
    
    //
    // Dry-Run...
    //
    if(args.dry_run) return 0;
    
    //
    // Start Program.
    //
    sigset_t sigset = {0};
    sigemptyset(&sigset);
    while(!atomic_load(&PLUTO_terminate))
    {
        if(!PLUTO_CoreProcess(PLUTO_core))
        {
            PLUTO_LoggerInfo(PLUTO_core->logger, "CoreProcess has finished.");
            atomic_store(&PLUTO_terminate, 1);
        }
        sleep(1);
    }

    PLUTO_LoggerInfo(PLUTO_core->logger, "Core: Bye Bye.");
    PLUTO_DestroyCore(&PLUTO_core);
    return 0;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static void PLUTO_CoreSignalHandler(int signum, siginfo_t *info, void *args)
{
    (void)args;
    (void)PLUTO_CoreSignalReceived(PLUTO_core, signum, info->si_pid);
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static void PLUTO_CoreSetUp(void)
{
    struct sigaction action = {
        0
    };

    static const int PLUTO_signums[] = 
    {
        SIGALRM,
        SIGINT,
        SIGCHLD,
    };
    
    //
    // https://man7.org/linux/man-pages/man2/sigaction.2.html
    //
    for(size_t i=0;i<(sizeof(PLUTO_signums)/sizeof(int)); ++i)
    {
        action.sa_flags = SA_SIGINFO;
        action.sa_sigaction = PLUTO_CoreSignalHandler;
        sigaction(
            PLUTO_signums[i],
            &action,
            NULL
        );
    }
    
    struct itimerval timer = {
        .it_interval={
            .tv_sec=1,
            .tv_usec=0
        },
        .it_value={
            .tv_sec=1,
            .tv_usec=0
        }
    };
    setitimer(ITIMER_REAL, &timer, NULL);
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static bool PLUTO_CORE_ParseArgs(PLUTO_CORE_Args_t *args, int argc, char **argv)
{
    args->dry_run = false;
    uint32_t required_arg = 0x1U;
    
    int c;
    while((c=getopt(argc, argv, "dc:")) != -1)
    {
        if(-1 == optopt)
        {
            break;
        }

        switch(c)
        {
            case 'd':
                args->dry_run = true;
                break;
            case 'c':
                memcpy(args->config_path, optarg, strlen(optarg));
                required_arg ^= 0x1U;
                break;
            default:
                printf("Unknown Character Code 0x%x\n", c);
                PLUTO_CORE_PrintHelp();
                return false;
        }
    }
    if(required_arg) PLUTO_CORE_PrintHelp();
    return 0x0U == required_arg;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static void PLUTO_CORE_PrintHelp(void)
{
    fprintf(
        stderr, 
        "pluto_core [-c config] [-d]\n"
        "\n"
        "  -c, Path to a Configurationfile.\n"
        "  -d, Dry-Run. Parse given Configurationfile and exit.\n"
    );
}
//
// --------------------------------------------------------------------------------------------------------------------
//
