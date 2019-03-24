#include <iostream>
#include <vector>

#include <fcntl.h>

#include "DataCapture.h"

using namespace std;

int main(int argc, char** argv){
    DataCapture dc;

    cout << "aaa" << endl;
    dc.setLength(512/2);
    auto vec = dc.runCapture();
    cout << "aho" << endl;
    cout << vec.size() << endl;
    for(int i=0;i<vec.size();i++){
        cout << i << "   " << vec.at(i) << endl;
    }
    cout << "end of vec" << endl;

    return 0;

}
