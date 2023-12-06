#include<iostream>
#include <stdint.h>
#include<string.h>
// #include"global.h"
// #include"main.c"

using namespace std;



uint16_t instructionEncodingLength = 1;//	ָ����볤��  ��λ���ֽ�
uint16_t instructionRoutingLength = 1;//	ָ��·�ɳ���
uint16_t g_instructionTableLength = 100;//	ָ�����

uint16_t g_instructionTable[2+instructionEncodingLength+instructionRoutingLength][g_instructionTableLength];//	ָ���

uint16_t g_instructionTableNum = 0;//	ָ������
uint16_t g_instructionTableNumID;//	ָ���ID����  ��ʲô�ģ�

/*
����ָ��
��������requestInstructionParsing
���ܣ���ָ�����й���
���룺һ��ָ��  data 
    ��ʽΪ��
    ָ�� ID��16bit����ҵ�� ID��8bit��+��ţ�bit����ɡ�����ڸ�ҵ��淶�ж��壬������ҵ��
    ���ö�������
    b) ָ��Ӧ�ù���ʶ�����16bit��ʹ�����еĵ� 11bit��Ӧ�ù��̱����������ע���ʽ����¼ A��
    c) ָ����룺��Ҫ����Ӧ�ù��̵���Ҫ���룬�ɾ�����Ŀ����ȷ����
    d) ָ��·�ɣ���Ҫ����Ӧ�ù��̵���Ҫ��ƣ��ɾ�����Ŀ����ȷ����


�����0���ܽ�������ָ��  1���Խ�������ָ��

��Ҫ����ָ���ָ�� ID ��ָ��Ӧ�ù���ʶ��� ��ָ����� ��ָ��·��

*/



uint8_t requestInstructionParsing(uint16_t data[]){
    //21��ҵ��ID����ߵ������

    if(data[0] && 0xFF == 128){
        //ָ�������һ��ָ��

        //�� ��ǰ32λ����һ��ָ�� �ŵ�ָ�����
        memcpy(g_instructionTable[g_instructionTableNum ++], &data[2], 2+instructionEncodingLength+instructionRoutingLength);

        return 1;
    }else if(data[0] && 0xFF == 129){
        //ɾ��n��ָ��
        int n = data[2];
        for(int i = 1;i <= n; i ++){
            uint16_t deleteID = data[i * 2] << 8 && data[i * 2 + 1]; //��ȡÿһ��Ҫɾ����ָ��ID

            for(int i = 0; i < g_instructionTableNum; i ++){ //����ָ���
                if(deleteID == g_instructionTable[i][0] << 8 && g_instructionTable[i][1]){//�ж�ID�Ƿ����

                    //ɾ��������������
                    g_instructionTable[i][0] = 0;
                    g_instructionTable[i][1] = 0;
                    g_instructionTableNum--;
                }
            }
        }
        return 1;

    }else if(data[0] && 0xFF == 130){
        //����n��ָ���Ӧ�ù���ʶ���
        int n = data[2];
        for(int i = 1;i <= 2 * n; i += 2){  //ÿ����2��
            uint16_t alterID = data[i * 2] << 8 && data[i * 2 + 1]; //��ȡÿһ��Ҫ�޸ĵ�ָ��ID

            for(int i = 0; i < g_instructionTableNum; i += 2){ //����ָ���
                if(alterID == g_instructionTable[i][0] << 8 && g_instructionTable[i][1]){//�ж�ID�Ƿ����

                    g_instructionTable[i][2] = data[i * 3];
                    g_instructionTable[i][3] = data[i * 3 + 1];
                }
            }
        }
        return 1;

    }else if(data[0] && 0xFF == 131){
        //����n��ָ��ı���
        int n = data[2];
        for(int i = 1;i <= 2 * n; i += 2){  //ÿ����2��
            uint16_t alterID = data[i * 2] << 8 && data[i * 2 + 1]; //��ȡÿһ��Ҫ�޸ĵ�ָ��ID

            for(int i = 0; i < g_instructionTableNum; i += 2){ //����ָ���
                if(alterID == g_instructionTable[i][0] << 8 && g_instructionTable[i][1]){//�ж�ID�Ƿ����

                    g_instructionTable[i][4] = data[i * 3];
                    g_instructionTable[i][5] = data[i * 3 + 1];
                }
            }
        }
        return 1;

    }else if(data[0] && 0xFF == 132){
        //����n��ָ���·��
        int n = data[2];
        for(int i = 1;i <= 2 * n; i += 2){  //ÿ����2��
            uint16_t alterID = data[i * 2] << 8 || data[i * 2 + 1]; //��ȡÿһ��Ҫ�޸ĵ�ָ��ID

            for(int i = 0; i < g_instructionTableNum; i += 2){ //����ָ���
                if(alterID == g_instructionTable[i][0] << 8 || g_instructionTable[i][1]){//�ж�ID�Ƿ����

                    g_instructionTable[i][6] = data[i * 3];
                    g_instructionTable[i][7] = data[i * 3 + 1];
                }
            }
        }
        return 1;
    }
    else return 0;
    
}


/*
ָ�����
��������instructionParsing	
���룺**һ��**   ����ִ�е�ָ��	
���ܣ�������������ָ�����󣬸���ָ������·�ɱ�ȷ��ָ��Ͷ���ȷ����ת�������ͷš�	
�����0����1��


*/
uint8_t instructionParsing(uint16_t data[]){
    //ָ��ID
    uint16_t instructionID = data[0];
    if(instructionID >> 8 == 21){//��������ָ��
        requestInstructionParsing(data);
        return 1;
    }else if(){ //��Ҫ���ݸ�ҵ�����ID���з��࣬����ת�������ͷ�
        for(int i =0;i < g_instructionTableNum;i++){
            if(instructionID == g_instructionTable[0]){
                printf("��ǰָ���ǣ�");
            }
        }
        
    }
    else return 0; //����
}




/*
ָ�������	
���룺ָ����е�һ��������ָ���	
���ܣ�  �Խ��յ�������ע��ֽ����ָ������з�����ʶ��ͬ��ִ�з�ʽ������ָ��¼���ָ���ָ���ע��������ת������
        ��ָ������ݷַ�����Ӧҵ��ֱ�Ӵ�����Ӧҵ���ִ�С�����Ҫ��ʶ��ָ����Ŀ�ͷ��
        ��Ҫά������������ִ��ָ���  ��ʱִ��ָ���
�����0/1


*/
uint8_t instructionPacketParsing(uint16_t data[]){
    //ָ������ݳ���  
    uint16_t instructionPacketLength = data[2] ;

    //ִ�з�ʽ���ͱ���  ռ���ֽڣ�2
    uint16_t executionType = data[3];



    if(executionType == 0xF0){ //����ָ��
        for(int i = 0; i < instructionPacketLength; i ++){
            instructionParsing(data[4 + i]);
        }

    }else if(executionType == 0xF1){ //��ʱָ��1

    }else if(executionType == 0xF2){ //��ʱָ��2

    }else if(executionType == 0xF3){ //��ʱָ��3

    }else if(executionType == 0xF4){ //��ָ��1

    }else if(executionType == 0xF5){ //��ָ��2

    }else if(executionType == 0xF6){ //��ָ��3

    }else if(executionType == 0xF9){ //��ʱָ��

    }else if(executionType == 0xFA){ //��ʱָ��4

    }else if(executionType == 0xFB){ //��ʱָ��5

    }else if(executionType == 0xFC){ //����ָ��

    }else 

    return 0;
}