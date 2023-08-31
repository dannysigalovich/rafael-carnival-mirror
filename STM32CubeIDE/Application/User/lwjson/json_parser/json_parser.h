/*
 * json_parser.h
 *
 *  Created on: Aug 27, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_LWJSON_JSON_PARSER_JSON_PARSER_H_
#define APPLICATION_USER_LWJSON_JSON_PARSER_JSON_PARSER_H_

#include "missionManager/missionManager.h"
#include "lwjson/lwjson.h"
#include "ICD/ICD.h"

int parse_missions(const char* json_str, Mission* missions, char words[2][MAX_SECRET_SIZE]);

#endif /* APPLICATION_USER_LWJSON_JSON_PARSER_JSON_PARSER_H_ */
