#include <iostream>

#include <fcntl.h>

#include "DataCapture.h"

using namespace std;

int main(int argc, char** argv){
    DataCapture dc;

    dc.runCapture();
    return 0;

}
