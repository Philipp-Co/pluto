#ifndef __PLUTO_COMPILE_CONFIG_H__
#define __PLUTO_COMPILE_CONFIG_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_TARGET_OS_FREE_BSD 1
#define PLUTO_TARGET_OS_DEBIAN 2

//
// \brief   This Definition is used to determine the target OS.
//
#define PLUTO_TARGET_OS (PLUTO_TARGET_OS_FREE_BSD)

#if (PLUTO_TARGET_OS_FREE_BSD) == (PLUTO_TARGET_OS)
#define PLUTO_KQUEUE_AVAILABLE 1
#elif (PLUTO_TARGET_OS_DEBIAN) == (PLUTO_TARGET_OS)
#define PLUTO_EPOLL_AVAILABLE 1
#define PLUTO_INOTIFY_AVAILABLE 1
#else
#error "Select a valid target OS!"
#endif

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_OS_INTERFACE_SYSTEM_V 1
#define PLUTO_OS_INTERFACE_POSIX 2

#if (PLUTO_TARGET_OS) == (PLUTO_TARGET_OS_FREE_BSD)
#define PLUTO_OS_INTERFACE (PLUTO_OS_INTERFACE_SYSTEM_V)
#else
#error "PLUTO_OS_INTERFACE not selected!"
#endif

//
// --------------------------------------------------------------------------------------------------------------------
//
#endif
