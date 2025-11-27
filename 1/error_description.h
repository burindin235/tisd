#ifndef ERROR_DESCRIPTION
#define ERROR_DESCRIPTION

#include "types.h"
#include <stddef.h>
#include <stdio.h>

#include "input_check.h"

void error_input_description(int rc);
void error_validate_first_num_description(int rc);
void error_validate_second_num_description(int rc);

#endif