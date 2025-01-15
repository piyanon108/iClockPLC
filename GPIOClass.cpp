#include "GPIOClass.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <QString>
using namespace std;

GPIOClass::GPIOClass()
{
    this->gpionum = "4"; //GPIO4 is default
}

GPIOClass::GPIOClass(string gnum)
{
    this->gpionum = gnum;  //Instatiate GPIOClass object for GPIO pin number "gnum"
    export_gpio();
}
int GPIOClass::export_gpio()
{
    string export_str = "/sys/class/gpio/export";
    ofstream exportgpio(export_str.c_str()); // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames
    if (exportgpio.fail()){
        cout << " OPERATION FAILED: Unable to export GPIO"<< this->gpionum <<" ."<< endl;
        return -1;
    }

    exportgpio << this->gpionum ; //write GPIO number to export
    exportgpio.close(); //close export file
    return 0;
}

int GPIOClass::unexport_gpio()
{
    string unexport_str = "/sys/class/gpio/unexport";
    ofstream unexportgpio(unexport_str.c_str()); //Open unexport file
    if (unexportgpio.fail()){
        cout << " OPERATION FAILED: Unable to unexport GPIO"<< this->gpionum <<" ."<< endl;
        return -1;
    }

    unexportgpio << this->gpionum ; //write GPIO number to unexport
    unexportgpio.close(); //close unexport file
    return 0;
}

int GPIOClass::setdir_gpio(string dir)
{

    string setdir_str ="/sys/class/gpio/gpio" + this->gpionum + "/direction";
    ofstream setdirgpio(setdir_str.c_str()); // open direction file for gpio
        if (setdirgpio.fail()){
            cout << " OPERATION FAILED: Unable to set direction of GPIO"<< this->gpionum <<" ."<< endl;
            return -1;
        }

        setdirgpio << dir; //write direction to direction file
        setdirgpio.close(); // close direction file
        return 0;
}
int GPIOClass::setGpio(){
    return setval_gpio("1");
}
int GPIOClass::resetGpio(){
    return setval_gpio("0");
}

int GPIOClass::setval_gpio(string val)
{

    string setval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
    ofstream setvalgpio(setval_str.c_str()); // open value file for gpio
        if (setvalgpio.fail()){
            cout << " OPERATION FAILED: Unable to set the value of GPIO"<< this->gpionum <<" ."<< endl;
            return -1;
        }

        setvalgpio << val ;//write value to value file
        setvalgpio.close();// close value file
        return 0;
}
int GPIOClass::setval_gpio(uint8_t val)
{
    string gpioval = QString::number(val).toStdString();
    string setval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
    ofstream setvalgpio(setval_str.c_str()); // open value file for gpio
        if (setvalgpio.fail()){
            cout << " OPERATION FAILED: Unable to set the value of GPIO"<< this->gpionum <<" ."<< endl;
            return -1;
        }

        setvalgpio << gpioval ;//write value to value file
        setvalgpio.close();// close value file
        return 0;
}
int GPIOClass::getval_gpio(string& val){
    string getval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
    ifstream getvalgpio(getval_str.c_str());// open value file for gpio
    if (getvalgpio.fail()){
//        cout << " OPERATION FAILED: Unable to get value of GPIO"<< this->gpionum <<" ."<< endl;
        return -1;
            }

    getvalgpio >> val ;  //read gpio value

    if(val != "0")
        val = "1";
    else
        val = "0";

    getvalgpio.close(); //close the value file
    return 0;
}
int GPIOClass::getGpioVal(){
    string val;
    string getval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
    ifstream getvalgpio(getval_str.c_str());// open value file for gpio
    if (getvalgpio.fail()){
//        cout << " OPERATION FAILED: Unable to get value of GPIO"<< this->gpionum <<" ."<< endl;
        return -1;
            }

    getvalgpio >> val ;  //read gpio value
    getvalgpio.close(); //close the value file
    if(val != "0"){
        val = "1";
        return 1;
    }
    else{
        val = "0";
        return 0;
    }
    return 0;
}

string GPIOClass::get_gpionum(){

return this->gpionum;

}
int GPIOClass::set_edge(string edge)
{
    string setdir_str ="/sys/class/gpio/gpio" + this->gpionum + "/edge";
    ofstream setdirgpio(setdir_str.c_str()); // open edge file for gpio
        if (setdirgpio.fail()){
            cout << " OPERATION FAILED: Unable to set direction of GPIO"<< this->gpionum <<" ."<< endl;
            return -1;
        }

        setdirgpio << edge; //write edge to direction file
        setdirgpio.close(); // close direction file
        return 0;
}

void GPIOClass::set_led_brightness(int brightness)
{
    std::ofstream brightness_file("/sys/class/leds/" +  this->gpionum + "/brightness");
    if (brightness_file.is_open()) {
        brightness_file << brightness;
        brightness_file.close();
    } else {
        std::cerr << "Unable to open brightness file for LED " <<  this->gpionum << std::endl;
    }
}
void GPIOClass::set_led_trigger(const std::string& trigger)
{
    std::ofstream brightness_file("/sys/class/leds/" +  this->gpionum + "/trigger");
    if (brightness_file.is_open()) {
        brightness_file << trigger;
        brightness_file.close();
    } else {
        std::cerr << "Unable to open trigger file for LED " <<  this->gpionum << std::endl;
    }
}
