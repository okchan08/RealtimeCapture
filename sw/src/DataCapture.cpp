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

DataCapture::DataCapture()
    :m_length(512),
     m_addr(DEST_ADDR),
     m_dest_length(m_length)
{
    m_capture_data.resize(m_dest_length);
    fd_uio = open(DEFAULT_CAPTURE_FILE, O_RDWR | O_SYNC);
    if(fd_uio < 0){
        perror("open");
        std::cout << "Failed to open " << DEFAULT_CAPTURE_FILE << std::endl;
    }

    command = mmap(NULL, CAPTURE_BLOCK_SIZE, PROT_READ | PROT_WRITE, 
                                                            MAP_SHARED, fd_uio, 0);
    if(command == MAP_FAILED){
        perror("mmap");
        std::cout << "Failed to mmap:  command" << std::endl;
    }
    close(fd_uio);

    fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd_mem < 0){
        perror("open");
        std::cout << "Failed to open " << DEFAULT_CAPTURE_FILE << std::endl;
    }
    
    data = (unsigned long long*)mmap(NULL, m_dest_length, PROT_READ | PROT_WRITE, 
                                                 MAP_SHARED, fd_mem, m_addr);
    if(data == MAP_FAILED){
        perror("mmap");
        std::cout << "Failed to mmap:  data" << std::endl;
    }

    setLength(m_length);
    setDestinationAddress(m_addr);

}

DataCapture::~DataCapture(){
    munmap(command, CAPTURE_BLOCK_SIZE);
    munmap(data, m_dest_length);
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
    }else if(cmd == "dest"){
        axiSendCommand(command, CAPTURE_START_ADDR, m_addr);
    }else{
        showCommandList();
    }
}

void DataCapture::axiSendCommand(void* addr, int offset, int value){
    ((unsigned int*)addr)[offset] = value;
}

int DataCapture::axiGetValue(void *addr, int offset){
    return ((unsigned int*) addr)[offset];
}

void DataCapture::setLength(int length){
    munmap(data,m_dest_length);
    m_length = length;
    m_dest_length = m_length;
    m_capture_data.resize(m_dest_length);
    data = (unsigned long long*)mmap(NULL, m_dest_length, PROT_READ | PROT_WRITE, 
                                                 MAP_SHARED, fd_mem, m_addr);
    if(data == MAP_FAILED){
        perror("mmap");
        std::cout << "Failed to mmap:  data" << std::endl;
    }
    sendCommand("reset");
    sendCommand("halt");
    sendCommand("length");
    sendCommand("halt");
}

void DataCapture::setDestinationAddress(int addr){
    m_addr = addr;
    sendCommand("reset");
    sendCommand("halt");
    sendCommand("dest");
    sendCommand("halt");
}

std::vector<unsigned long long> DataCapture::runCapture(void){
    sendCommand("run");
    while(true){
        if(axiGetValue(command, CAPTURE_DMSTS) & 0x80) break;
    }
    sendCommand("reset");
    sendCommand("halt");

    for(int i=0;i<m_dest_length;i++){
        m_capture_data[i] = data[i];
    }
    return m_capture_data;
}
