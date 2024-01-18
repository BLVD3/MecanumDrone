# Mecanum Drohne
![Gesamtbild Drohne](https://cdn.discordapp.com/attachments/1026937665813221467/1197607230200094720/drone.jpg?ex=65bbe1a6&is=65a96ca6&hm=d73b7fdfc8561f6b4cedcd73d1040e2d21170449d3077918ae0754b9f52cb909&)
## Teammitglieder
- Henri Staudenrausch - 215994
- Lukas Vier - 215997
- Nico Vogel - 215998
- Antonia Friese - Während der Winterpause ausgetreten

## Inhaltsverzeichniss
- [1. Ziel des Projekts](#1-ziel-des-projekts)
- [2. Platine](#2-platine)
- [3. Fahrwerk](#3-fahrwerk)
   * [3.1 Funktionalität eines Mecanum Rads](#31-funktionalität-eines-mecanum-rads)
   * [3.2 Ansteuerungsberechnung](#32-ansteuerungsberechnung)
   * [3.3 Hardwarelösung zur Ansteuerung der Räder](#33-hardwarelösung-zur-ansteuerung-der-räder)
      + [3.3.1 Ausgangsproblem](#331-ausgangsproblem)
      + [3.3.2 DRV8212 H-Brücke](#332-drv8212-h-brücke)
   * [3.4 Softwarelösung zur Ansteuerung der Räder](#34-softwarelösung-zur-ansteuerung-der-räder)
- [4. Energie](#4-energie)
   * [4.1 Energiequelle](#41-energiequelle)
   * [4.2 Spannungregulation](#42-spannungregulation)
- [5. Greifarm](#5-greifarm)
   * [5.1 Funktionalität](#51-funktionalität)
   * [5.2 Hardware](#52-hardware)
      + [5.2.1 3D-Druck Teile](#521-3d-druck-teile)
         - [5.2.1.1 Allgemeines](#5211-allgemeines)
         - [5.2.1.1 Arme](#5211-arme)
         - [5.2.1.1 “Klauen”-Platte](#5211-klauen-platte)
         - [5.2.1.1 Eigene Grundplatte](#5211-eigene-grundplatte)
      + [5.2.2 Servomotoren](#522-servomotoren)
- [6. Steuerung](#6-steuerung)
   * [6.1 Hardwarekomponenten der Fernbedienung](#61-hardwarekomponenten-der-fernbedienung)
   * [6.2 Inputs](#62-inputs)
   * [6.3 Kommunikation](#63-kommunikation)
- [7. Projektverlauf](#7-projektverlauf)
   * [7.1 Hardwareplanung und Spezifikation](#71-hardwareplanung-und-spezifikation)
   * [7.2 Verzögerungen der NRF24L01](#72-verzögerungen-der-nrf24l01)
   * [7.3 Kurzschluss](#73-kurzschluss)
- [8. Links](#8-links)
## 1. Ziel des Projekts
Das Ziel dieses Projekts ist das bauen einer Drohne mit folgenden Features:
- Fahrwerk mit Mecanumrädern.
- Greifarm gesteuert mit 2 Servos.
- Steuerung über NRF24L01 Chips.
    - Sender ist ein Seperater Microcontroller, ausgestattet mit 2 Analogsticks.

## 2. Platine
Die Hardware des Roboters wurde auf einer in KiCad gestalteten Platine umgesetzt, die von JLCPCB produziert wurde. Komponenten der Platine werden in der Dokumentation an verschiedenen Stellen erwähnt.

Zusammengefasst ist diese Platine zuständig für die Spannungsversorgung und die Verbindung zum Microcontroller.

Im Links-Abschnitt befindet sich ein Link zu einem GitHub-Repository, in dem sich das KiCad-Projekt finden lässt.

## 3. Fahrwerk

### 3.1 Funktionalität eines Mecanum Rads
Ein Mecanumrad ist eine Art Allseitenrad (Omni Wheel auf Englisch), das seine Funktionalität durch schräge Rollen entlang des Radprofils erfüllt.

Ein Rad, dass sich nicht dreht, hat die Tendenz, sich entlang der Ausrichtung der Rollen zu bewegen. Ein sich drehendes Rad hingegen bewegt sich aktiv gegen die Achse der Rollen.

Mit 4 Rädern ist es, durch gezieltes Ansteuern, möglich in jede beliebige Richtung zu fahren.

### 3.2 Ansteuerungsberechnung
![Radsteuerung](https://upload.wikimedia.org/wikipedia/commons/thumb/c/c4/Mecanum_wheel_control_principle.svg/1024px-Mecanum_wheel_control_principle.svg.png)

In diesem Bild sieht man, wie man die Räder ansteuern muss (blaue Pfeile), um bestimmte Bewegungen zu erzielen (rote Pfeile).

Für uns sind die Ansteuerungen a), b) und e) wichtig. Diese können benutzt werden, um die anderen Bewegungen zu erzielen.


Um auszurechnen, wie die Räder angesteuert werden, bezeichnen wir von nun an die Rotationsgeschwindigkeit und Laufrichtung der Räder mit Hilfe von Zahlen zwischen -1 und 1. -1 (blauer Pfeil zurück) bedeutet, dass ein Rad mit voller Kraft sich rückwärts dreht. 1 bedeutet dann das genaue Gegenteil.

Nun haben wir drei Zahlen:

- frontBias ⟶ $-1 <= fb <= 1$
- rightBias ⟶ $-1 <= rb <= 1$
- clockwiseBias ⟶ $-1 <= cb <= 1$

Für $fb$ wird zu jedem Rad die Zahl $1 \times fb$ addiert.

Bei $rb$ kommt es nun darauf an, um welches Rad es sich handelt.

Für das vordere rechte Rad wird nicht $1 \times fb$ verwendet, sondern $-1 \times fb$. Der Faktor lässt sich im Bild ablesen.

Die gesamte Formel für das vordere rechte Rad sieht dann wie folgt aus:

$vl = 1 \times fb + -1 \times rb + -1 \times cb$

Wenn alle Räder ausgerechnet sind, werden alle Werte durch den größten absoluten Wert geteilt, der errechnet wurde. Dieser Schritt wird jedoch nur dann durchgeführt, wenn der errechnete Dividend größer als 1 ist.

Dadurch, dass $fb$, $rb$ und $cb$ beliebige Zahlen zwischen -1 und 1 sein können, lassen sich nun mehr als nur die Kombinationen im Bild erzeugen.

### 3.3 Hardwarelösung zur Ansteuerung der Räder

#### 3.3.1 Ausgangsproblem
Die Mecanumräder setzen voraus, dass alle 4 Räder unabhängig voneinander angesteuert werden können.

#### 3.3.2 DRV8212 H-Brücke
Da der Raspberry Pi Pico, den wir verwenden, natürlich nicht die benötigte Kraft hat, um die Räder direkt anzusteuern, benötigen wir Treiberchips.

Wir haben uns für den [DRV8212](https://www.ti.com/lit/ds/symlink/drv8212.pdf) entschieden.

Zusammengefasst stellt uns der Chip 2 Pins zur Verfügung, IN1 und IN2, durch die wir die Polung an den Motoren steuern können.

Wenn beide Pins LOW sind, befindet sich der Motor im "Leerlauf". Sind beide Pins HIGH, dann wird der Motor mit sich selbst kurzgeschlossen, und es entsteht eine Bremswirkung.

Wenn IN1 HIGH und IN2 LOW sind, wird der Motor sich in eine Richtung drehen. Das Ganze geht auch umgekehrt. Mit einem PWM-Signal lässt sich so nun auch die Geschwindigkeit des Rads regulieren.

Das Ganze kann man im Datenblatt unter Kapitel 8.3.2.1 nochmal nachlesen.

### 3.4 Softwarelösung zur Ansteuerung der Räder
Die in Punkt N.2 aufgezeigte Berechnung lässt sich sehr gut im Code umsetzen.

Um die main.cpp-Datei zu entlasten, haben wir unseren Code in andere Dateien ausgelagert.

**WheelController.hpp**
```c++
...
/// @brief Controller Class for a 4WD Mecanum Drone that uses PWM Controlled driverchips like the DRV8212.
class WheelController {
private:
    typedef struct {
        pin_size_t pPin;
        pin_size_t nPin;
    } Wheel;
    Wheel wheels[4];
    float speeds[4];
    void apply();
public:
    
    /// @brief Initializes the 4WD Mecanum Wheel Controller.
    ...
    void init(pin_size_t flPin1, ...);

    /// @brief Sets the speed of the drone.
    ...
    void setSpeed(float frontBias, float rightBias, float clockwiseBias);
};
...
```

`speeds` ist eine Variable, die zur Berechnung der aktuellen Radgeschwindigkeiten genutzt wird. Nach `setSpeed` sind alle Werte garantiert zwischen -1.f und 1.f.

In `wheels` stehen die Pins zur Radsteuerung drin. `pPin` ist der Pin, der auf HIGH gesetzt wird, wenn das Rad sich nach vorne drehen soll.

Da die komplette Implementationsdatei den Rahmen der Dokumentation sprengen würde, werden hier noch ein paar interessante Codeabschnitte aufgezeigt.

**WheelController::setSpeed**
```c++
...
for (byte i = 0; i < 4; i++) {
    speeds[i] = frontBias;
    speeds[i] += rightBias * ((i & 1) ^ ((i >> 1) & 1) ? -1.f : 1.f);
    speeds[i] += clockwiseBias * (i & 1 ? -1.f : 1.f);
    ...
}
...
```
Hier wird der erste Schritt der Berechnung durchgeführt.

In Zeile 28 wird ein ternärer Operator benutzt, um herauszufinden, welcher Faktor verwendet werden muss. Der TRUE-Case wird angewendet, wenn `i` 1 oder 2 ist ⟶ bit 0 != bit 1. Der Faktor -1 wird dadurch auf das vordere rechte Rad (1) und das hintere linke Rad (-1) angewendet.

**WheelController::apply**
```c++
for (byte i = 0; i < 4; i++) {
    if (speeds[i] == 0.f) {
        digitalWrite(wheels[i].pPin, HIGH);
        digitalWrite(wheels[i].nPin, HIGH);
    } else if (speeds[i] > 0) {
        digitalWrite(wheels[i].nPin, LOW);
        analogWrite(wheels[i].pPin, abs(speeds[i]) * 255);
    } else {
        digitalWrite(wheels[i].pPin, LOW);
        analogWrite(wheels[i].nPin, abs(speeds[i]) * 255);
    }
}
```

Hier werden die errechneten Geschwindigkeiten angewendet. Bei einer Geschwindigkeit von 0 wird das Rad durch das Setzen von HIGH auf beiden Pins gebremst.

## 4. Energie

### 4.1 Energiequelle
Als Energiequelle für die Drohne benutzen wir eine Lithium-Polymer-Batterie.

Diese besteht aus 4 Zellen und liefert eine Spannung zwischen 16.8 und 14.8 Volt.

Unsere Batterie besitzt bereits Schutzschaltungen zum Schutz der Batterie.

### 4.2 Spannungregulation
Wir benötigen 3 unterschiedliche Spannungen:
- Logik benutzt eine Spannung von **3.3V**.
- Die Motoren laufen mit einer Spannung von bis zu **12V**.
- Die Servomotoren des Arms benötigen eine Spannung von **5V**.

Zur Regulierung der Spannung verwenden wir 2 Buckconverter und einen Festspannungsregler auf unserer Platine.

- [LMR51420YFDDCR](https://www.ti.com/lit/ds/symlink/lmr51420.pdf) - 12V
- [TPS563200](https://www.ti.com/lit/ds/symlink/tps563200.pdf) - 5V
- [AMS1117-3.3](http://www.advanced-monolithic.com/pdf/ds1117.pdf) - 3.3V

## 5. Greifarm

### 5.1 Funktionalität
Um dem Fahrzeug zu ermöglichen, Objekte anzuheben und zu transportieren, haben wir einen Greifarm modelliert, der am vorderen Ende des Fahrzeugs befestigt ist. Zum Bewegen der Klaue sind 2 Servomotoren verbaut. Diese ermöglichen einerseits das Öffnen und Schließen der Klaue und zusätzlich das Anheben und Senken der Klaue.

### 5.2 Hardware
Der Greifarm ist in 8 Einzelteilen als 3D-Druck mit dem eigenen 3D-Drucker angefertigt. Als Motoren werden 2 [Multiplex - Tiny S](https://servodatabase.com/servo/multiplex/tiny-s) Micro Servos genutzt. Nahezu alle Teile sind mit M3 x 16 mm Schrauben verbunden.

#### 5.2.1 3D-Druck Teile
Der Greifarm ist an ein Design, das wir online gefunden haben, angelehnt: 
[Superb Tech - AWESOME 3D Printed Servo Robot Gripper!](https://www.youtube.com/watch?v=IalkWUN6wvE). 
In unserer eigenen Version haben wir einige Features abgeändert und hinzugefügt.

Alle 3D Modelle wurden in der Software Autodesk Fusion 360 erstellt / bearbeitet.

##### 5.2.1.1 Allgemeines
Es wurden diverse Löcher für Schrauben (hauptsächlich M3) angepasst. Wo es feste Schrauben gibt, sind Aussparungen für Schraubenmuttern gelassen, um das Befestigen zu erleichtern.

##### 5.2.1.1 Arme
Durch das Verlängern der Arme zwischen der Platte und den Backen öffnen sich die Backen der Klaue, wenn ganz geöffnet, breiter und wenn eher geschlossen, weiter vorne. Dadurch lassen sich größere Objekte oder kleinere Objekte in etwas größerer Entfernung anheben.

##### 5.2.1.1 “Klauen”-Platte
![Klauenplatte](https://cdn.discordapp.com/attachments/1030072796610568285/1197605795441946766/Klaue.PNG?ex=65bbe050&is=65a96b50&hm=526c1a8b0fd4a0b9b4aedd4680571db96075e6ae786cad168d798c9f6782e62c&)
Die Aussparungen der Vorlage waren für einen [MG90S Mini Servo](https://servodatabase.com/servo/towerpro/mg90) ausgelegt. Deshalb haben wir diese eigenständig auf die Maße unseres Servos angepasst.

Zur Verbindung mit anderen Komponenten war ursprünglich eine vertikale Platte mit einem Loch für Schrauben vorgesehen. Da wir die Klaue insgesamt anheben und senken wollen, haben wir die Platte entfernt und einen Teil eines Scharniers ergänzt.

Um das Anheben des Greifarms zu ermöglichen, haben wir zusätzlich am hinteren Teil der Klauen-Platte eine Wand mit Aussparung und Loch (M3 Metallstift) angebracht, die über eine Stange das Verbinden mit einem Servomotor ermöglicht.

##### 5.2.1.1 Eigene Grundplatte
![Grundplatte](https://cdn.discordapp.com/attachments/1030072796610568285/1197605795685204143/Grundplatte.PNG?ex=65bbe050&is=65a96b50&hm=a5a04b638ecee6372e194787e653f0e291ef6af47b95d43abb7714b24a726083&)
Um den Greifarm am Fahrzeugfahrgestell zu befestigen und die vertikale Bewegung zu ermöglichen, haben wir zusätzlich eine Grundplatte erstellt. Die Grundplatte lässt sich durch ein eingebautes Scharnier (einem M4 Metallstift) beweglich mit der Klauen-Platte verbinden. Zum Befestigen des Servomotors ist auf der Grundplatte eine Halterung angebracht, an die der Motor festgeschraubt wird. Auf der Unterseite der Grundplatte sind 2 Aussparungen, die Platz für die Schrauben der Radmotoren lassen. Um die Platte am Fahrgestell zu befestigen, ist im hinteren Bereich der Grundplatte ein Langloch eingelassen. Durch dieses wird sie mit M3-Schrauben am Gestell festgeschraubt.

#### 5.2.2 Servomotoren
Multiplex Tiny S - Micro Servo
https://servodatabase.com/servo/multiplex/tiny-s
|  |  |
| ---- | ---- |
| Drehmoment | **4.8V:** 2.60 kg-cm  <br>**6.0V:** 3.00 kg-cm |
| Geschwindigkeit | **4.8V:** 0.11 sec/60°  <br>**6.0V:** 0.09 sec/60° |
| Gewicht | 16,4g |
| Maße | **Länge:** 29.7 mm  <br>**Breite:** 11.9 mm  <br>**Höhe:** 29.5 mm |

## 6. Steuerung
Die Fernbedienung ermöglicht die Steuerung der Drone mithilfe von zwei analogen Joysticks, einem Gyroskop, einem Schalter und einem Potentiometer. Die Kommunikation zwischen der Fernbedienung und der Autodrone erfolgt über den NRF24L01. Wir konnten sie leider nie so umsetzen, da die NRF24L01-Module erst nach Abschluss des Projekts eingetroffen sind.

### 6.1 Hardwarekomponenten der Fernbedienung
1. Raspberry Pi Pico
2. 2 Analog-Joysticks
3. Gyroskop
4. Schalter
5. Potentiometer
6. NRF24L01

### 6.2 Inputs
Die Fernbedienung sollte zwei verschiedene Steuerungsmöglichkeiten haben:
- Joystick-Steuerung

Die Joysticks ermöglichen die Steuerung der Drone in alle Richtungen. Der rechte Joystick steuert die Bewegung nach links und rechts, während der linke Joystick den Roboter drehen kann.
- Gyro-Steuerung

Durch Umschalten auf die Gyro-Steuerung kann sie mithilfe von Neigungen und Drehungen gesteuert werden. Das Gyroskop erfasst die Bewegungen der Fernbedienung und überträgt sie auf die Drone.
- Modusumschaltung

Ein Schalter auf der Fernbedienung ermöglicht das Umschalten zwischen Joystick- und Gyro-Steuerung. 

### 6.3 Kommunikation
Die NRF24L01 ermöglichen eine drahtlose Kommunikation zwischen der Fernbedienung und der Autodrone. Die Kommunikation erfolgt über definierte Payloads, wobei jedes Byte eine spezifische Bedeutung hat. 

## 7. Projektverlauf

### 7.1 Hardwareplanung und Spezifikation
Am Anfang des Projekts hatten wir bereits den "Körper" der Drone, einschließlich der Motoren, Räder und der Batterie. Wir begannen mit der Planung des Projekts und der Spezifikation aller benötigten Hardwarekomponenten. Währenddessen entwarf Nico die Platine, Lukas kümmerte sich um den Greifarm mit den 2 Servos und druckte ihn 3D aus, während Henri die Fernbedienung entwickelte.

### 7.2 Verzögerungen der NRF24L01
Verzögerungen bei der Beschaffung der NRF24L01 führten zu Herausforderungen in der Projektumsetzung. Als Alternative haben wir versucht die Kommunikation über den WLAN-Chip des Raspberry Pi Pico zu realisieren. Leider stellte sich heraus, dass der WLAN-Chip nicht funktionierte. Als vorübergehende Lösung haben wir einen analogen Joystick über ein langes Kabel direkt an die Drone angeschlossen, um die Steuerung zu testen, was gut funktionierte.

### 7.3 Kurzschluss
Während weiterer Tests am haben wir ausversehen einen Kurzschluss verursacht, was dazu führte, dass der Buck-Converter auf der Platine durchgebrannt ist. Leider haben wir es nicht geschafft herrauszufinden, was den Kurzschluss verursacht hat. Der Kurzschluss ist am Freitag vor der Präsentation passiert, weshalb wir das Projekt nicht wieder rechtzeitig reparieren konnten.

## 8. Links
- GitHub-Repositories
    - [Drohne](https://github.com/BLVD3/MecanumDrone)
    - [Platine](https://github.com/BLVD3/MecanumControllerPCB)
- Bilder
    - [Ansteuerung](https://upload.wikimedia.org/wikipedia/commons/thumb/c/c4/Mecanum_wheel_control_principle.svg/1024px-Mecanum_wheel_control_principle.svg.png)
- Datenblätter
    - [RaspberryPi Pico](https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf)
    - [DRV8212](https://www.ti.com/lit/ds/symlink/drv8212.pdf)
    - [LMR51420YFDDCR](https://www.ti.com/lit/ds/symlink/lmr51420.pdf)
    - [TPS563200](https://www.ti.com/lit/ds/symlink/tps563200.pdf)
    - [AMS1117-3.3](http://www.advanced-monolithic.com/pdf/ds1117.pdf)
    - [NRF24L01+](https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf)
