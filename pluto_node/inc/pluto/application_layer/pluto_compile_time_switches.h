#ifndef __PLUTO_COMPILE_TIME_SWITCHES_H__
#define __PLUTO_COMPILE_TIME_SWITCHES_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

/*
#if !defined(PLUTO_CTS_RTM_PYTHON) && !defined(PLUTO_CTS_RTM_SHARED_LIB)
#define PLUTO_CTS_RTM_PASSTHROUGH 0
#endif


#if !defined(PLUTO_CTS_RTM_PASSTHROUGH) && !defined(PLUTO_CTS_RTM_PYTHON)
#define PLUTO_CTS_RTM_SHARED_LIB 1
#endif

#if !defined(PLUTO_CTS_RTM_PASSTHROUGH) && !defined(PLUTO_CTS_RTM_SHARED_LIB)
#define PLUTO_CTS_RTM_PYTHON 2
#endif
*/

#if !defined(PLUTO_CTS_RTM_PASSTHROUGH) && !defined(PLUTO_CTS_RTM_SHARED_LIB) && !defined(PLUTO_CTS_RTM_PYTHON)
#define PLUTO_CTS_RTM_PASSTHROUGH 0
#endif

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
