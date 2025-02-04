#ifndef vec2_H
#define vec2_H

#include <vector>
#include <cmath>
#include <math.h>

using namespace std;

class vec2 {

private:
    vector<double> data;

public:
    double x() const { return data[0]; }
    double y() const { return data[1]; }

    vec2() {
        data.push_back(0);
        data.push_back(0);
    }
    vec2(int* v) {
        data.push_back(double(v[0]));
        data.push_back(double(v[1]));
    }

    vec2(double f1, double f2) {
        data.push_back(f1);
        data.push_back(f2);
    }

    vec2(double* v) {
        data.push_back(v[0]);
        data.push_back(v[1]);
    }

    vec2(vec2 const& u) { ///////////////// A REVOIR
        data.push_back(u.x());
        data.push_back(u.y());
    }

    vec2(std::pair<double,double> v) {
        data.push_back(v.first);
        data.push_back(v.second);
    }

    vec2& operator=(vec2 u) {
        data[0] = u.x();
        data[1] = u.y();
        return (*this);
    }

    vec2 operator+(vec2 w) const {
        double v[2] = { this->x() + w.x(), this->y() + w.y() };
        return vec2(v);
    }

    vec2 operator-(vec2 w) const {
        double v[2] = { this->x() - w.x(), this->y() - w.y() };
        return vec2(v);
    }

	vec2 operator-() const{
		double v[2] = { -this->x(), -this->y() };
		return vec2(v);
	}

    vec2 operator*(double lbd) {
        double v[2] = { this->x() * lbd, this->y() * lbd };
        return vec2(v);
    }

    vec2 operator+=(vec2 w) {
        *this = (*this) + w;
        return ((*this) + w);
    }

    vec2 operator-=(vec2 w) {
		return this->operator+=(-w);
    }

    vec2 operator/(double lbd) {
        double v[3] = { this->x() / lbd, this->y() / lbd };
        return vec2(v);
    }

    double sq_norm() const { return (pow(this->x(), 2) + pow(this->y(), 2)); }
    double norm() const { return sqrt(this->sq_norm()); }
    double norm(vec2 vec) const {
        return vec.norm();
    }

    double distance(vec2& other) const {
        vec2 t = (*this) - other;
        return t.norm();
    }

    double dot(vec2 w) const { return (this->x() * w.x() + this->y() * w.y()); }
    
    double operator| (vec2 w) {
        return this->dot(w);
    }

    double argument() const{
        if (this->x() == 0 && this->y() == 0) {
			return 0; // Convention
        }
        else {
            return atan2(this->y(), this->x()) * 180 / 3.14; // Module du complexe x+iy (avec les formules de l'arc moitié)
        };
    }

    vec2 normalized() {
        return *this / (this->norm());
    }

    vec2 rotate(double th) {
        // Reviens à multiplier le complexe x+iy par exp(i.th)
        // (x+iy)*exp(i.th) = (x.cos(th)-y.sin(th)) + i (x.sin(th) + y.cos(th))
    }
};

vec2 operator*(double lbd, vec2& a)
{
    return a * lbd;
};

vec2 operator/(double lbd, vec2& a)
{
    return a / lbd;
};

vec2 rnd_sphere_sample() {
    double th = 2 * 3.14 * double((rand() % 360)) / 360;

    return *(new vec2(cos(th), sin(th)));
}

using point2 = vec2;

#endif