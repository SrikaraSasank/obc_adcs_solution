
#pragma once
#include <array>
#include <cmath>
#include <stdexcept>
#include <algorithm>

struct Vec3 {
    double x{0}, y{0}, z{0};
    Vec3() = default;
    Vec3(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(double s) const { return {x*s, y*s, z*s}; }
    Vec3 operator/(double s) const { if (s==0) throw std::runtime_error("div0"); return {x/s, y/s, z/s}; }
    Vec3& operator+=(const Vec3& o){ x+=o.x; y+=o.y; z+=o.z; return *this; }
    double norm() const { return std::sqrt(x*x + y*y + z*z); }
    static Vec3 cross(const Vec3& a, const Vec3& b){
        return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
    }
};

struct Quat {
    // Hamilton quaternion [w, x, y, z], unit quaternion for attitude
    double w{1}, x{0}, y{0}, z{0};
    Quat() = default;
    Quat(double W,double X,double Y,double Z):w(W),x(X),y(Y),z(Z){}
    static Quat identity(){ return {1,0,0,0}; }

    double norm() const { return std::sqrt(w*w + x*x + y*y + z*z); }
    void normalize(){
        double n = norm();
        if(n==0) { w=1; x=y=z=0; return; }
        w /= n; x /= n; y /= n; z /= n;
        // Keep scalar part non-negative to avoid double-cover sign flips in control
        if(w < 0){ w=-w; x=-x; y=-y; z=-z; }
    }
    static Quat multiply(const Quat& a, const Quat& b){
        // (a*b) with Hamilton convention
        return {
            a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
            a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
            a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
            a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w
        };
    }
    static Quat conj(const Quat& q){ return {q.w, -q.x, -q.y, -q.z}; }
    static Quat fromAxisAngle(const Vec3& axis, double angle){
        double n = axis.norm();
        if(n==0) return identity();
        double s = std::sin(angle*0.5);
        return {std::cos(angle*0.5), axis.x/n*s, axis.y/n*s, axis.z/n*s};
    }
    Vec3 rotate(const Vec3& v) const {
        // v' = q * [0,v] * conj(q)
        Quat qv{0,v.x,v.y,v.z};
        Quat r = multiply(multiply(*this, qv), conj(*this));
        return {r.x, r.y, r.z};
    }
};

inline Vec3 quatErrorVector(const Quat& q_ref, const Quat& q_meas){
    // e = 2 * vec(q_err), where q_err = q_ref * conj(q_meas)
    Quat qerr = Quat::multiply(q_ref, Quat::conj(q_meas));
    Quat qn = qerr; qn.normalize();
    return {2*qn.x, 2*qn.y, 2*qn.z};
}
