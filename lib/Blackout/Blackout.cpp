#include "Blackout.h"

const char *Blackout::CLOCKWISE = "clockwise";
const char *Blackout::COUNTERCLOCKWISE = "counterclockwise";

Blackout::~Blackout()
{
}

Blackout::Blackout(const char *name, const int stepsPerRevolution, const int speed, int pin1, int pin2, int pin3, int pin4)
    : name(name),
      stepper(Stepper(stepsPerRevolution, pin1, pin2, pin3, pin4)),
      stepsPerRevolution(stepsPerRevolution),
      speed(speed)
{
    //testear si es buena idea:
    stepper.setSpeed(speed);
}

void Blackout::handleRoller(char *topic, byte *payload, unsigned int length)
{
    int vueltas = 0, vueltasActual = 0;
    int sentidoPasos = stepsPerRevolution;
    String sentido = "";
    int porcentaje = ((vueltasActual * 100) / vueltas);

    JsonStepper jsonStepper;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &write = jsonBuffer.createObject();

    JsonObject &root = jsonStepper.decode_json(payload);
    vueltas = root["vueltas"].as<int>();
    sentido = root["sentido"].as<String>();

    // if (!root.success())
    // {
    //     serverClient.println(
    //         "Hay un error en la instrucción. Revise de nuevo el formato.");
    //     root.printTo(Serial);
    //     return;
    // }

    if (sentido == CLOCKWISE)
    {
        write["sentido"] = CLOCKWISE;
    }
    else if (sentido == COUNTERCLOCKWISE)
    {
        sentidoPasos *= -1;
        write["sentido"] = COUNTERCLOCKWISE;
    }
    write["vueltas"] = vueltas;
    write["progreso"] = porcentaje;
    write["estado"] = "girando";
    //client.publish(jsonStepper.encode_json(write).c_str());
    Serial.println(jsonStepper.encode_json(write).c_str());
    do
    {
        //serverClient.println("Iniciando paso.");
        delay(15);
        stepper.step(sentidoPasos);
        delay(15);
        //serverClient.println("Terminando paso.");
        vueltasActual++;
        porcentaje = calcular_porcentaje(vueltasActual, vueltas);
        write["progreso"] = porcentaje;
        if ((porcentaje % 5) == 0)
        {
            //client.publish(jsonStepper.encode_json(write).c_str());
            Serial.println(jsonStepper.encode_json(write).c_str());
        }
    } while (vueltasActual < vueltas);
    write["estado"] = "finalizado";
    //serverClient.println(jsonStepper.encode_json(write));
    //client.publish(jsonStepper.encode_json(write).c_str());
    Serial.println(jsonStepper.encode_json(write).c_str());
}
