#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class led_handler {
private:
    int _num_pixels;
    float _interp_ratio;
    int _pot_min;
    int _pot_max;
    int _max_value;
    int last_updated_pixel;
    Adafruit_NeoPixel pixels;
    long last_millis;
    int last_value;
    bool begin_idle;
    bool b_enable_idle;

public:
    void begin(int numpixels, float interpolation_ratio, int pot_min, int pot_max, int led_pin) {
        _num_pixels = numpixels;
        _interp_ratio = interpolation_ratio;
        _pot_min = pot_min;
        _pot_max = pot_max;
        _max_value = numpixels / interpolation_ratio;
        pixels.setPin(led_pin); // TEMP
        pixels.updateLength(numpixels);
        pixels.updateType(NEO_GRB + NEO_KHZ800);
        pixels.begin();
        last_millis = millis();
        begin_idle = true;
        b_enable_idle = false;
        
    }

    void test_progress(int value) {
        int p_value = constrain(map(value, _pot_min, _pot_max, 0, _num_pixels / 2), 0, _num_pixels / 2);
        int calc_pixel = p_value;
        //if((last_value >= value - 5 && last_value <= value + 5) && millis() - last_millis > 5000){
            //idle_lights_animation(begin_idle);
            begin_idle = false;
        //}
        if(last_updated_pixel != calc_pixel){
            //if(begin_idle == false) {
            //    end_idle_animation();
            //}
            light_up_with_smooting(calc_pixel);
            last_millis = millis();
            begin_idle = true;
        }
        last_updated_pixel = calc_pixel;
        last_value = value;
        Serial.print("Raw: ");
        Serial.print(value);
        Serial.print(", P_VAL: ");
        Serial.print(p_value);
        Serial.print(", Pixel: ");
        Serial.print(calc_pixel);
        Serial.println();
    }

    void progress(int value) {
        int p_value = constrain(map(value, _pot_min, _pot_max, 0, 100), 0, 100);
        int calc_pixel = (p_value / 100.f) * (_num_pixels/2);
        Serial.println(millis() - last_millis > 5000);
        if(b_enable_idle == true && millis() - last_millis > 2500){
            idle_lights_animation(begin_idle);
            
            //last_millis = millis();
            begin_idle = false;
        }
        if(last_updated_pixel != calc_pixel){
            if(begin_idle == false) {
                end_idle_animation();
            }
            light_up_with_smooting(calc_pixel);
            last_millis = millis();
            begin_idle = true;
        }
        last_updated_pixel = calc_pixel;
        last_value = value;
        //Serial.print("Raw: ");
        //Serial.print(value);
        //Serial.print(", P_VAL: ");
        //Serial.print(p_value);
        //Serial.print(", Pixel: ");
        //Serial.print(calc_pixel);
        //Serial.println();
    }

    void enable_idle() {
        if(!b_enable_idle)
            last_millis = millis();
        b_enable_idle = true;
    }

    void disable_idle() {
        b_enable_idle = false;
        if(!begin_idle)
            end_idle_animation();
    }

    void enable_full_on_mode() {
        for(int c = 0; c < 150; c+=15){
                for(int p = 0; p < _num_pixels/2; p++) {
                    pixels.setPixelColor(p, pixels.Color(100, 100, 100));
                    pixels.setPixelColor(_num_pixels - p, pixels.Color(100, 100, 100));
                }
                pixels.show();
            }
    }

private:

    void light_up_with_smooting(int new_position) {
        if(new_position < last_updated_pixel){
            for(int c = 150; c >= 0; c-=15){
                for(int p = last_updated_pixel; p >= new_position; p--) {
                    pixels.setPixelColor(p, pixels.Color(0, 0, c));
                    pixels.setPixelColor(_num_pixels - p, pixels.Color(0, 0, c));
                }
                pixels.show();
            }
        }else {
            
            for(int c = 0; c < 150; c+=15){
                for(int p = last_updated_pixel; p < new_position; p++) {
                    pixels.setPixelColor(p, pixels.Color(0, 0, c));
                    pixels.setPixelColor(_num_pixels - p, pixels.Color(0, 0, c));
                }
                pixels.show();
            }
        }
    }

    void end_idle_animation() {
        pixels.clear();
        pixels.show();
        return;
        int amount = 10;
        for(int c = 0; c < 150; c+=50){
            for(int p = _num_pixels / 2; p >= 0 ; p-=amount) {
                for(int a = p - amount; a < p; a++) {
                    pixels.setPixelColor(a, pixels.Color(0, 0, 0));
                    pixels.setPixelColor(_num_pixels - a, pixels.Color(0, 0, 0));
                }
            }
            pixels.show();
        }
    }

    void idle_lights_animation(bool _begin_idle) {
        int blue_brightness = 50;
        int green_brightness = 100;
        int led_amount = 50;
        int return_index = 50;
        if(_begin_idle) {
            for(int c = 0; c < blue_brightness; c+=10){
                for(int p = 0; p < _num_pixels/2; p++) {
                    if(analogRead(A0) > return_index && analogRead(A1) > return_index)
                        return;
                    pixels.setPixelColor(p, pixels.Color(0, 0, c));
                    pixels.setPixelColor(_num_pixels - p, pixels.Color(0, 0, c));
                }
                if(analogRead(A0) > return_index && analogRead(A1) > return_index)
                    return;
                pixels.show();
            }
            return;
        }
        int random_num = random() % _num_pixels;
        // Trail
        for(int c = 0; c < green_brightness; c+=10){
            for(int p = random_num - led_amount; p < random_num; p++) {
                if(analogRead(A0) > return_index && analogRead(A1) > return_index)
                    return;
                pixels.setPixelColor(p, pixels.Color(0, c, blue_brightness));
            }
            if(analogRead(A0) > return_index && analogRead(A1) > return_index)
                return;
            pixels.show();
            delay(1);
        }
        delay(10);
        for(int c = green_brightness; c >= 0; c-=5){
            for(int p = random_num - led_amount; p < random_num; p++) {
                if(analogRead(A0) > return_index && analogRead(A1) > return_index)
                    return;
                pixels.setPixelColor(p, pixels.Color(0, c, blue_brightness));
            }
            if(analogRead(A0) > return_index && analogRead(A1) > return_index)
                return;
            pixels.show();
            delay(1);
        }
        //
        for(int p = 0; p < _num_pixels; p++) {
            pixels.setPixelColor(p, pixels.Color(0, 0, blue_brightness));
            if(analogRead(A0) > return_index && analogRead(A1) > return_index)
                return;
        }
        if(analogRead(A0) > return_index && analogRead(A1) > return_index)
            return;
        pixels.show();
    }

};
