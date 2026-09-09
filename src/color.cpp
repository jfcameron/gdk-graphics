// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/color.h>

#include <algorithm>
#include <iostream>
#include <sstream>

using namespace gdk;
using namespace gdk::graphics;
void color::clamp() {
    r = std::clamp<channel_type>(r, 0.f, 1.f);
    g = std::clamp<channel_type>(g, 0.f, 1.f);
    b = std::clamp<channel_type>(b, 0.f, 1.f);
    a = std::clamp<channel_type>(a, 0.f, 1.f);
}

void color::operator+=(const color &aOther) {
    r += aOther.r;
    g += aOther.g;
    b += aOther.b;
    a += aOther.a;
}

std::ostream &gdk::graphics::operator<<(std::ostream &s, const color &a) {
    std::stringstream ss;
    ss << "{r:" << a.r << ", g:" << a.g << ", b:" << a.b << ", a:" << a.a << "}";
    s << ss.str();
    return s; 
}

