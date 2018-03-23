#include "tracker/PAOT.h"

#include "tracker/Affinity.h"
#include "tracker/predictor/kalman/KalmanPredictor.h"
#include "tracker/predictor/particle/ParticlePredictor.h"

#include <dlib/optimization.h>

// Methods

std::vector<Tracking> PAOT::track(const std::vector<Detection> &detections) {
    frameCount++;

    // Filter detections on confidence
    std::vector<Detection> strongDetections;
    for (const auto &detection : detections) {
        if (detection.confidence > detectionThreshold) {
            strongDetections.push_back(detection);
        }
    }

    Association association = associateDetectionsToPredictors(strongDetections, predictors,
                                                              Affinity::expCost, affinityThreshold);

    // Update matched predictors with assigned detections

    for (const auto &match : association.matching) {
        predictors.at(match.second)->update(strongDetections.at(match.first));
    }

    for (const auto p : association.unmatchedPredictors) {
        predictors.at(p)->update();
    }

    // Create and initialise new predictors for unmatched detections
    for (const auto id : association.unmatchedDetections) {
        auto predictor = std::make_shared<ParticlePredictor>(strongDetections.at(id), ++trackCount);
        predictors.push_back(predictor);
    }
#if 0
    // Remove predictors that have been inactive for too long
    predictors.erase(std::remove_if(
            predictors.begin(), predictors.end(),
            [this](const std::shared_ptr<Predictor> &predictor) {
            std::cout<<"111111111111"<<std::endl;
                return predictor->getTimeSinceUpdate() > maxAge;
            }), predictors.end());

    // Return trackings from active predictors
#endif
    std::vector<Tracking> trackings;
    for (auto it = predictors.begin(); it != predictors.end(); ++it) {
#if 0
        if ((*it)->getTimeSinceUpdate() < 5 &&
            ((*it)->getHitStreak() >= minHits || frameCount <= minHits)) {
#else
        if (1) {
#endif
            trackings.push_back((*it)->getTracking());
        }
    }
    return trackings;
}

PAOT::Association PAOT::associateDetectionsToPredictors(
        const std::vector<Detection> &detections,
        const std::vector<std::shared_ptr<Predictor>> &predictors,
        double (*affinityMeasure)(const BoundingBox &a, const BoundingBox &b),
        double affinityThreshold) {

    const int DOUBLE_PRECISION = 100;
    std::vector<std::pair<int, int>> matches;
    std::vector<int> unmatchedDetections;
    std::vector<int> unmatchedPredictors;

    if (predictors.empty()) {
        for (int i = 0; i < detections.size(); ++i)
            unmatchedDetections.push_back(i);
        return PAOT::Association{matches, unmatchedDetections, unmatchedPredictors};
    }

    dlib::matrix<int> cost(detections.size(), predictors.size());
    for (size_t row = 0; row < detections.size(); ++row) {
        for (size_t col = 0; col < predictors.size(); ++col) {
            cost(row, col) = int(DOUBLE_PRECISION * affinityMeasure(
                    detections.at(row).bb,
                    predictors.at(col)->getPredictedNextDetection().bb));
        }
    }

    // Zero-pad to make it square
    if (cost.nr() > cost.nc()) { //rows > cols
        cost = dlib::join_rows(cost, dlib::zeros_matrix<int>(1, cost.nr() - cost.nc()));
    } else if (cost.nc() > cost.nr()) {
        cost = dlib::join_cols(cost, dlib::zeros_matrix<int>(cost.nc() - cost.nr(), 1));
    }

    std::vector<long> assignment = dlib::max_cost_assignment(cost);

    // Filter out matches with low IoU, including those for indices from padding
    for (int d = 0; d < assignment.size(); ++d) {
        if (cost(d, assignment[d]) < affinityThreshold * DOUBLE_PRECISION) {
            if (d < detections.size()) {
                unmatchedDetections.push_back(d);
            }
            if (assignment[d] < predictors.size()) {
                unmatchedPredictors.push_back(int(assignment[d]));
            }
        } else {
            matches.push_back(std::pair<int, int>(d, assignment[d]));
        }
    }
    return PAOT::Association{matches, unmatchedDetections, unmatchedPredictors};
}
