#ifndef boids_H
#define boids_H

#include "Chainlist.h"

class boids
{
private:
    point2 pos;
    vec2 speed;
    vec2 accel; //Vecteurs nuls.
    vec2 wind;
    int behavior;

public:

    using BoidList = ChainList<boids*>; // Rajout local pour pouvoir utiliser ce type dans la def des fonctions membres

    bool is_active = false;

    boids() {
        pos = point2();
        speed = vec2();
        accel = vec2();
        wind = vec2();
        behavior = 0;
    }
    boids(point2 init_pos) {
        pos = init_pos;
        speed = vec2();
        accel = vec2();
        wind = vec2();
        behavior = 0;
    };

    boids(std::pair<double, double> init_pos) {
        pos = point2(init_pos.first, init_pos.second);
        speed = vec2();
        accel = vec2();
        wind = vec2();
        behavior = 0;
    };

    point2 get_pos() {
        return pos;
    }

    vec2 get_speed() {
        return speed;
    }

    int get_behavior() {
        return behavior;
    }

    void toggle_activity() {
        is_active = !is_active;
    }

    void set_wind(vec2 w) {
        wind = w;
    }

    friend double get_distance(boids* alice, vec2 bob) {
        return (alice->pos).distance(bob);
    }

    VecList* get_closest(PointList& pos_l, double threshold) {
        VecList* closest = new VecList;
        auto push_into = [&](point2* v) {
            double d = get_distance(this, *v);
            if (d > 0 && d < threshold) { closest->push(v); };
            };
        pos_l.iter(push_into);
        return closest;
    }

    vec2 cohesion(PointList& pos_l, VecList& speed_l) {
        vec2 a ( *( pos_l.sum() ) / pos_l.get_size() - this->pos);
        return vec2(0.05f * a);
    }

    vec2 separation(PointList& pos_l, VecList& speed_l, double threshold) {
        vec2 accum; int N = 0;

        auto add_diff = [&](vec2* loc) {
            vec2 delta = pos - *loc;
            if (delta.sq_norm() > 0) {
                accum += delta / (delta.norm() * delta.sq_norm()); //inversement proportionnel au carré de la norme de la distance (comme une interaction coulombienne)
                N += 1;
            }
            };

        VecList* closest = (this->get_closest(pos_l, threshold));
        closest->iter(add_diff);
        vec2 a_n = N > 0 ? accum / N : accum;
        return 3*a_n;
    }

    vec2 alignement(PointList& pos_l, VecList& speed_l) {
        vec2 accum; int N = 0;
        auto add_speed = [&](vec2* speed) {
            accum += *speed;
            N += 1;
            };
        speed_l.iter(add_speed);
        return 0.02f * accum / N;
    }

    vec2 wind_effect() {
        double gamma = 1.0f;
        vec2 relative_speed = speed - wind;
        return -gamma * relative_speed;
    }

    boids* check_collision(BoidList& l_boids) {
        boids* candidate = nullptr;
        double lowest_distance = INFINITY;
        auto find_candidate = [&](boids* b) {
            vec2 OM = (this->get_pos() - b->get_pos());
            if (OM.norm() <= 20 && ((this->get_speed()) | OM) < 0) {
                // Deux conditions : une distance suffisament petite et la vitesse qui pointe vers le boid concerné.
                if (OM.norm() < lowest_distance) {
                    // On traite les collisions par ordre de proximité.
                    candidate = b;
                }
            }
        };
        l_boids.iter(find_candidate);
        return candidate;
    }

    vec2 special_behavior() {
        return vec2();
    }

    void update(PointList& pos_l, VecList& speed_l, BoidList& l_boids, double dt) {
        if (is_active)
        {
            boids* candidate = this->check_collision(l_boids);
            if (candidate) {
                // Gestion des collisions
                vec2 OM = (candidate->get_pos() - this->get_pos());
                vec2 n = OM.normalized();
                vec2 t(n.y(), -n.x()); //Rotation de 90°
                double vn = speed | n;
                double vt = speed | t;
                // Et enfin, réflexion comme pour une loi de Descartes, avec une accentuation de la vitesse pour un effet + visuel
                speed = (t * vt - n * vn)*2;
                pos += speed * dt;
            }
            else {
                vec2 c = vec2 (cohesion(pos_l, speed_l));
                vec2 s = vec2 (separation(pos_l, speed_l, 50));
                vec2 a = vec2 (alignement(pos_l, speed_l));
                vec2 v = wind_effect();
                accel = (c + s + a + v) * 10;
                speed += accel * dt;
                pos += speed * dt;
            }
        }
    }
};

using BoidList = ChainList<boids*>;

PointList extract_pos(BoidList l_boids) {
    auto find_pos = [&](boids* e) {
        return e->get_pos();
        };
    VecList* res = new VecList;
    l_boids.map_vec(*res, find_pos);
    return *res;
}

VecList extract_speed(BoidList l_boids) {
    auto find_speed = [&](boids* e) {
        return e->get_speed();
        };
    VecList* res = new VecList;
    l_boids.map_vec(*res, find_speed);
    return *res;
}

boids* check_clicked(vec2& mouse_pos, BoidList& l_boids) {
    boids* candidate = nullptr;
    auto find_candidate = [&](boids* b) {
        vec2 OM = (mouse_pos - b->get_pos());
        //cout << int(mouse_pos.x()) << " " << int(mouse_pos.y()) << "   " << b->get_pos().x() << " " << b->get_pos().y() << endl;
        if (OM.norm() <= 20) {
            candidate = b;
        }
        };
    l_boids.iter(find_candidate);
    return candidate;
}

///////////////////////////////////////////// SOUS-CLASSES DE BOIDS /////////////////////////////////////////////

// Cette partie n'a pas pu être complètement implémentée par manque de temps.

class curious : public boids {
    // Ce boid n'est pas affecté par le vent et change de direction de façon assez aléatoire, comme si il voulait explorer un peu partout.
    // Les autres boids ont tendance par défaut à le suivre, même si cela implique de resister au vent.
    private :
        point2 pos;
        vec2 speed;
        vec2 accel; //Vecteurs nuls.
        vec2 wind;
        int behavior;
    public :
        curious() {
            pos = point2();
            speed = vec2();
            accel = vec2();
            wind = vec2();
            behavior = 1;
        }
        curious(point2 init_pos) {
            pos = init_pos;
            speed = vec2();
            accel = vec2();
            wind = vec2();
            behavior = 1;
        };

        curious(std::pair<double, double> init_pos) {
            cout << "bonjour" << endl;
            pos = point2(init_pos.first, init_pos.second);
            speed = vec2();
            accel = vec2();
            wind = vec2();
            behavior = 1;
        };
        
        void set_wind(vec2 w) {
            this->wind = rnd_sphere_sample() * 20; //Un vent aléatoire
            cout << wind.x() << " " << wind.y() << endl;
        }

};

class predator : public boids {
    // Prend en chasse le boid le plus proche
    // Ce dernier tente de le fuir.
private:
    point2 pos;
    vec2 speed;
    vec2 accel; //Vecteurs nuls.
    vec2 wind;
    int behavior;
public:
    predator() {
        pos = point2();
        speed = vec2();
        accel = vec2();
        wind = vec2();
        behavior = 2;
    }
    predator(point2 init_pos) {
        pos = init_pos;
        speed = vec2();
        accel = vec2();
        wind = vec2();
        behavior = 2;
    };

    predator(std::pair<double, double> init_pos) {
        cout << "bonjour" << endl;
        pos = point2(init_pos.first, init_pos.second);
        speed = vec2();
        accel = vec2();
        wind = vec2();
        behavior = 2;
    };

};


#endif