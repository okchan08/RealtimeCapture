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
     m_addr(DEST_ADDR)

{
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
    
    data = (unsigned long long*)mmap(NULL, DEST_SIZE, PROT_READ | PROT_WRITE, 
                                                 MAP_SHARED, fd_mem, DEST_ADDR);
    if(data == MAP_FAILED){
        perror("mmap");
        std::cout << "Failed to mmap:  data" << std::endl;
    }

    close(fd_mem);
    
    setLength(2048*2);
    setDestinationAddress(DEST_ADDR);

}

DataCapture::~DataCapture(){
    munmap(command, CAPTURE_BLOCK_SIZE);
    munmap(data, DEST_SIZE);
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
    m_length = length;
    sendCommand("reset");
    sendCommand("halt");
    sendCommand("length");
}

void DataCapture::setDestinationAddress(int addr){
    m_addr = addr;
    sendCommand("reset");
    sendCommand("halt");
    sendCommand("dest");
}

void DataCapture::runCapture(void){
    sendCommand("run");
    while(true){
        if(axiGetValue(command, CAPTURE_DMSTS) & 0x80) break;
    }
    sendCommand("reset");
    sendCommand("halt");
    //cout << "Status:  0x" << hex << get_value(axi_capture_addr,CAPTURE_DMSTS)
    //     << "    Trans:  0x" << get_value(axi_capture_addr, CAPTURE_TRANS)
    //     << "    Last :  0x" << get_value(axi_capture_addr, CAPTURE_LAST_ADDR) 
    //     << "    Size :  0x" << get_value(axi_capture_addr, CAPTURE_SIZE) << endl;
    //cout << "---- DMA result ----" << endl << endl;
    memdump(data, 512);
    //cout << "---- end ----" << endl << endl;
}
