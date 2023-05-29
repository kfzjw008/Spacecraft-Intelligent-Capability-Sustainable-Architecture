//
// Created by kfzjw008 on 2023-05-24.
//

#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include "windowsUtil.h"
#include "global.h"

void mallocInit() {
    MEMORYSize = MAX_MEMORY_SIZE;
    MEMORY = (uint8_t *) malloc(MEMORYSize * sizeof(uint8_t));
    if (MEMORY == NULL) {
        printf("failed!\n");
    }

}

