#ifndef __PLUTO_MESSAGE_PARSER_H__
#define __PLUTO_MESSAGE_PARSER_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/pluto_types.h>

#include <stdint.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

bool PLUTO_MessageParserLoadRequest(const char *message, PLUTO_Request_t request);
void PLUTO_MessageParserDumpResponse(const PLUTO_Response_t response, char *buffer, int32_t size);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
