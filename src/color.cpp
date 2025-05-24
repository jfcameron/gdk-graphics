// © Joseph Cameron - All Rights Reserved

#include <gdk/color.h>

#include <algorithm>
#include <iostream>
#include <sstream>

using namespace gdk;
const color color::black       (0.0f,0.0f,0.0f,1.0f);
const color color::blue        (0.0f,0.0f,1.0f,1.0f);
const color color::dark_green  (0.0f,0.6f,0.0f,1.0f);
const color color::deathly_pink(1.0f,0.2f,0.8f,1.0f);
const color color::green       (0.0f,1.0f,0.0f,1.0f);
const color color::red         (1.0f,0.0f,0.0f,1.0f);
const color color::white       (1.0f,1.0f,1.0f,1.0f);

const color color::cornflower_blue(
    0.3921568627450980392156862745098,
    0.58431372549019607843137254901961,
    0.92941176470588235294117647058824,
    1.);

static constexpr char TAG[] = "color";

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

std::ostream &gdk::operator<<(std::ostream &s, const color &a) {
    std::stringstream ss;

    ss << "{r:" << a.r << ", g:" << a.g << ", b:" << a.b << ", a:" << a.a << "}";

    s << ss.str();

    return s; 
}

bool color::operator==(const color &that) const
{
    return
        r == that.r &&
        g == that.g &&
        b == that.b &&
        a == that.a;
}

bool color::operator!=(const color &that) const 
{
    return !(*this == that);
}

color::color(const channel_type aR, const channel_type aG, const channel_type aB, const channel_type aA)
: r(aR)
, g(aG)
, b(aB)
, a(aA)
{}

