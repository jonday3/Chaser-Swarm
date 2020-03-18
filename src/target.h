#ifndef __TARGET_AGENT__H
#define __TARGET_AGENT__H 

#include "enviro.h"

using namespace enviro;

class TargetController : public Process, public AgentInterface {

    public:
    TargetController() : Process(), AgentInterface() {}

    void init() {
        watch("screen_click", [this](Event e) {
            if(e.value()["x"] > -850 && e.value()["x"] < 850 && e.value()["y"] > -430 && e.value()["y"] < 430) {
                teleport(e.value()["x"], e.value()["y"], 0); //Teleport Target to screen clicked location. Don't click out of the grey box. 
                std::cout << "Target Moved \n"; 
            }
        });

        auto pos = position();
        x = pos.x;
        y = pos.y;
        emit(Event("j", { 
                { "x", x }, 
                { "y", y} 
            }));
        desired_heading = 0;
        counter = 0;
        watch("button_click", [this](Event e) { // Pause button used to pause the Chaser bots. This ensures no more chasers can be added.
           if(!pause) {
                pause = true;
                std::cout << "Paused! No more agents can be added. \n";    
           } else {
               pause = false;
           }
           
        });
    }

    void start() {}
    void update(){
        damp_movement();
        if(sensor_value(0) < 70 ) {
            if((sensor_value(0) < 70)) {
                if(counter == 0) {
                    desired_heading += M_PI / 2;
                    track_velocity(10, 300); 
                    counter++;
                } else {
                    track_velocity(10, 300); 
                    counter = 0;
                }
                
            } else if ((sensor_value(1) < 70)) {
                track_velocity(10, 300);
            } else if((sensor_value(2) < 70)){
                track_velocity(10, 300);
            }
            
        } else {
            track_velocity(30,300*sin(desired_heading-angle()) - 100*angular_velocity());
        }
        auto pos = position();
        x = pos.x;
        y = pos.y;
        emit(Event("tracker", { 
                { "x", x }, 
                { "y", y} 
        }));
        watch("keydown", [this](Event e) {
            if(!keydown && !pause) {
                if(e.value()["shiftKey"]){
                    if(agent_count < 1) {
                        Agent& v = add_agent("Chaser", 0, 0, 0, {{"fill", "blue"},{"stroke", "black"}});
                        emit(Event("newagent")); 
                        agent_count++;
                    }
                }
                keydown = true;
            }
            agent_count = 0;
        });
        keydown = false;
    }
    void stop() {}
    
    int id() {return 1;}

    int counter, agent_count;
    double desired_heading, rate;
    Event tracker();
    double x, y;
    bool keydown = false;
    bool pause = false;
    Event newagent(); 
    private:
    
};

class Target : public Agent {
    public:
    Target(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    TargetController c;
};

DECLARE_INTERFACE(Target)

#endif