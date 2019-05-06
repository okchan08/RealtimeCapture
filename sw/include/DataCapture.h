#ifndef DATACAPTURE_H
#define DATACAPTURE_H

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define CAPTURE_ADDR       0x43c00000 // AXI_Stream_Capture address
#define CAPTURE_BLOCK_SIZE 0xFFFF // AXI_Stream_Capture size
#define CAPTURE_CTRL       0x00 // Control register R/W
#define CAPTURE_SIZE       0x01 // data size (Byte) R/W
#define CAPTURE_TRANS      0x02 // transfered data size (Byte) Read only
#define CAPTURE_START_ADDR 0x03 // Transfer destination start address R/W
#define CAPTURE_LAST_ADDR  0x04 // Address on which last transfer queued Read only
#define CAPTURE_PRETRIG    0x05 // Control pretrigger setting R/W
#define CAPTURE_DMSTS      0x06 // Status data from AXI DataMover Read only
#define CAPTURE_RSVD       0x07 // Reserved register. Just write and read. No effect on Capture logic

#define DEST_ADDR          0x0f000000 // DMA destination address
#define DEST_SIZE          0xffff     // DMA destination buffer size

#define DEFAULT_CAPTURE_FILE  "/dev/uio0"
class DataCapture{
    public:
        DataCapture(size_t length, unsigned int dest_addr);
        ~DataCapture();
        void sendCommand(const char* cmd);
        void setLength(int length);
        int getStatus(std::string &cmd);
        std::vector<unsigned long long> runCapture(void);
        

    private:
        void* data;
        void*  command;
        
        int fd_uio, fd_mem;
        int m_length;
        unsigned int m_addr;
        unsigned long long m_dest_length;
        
        void showCommandList(void);
    
        void axiSendCommand(volatile void* address, int unsigned offset, unsigned int value);
        unsigned int axiGetValue(volatile void* address, unsigned int offset);

        void setDestinationAddress(int addr);
    
        std::vector<unsigned long long> m_capture_data;
        
        
};

#endif
