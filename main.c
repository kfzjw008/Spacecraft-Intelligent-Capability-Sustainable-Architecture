#include <stdio.h>
#include <stdint.h>
#include "test/testConsole.h"

int main() {
    //FF为1字节，uint16_t代表着占用2字节，uint8_t占用1字节。
    printf("Hello,CISCA!!!!!!!\n %d",sizeof(uint8_t));
    //哪吒架构第一行代码
    return 0;
}
