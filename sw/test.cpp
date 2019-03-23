#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>

#include <stdio.h>
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

#define DEFAULT_CAPTURE_FILE  "/dev/uio0"

using namespace std;

unsigned int get_value(volatile void* address, unsigned int offset){
    return ((unsigned int*) address)[offset];
}

void set_value(volatile void* address, int unsigned offset, unsigned int value){
    ((unsigned int*) address)[offset] = value;
}

void memdump(void* virtual_address, int byte_count){
    //char *p = (char*) virtual_address;
    unsigned int *p = (unsigned int*) virtual_address;
    int offset;
    //for(offset=0;offset<byte_count;offset++){
    //    printf("%02x", p[offset]);
    //    if(offset % 8 == 7){ printf("\n");}
    //}
    for(offset=0;offset<byte_count;offset++){
        printf("%d\n", *p);
        p += 2;
    }
}

int main(int argc, char** argv){

    int fd_uio, fd_mem;
    fd_uio = open(DEFAULT_CAPTURE_FILE, O_RDWR | O_SYNC);
    if(fd_uio < 0){
        perror("open");
        return -1;
    }

    void* axi_capture_addr = mmap(NULL, CAPTURE_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_uio, 0);
    if(axi_capture_addr == MAP_FAILED){
        perror("mmap");
        return -1;
    }

    //volatile void* axi_capture = axi_capture_addr;
    close(fd_uio);

    /////////////////////////////////////

    fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd_mem < 0){
        perror("open");
        return -1;
    }
    
    void* axi_capture_dest_addr = mmap(NULL, CAPTURE_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, DEST_ADDR);
    if(axi_capture_dest_addr == MAP_FAILED){
        perror("mmap"); 
        return -1;
    }

    //volatile void* axi_capture_dest = axi_capture_addr;
    close(fd_mem);

    int length = 2048*2;

    memset((void*)axi_capture_dest_addr, 0, 128);
    memdump((void*)axi_capture_dest_addr, 512);

    set_value(axi_capture_addr, CAPTURE_RSVD, 0x1234);
    cout << "RSVD:  " << hex << get_value(axi_capture_addr, CAPTURE_RSVD) << endl;

    set_value(axi_capture_addr, CAPTURE_CTRL, 0x4);
    cout << "CTRL:  " << hex << get_value(axi_capture_addr, CAPTURE_CTRL) << endl;


    set_value(axi_capture_addr, CAPTURE_CTRL, 0x0);
    cout << "CTRL:  " << hex << get_value(axi_capture_addr, CAPTURE_CTRL) << endl;

    set_value(axi_capture_addr, CAPTURE_SIZE, length);
    cout << "SIZE:  " << hex << get_value(axi_capture_addr, CAPTURE_SIZE) << endl;

    set_value(axi_capture_addr, CAPTURE_START_ADDR, DEST_ADDR);
    cout << "DEST:  " << hex << get_value(axi_capture_addr, CAPTURE_START_ADDR) << endl;
    
    for(int i=0;i<0xF;i++){
        set_value(axi_capture_addr, CAPTURE_CTRL, 0x1);
        while(true){
            if(get_value(axi_capture_addr, CAPTURE_DMSTS) & 0x80) break;
        }
        set_value(axi_capture_addr, CAPTURE_CTRL, 0x4);
        set_value(axi_capture_addr, CAPTURE_CTRL, 0x0);
        cout << "Status:  0x" << hex << get_value(axi_capture_addr,CAPTURE_DMSTS)
             << "    Trans:  0x" << get_value(axi_capture_addr, CAPTURE_TRANS)
             << "    Last :  0x" << get_value(axi_capture_addr, CAPTURE_LAST_ADDR) 
             << "    Size :  0x" << get_value(axi_capture_addr, CAPTURE_SIZE) << endl;
        cout << "---- DMA result ----" << endl << endl;
        memdump((void*)axi_capture_dest_addr, 512);
        cout << "---- end ----" << endl << endl;
    }

    //cout << "Start addr:  " << hex << get_value(axi_capture_addr, CAPTURE_START_ADDR) << endl;

    memdump((void*)axi_capture_dest_addr, 512);

    cout << (unsigned int*) axi_capture_addr << endl;

    //set_value(axi_capture, CAPTURE_CTRL, 0);
    //set_value(axi_capture, CAPTURE_SIZE, 4);

    //cout << get_value(axi_capture, CAPTURE_DMSTS) << endl;

    munmap(axi_capture_addr, CAPTURE_BLOCK_SIZE);
    munmap(axi_capture_dest_addr, CAPTURE_BLOCK_SIZE);

    return 0;
}
