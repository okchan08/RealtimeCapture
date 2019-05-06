#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#include "DataCapture.h"

void memdump(unsigned long long* virtual_address, int byte_count){
    unsigned long long *p = (unsigned long long*) virtual_address;
    for(int offset=0;offset<byte_count-1;offset++){
        printf("%lld\n", p[offset]);
    }
}

DataCapture::DataCapture(size_t data_length, unsigned int dest_addr)
    :m_length(data_length),
     m_addr(DEST_ADDR),
     m_dest_length(data_length)
{
    m_capture_data.resize(data_length);
    int fd_uio, fd_mem;
    fd_uio = open(DEFAULT_CAPTURE_FILE, O_RDWR | O_SYNC);
    if(fd_uio < 0){
        perror("open");
    }

    command = mmap(NULL, CAPTURE_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_uio, 0);
    if(command == MAP_FAILED){
        perror("mmap");
    }

    close(fd_uio);

    /////////////////////////////////////

    fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd_mem < 0){
        perror("open");
    }
    
    data = mmap(NULL, CAPTURE_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, DEST_ADDR);
    if(data == MAP_FAILED){
        perror("mmap"); 
    }

    close(fd_mem);

    int length = 2048*2;

    memset((void*)data, 0, 128);
    //memdump((void*)data, 512);

    axiSendCommand(command, CAPTURE_RSVD, 0x1234);
    std::cout << "RSVD:  " << std::hex << axiGetValue(command, CAPTURE_RSVD) << std::endl;

    axiSendCommand(command, CAPTURE_CTRL, 0x4);
    std::cout << "CTRL:  " << std::hex << axiGetValue(command, CAPTURE_CTRL) << std::endl;


    axiSendCommand(command, CAPTURE_CTRL, 0x0);
    std::cout << "CTRL:  " << std::hex << axiGetValue(command, CAPTURE_CTRL) << std::endl;

    axiSendCommand(command, CAPTURE_SIZE, length);
    std::cout << "SIZE:  " << std::hex << axiGetValue(command, CAPTURE_SIZE) << std::endl;

    axiSendCommand(command, CAPTURE_START_ADDR, DEST_ADDR);
    std::cout << "DEST:  " << std::hex << axiGetValue(command, CAPTURE_START_ADDR) << std::endl;
}

DataCapture::~DataCapture(){
    munmap(command, CAPTURE_BLOCK_SIZE);
    munmap(data, CAPTURE_BLOCK_SIZE);
    close(fd_mem);
}

void DataCapture::showCommandList(void){

    }

void DataCapture::sendCommand(const char* cmd){
    if(cmd == "reset"){
        axiSendCommand(command, CAPTURE_CTRL, 0x4);
    }else if(cmd == "halt"){
        axiSendCommand(command, CAPTURE_CTRL, 0x0);
    }else if(cmd == "run"){
        axiSendCommand(command, CAPTURE_CTRL, 0x1);
    }else if(cmd == "length"){
        axiSendCommand(command, CAPTURE_SIZE, m_length);   
        std::cout << "size init." << std::endl;
        std::cout << axiGetValue(command, CAPTURE_SIZE) << std::endl;
    }else if(cmd == "dest"){
        axiSendCommand(command, CAPTURE_START_ADDR, m_addr);
    }else{
        showCommandList();
    }
}

void DataCapture::axiSendCommand(volatile void* address, int unsigned offset, unsigned int value){
    ((unsigned int*)address)[offset] = value;
}

unsigned int DataCapture::axiGetValue(volatile void* address, unsigned int offset){
    return ((unsigned int*) address)[offset];
}

void DataCapture::setLength(int length){
    //munmap(data,MMAP_SIZE_TEMP);
    //m_length = length;
    //m_dest_length = m_length;
    //m_capture_data.resize(m_dest_length);
    ////data = (unsigned long long*)mmap(NULL, MMAP_SIZE_TEMP, PROT_READ | PROT_WRITE, 
    ////                                                MAP_SHARED, fd_mem, m_addr);
    //if(data == MAP_FAILED){
    //    perror("mmap");
    //    std::cout << "Failed to mmap:  data" << std::endl;
    //}
    //sendCommand("reset");
    //sendCommand("halt");
    //sendCommand("length");
    //sendCommand("halt");
}

void DataCapture::setDestinationAddress(int addr){
    m_addr = addr;
    sendCommand("reset");
    sendCommand("halt");
    sendCommand("dest");
    sendCommand("halt");
}

std::vector<unsigned long long> DataCapture::runCapture(void){
    axiSendCommand(command, CAPTURE_CTRL, 0x1);
    while(true){
        if(axiGetValue(command, CAPTURE_DMSTS) & 0x80) break;
    }
    axiSendCommand(command, CAPTURE_CTRL, 0x4);
    axiSendCommand(command, CAPTURE_CTRL, 0x0);
    memdump((unsigned long long*)data, 512);
    std::cout << "---- end ----" << std::endl << std::endl;
    unsigned long long* tmp = (unsigned long long*)data;
    for(int i=0;i<m_length;i++) m_capture_data.at(i) = tmp[i];

    return m_capture_data;
}
