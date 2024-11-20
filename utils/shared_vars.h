//
// Created by heros on 20/11/24.
//

#ifndef SHARED_VARS_H
#define SHARED_VARS_H

#include "lists.h"

extern list blocks;
extern list files;
extern list history;
extern list get_pmap();

void shared_vars_init();
void sahred_vars_exit();

#endif //SHARED_VARS_H
