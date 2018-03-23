#include "tracker/Affinity.h"

#include <cmath>

double Affinity::euclDist(const BoundingBox &a, const BoundingBox &b) {
    return std::sqrt(std::pow(a.cx - b.cx, 2) + std::pow(a.cy - b.cy, 2));
}

double Affinity::iou(const BoundingBox &a, const BoundingBox &b) {
    double xx1 = std::max(a.x1(), b.x1());
    double yy1 = std::max(a.y1(), b.y1());
    double xx2 = std::min(a.x2(), b.x2());
    double yy2 = std::min(a.y2(), b.y2());
    double width = std::max(xx2 - xx1, 0.);
    double height = std::max(yy2 - yy1, 0.);
    double intersection = width * height;
    double uni = a.area() + b.area() - intersection;
    return intersection / uni;
}

double Affinity::linCost(const BoundingBox &a, const BoundingBox &b) {
    double positionCost = std::sqrt(std::pow(a.cx - b.cx, 2) + std::pow(a.cy - b.cy, 2));
    double shapeCost = std::sqrt(std::pow(a.width - b.width, 2) + std::pow(a.height - b.height, 2));
    return positionCost * shapeCost;
}

double Affinity::expCost(const BoundingBox &a, const BoundingBox &b) {
    // It is expected that a is detection and b is prediction
    double positionWeight = 0.5;
    double shapeWeight = 1.5;
    double positionCost = std::exp(-positionWeight * (
            std::pow((a.cx - b.cx) / a.width, 2) +
            std::pow((a.cy - b.cy) / a.height, 2)));
    double shapeCost = std::exp(-shapeWeight * (
            std::abs(a.width - b.width) / (a.width + b.width) +
            std::abs(a.height - b.height) / (a.height + b.height)));
    return positionCost * shapeCost;
}