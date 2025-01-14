// Approche explicite ou implicite ?

// Template de file d'attente -> pour les oiseaux
//                            -> pour leurs vecteurs

#include <vector>
#include <iostream>
#include <cmath>
#include <functional>


// for initializing and shutdown functions
#include <SDL2/SDL.h>

// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

using namespace std;

class vec2 {

private:
    vector<float> data;

public:
    float x() const { return data[0]; }
    float y() const { return data[1]; }

    vec2() {
        data[0] = 0;
        data[1] = 0;
    }
    vec2(int* v) {
        data[0] = float(v[0]);
        data[1] = float(v[1]);
    }

    vec2(float* v) {
        data[0] = v[0];
        data[1] = v[1];
    }

    vec2(vec2 const& u) { ///////////////// A REVOIR
        data[0] = u.x();
        data[1] = u.y();
    }

    vec2& operator=(vec2 u) {
        data[0] = u.x();
        data[1] = u.y();
        return (*this);
    }

    vec2 operator+(vec2 w) {
        float v[3] = { this->x() + w.x(), this->y() + w.y() };
        return vec2(v);
    }

    vec2 operator-(vec2 w) {
        float v[3] = { this->x() - w.x(), this->y() - w.y() };
        return vec2(v);
    }

    vec2 operator*(float lbd) {
        float v[3] = { this->x() * lbd, this->y() * lbd };
        return vec2(v);
    }

    vec2 operator+=(vec2 w) {
        return ((*this) + w);
    }

    vec2 operator/(float lbd) {
        float v[3] = { this->x() / lbd, this->y() / lbd };
        return vec2(v);
    }

    float sq_norm() { return (pow(this->x(), 2) + pow(this->y(), 2)); }
    float norm() { return sqrt(this->sq_norm()); }
    float norm(vec2 vec) {
        return vec.norm();
    }

    float distance(vec2& other) {
        vec2 t = (*this) - other;
        return t.norm();
    }

    float dot(vec2 w) { return (this->x() * w.x() + this->y() * w.y()); }
};

vec2 operator*(float lbd, vec2& a)
{
    return a * lbd;
};

vec2 operator/(float lbd, vec2& a)
{
    return a / lbd;
};

using point2 = vec2;



template<typename T>

class Maillon {
private:
    bool empty;
    T content;
    Maillon* pt;
public:
    Maillon() {
        content = T();
        pt = 0;
        empty = true;
    }

    Maillon(T c, Maillon* next = 0) {
        content = c;
        pt = next;
        empty = false;
    }

    Maillon operator=(Maillon m) {
        content = m.content;
        pt = m.pt;
        empty = m.empty;
        return *this;
    }

    bool is_empty() {
        return empty;
    }

    T get_content() {
        return content;
    };

    Maillon* get_next() {
        return pt;
    }

    void set_content(int n) {
        content = n;
    }

    void set_next(Maillon* loc) {
        pt = loc;
    }

    void set_isempty(bool p) {
        empty = p;
    }
};

template<typename T>
class ChainList
{
private:
    Maillon<T>* head;
    int size;

public:
    ChainList() {
        head = new Maillon<T>();
        size = 0;
    }

    ChainList(const ChainList& base) {
        head = new Maillon<T>();
        Maillon<T>* newcurs = head;
        Maillon<T>* current = base.head;
        while (!(current->is_empty())) {
            Maillon<T>* next = new Maillon<T>();
            *newcurs = Maillon<T>(current->get_content(), next);
            newcurs = newcurs->get_next();
            current = current->get_next();
        }
        size = base.size;
    }

    ~ChainList() {
        Maillon<T>* nxt = head;
        while (!(nxt->is_empty())) {
            Maillon<T>* tmp = nxt;
            nxt = (nxt->get_next());
            delete tmp;
        }
    }


    ChainList operator=(ChainList obj) {
        this -> ~ChainList();

        head = new Maillon<T>();
        Maillon<T>* newcurs = head;
        Maillon<T>* current = obj.head;

        while (!(current->is_empty())) {
            Maillon<T>* next = new Maillon<T>();
            *newcurs = Maillon<T>(current->get_content(), next);
            newcurs = newcurs->get_next();
            current = current->get_next();
        }
        size = obj.size;
        return *this;
    }

    ChainList operator+(ChainList obj) { //Concaténation.
        ChainList left = *this;
        ChainList right = obj;
        Maillon<T>* bottom = left.head;
        while (!(bottom->is_empty())) {
            bottom = (bottom->get_next());
        }
        *bottom = *right.head;
        left.size += right.size;
        return left;
    }

    template<typename F>
    void iter(F func) {
        Maillon<T>* current = head;
        while (!(current->is_empty())) {
            func(current->get_content());
            current = current->get_next();
        }
    }

    template<typename F>
    void map_float(ChainList<float>& new_chain, F func) {
        Maillon<T>* current = head;
        while (!(current->is_empty())) {
            new_chain.push(func(current->get_content()));
            current = current->get_next();
        }
    } // Ne conserve pas l'ordre, mais ça ne sera jamais un problème.

    template<typename F>
    void map_vec(ChainList<vec2*>& new_chain, F func) {
        Maillon<T>* current = this->head;
        while (!(current->is_empty())) {
            vec2* v = new vec2(func(current->get_content()));
            new_chain.push(v);
            current = current->get_next();
        }
    }

    void push(T n) {
        head = new Maillon<T>(n, head); //On fait pointer vers une nouvelle instance dans laquelle l'ancienne est imbriquée.
        size += 1;
    }

    T sum() {};

    int get_size() {
        return size;
    }

    T pop() {
        Maillon<T>* nxt = (*head).get_next();
        if (!(nxt->is_empty())) {
            T m = (head->get_content());
            head = (head->get_next());
            return m;
            size--;
        }
        else {
            cout << "Stack is empty" << endl;
        }

    }

    void print() {
        Maillon<T>* nxt = head;
        while (!(nxt->is_empty())) {
            int m = (nxt->get_content());
            nxt = (nxt->get_next());
            cout << m << " -> ";
        }
        cout << "//" << endl;
    }
};


using VecList = ChainList<vec2*>;
using PointList = ChainList<point2*>; // Inutile sauf pour la clarté
using FloatList = ChainList<float>;

template<>
vec2* VecList::sum() {
    vec2* acc = new vec2(); // Vecteur nul
    auto add_to = [&](vec2* e) {
        *acc += *e;
        };
    this->iter(add_to);
    return acc;
}

VecList operator+(VecList l, vec2 v) {
    auto sum = [&](vec2* e) {
        return *e + v;
        };
    VecList* res = new VecList;
    res->map_vec(l, sum);
    return *res;
}

VecList operator+(vec2 v, VecList l) {
    return l + v;
}

VecList operator-(VecList l, vec2 v) {
    auto sum = [&](vec2* e) {
        return *e - v;
        };
    VecList* res = new VecList;
    res->map_vec(l, sum);
    return *res;
}

VecList operator-(vec2 v, VecList l) {
    auto sum = [&](vec2* e) {
        return v - *e;
        };
    VecList* res = new VecList;
    res->map_vec(l, sum);
    return *res;
}

template<>
float FloatList::sum() {
    float acc; // Vecteur nul
    auto add_to = [&](float e) {
        acc += e;
        };
    this->iter(add_to);
    return acc;
}

class boids
{
private:
    point2 pos;
    vec2 speed;
    vec2 accel; //Vecteurs nuls.

public:
    boids() {
    }
    boids(point2 init_pos) {
        pos = init_pos;
    };

    friend float get_distance(boids* alice, vec2 bob) {
        return (alice->pos).distance(bob);
    }

    VecList* get_closest(PointList& pos_l, float threshold) {
        VecList* closest = new VecList;
        auto push_into = [&](point2* v) {
            float d = get_distance(this, *v);
            if (d > 0 && d < threshold) { closest->push(v); };
            };
        pos_l.iter(push_into);
        return closest;
    }

    vec2 cohesion(PointList& pos_l, VecList& speed_l) {
        vec2 v(*(pos_l.sum()) / pos_l.get_size() - this->pos);
        return vec2(0.01f * v);
    }

    vec2 separation(PointList& pos_l, VecList& speed_l, float threshold) {
        vec2 accum; int N = 0;

        auto add_diff = [&](vec2* v) {
            vec2 delta = *v - pos;
            accum += delta / delta.sq_norm();
            N += 1;
            };

        VecList* closest = (this->get_closest(pos_l, threshold));
        closest->iter(add_diff);
        vec2 a_n = N > 0 ? accum / N : accum;
        return 0.5 * a_n;
    }

    vec2 alignement(PointList& pos_l, VecList& speed_l) {
        vec2 accum; int N = 0;
        auto add_speed = [&](vec2* speed) {
            accum += *speed;
            N += 1;
            };
        speed_l.iter(add_speed);
        return 0.2f * accum / N;
    }

    void update(PointList& pos_l, VecList& speed_l, float dt) {
        vec2 c = cohesion(pos_l, speed_l);
        vec2 s = separation(pos_l, speed_l, 0.1f);
        vec2 a = alignement(pos_l, speed_l);
        accel = c + s + a;
        speed += accel * dt;
        pos += speed * dt;
    }
};



int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Erreur SDL_Init: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Fenêtre SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Erreur SDL_CreateWindow: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Delay(3000); // Garde la fenêtre ouverte 3 secondes

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

}
