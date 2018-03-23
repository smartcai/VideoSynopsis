#ifndef CPP_TRACKING_H
#define CPP_TRACKING_H


#include "util/BoundingBox.h"

#include <ostream>

struct Tracking {
    Tracking(int label, int ID, const BoundingBox &bb);

    Tracking(const Tracking &rhs);

    Tracking(Tracking &&rhs);

    // Prevent assignment
    Tracking &operator=(const Tracking &rhs) = delete;

    Tracking &operator=(Tracking &&rhs) = delete;

    const int label;
    const int ID;
    const BoundingBox bb;
};

std::ostream &operator<<(std::ostream &os, const Tracking &t);


#endif //CPP_TRACKING_H