#ifndef ChainList_H
#define ChainList_H

#include "vec2.h"

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

    T operator[](int n) {
        int i = 0;
        Maillon<T>* current = head;
        while (!(current->is_empty()) && i++ < n) {
            current = current->get_next();
        }
        if (i <= n) {
            cout << "Index out of range" << endl;
        }
        else {
            return current->get_content();
        };
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
    void map_double(ChainList<double>& new_chain, F func) {
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
using doubleList = ChainList<double>;

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
double doubleList::sum() {
    double acc = 0.f; // Vecteur nul
    auto add_to = [&](double e) {
        acc += e;
        };
    this->iter(add_to);
    return acc;
}

#endif