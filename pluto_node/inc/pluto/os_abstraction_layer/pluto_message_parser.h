#ifndef __PLUTO_MESSAGE_PARSER_H__
#define __PLUTO_MESSAGE_PARSER_H__

//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction_layer/pluto_types.h>

#include <stdint.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

///
/// \brief  Parse the given Message and transform it to a Request.
///
bool PLUTO_MessageParserLoadRequest(const char *message, PLUTO_Request_t request);
///
/// \brief  Dump a Response to a Message Buffer.
///
void PLUTO_MessageParserDumpResponse(const PLUTO_Response_t response, char *buffer, int32_t size);

//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
