//
// Created by kfzjw008 on 2023-05-16.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>


int func1(int a, int b) {
    return a + b;
}

int func2(int a, int b) {
    return a * b;
}

int main23() {
    int counter = 0;
    while (1) {
        // 定期输出计数器的值
        if (counter % 10 == 0) {
            printf("counter = %d\n", counter);
        }

        // 等待用户输入
        printf("Please enter a command:\n");
        char cmd;
        scanf("%c", &cmd);
        int a, b;
        scanf("%d %d", &a, &b);

        // 根据用户输入调用相应的函数
        int result;
        if (cmd == '1') {
            result = func1(a, b);
            printf("%d + %d = %d\n", a, b, result);
        } else if (cmd == '2') {
            result = func2(a, b);
            printf("%d * %d = %d\n", a, b, result);
        } else {
            printf("invalid command\n");
        }

        // 每次循环计数器加1
        counter++;

        // 暂停一段时间
        //sleep(1);
    }
    return 0;
}

