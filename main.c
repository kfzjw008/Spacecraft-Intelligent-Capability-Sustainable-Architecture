#include <stdio.h>
#include <stdint.h>
#include "test/testConsole.h"
#include "global.h"
#include "windowsUtil.h"
/*
 * 按位与（&）：对两个操作数的每个对应位执行逻辑与操作，结果为1则表示对应位上的位都为1，否则为0。

按位或（|）：对两个操作数的每个对应位执行逻辑或操作，结果为1则表示对应位上的位至少有一个为1，否则为0。

按位异或（^）：对两个操作数的每个对应位执行逻辑异或操作，结果为1则表示对应位上的位不同，否则为0。

按位取反（~）：对操作数的每个位执行逻辑取反操作，结果为1则表示对应位上的位为0，否则为1。

左移（<<）：将操作数的所有位向左移动指定的位数。左移操作会在右侧填充零位。

右移（>>）：将操作数的所有位向右移动指定的位数。右移操作可能会在左侧填充零位或符号位，具体取决于操作数的类型。


 *
 *
 *
 *
 *
 * */



int main() {
    //FF为1字节，uint16_t代表着占用2字节，uint8_t占用1字节。
    printf("Hello,CISCA!!!!!!!\n", sizeof(uint8_t));
    mallocInit();//windows专用
    printf("First address of the virtual space= 0x%08X \n", MEMORY);
    printf("Online monitoring first address= 0x%08X \n", MONI_TAB_STR_ADRS);
    outpDw(2, 11);
    printf("inpDw Test = %d ", inpDw(2));
    //哪吒架构第一行代码
    return 0;
}

void maininit(){
    g_InjectionPacketBufferNum=0;
}
