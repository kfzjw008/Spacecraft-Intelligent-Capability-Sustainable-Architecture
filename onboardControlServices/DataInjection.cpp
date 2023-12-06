#include<iostream>
#include <stdint.h>
#include<string.h>
#include"global.h"
#include"main.c"
#include<bitset>
using namespace std;

/***********************************************************
packetSplicing
����������ע���ƴ��
���������  uint8_t* data ע�������ʽΪ���飬һ��Ԫ�ش�СΪ8λ��
            uint16_t size ע������ȣ���λ���ֽ�
���������ƴ�Ӻ���ĳ���
˵�������ô˺���ʱ���ѽ��յ�β�����˺��������ע���ƴ��Ϊһ����
***********************************************************/
uint16_t packetSplicing(){
    //��g_InjectionTempPacketBuffer�����ע���ƴ�ӣ�Ȼ�����g_InjectionTempPacketBuffer
    uint8_t Psize = 0;
    uint8_t size = 0;
    uint8_t result[50];
    uint8_t resultPointer = 6;
    for(int i = 0; i < g_InjectionTempPacketBufferPointer; i ++){
        uint8_t t = g_InjectionTempPacketBuffer[i]; //��һ������
        if(!i){
            result[0] = t[0];
            result[1] = t[1];
            result[3] = t[2] | 0b11000000;
            result[4] = t[3];
        }
        size = ((t[4]) << 8) | t[5];
        Psize += size;
        for(uint8_t i = 0; i < size; i ++){
            result[resultPointer++] = t[i + 6];
        }
    }
    result[5-6] = size;

    //���
    g_InjectionTempPacketBufferPointer = 0;
    
    return result,Psize;
}
/***********************************************************
�������ƣ�pac ketIdentification
����������ע���ʶ��
���������  uint8_t* data ע�������ʽΪ���飬һ��Ԫ�ش�СΪ8λ��
            uint16_t size ע������ȣ���λ���ֽ�
���������0:���ǰ�  ����:��0�ֽڵ������ֽڹ���һ����
˵    ����ע���ʶ�𣬳���ʶ��ע����Ƿ���ȷ�����ж����װ���β������������
***********************************************************/
uint16_t packetIdentification(uint8_t *data, uint16_t size) {
    // ������ݳ����Ƿ��㹻
    if (size < 6) {
        return 0;  // ���ݳ��Ȳ��㣬�޷����������İ�
    }

    // �������汾��
    uint8_t version = (data[0] >> 5 & 0b0111);//��1�ֽ�ǰ3λ
    // ����������
    uint8_t packetType = (data[0] >> 4) & 0b0001;
    // ��������ͷ��־
    uint8_t secondaryHeaderFlag = (data[0] >> 3) & 0b0001;
    // ����Ӧ�ù��̱�ʶ��
    uint16_t applicationProcessID = ((data[0] & 0b0111) << 8) | data[1]; //����8λ�Ǹ�data[1]��λ�á�
    //�������б�־
    uint8_t sequenceFlag = (data[2] >> 6 & 0x03); //��3�ֽ�ǰ2λ
    // ���������м���
    uint16_t sequenceCount = ((data[2] & 0x3F) << 8) | data[3];//��3�ֽں�6λ + ��4�ֽ�ǰ8λ
    // ���������ݳ���
    uint16_t Psize = ((data[4]) << 8) | data[5];//��5�ֽ� + ��6�ֽ�

    //ע���û�и���ͷ������Ҫ����
    // printf("%d\n",sequenceFlag );
    // printf("%d\n",sequenceFlag & 0x01);

    if (version == 0 && secondaryHeaderFlag == 0 && packetType == 1 && applicationProcessID == 0b01100110100 &&
        Psize <= INJECTDATA_SIZE){
            if(sequenceFlag  == 0x01) {
                // Ϊ�װ�
                g_InjectionPacketFullFlag = 1;
                //������ʱ������
                *g_InjectionTempPacketBuffer[g_InjectionTempPacketBufferPointer++] = *data;
                printf("�ð�Ϊ�װ���");
                return 0;
            }else if(sequenceFlag  == 0x00){
                // �м��
                printf("�ð�Ϊ�м����");
                //������ʱ������
                *g_InjectionTempPacketBuffer[g_InjectionTempPacketBufferPointer++] = *data;
                return 0;
            }
            else if(sequenceFlag  == 0x02){
                printf("�ð�Ϊβ����");
                //ĩβ��
                g_InjectionPacketFullFlag = 0;
                //������ʱ������
                *g_InjectionTempPacketBuffer[g_InjectionTempPacketBufferPointer++] = *data;
                Psize = packetSplicing();
                return Psize;  // ����β��������һ�������İ������ذ�����
            }
            else if(sequenceFlag  == 0x03){
                printf("�ð�Ϊ��������");
                //������
                return Psize;  // ����һ�������İ������ذ�����
            }else return 0;
        }
        
    else {
        // printf("%d",sequenceFlag);
        return 0;
    }
}

//ע�������
/***********************************************************
�������ƣ� injectionPacketReception(uint8_t data[],uint8_t size)
��������������ע�������
���������uint8_t data[],uint8_t size
�����������
˵    ����
***********************************************************/

void injectionPacketReception(uint8_t data[],uint8_t size){ 

    //data��һ�����ݰ���һ�����ݰ��������ָ���
    if(size < 6){
        return ;
    }
    g_ingectSize = size;//ע�����ݳ���

    uint8_t i; //ѭ������
    uint8_t start,end;

    while (i < size) {
        // ���� packetIdentification ����������Ƿ���Թ��������İ�
        uint16_t packetIndex = packetIdentification(&data[i], size - i);
        
        if (packetIndex == 0) {
            // �����Թ��������İ����˳�ѭ��
            break;
        }

        // ���ˣ�ʶ��һ�������İ�����i��ʼ������ΪpacketIndex

        // ����ע�������ʼ�ͽ�������
        start = i;
        end= i + packetIndex - 1;

        // ��ע������Ƶ� g_InjectionPacketBuffer
        memcpy(g_InjectionPacketBuffer[(g_InjectionPacketBufferNum ++) % MAX_INJECT_BUFFER_NUM], &data[start], packetIndex);
        printf("ע����ѷŵ�������~\n");
        printf("%d",g_InjectionPacketBufferNum);
        // �ƶ�����һ��ע�������ʼλ��
        i = end + 1;
    }


    return;
}

//�ж���ָ�����ͷ
//ָ����̶���ʼΪ��00011000 10110100
bool isInstructionPacket(uint8_t data1,uint8_t data2){
    if(data1 == 0b00011000 && data2 == 0b10110100){
        return 1;
    }
    else return 0;
}
//ע������
//������һ��ע���
//����ֵ��   �� | ָ�����������ָ�룿

void injectionPacketUnpacking(uint8_t data[]){
    uint8_t Psize = (data[4] << 8 )|data[5]; //������ָ��ֽ���

    uint8_t  head = 0,tail = 0;
    for(uint8_t i = 0;i < Psize - 1; i ++){
        if(isInstructionPacket(data[i],data[i + 1])){//�ж�Ϊ��ָ��ͷ��������ָ�������
            if(i - tail > 0){ //����0ʱ�ǵ�һ����
                
                tail = i -1;
                //��ָ�� �±�Ϊ��head - tail;
                // ��ע������Ƶ� g_instructionPacket
                memcpy(g_instructionPacketBuffer[g_instructionPacketBufferNum ++], &data[head], tail - head + 1);
            }
            head = i;
        }
    }

}


    // #define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*ע�����������ʼ��ַ
    // #define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*ע���������������ַ
    // #define  MAX_INJECT_BUFFER_NUM                      (10)        /*ע���������������*/


//����ȷ�Լ��
int packageValidity(uint8_t *item){
//���Ϲ淶Ϊ0,�����Ϲ淶����1
// �� ע��������� ��ע���
    printf("��ʼ����ȷ�Լ��~\n");
    printf("item:%d\n",*item);
    uint16_t packageSymbol = inpDw(*item) & 0xFFFF; //ǰ���ֽ�Ϊ����ʶ��һ��16λ
    printf("packageSymbol��%d\n",packageSymbol);
    uint16_t packageSequenceControl = inpDw(*item + 2)& 0xFFFF; // �м����ֽ�Ϊ�����п���
    uint16_t packageDataLen = inpDw(*item + 4)& 0xFFFF; //������

    // �汾�Ŵ���1
    uint8_t version = packageSymbol >> 13; // ����13λ��ʣ����λΪ���汾
    if(version != 0b000){
        return 0;
    }
    // �����ʹ���2
    uint8_t packetType = (packageSymbol >> 12 ) & 0b0001;

    // ����ͷ��־����3
    uint8_t secondaryHeaderFlag = (packageSymbol >> 11 )& 0b00001;

    // Ӧ�ù��̱�ʶ������4  ��Ч�غ�ϵͳע�������Ϊ 0b01100110100 
    uint8_t applicationProcessID = packageSymbol  & 0b0000011111111111;

    //�������б�־
    uint8_t sequenceFlag = packageSequenceControl >> 14;
    // ���������м���
    uint16_t sequenceCount = packageSequenceControl & 0b0011111111111111;
    // ���������ݳ���
    uint16_t Psize = packageDataLen & 0b1111111111111111;

    if (version != 0b000) {
        return 1;
    }
    if (packetType != 0b1) {
        return 1;
    }
    if (secondaryHeaderFlag != 0b0) {
        return 1;
    }
    if (applicationProcessID != 0b0001001100110100) {
        return 1;
    }
    if (sequenceFlag >= 4) {
        return 1;
    }

    return 0;
}


/***********************************************************
�������ƣ�
���������� ��Ψһ��
���������
���������ָ���
˵    �����Ի�����������չ����ؼ��
#define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*ע�����������ʼ��ַ
#define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*ע���������������ַ
***********************************************************/
int packageUniqueness() {
    uint16_t sequenceCounts[MAX_INJECT_BUFFER_NUM];
    int i = 0;
    // �������������еİ����
    uint32_t item;
    for (item = ((uint32_t) (MONI_ZRB_STR_ADRS)); item < ((uint32_t) (MONI_ZRB_END_ADRS));item += MAX_INJECT_PKG_LEN) {
        if (i == g_InjectionPacketBufferNum) {
            // �������һ������ֹͣ
            break;
        }
        uint16_t n_ui16VarT1MIEn1 = inpDw(item + 0) & 0xFFFF;
        uint16_t n_ui16VarT1MIEn2 = inpDw(item + 2) & 0xFFFF;
        uint16_t n_ui16VarT1MIEn3 = inpDw(item + 4) & 0xFFFF;
        // �������汾��
        uint8_t version = n_ui16VarT1MIEn1 >> 13;
        // ����������
        uint8_t packetType = (n_ui16VarT1MIEn1 >> 12) & 0b0001;
        // ��������ͷ��־
        uint8_t secondaryHeaderFlag = (n_ui16VarT1MIEn1 >> 11) & 0b00001;
        // ����Ӧ�ù��̱�ʶ��
        uint16_t applicationProcessID = n_ui16VarT1MIEn1 & 0b0000011111111111;
        //�������б�־
        uint8_t sequenceFlag = n_ui16VarT1MIEn2 >> 14;
        // ���������м���
        uint16_t sequenceCount = n_ui16VarT1MIEn2 & 0b0011111111111111;
        // ���������ݳ���
        uint16_t Psize = n_ui16VarT1MIEn3 & 0b1111111111111111;


        for (int j = 0; j < g_InjectionPacketBufferNum; j++) {
            if (sequenceCounts[j] == sequenceCount && j != i && j < i) {
                g_InjectionPacketBufferCheckFlag[j] = 2;
            }
        }
        i = i + 1;
    }
}

/***********************************************************
�������ƣ�
���������� ���Ϸ���
���������
���������ָ���
˵    �����Ի�����������չ����ؼ��
#define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*ע�����������ʼ��ַ
#define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*ע���������������ַ
***********************************************************/
int packageLegitimacy(uint8_t data[]) {
    //���ý������
    injectionPacketUnpacking(data);
    uint8_t Psize = (data[4] << 8) | data[5];
    uint8_t Isize = 0;
    for(uint8_t i = 0; i < g_instructionPacketBufferNum; i ++){
        Isize += (g_instructionPacketBuffer[i][4] << 8) | g_instructionPacketBuffer[i][5];
    }
    if(Psize == Isize) return 1;

    return 0; 
    

}

/***********************************************************
�������ƣ�
���������� �������
���������
���������ָ���
˵    �����Ի�����������չ����ؼ��
#define    MONI_ZRB_STR_ADRS            (0x0000A000+MEMORY)    /*ע�����������ʼ��ַ
#define    MONI_ZRB_END_ADRS            (0x0000BFFF+MEMORY)    /*ע���������������ַ
***********************************************************/

bool packageRelevance(uint32_t item){
    //�����ǰ���յ���ͨ�����İ�֮��İ���ź���ĩ�������߼���ϵ��ͨ����������ơ������б�
    uint32_t item;
    int i;
    for (item = ((uint32_t) (MONI_ZRB_STR_ADRS)); item < ((uint32_t) (MONI_ZRB_END_ADRS));item += MAX_INJECT_PKG_LEN) {
        if (i == g_InjectionPacketBufferNum) {
            // �������һ������ֹͣ
            break;
        }
        uint16_t packageSequenceControl = inpDw(item + 2); // �м����ֽ�Ϊ�����п���
        uint8_t seqSymbol = packageSequenceControl >> 14;
        if(seqSymbol == 1){//�װ�
            if(g_InjectionPacketFullFlag == 1) return 0;
            else g_InjectionPacketFullFlag = 1;
        }else if(seqSymbol== 0) {// �м����
            if(g_InjectionPacketFullFlag == 0) return 0;
        } else if(seqSymbol == 0x03){// ��������
            if(g_InjectionPacketFullFlag == 1) return 0;
        } else if(seqSymbol == 0x02) { //ĩβ����
            if(g_InjectionPacketFullFlag == 0) return 0;
            else g_InjectionPacketFullFlag = 0;
        }
    }
    
}

// ע������
bool injectionPacketInspection(int size){

    printf("%d",*g_InjectionPacketBuffer[g_InjectionPacketBufferNum]); //ע���������������
    // for(int i = 0; i < size; i ++){
    //     printf("%d\n",g_InjectionPacketBuffer[g_InjectionPacketBufferNum][i]);
    // }
    bool flag;
    flag = packageValidity(g_InjectionPacketBuffer[g_InjectionPacketBufferNum]);
    if(flag == 1) return 0;
    packageUniqueness();
    if(flag == 1) return 0;

    // packageLegitimacy(data);
    // if(flag == 1) return 0;
    // packageRelevance(data);
    // if(flag == 1) return 0;
    return 1;
}


int main()
{
    int i;// ѭ��data���ݵ��±�

    // uint8_t data[5] = {0b00011110,0b00001100,0b00000111,0b10101010,0b01101010};  // �豸���ʴ��ݵ�����
    // for(i = 0;i < 5;i ++){
    //     cout << bitset<8>(data[i]) << "   ";
    //     // printf("data�е�i������Ϊ��%B",data[i]);
    // }
    int size = 7;
    uint8_t data[size] = {0b00010011,0b00110100,0b11000000,0b00000001,0b00000000,0b00000111,0b11111111};// �豸���ʴ��ݵ�����
    // packetIdentification(data,6);
// 00010011001101001100000000000001000000000000011111111111
    // ע�������

    injectionPacketReception(data,size);
    g_injectionPacketRxCount ++;

    //ע������
    if(injectionPacketInspection(size)){
        printf("ע�����ȷ~");
    }else{
        printf("ע�������ȷ~");
    }

    //ע���ƴ��Ϊע��֡


    char myvar[8];
    
    // while(1){ // һֱ���ڵȴ��������ݵ�״̬
    //     cout << "input the data"  << endl;
        
    //     for(int i = 0;i < 8; i ++){
    //         cin >> myvar[i];
    //     }
    //     for(int i = 0; i < 8; i ++){
    //         if(myvar[i] == '0'){
    //             data = (data << 1); // Shift left
    //         } else if (myvar[i] == '1') {
    //             data = (data << 1) | 1; // Shift left and set the least significant bit
    //         }
    //     }

    //     // �Դˣ��õ���ÿ�δ��ݵ�һ��uint8_t���ݡ�

    //     cout << bitset<8>(data) << endl;
    // }

    puts("");
    mallocInit();
    printf("�׵�ַΪ��%08X",MEMORY);
    printf("β��ַΪ��%08X",MEMORY + 0x000FFFFF);

    return 0;
}