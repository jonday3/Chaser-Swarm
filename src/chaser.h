#ifndef __CHASER_AGENT__H
#define __CHASER_AGENT__H 

#include "enviro.h"

using namespace enviro;

/*! Chaser Controller Class
* This class is the main class used to control the behavior of the Chaser Agent 
* All implementations are in this header file. No .cc used.
*/
class ChaserController : public Process, public AgentInterface {

    public:
    ChaserController() : Process(), AgentInterface() {}

    void init() {
        set_client_id("chaser"); // Set string ID used for interaction with other agents.
        watch("tracker", [this](Event e) { // Event emitted from the Target Agent to find the location in the World to move to.
            goal_x = e.value()["x"];
            goal_y = e.value()["y"];
        });
        watch("button_click", [this](Event e) { // Pause button used to pause the Chaser bots. Method copied into Target to prevent more agents to be added.
           if(!pause) {
                auto pos = position();
                pause_x = pos.x;
                pause_y = pos.y;
                pause = true;
                std::cout << "Paused! No more agents can be added. \n";
           } else {
               pause = false;
           }
        });
    }
    void start() {}
    void update() {
        damp_movement();
        if(pause) {
            move_toward(pause_x, pause_y, 10, 0);
        } else {
            if((sensor_value(0) < 15 || sensor_value(1) < 15 || sensor_value(2) < 15) || 
            ((sensor_reflection_type(0) != "Chaser" && sensor_reflection_type(1) != "Chaser" && sensor_reflection_type(2) != "Chaser") &&
             (sensor_value(0) < 20 || sensor_value(1) < 20 || sensor_value(2) < 20))) {
                track_velocity(40*cos(goal_x-angle()) - 80*angular_velocity(),30*cos(goal_y-angle()) - 80*angular_velocity());
            } else {
                move_toward(goal_x, goal_y, 500, 300);
            }
            notice_collisions_with("Target", [this](Event &e) {
                std::cout << "Too close! Agent Removed. Be careful not to add too many agents. There is a chance to crash\n"; 
                remove_agent(this->agent->get_id());
            });
            
        }

    }
    void stop() {}
    double goal_x, goal_y, pause_x, pause_y;
    int agent_count;
    bool pause = false;
    bool keydown = false;
    Event newagent();
};

class Chaser : public Agent {
    public:
    Chaser(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    ChaserController c;
};

DECLARE_INTERFACE(Chaser)

#endif