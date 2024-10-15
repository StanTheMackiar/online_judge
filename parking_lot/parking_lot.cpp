#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <numeric>

const int MAX_POSITION = 20;

struct Params {
    std::vector<int> waitingPositions;
    std::vector<int> vacatedPositions;
};

struct CarPosition {
    int original;
    int current;
};

struct Result {
    int initial;
    int final;
};

int getDistance(int vacatedPosition, int carPosition) {
    int difference = vacatedPosition - carPosition;
    bool isBehind = vacatedPosition < carPosition;

    int distance = isBehind ? (MAX_POSITION - carPosition) + vacatedPosition : difference;

    return distance;
}

bool hasDuplicates(const std::vector<int>& array) {
    std::set<int> uniqueElements(array.begin(), array.end());
    return uniqueElements.size() != array.size();
}

bool validatePositions(const std::vector<int>& positions) {
    bool containInvalidValues = std::any_of(positions.begin(), positions.end(), [](int pos) {
        return pos <= 0 || pos > MAX_POSITION;
    });

    bool containDuplicates = hasDuplicates(positions);

    return containInvalidValues || containDuplicates;
}

void parkingLotAlgorithm(const Params& params) {
    const auto& waitingPositions = params.waitingPositions;
    const auto& vacatedPositions = params.vacatedPositions;

    if (validatePositions(waitingPositions) || validatePositions(vacatedPositions)) {
        throw std::invalid_argument("Invalid input");
    }

    std::vector<CarPosition> waitingCarPositions;
    std::vector<Result> results;

    for (int pos : waitingPositions) {
        waitingCarPositions.push_back({pos, pos});
        results.push_back({pos, -1});
    }

    for (int vacatedPosition : vacatedPositions) {
        auto closestCar = std::accumulate(waitingCarPositions.begin(), waitingCarPositions.end(), 
            CarPosition{waitingCarPositions[0].original, waitingCarPositions[0].current},
            [vacatedPosition](CarPosition nearest, CarPosition carPosition) {
                int newDistance = getDistance(vacatedPosition, carPosition.current);
                int nearestDistance = getDistance(vacatedPosition, nearest.current);

                if (newDistance < nearestDistance) {
                    return carPosition;
                }
                return nearest;
            });

        waitingCarPositions.erase(
            std::remove_if(waitingCarPositions.begin(), waitingCarPositions.end(),
                           [closestCar](CarPosition car) { return car.current == closestCar.current; }),
            waitingCarPositions.end());

        for (auto& car : waitingCarPositions) {
            int sum = car.current + getDistance(vacatedPosition, closestCar.current);
            car.current = sum > MAX_POSITION ? sum - MAX_POSITION : sum;
        }

        auto resultIt = std::find_if(results.begin(), results.end(), 
                                     [closestCar](const Result& r) { return r.initial == closestCar.original; });

        if (resultIt != results.end()) {
            resultIt->final = vacatedPosition;
        }
    }

    for (const auto& result : results) {
        if (result.final != -1) {
            std::cout << "Original position " << result.initial << " parked in " << result.final << std::endl;
        } else {
            std::cout << "Original position " << result.initial << " did not park" << std::endl;
        }
    }
}

int main() {
    Params params = {
        {6, 19, 17, 13, 1},
        {1, 3, 20, 16}
    };

    try {
        parkingLotAlgorithm(params);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
