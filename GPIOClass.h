#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H

#include <string>
//INPUT PIN 960 - 991

#define RX1             "960"
#define RX2             "961"
#define RX3             "962"
#define RX4             "963"
#define RX5             "964"
#define RX6             "965"
#define RX7             "966"
#define RX8             "967"
#define TX1             "968"
#define TX2             "969"
#define TX3             "970"
#define TX4             "971"
#define TX5             "972"
#define TX6             "973"
#define TX7             "974"
#define TX8             "975"

#define PTT_HEAD_PIN            "981"
#define HEAD_AMP_CTRL_PIN       "982"

#define CODEC_RESET_PIN           "1019"
#define A_SHD_PIN                 "1020"
#define A_MUTE_PIN                "1021"
#define V_MUTE                    "1022"
#define PGA2500_HS_0DB            "1023"

#define SW_EN1_PIN              "978"
#define SW_EN2_PIN              "979"
#define SW_EN3_PIN              "980"

#define CLK_EN1_PIN             "954"
#define DT_EN1_PIN              "955"
#define CLK_EN2_PIN             "956"
#define DT_EN2_PIN              "957"
#define CLK_EN3_PIN             "958"
#define DT_EN3_PIN              "959"



using namespace std;
/* GPIO Class
 * Purpose: Each object instantiated from this class will control a GPIO pin
 * The GPIO pin number must be passed to the overloaded class constructor
 */
class GPIOClass
{
public:
    GPIOClass();  // create a GPIO object that controls GPIO4 (default
    GPIOClass(string x); // create a GPIO object that controls GPIOx, where x is passed to this constructor
    int export_gpio(); // exports GPIO
    int unexport_gpio(); // unexport GPIO
    int setdir_gpio(string dir); // Set GPIO Direction
    int setval_gpio(string val); // Set GPIO Value (putput pins)
    int setval_gpio(uint8_t val);
    int setGpio();
    int resetGpio();
    int set_edge(string edge);
    int getval_gpio(string& val); // Get GPIO Value (input/ output pins)
    int getGpioVal();
    string get_gpionum(); // return the GPIO number associated with the instance of an object
    void set_led_brightness(int brightness);
    void set_led_trigger(const std::string& trigger);
private:
    string gpionum; // GPIO number associated with the instance of an object
};

#endif
