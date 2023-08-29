/*
 * json_parser.c
 *
 *  Created on: Aug 27, 2023
 *      Author: eladsez
 */

#include "json_parser.h"

int parse_missions(const char* json_str, Mission* missions, char words[2][MAX_SECRET_SIZE]) {
	lwjson_token_t tokens[128];
	lwjson_t lwjson;

	lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));

	if (lwjson_parse(&lwjson, json_str) == lwjsonOK) {
		const lwjson_token_t* mission_array = lwjson_find(&lwjson, "missions");
		const lwjson_token_t* secrets_object = lwjson_find(&lwjson, "secrets");

		if (mission_array != NULL && secrets_object != NULL) {

			int mission_count = 0;
			const lwjson_token_t* mission_token = lwjson_get_first_child(mission_array);
			while (mission_token != NULL && mission_count < MAX_MISSIONS) {
				missions[mission_count].mission_number = lwjson_get_val_int(lwjson_find_ex(&lwjson, mission_token, "mission_number"));
				missions[mission_count].priority = lwjson_get_val_int(lwjson_find_ex(&lwjson, mission_token, "priority"));

				mission_token = mission_token->next;
				mission_count++;
			}

			const lwjson_token_t* word1_token = lwjson_find_ex(&lwjson, secrets_object, "word1");
			const lwjson_token_t* word2_token = lwjson_find_ex(&lwjson, secrets_object, "word2");

			size_t word1_len = lwjson_get_val_string_length(word1_token);
			size_t word2_len = lwjson_get_val_string_length(word2_token);

			if (word1_len != 0 && word2_len != 0) {
				const char *word1 = lwjson_get_val_string(word1_token, NULL);
				const char *word2 = lwjson_get_val_string(word2_token, NULL);
				memcpy(words[0], word1 , word1_len);
				memcpy(words[1], word2, word2_len);

				words[0][word1_len] = '\0';
				words[1][word2_len] = '\0';
				lwjson_free(&lwjson);
				return 0;
			}
		}
	}

	lwjson_free(&lwjson);
	return 1;
}
