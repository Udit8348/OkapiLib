/**
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "okapi/control/iterative/iterativeVelPidController.hpp"
#include "api.h"
#include <algorithm>
#include <cmath>

namespace okapi {

IterativeVelPIDControllerArgs::IterativeVelPIDControllerArgs(const double ikP, const double ikD)
  : kP(ikP), kD(ikD) {
}

IterativeVelPIDControllerArgs::IterativeVelPIDControllerArgs(const double ikP, const double ikD,
                                                             const VelMathArgs &iparams)
  : kP(ikP), kD(ikD), params(iparams) {
}

IterativeVelPIDController::IterativeVelPIDController(const double ikP, const double ikD)
  : IterativeVelPIDController(ikP, ikD, std::make_unique<VelMath>(1800), std::make_unique<Timer>(),
                              std::make_unique<SettledUtil>()) {
}

IterativeVelPIDController::IterativeVelPIDController(const double ikP, const double ikD,
                                                     const VelMathArgs &iparams)
  : IterativeVelPIDController(ikP, ikD, std::make_unique<VelMath>(iparams),
                              std::make_unique<Timer>(), std::make_unique<SettledUtil>()) {
}

IterativeVelPIDController::IterativeVelPIDController(const IterativeVelPIDControllerArgs &iparams)
  : IterativeVelPIDController(iparams.kP, iparams.kD, std::make_unique<VelMath>(iparams.params),
                              std::make_unique<Timer>(), std::make_unique<SettledUtil>()) {
}

// std::make_unique<Timer>(), std::make_unique<SettledUtil>()

IterativeVelPIDController::IterativeVelPIDController(const double ikP, const double ikD,
                                                     std::unique_ptr<VelMath> ivelMath,
                                                     std::unique_ptr<Timer> iloopDtTimer,
                                                     std::unique_ptr<SettledUtil> isettledUtil)
  : velMath(std::move(ivelMath)),
    loopDtTimer(std::move(iloopDtTimer)),
    settledUtil(std::move(isettledUtil)) {
  setGains(ikP, ikD);
}

void IterativeVelPIDController::setGains(const double ikP, const double ikD) {
  kP = ikP;
  kD = ikD * static_cast<double>(sampleTime) / 1000.0;
}

void IterativeVelPIDController::setSampleTime(const std::uint32_t isampleTime) {
  if (isampleTime > 0) {
    kD /= static_cast<double>(isampleTime) / static_cast<double>(sampleTime);
    sampleTime = isampleTime;
  }
}

void IterativeVelPIDController::setOutputLimits(double imax, double imin) {
  // Always use larger value as max
  if (imin > imax) {
    const double temp = imax;
    imax = imin;
    imin = temp;
  }

  outputMax = imax;
  outputMin = imin;

  output = std::clamp(output, outputMin, outputMax);
}

double IterativeVelPIDController::stepVel(const double inewReading) {
  return velMath->step(inewReading);
}

double IterativeVelPIDController::step(const double inewReading) {
  if (isOn) {
    loopDtTimer->placeHardMark();

    if (loopDtTimer->getDtFromHardMark() >= sampleTime) {
      stepVel(inewReading);
      error = target - velMath->getVelocity();

      // Derivative over measurement to eliminate derivative kick on setpoint change
      derivative = velMath->getAccel();

      output += kP * error - kD * derivative;
      output = std::clamp(output, outputMin, outputMax);

      lastError = error;
      loopDtTimer->clearHardMark(); // Important that we only clear if dt >= sampleTime

      settledUtil->isSettled(error);
    }

    return output;
  }

  return 0; // Can't set output to zero because the entire loop in an integral
}

void IterativeVelPIDController::setTarget(const double itarget) {
  target = itarget;
}

double IterativeVelPIDController::getOutput() const {
  return isOn ? output : 0;
}

double IterativeVelPIDController::getError() const {
  return error;
}

double IterativeVelPIDController::getDerivative() const {
  return derivative;
}

bool IterativeVelPIDController::isSettled() {
  return settledUtil->isSettled(error);
}

void IterativeVelPIDController::reset() {
  error = 0;
  lastError = 0;
  output = 0;
}

void IterativeVelPIDController::flipDisable() {
  isOn = !isOn;
}

void IterativeVelPIDController::flipDisable(const bool iisDisabled) {
  isOn = !iisDisabled;
}

bool IterativeVelPIDController::isDisabled() const {
  return !isOn;
}

void IterativeVelPIDController::setTicksPerRev(const double tpr) {
  velMath->setTicksPerRev(tpr);
}

double IterativeVelPIDController::getVel() const {
  return velMath->getVelocity();
}

std::uint32_t IterativeVelPIDController::getSampleTime() const {
  return sampleTime;
}
} // namespace okapi