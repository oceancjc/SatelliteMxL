#include <iostream>
#include <string>
#include <fstream>
#include "protect.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


using namespace std;

/*
 * 序列号+随机字符32位+mac   算个md5  存到树莓派的一个文件里，
 * 这个文件最好是和树莓派系统配置公用的文件。校验失败snmp不工作。
 *
 * CODE_VERIFICATION 定义了表示做校验，并在校验成功后执行某个程序
 *                   不定义表示写校验表。运行就行
 **/

#define DEBUG_PRINT           //
#define SECRETFILE            "/etc/rsyslog.conf"
#define SECRETLINE           (58)
#define CMDTODO_CORRECT      "/home/pi/oceancjc/C/OptiMeter_V2/src_cjc/opt_app &"
#define CMDTODO_WRONG        "pkill opt_app;pkill cpu_opt"
#define SEED                 "Oceancjcisagoodguy^_^"





#define CODE_VERIFICATION    

struct Key_str
{
    string serial_num;
    string mac_num;
    string seed;
    string md5Code;
};



int get_secretCode(const string file, const int line, string &code){
    string buf = "";
    ifstream f;
    try{
        f.open(file.c_str(), ios::in);
    }    
    catch (exception& e){
        DEBUG_PRINT cout << e.what() << endl; 
        DEBUG_PRINT cout << "Open file fail: " << file << endl;        return -2;
    }
        
    for (int i = 0; i < line - 1; i++) {
        if (!getline(f, buf))    return -2;
    }
    if (getline(f, buf)) {
        if ("### " != buf.substr(0, 4)){
            DEBUG_PRINT cout << "Invalid Authoration" << endl;
            return -3;
        }
        code = buf.substr(4);
        DEBUG_PRINT cout << code << endl;
        return 0;
    }    
    else     return -2;
}



int set_secretCode(const string file, const int line, string &code){
    //ofstream f(file);
    DEBUG_PRINT cout << file << line << code << endl;
    return 0;
}





int main(){
    int ret = 0;
    /* Step 1 Generate MD5 Code accroding to MPU Info */
    Key_str key_s = { "", "", SEED, ""};
    char *mac_cstr = (char*)calloc(30,sizeof(char));
    char *serial_cstr = (char*)calloc(128, sizeof(char));
    ret = getmac(mac_cstr);    ret |= getcpunum(serial_cstr);
    key_s.mac_num = mac_cstr;    free(mac_cstr);
    key_s.serial_num = serial_cstr;    free(serial_cstr);
    string formatCode = key_s.serial_num + SEED + key_s.mac_num;
    uint8_t* md5Code = (uint8_t*)calloc(128, sizeof(uint8_t));
    getmd5((uint8_t*)formatCode.c_str(), md5Code);
    key_s.md5Code = (char*)md5Code;    free(md5Code);
    DEBUG_PRINT cout << "MAC is: " << key_s.mac_num << endl;
    DEBUG_PRINT cout << "Serial is: " << key_s.serial_num << endl;
    DEBUG_PRINT cout << "Before: " << formatCode.c_str() << endl;
    DEBUG_PRINT cout << "Code is: " << key_s.md5Code << endl;
    if (ret)    return 0;
    
    /* **************************************** 
     * Mode select: 
     *   1 for code generation
     *   2 for code verification
     * ****************************************/
#ifdef CODE_VERIFICATION
    /* Step 2: Get MD5Code from hidden file */
    string code = "";
    ret = get_secretCode(SECRETFILE, SECRETLINE, code);
    if (ret){
        DEBUG_PRINT cout << "Error in Get secret code from file" << endl;
        return 0;
    }
    
    /* Step 3: Compare two codes and do corresponding actions*/
    if (code == key_s.md5Code){
        cout << "Authorization Success ..." << endl;
        system(CMDTODO_CORRECT);
    }
    else{
        cout << "Authorization Fail ... Do nothing" << endl;
        system(CMDTODO_WRONG);
    }
#else
    /* Step 2: Write MD5Code in the correct posistion of the file */
    ret  = set_secretCode(SECRETFILE, SECRETLINE, key_s.md5Code);
    if (ret)    DEBUG_PRINT cout << "Write Authorization code fail ..." << endl;
#endif
    
    
    return 0;
}