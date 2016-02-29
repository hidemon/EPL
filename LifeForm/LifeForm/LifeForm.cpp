/* main test simulator */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h"
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"

using namespace std;


template <typename T>
void bound(T& x, const T& min, const T& max) {
    assert(min < max);
    if (x > max) { x = max; }
    if (x < min) { x = min; }
}



ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
    ObjInfo info;
    
    info.species = neighbor->species_name();
    info.health = neighbor->health();
    info.distance = pos.distance(neighbor->position());
    info.bearing = pos.bearing(neighbor->position());
    info.their_speed = neighbor->speed;
    info.their_course = neighbor->course;
    return info;
}

void LifeForm::compute_next_move(void) { // a simple function that creates the next border_cross_event
    if (!is_alive) return;
    if (border_cross_event != nullptr) border_cross_event -> cancel();
    if (speed > 0) {
        SmartPointer<LifeForm> p {this};
        border_cross_event = new Event(space.distance_to_edge(pos, course)/speed + Point::tolerance,
                                       [p](){ p -> border_cross();});
    }
}

void LifeForm::update_position() {
    double delta = Event::now() - update_time;
    if (!is_alive) return;
    if (delta < 0.001) return;
    update_time = Event::now();
    Point newPos;
    newPos.xpos = pos.xpos + delta*speed*cos(course);
    newPos.ypos = pos.ypos + delta*speed*sin(course);
    energy -= movement_cost(speed, delta);
    if (space.is_out_of_bounds(newPos)) {
        die();
    }
    else if(energy < min_energy) {
        die();
    }else{
        space.update_position(pos, newPos);
        pos = newPos;
    }
}

void LifeForm::set_course(double course) {
    if (!is_alive) return;
    if (this->course == course)
        return;
    if (border_cross_event != nullptr)
        border_cross_event -> cancel();
    update_position();
    this->course = course;
    if(speed != 0)
        compute_next_move();
}

void LifeForm::set_speed(double speed) {
    if (!is_alive) return;
    if (this->speed == speed)
        return;
    if (border_cross_event != nullptr)
        border_cross_event -> cancel();
    update_position();
    this->speed = speed;
    if(speed != 0)
        compute_next_move();
}

ObjList LifeForm::perceive(double distance) {
    if (!is_alive) return vector<ObjInfo>{};
    if (distance > max_perceive_range) distance = max_perceive_range;
    if (distance < min_perceive_range) distance = min_perceive_range;
    vector<ObjInfo> res{};
    energy -= perceive_cost(distance);
    if (energy < min_energy) {
        die();
        return res;
    }
    vector<SmartPointer<LifeForm>> ObjList = space.nearby(pos, distance);
    for (auto i : ObjList) {
        res.push_back(info_about_them(i));
    }
    return res;
}

void LifeForm::age() {
    if (!is_alive) return;
    energy -= age_penalty;
    if (energy < min_energy) {
        die();
        return;
    }
    SmartPointer<LifeForm> p(this);
    new Event(age_frequency, [p]() { p->age(); });
}

void LifeForm::eat(SmartPointer<LifeForm> other) {
    if (!is_alive || !other -> is_alive) return;
    energy -= eat_cost_function();
    if (energy < min_energy) {
        die();
        return;
    }
    SmartPointer<LifeForm> p{this};
    double gain = other -> energy * eat_efficiency;
    new Event(digestion_time, [p, gain](){ p -> gain_energy(gain); });
    other->die();
}

void LifeForm::gain_energy(double gain) {
    if(is_alive)
        energy += gain;
}

void LifeForm::check_encounter() {
    if (!is_alive) return;
    SmartPointer<LifeForm> other = space.closest(pos);
    if (!other -> is_alive) return;
    if (pos.distance(other -> pos) < encounter_distance) {
        this->resolve_encounter(other);
    }
}

void LifeForm::border_cross() {
    update_position();
    check_encounter();
    compute_next_move();
}

void LifeForm::region_resize() {
    update_position();
    compute_next_move();
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> other) {
    if (!is_alive || !other -> is_alive) return;
    if ((energy -= encounter_penalty) < min_energy) {
        die();
    }
    if ((other -> energy -= encounter_penalty) < min_energy) {
        die();
    }
    if (!is_alive || !other -> is_alive) return;
    
    Action a1 = encounter(info_about_them(other));
    SmartPointer<LifeForm> p {this};
    Action a2 = other -> encounter(other -> info_about_them(p));
    if (a1 == LIFEFORM_IGNORE && a2 == LIFEFORM_IGNORE) {
        return;
    } else if (a1 == LIFEFORM_EAT && a2 == LIFEFORM_IGNORE) {
        if (drand48() < eat_success_chance(energy, other -> energy)) {
            eat(other);
        }
    } else if (a1 == LIFEFORM_IGNORE && a2 == LIFEFORM_EAT) {
        if (drand48() < eat_success_chance(other -> energy, energy)) {
            other -> eat(p);
        }
    } else if (a1 == LIFEFORM_EAT && a2 == LIFEFORM_EAT) {
        double chanceMe = drand48();
        double chanceOther = drand48();
        double successMe = eat_success_chance(energy, other -> energy);
        double successOther = eat_success_chance(other -> energy, energy);
        if (chanceMe < successMe && chanceOther >= successOther)
            eat(other);
        else if (chanceOther < successOther && chanceMe >= successMe)
            other -> eat(p);
        else if (chanceMe >= successOther && chanceOther >= successOther)
            return;
        else {
            switch (encounter_strategy) {
                case EVEN_MONEY:
                    if (drand48() < 0.5)
                        eat(other);
                    else
                        other -> eat(p);
                    break;
                case BIG_GUY_WINS:
                    if (energy >= other -> energy)
                        eat(other);
                    else
                        other -> eat(p);
                    break;
                case UNDERDOG_IS_HERE:
                    if (energy <= other -> energy)
                        eat(other);
                    else
                        other -> eat(p);
                    break;
                case FASTER_GUY_WINS:
                    if (speed >= other -> speed)
                        eat(other);
                    else
                        other -> eat(p);
                    break;
                case SLOWER_GUY_WINS:
                    if (speed <= other -> speed)
                        eat(other);
                    else
                        other -> eat(p);
                    break;
            }
        }
        
    }
    
}

void LifeForm::reproduce(SmartPointer<LifeForm> child){
    double timeInterval = Event::now() - reproduce_time;
    if((!is_alive) || (timeInterval < min_reproduce_time)){
        if(child->border_cross_event != nullptr)
            child->border_cross_event->cancel();
        child->die();
    }else{
        double newEnergy = (this->energy * (1.0 - reproduce_cost)) / 2;
        if(newEnergy < min_energy){
            if(child->border_cross_event != nullptr)
                child->border_cross_event->cancel();
            child->die();
            if(this->border_cross_event != nullptr)
                this->border_cross_event->cancel();
            this->die();
            return;
        }
        this->energy = newEnergy;
        child->energy = newEnergy;
        SmartPointer<LifeForm> nearest;
        bool placeFinded = false;
        int i = 0;
        while((i < 20) && (placeFinded == false)){
            child->pos.ypos = this->pos.ypos + sin(drand48() * 2.0 * M_PI) * drand48() * reproduce_dist;
            child->pos.xpos = this->pos.ypos + cos(drand48() * 2.0 * M_PI) * drand48() * reproduce_dist;
            nearest = space.closest(child->pos);
            if(nearest && nearest->position().distance(child->position()) > encounter_distance
               && !space.is_out_of_bounds(child -> position()))
                placeFinded = true;
            i++;
        }
        child->start_point = child->pos;
        child->is_alive = true;
        cout << "I'm here!!" << endl;
        space.insert(child, child->pos, [child](void) { child->region_resize(); });
        cout << "Finish insertion" << endl;
        new Event(age_frequency, [child](void) { child->age(); });
        if(child->speed != 0 && child->is_alive)
            child->compute_next_move();
        cout << "Add border_cross event!" << endl;
        this->reproduce_time = Event::now();
        if(placeFinded == false){
            cout << "Can't find safety place" << endl;
            child->resolve_encounter(nearest);
        }
        
    }
}



