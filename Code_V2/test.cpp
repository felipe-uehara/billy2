#define LED 5

void setup(){
    pinMode(LED, OUTPUT);
}

void loop{
    for(i = 0; i < 100; i++){
        if (i % 10 == 0)
            digitalWrite(LED, HIGH);
        else
            digitalWrite(LED, LOW);
    }
    delay(10);
}