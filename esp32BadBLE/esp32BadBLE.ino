/**
badBLE
  2DO
  1. Problem of putting text then key return input(is delay needed?)
  2. Problem of closing other tabs not to interfere with the typing
      if anything is opened that involves a type box input will be there and not cmd
  3. Problem of it having to run on loop only
 */
#include <BleKeyboard.h>

BleKeyboard bleKeyboard("Hello Kitty<3", "cat", 47);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BadBLE!");
  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()) {
    Serial.println("Mwere connected :)");

    //  Win + R
    bleKeyboard.press(KEY_LEFT_GUI); //Windows Gui key
    bleKeyboard.press('r'); // DIRISHA + r
    bleKeyboard.releaseAll();
    delay(1000);
    bleKeyboard.setDelay(1000); // 2 second pause

    //type cmd 
    bleKeyboard.print("cmd /K echo wewe ni mwere");
    bleKeyboard.press(KEY_RETURN);
    bleKeyboard.releaseAll();
    delay(10);     

    delay(50000);
  }
}