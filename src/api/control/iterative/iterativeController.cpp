/**
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "okapi/api/control/iterative/iterativeController.hpp"

namespace okapi {
IterativeControllerArgs::~IterativeControllerArgs() = default;

void IterativeController::setSampleTime(const QTime) {
}

void IterativeController::setOutputLimits(double, double) {
}

QTime IterativeController::getSampleTime() const {
  return 10_ms;
}
} // namespace okapi