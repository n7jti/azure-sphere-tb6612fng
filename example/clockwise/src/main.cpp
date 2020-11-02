

#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <applibs/eventloop.h>
#include <applibs/log.h>
#include <applibs/pwm.h>
#include <applibs/gpio.h>

#include "../../../tb6621fng.h"

// <summary>
/// Exit codes for this application. These are used for the
/// application exit code. They must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum
{
    ExitCode_Success = 0,
    ExitCode_TermHandler_SigTerm = 1,
    ExitCode_Init_EventLoop = 2,
    ExitCode_Main_EventLoopFail = 3,
    ExitCode_PWM_Open = 4, 
    ExitCode_GPIO_Open = 5, 
} ExitCode;


static volatile sig_atomic_t exitCode = ExitCode_Success;

// Timer / polling
static EventLoop *eventLoop = NULL;

static int fd_pwm = -1; 
static int fd_in1 = -1;
static int fd_in2 = -1;



/// <summary>
///     Signal handler for termination requests. This handler must be async-signal-safe.
/// </summary>
static void TerminationHandler(int signalNumber)
{
    // Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
    exitCode = ExitCode_TermHandler_SigTerm;
}

/// <summary>
///     Set up SIGTERM termination handler, initialize peripherals, and set up event handlers.
/// </summary>
/// <returns>
///     ExitCode_Success if all resources were allocated successfully; otherwise another
///     ExitCode value which indicates the specific failure.
/// </returns>
static ExitCode InitPeripheralsAndHandlers(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

    eventLoop = EventLoop_Create();
    if (eventLoop == NULL)
    {
        Log_Debug("Could not create event loop.\n");
        return ExitCode_Init_EventLoop;
    }

    // open PWM
    fd_pwm = PWM_Open(1);
    if (fd_pwm < 0){
        return ExitCode_PWM_Open; 
    }

    // open GPIO
    fd_in1 = GPIO_OpenAsOutput(8, GPIO_OutputMode_PushPull, GPIO_Value_Low);
    if (fd_in1 < 0){
        return ExitCode_GPIO_Open; 
    }

    fd_in2 = GPIO_OpenAsOutput(9, GPIO_OutputMode_PushPull, GPIO_Value_Low);
    if (fd_in2 < 0){
        return ExitCode_GPIO_Open; 
    }


    return ExitCode_Success; 
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
    Log_Debug("Closing file descriptors\n");

    close(fd_pwm);
    fd_pwm = -1;

    close(fd_in1);
    fd_in1 = -1; 

    close(fd_in2); 
    fd_in2 = -1; 

}

/// <summary>
///     Main entry point for this sample.
/// </summary>
int main(int argc, char *argv[])
{

    // setup
    Log_Debug("tb6612fng application starting.\n");

    exitCode = InitPeripheralsAndHandlers();

    // Main loop
    while (exitCode == ExitCode_Success)
    {
        EventLoop_Run_Result result = EventLoop_Run(eventLoop, -1, true);
        // Continue if interrupted by signal, e.g. due to breakpoint being set.
        if (result == EventLoop_Run_Failed && errno != EINTR)
        {
            exitCode = ExitCode_Main_EventLoopFail;
        }
    }

    ClosePeripheralsAndHandlers();

    Log_Debug("tb6612fng application exiting.\n");

    return 0; 
}