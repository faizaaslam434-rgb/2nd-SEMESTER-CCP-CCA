/*
=============================================================
  Smart Home Automation System
  Object Oriented Programming — C++11

  Design Patterns:
    1. Singleton Pattern  — HomeController (one central hub)
    2. Observer Pattern   — Devices react to sensor events
    3. Command Pattern    — Device operations with undo support

  OOP Pillars:
    1. Encapsulation  — Private/protected data in all classes
    2. Inheritance    — Device base -> Light, Fan, AC, Camera, Lock
    3. Polymorphism   — Virtual turnOn/turnOff/getStatus/onNotify
    4. Abstraction    — Abstract Device class with pure virtual methods
=============================================================
*/

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// HELPER — int to string (Dev C++ compatible)
// ============================================================
string intToStr(int n) {
    if (n == 0) return "0";
    string result = "";
    bool negative = false;
    if (n < 0) { negative = true; n = -n; }
    while (n > 0) {
        result = (char)('0' + n % 10) + result;
        n /= 10;
    }
    if (negative) result = "-" + result;
    return result;
}

// ============================================================
// OBSERVER PATTERN — Interface
// ============================================================
class IObserver {
public:
    virtual void onNotify(const string& event, const string& location) = 0;
    virtual ~IObserver() {}
};

// ============================================================
// ABSTRACT BASE CLASS — Device
// ============================================================
class Device : public IObserver {
protected:
    string deviceName;
    string location;
    bool   isOn;
    int    powerWatts;

public:
    Device(const string& name, const string& loc, int watts)
        : deviceName(name), location(loc), isOn(false), powerWatts(watts) {}

    virtual void   turnOn()          = 0;
    virtual void   turnOff()         = 0;
    virtual string getStatus() const = 0;
    virtual string getType()   const = 0;

    string getName()     const { return deviceName; }
    string getLocation() const { return location;   }
    bool   getIsOn()     const { return isOn;        }
    int    getPower()    const { return powerWatts;  }

    void displayInfo() const {
        cout << "  [" << getType() << "] "
             << deviceName
             << " | Location: " << location
             << " | Status: "   << getStatus()
             << " | Power: "    << powerWatts << "W" << endl;
    }

    virtual ~Device() {}
};

// ============================================================
// Smart Light
// ============================================================
class SmartLight : public Device {
private:
    int    brightness;
    string colorMode;

public:
    SmartLight(const string& name, const string& loc)
        : Device(name, loc, 15), brightness(100), colorMode("White") {}

    void turnOn() {
        isOn = true;
        cout << "  [LIGHT] " << deviceName
             << " ON | Brightness: " << brightness
             << "% | Color: " << colorMode << endl;
    }

    void turnOff() {
        isOn = false;
        cout << "  [LIGHT] " << deviceName << " OFF" << endl;
    }

    void setBrightness(int level) {
        if (level < 0)   level = 0;
        if (level > 100) level = 100;
        brightness = level;
        cout << "  [LIGHT] " << deviceName
             << " brightness -> " << brightness << "%" << endl;
    }

    void setColor(const string& color) {
        colorMode = color;
        cout << "  [LIGHT] " << deviceName
             << " color -> " << colorMode << endl;
    }

    string getStatus() const {
        if (!isOn) return "OFF";
        return "ON | Brightness: " + intToStr(brightness) + "% | Color: " + colorMode;
    }

    string getType() const { return "Smart Light"; }

    void onNotify(const string& event, const string& loc) {
        if (event == "MOTION_DETECTED" && loc == location) {
            cout << "  [AUTO] Motion in " << location << " -> ";
            turnOn();
        } else if (event == "NO_MOTION" && loc == location) {
            cout << "  [AUTO] No motion in " << location << " -> ";
            turnOff();
        } else if (event == "DAYLIGHT") {
            if (isOn) turnOff();
        }
    }
};

// ============================================================
// Smart Fan
// ============================================================
class SmartFan : public Device {
private:
    int speed;

public:
    SmartFan(const string& name, const string& loc)
        : Device(name, loc, 75), speed(3) {}

    void turnOn() {
        isOn = true;
        cout << "  [FAN]   " << deviceName
             << " ON | Speed: " << speed << "/5" << endl;
    }

    void turnOff() {
        isOn = false;
        cout << "  [FAN]   " << deviceName << " OFF" << endl;
    }

    void setSpeed(int s) {
        if (s < 1) s = 1;
        if (s > 5) s = 5;
        speed = s;
        cout << "  [FAN]   " << deviceName
             << " speed -> " << speed << "/5" << endl;
    }

    string getStatus() const {
        if (!isOn) return "OFF";
        return "ON | Speed: " + intToStr(speed) + "/5";
    }

    string getType() const { return "Smart Fan"; }

    void onNotify(const string& event, const string& loc) {
        if (event == "HIGH_TEMP") {
            if (!isOn) turnOn();
            setSpeed(5);
        } else if (event == "NORMAL_TEMP") {
            setSpeed(2);
        } else if (event == "LEAVING_HOME") {
            if (isOn) turnOff();
        }
    }
};

// ============================================================
// Air Conditioner
// ============================================================
class AirConditioner : public Device {
private:
    int    targetTemp;
    string mode;

public:
    AirConditioner(const string& name, const string& loc)
        : Device(name, loc, 1500), targetTemp(22), mode("Cool") {}

    void turnOn() {
        isOn = true;
        cout << "  [AC]    " << deviceName
             << " ON | Mode: " << mode
             << " | Temp: " << targetTemp << "C" << endl;
    }

    void turnOff() {
        isOn = false;
        cout << "  [AC]    " << deviceName << " OFF" << endl;
    }

    void setTemperature(int temp) {
        if (temp < 16) temp = 16;
        if (temp > 30) temp = 30;
        targetTemp = temp;
        cout << "  [AC]    " << deviceName
             << " temperature -> " << targetTemp << "C" << endl;
    }

    void setMode(const string& m) {
        mode = m;
        cout << "  [AC]    " << deviceName
             << " mode -> " << mode << endl;
    }

    string getStatus() const {
        if (!isOn) return "OFF";
        return "ON | Mode: " + mode + " | Temp: " + intToStr(targetTemp) + "C";
    }

    string getType() const { return "Air Conditioner"; }

    void onNotify(const string& event, const string& loc) {
        if (event == "HIGH_TEMP") {
            if (!isOn) turnOn();
            setTemperature(20);
        } else if (event == "LEAVING_HOME") {
            if (isOn) turnOff();
        }
    }
};

// ============================================================
// Security Camera
// ============================================================
class SecurityCamera : public Device {
private:
    bool isRecording;

public:
    SecurityCamera(const string& name, const string& loc)
        : Device(name, loc, 10), isRecording(false) {}

    void turnOn() {
        isOn = true;
        cout << "  [CAM]   " << deviceName << " ACTIVATED" << endl;
    }

    void turnOff() {
        isOn = false;
        isRecording = false;
        cout << "  [CAM]   " << deviceName << " DEACTIVATED" << endl;
    }

    void startRecording() {
        if (isOn) {
            isRecording = true;
            cout << "  [CAM]   " << deviceName << " RECORDING..." << endl;
        } else {
            cout << "  [CAM]   " << deviceName << " is OFF. Turn it on first!" << endl;
        }
    }

    void stopRecording() {
        isRecording = false;
        cout << "  [CAM]   " << deviceName << " recording stopped." << endl;
    }

    string getStatus() const {
        if (!isOn)       return "OFF";
        if (isRecording) return "ON | RECORDING";
        return "ON | Monitoring";
    }

    string getType() const { return "Security Camera"; }

    void onNotify(const string& event, const string& loc) {
        if (event == "INTRUDER_ALERT") {
            if (!isOn) turnOn();
            startRecording();
        } else if (event == "LEAVING_HOME") {
            if (!isOn) turnOn();
        }
    }
};

// ============================================================
// Smart Lock
// ============================================================
class SmartLock : public Device {
private:
    bool   isLocked;
    string accessCode;

public:
    SmartLock(const string& name, const string& loc)
        : Device(name, loc, 5), isLocked(true), accessCode("1234") {}

    void turnOn() {
        isOn = true;
        cout << "  [LOCK]  " << deviceName << " ACTIVATED" << endl;
    }

    void turnOff() {
        isOn = false;
        cout << "  [LOCK]  " << deviceName << " DEACTIVATED" << endl;
    }

    void lock() {
        isLocked = true;
        cout << "  [LOCK]  " << deviceName << " LOCKED" << endl;
    }

    bool unlock(const string& code) {
        if (code == accessCode) {
            isLocked = false;
            cout << "  [LOCK]  " << deviceName << " UNLOCKED successfully!" << endl;
            return true;
        }
        cout << "  [LOCK]  Wrong code! Door remains LOCKED." << endl;
        return false;
    }

    string getStatus() const {
        if (!isOn) return "OFF";
        return isLocked ? "ON | LOCKED" : "ON | UNLOCKED";
    }

    string getType() const { return "Smart Lock"; }

    void onNotify(const string& event, const string& loc) {
        if (event == "LEAVING_HOME") {
            lock();
        } else if (event == "INTRUDER_ALERT") {
            lock();
            cout << "  [LOCK]  " << deviceName << " auto-locked!" << endl;
        }
    }
};

// ============================================================
// COMMAND PATTERN
// ============================================================
class ICommand {
public:
    virtual void   execute()              = 0;
    virtual void   undo()                 = 0;
    virtual string getDescription() const = 0;
    virtual ~ICommand() {}
};

class TurnOnCommand : public ICommand {
    Device* device;
public:
    TurnOnCommand(Device* d) : device(d) {}
    void execute() { device->turnOn();  }
    void undo()    { device->turnOff(); }
    string getDescription() const { return "Turn ON  " + device->getName(); }
};

class TurnOffCommand : public ICommand {
    Device* device;
public:
    TurnOffCommand(Device* d) : device(d) {}
    void execute() { device->turnOff(); }
    void undo()    { device->turnOn();  }
    string getDescription() const { return "Turn OFF " + device->getName(); }
};

// ============================================================
// SINGLETON PATTERN — HomeController
// ============================================================
class HomeController {
private:
    static HomeController* instance;
    vector<Device*>   devices;
    vector<ICommand*> commandHistory;

    HomeController() {}

public:
    static HomeController* getInstance() {
        if (instance == NULL)
            instance = new HomeController();
        return instance;
    }

    void addDevice(Device* device) {
        devices.push_back(device);
        cout << "  [+] " << device->getName()
             << " registered [" << device->getType() << "]" << endl;
    }

    Device* getDevice(int index) {
        if (index >= 0 && index < (int)devices.size())
            return devices[index];
        return NULL;
    }

    int getDeviceCount() {
        return (int)devices.size();
    }

    void executeCommand(ICommand* cmd) {
        cmd->execute();
        commandHistory.push_back(cmd);
    }

    void undoLastCommand() {
        if (commandHistory.empty()) {
            cout << "  Nothing to undo." << endl;
            return;
        }
        ICommand* last = commandHistory.back();
        cout << "  [UNDO] " << last->getDescription() << endl;
        last->undo();
        commandHistory.pop_back();
        delete last;
    }

    void broadcastEvent(const string& event, const string& location = "") {
        cout << "\n  >> Event: [" << event << "]";
        if (location != "") cout << " at [" << location << "]";
        cout << endl;
        for (int i = 0; i < (int)devices.size(); i++)
            devices[i]->onNotify(event, location);
    }

    void displayStatus() const {
        cout << "\n  ========================================" << endl;
        cout << "           SMART HOME STATUS              " << endl;
        cout << "  ========================================" << endl;
        for (int i = 0; i < (int)devices.size(); i++) {
            cout << "  " << i+1 << ". ";
            devices[i]->displayInfo();
        }
        cout << "  ========================================" << endl;
    }

    void powerReport() const {
        int total = 0;
        cout << "\n  ---- POWER CONSUMPTION REPORT ----" << endl;
        bool anyOn = false;
        for (int i = 0; i < (int)devices.size(); i++) {
            if (devices[i]->getIsOn()) {
                cout << "  " << devices[i]->getName()
                     << ": " << devices[i]->getPower() << "W" << endl;
                total += devices[i]->getPower();
                anyOn = true;
            }
        }
        if (!anyOn)
            cout << "  No devices are currently ON." << endl;
        cout << "  ----------------------------------" << endl;
        cout << "  Total Active Power: " << total << "W" << endl;
        cout << "  ----------------------------------" << endl;
    }

    ~HomeController() {
        for (int i = 0; i < (int)commandHistory.size(); i++)
            delete commandHistory[i];
    }
};

HomeController* HomeController::instance = NULL;

// ============================================================
// DISPLAY FUNCTIONS
// ============================================================
void printLine() {
    cout << "  ----------------------------------------" << endl;
}

void printMainMenu() {
    cout << "\n  ========================================" << endl;
    cout << "       SMART HOME AUTOMATION SYSTEM       " << endl;
    cout << "  ========================================" << endl;
    cout << "  1. View All Device Status" << endl;
    cout << "  2. Control a Device Manually" << endl;
    cout << "  3. Simulate Auto Event" << endl;
    cout << "  4. Undo Last Command" << endl;
    cout << "  5. Power Consumption Report" << endl;
    cout << "  6. Exit" << endl;
    cout << "  ========================================" << endl;
    cout << "  Enter your choice: ";
}

void printDeviceMenu() {
    cout << "\n  -- Select Device --" << endl;
    cout << "  1. Living Room Light" << endl;
    cout << "  2. Bedroom Light" << endl;
    cout << "  3. Bedroom Fan" << endl;
    cout << "  4. Main AC" << endl;
    cout << "  5. Front Camera" << endl;
    cout << "  6. Front Lock" << endl;
    cout << "  Enter device number: ";
}

void printControlMenu(const string& deviceType) {
    cout << "\n  -- Control Options --" << endl;
    cout << "  1. Turn ON" << endl;
    cout << "  2. Turn OFF" << endl;

    if (deviceType == "Smart Light") {
        cout << "  3. Set Brightness" << endl;
        cout << "  4. Set Color" << endl;
    } else if (deviceType == "Smart Fan") {
        cout << "  3. Set Speed (1-5)" << endl;
    } else if (deviceType == "Air Conditioner") {
        cout << "  3. Set Temperature (16-30C)" << endl;
        cout << "  4. Set Mode (Cool/Heat/Fan)" << endl;
    } else if (deviceType == "Security Camera") {
        cout << "  3. Start Recording" << endl;
        cout << "  4. Stop Recording" << endl;
    } else if (deviceType == "Smart Lock") {
        cout << "  3. Lock" << endl;
        cout << "  4. Unlock (requires code)" << endl;
    }
    cout << "  Enter option: ";
}

void printEventMenu() {
    cout << "\n  -- Simulate Auto Event --" << endl;
    cout << "  1. Motion Detected in Bedroom" << endl;
    cout << "  2. Motion Detected in Living Room" << endl;
    cout << "  3. No Motion in Bedroom" << endl;
    cout << "  4. High Temperature Alert" << endl;
    cout << "  5. Normal Temperature" << endl;
    cout << "  6. Intruder Alert" << endl;
    cout << "  7. Owner Leaving Home" << endl;
    cout << "  8. Daylight Detected" << endl;
    cout << "  Enter event number: ";
}

// ============================================================
// MAIN
// ============================================================
int main() {
    cout << "\n  ========================================" << endl;
    cout << "    Smart Home Automation System" << endl;
    cout << "    Initializing..." << endl;
    cout << "  ========================================" << endl;

    HomeController* home = HomeController::getInstance();

    // Create devices
    SmartLight*     livingLight  = new SmartLight    ("Living Room Light", "Living Room");
    SmartLight*     bedroomLight = new SmartLight    ("Bedroom Light",     "Bedroom"    );
    SmartFan*       bedroomFan   = new SmartFan      ("Bedroom Fan",       "Bedroom"    );
    AirConditioner* ac           = new AirConditioner("Main AC",           "Living Room");
    SecurityCamera* frontCam     = new SecurityCamera("Front Camera",      "Entrance"   );
    SmartLock*      frontLock    = new SmartLock     ("Front Lock",        "Entrance"   );

    // Register all devices
    cout << "\n  -- Registering Devices --" << endl;
    home->addDevice(livingLight);
    home->addDevice(bedroomLight);
    home->addDevice(bedroomFan);
    home->addDevice(ac);
    home->addDevice(frontCam);
    home->addDevice(frontLock);

    // Activate lock by default
    frontLock->turnOn();
    frontCam->turnOn();

    cout << "\n  System Ready! All devices registered." << endl;

    // ── MAIN MENU LOOP ──
    int choice = 0;
    while (choice != 6) {
        printMainMenu();
        cin >> choice;

        // ── 1. VIEW STATUS ──
        if (choice == 1) {
            home->displayStatus();
        }

        // ── 2. MANUAL DEVICE CONTROL ──
        else if (choice == 2) {
            printDeviceMenu();
            int devChoice;
            cin >> devChoice;

            Device* selected = home->getDevice(devChoice - 1);
            if (selected == NULL) {
                cout << "  Invalid device!" << endl;
                continue;
            }

            cout << "\n  Selected: " << selected->getName()
                 << " [" << selected->getType() << "]" << endl;
            cout << "  Current Status: " << selected->getStatus() << endl;

            printControlMenu(selected->getType());
            int ctrlChoice;
            cin >> ctrlChoice;

            if (ctrlChoice == 1) {
                home->executeCommand(new TurnOnCommand(selected));
            }
            else if (ctrlChoice == 2) {
                home->executeCommand(new TurnOffCommand(selected));
            }
            else if (ctrlChoice == 3) {
                // Light -> brightness
                if (selected->getType() == "Smart Light") {
                    cout << "  Enter brightness (0-100): ";
                    int b; cin >> b;
                    SmartLight* l = (SmartLight*)selected;
                    l->setBrightness(b);
                }
                // Fan -> speed
                else if (selected->getType() == "Smart Fan") {
                    cout << "  Enter speed (1-5): ";
                    int s; cin >> s;
                    SmartFan* f = (SmartFan*)selected;
                    f->setSpeed(s);
                }
                // AC -> temperature
                else if (selected->getType() == "Air Conditioner") {
                    cout << "  Enter temperature (16-30): ";
                    int t; cin >> t;
                    AirConditioner* a = (AirConditioner*)selected;
                    a->setTemperature(t);
                }
                // Camera -> start recording
                else if (selected->getType() == "Security Camera") {
                    SecurityCamera* c = (SecurityCamera*)selected;
                    c->startRecording();
                }
                // Lock -> lock
                else if (selected->getType() == "Smart Lock") {
                    SmartLock* lk = (SmartLock*)selected;
                    lk->lock();
                }
            }
            else if (ctrlChoice == 4) {
                // Light -> color
                if (selected->getType() == "Smart Light") {
                    cout << "  Enter color (White/Warm White/Blue/Red): ";
                    string c; cin >> c;
                    SmartLight* l = (SmartLight*)selected;
                    l->setColor(c);
                }
                // AC -> mode
                else if (selected->getType() == "Air Conditioner") {
                    cout << "  Enter mode (Cool/Heat/Fan): ";
                    string m; cin >> m;
                    AirConditioner* a = (AirConditioner*)selected;
                    a->setMode(m);
                }
                // Camera -> stop recording
                else if (selected->getType() == "Security Camera") {
                    SecurityCamera* c = (SecurityCamera*)selected;
                    c->stopRecording();
                }
                // Lock -> unlock
                else if (selected->getType() == "Smart Lock") {
                    cout << "  Enter access code: ";
                    string code; cin >> code;
                    SmartLock* lk = (SmartLock*)selected;
                    lk->unlock(code);
                }
            }
            else {
                cout << "  Invalid option!" << endl;
            }
        }

        // ── 3. SIMULATE EVENT ──
        else if (choice == 3) {
            printEventMenu();
            int evChoice;
            cin >> evChoice;

            if      (evChoice == 1) home->broadcastEvent("MOTION_DETECTED", "Bedroom");
            else if (evChoice == 2) home->broadcastEvent("MOTION_DETECTED", "Living Room");
            else if (evChoice == 3) home->broadcastEvent("NO_MOTION",       "Bedroom");
            else if (evChoice == 4) home->broadcastEvent("HIGH_TEMP");
            else if (evChoice == 5) home->broadcastEvent("NORMAL_TEMP");
            else if (evChoice == 6) home->broadcastEvent("INTRUDER_ALERT");
            else if (evChoice == 7) home->broadcastEvent("LEAVING_HOME");
            else if (evChoice == 8) home->broadcastEvent("DAYLIGHT");
            else cout << "  Invalid event!" << endl;
        }

        // ── 4. UNDO ──
        else if (choice == 4) {
            home->undoLastCommand();
        }

        // ── 5. POWER REPORT ──
        else if (choice == 5) {
            home->powerReport();
        }

        // ── 6. EXIT ──
        else if (choice == 6) {
            cout << "\n  Shutting down Smart Home System..." << endl;
            cout << "  Goodbye!" << endl;
        }

        else {
            cout << "  Invalid choice! Please try again." << endl;
        }
    }

    // Cleanup
    delete livingLight;
    delete bedroomLight;
    delete bedroomFan;
    delete ac;
    delete frontCam;
    delete frontLock;

    return 0;
}
