/**
 * @author Kevin Harrington, Common Wealth Robotics
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _OKAPI_PIDTUNER_HPP_
#define _OKAPI_PIDTUNER_HPP_

#include "okapi/util/mathUtil.hpp"

namespace okapi {
class FlywheelSimulator {
  public:
  /**
   * A simulator for a 1 DOF link with changing center of mass. Imagine a single link hanging
   * downwards. The center of mass of the system changes as the link rotates.
   */
  FlywheelSimulator(const double imass = 0.01, const double ilinkLen = 1,
                    const double imuStatic = 0.5, const double imuDynamic = 0.3,
                    const double itimestep = 0.0005);

  virtual ~FlywheelSimulator();

  /**
   * Step the simulation by the timestep.
   *
   * @return the current angle
   */
  double step();

  /**
   * Set the input torque. The input will be bounded by the max torque.
   *
   * @param itorque new input torque
   */
  void setTorque(const double itorque);

  /**
   * Sets the max torque. The input torque cannot exceed this maximum torque.
   *
   * @param imaxTorque new maximum torque
   */
  void setMaxTorque(const double imaxTorque);

  /**
   * Sets the mass (kg).
   *
   * @param imass new mass
   */
  void setMass(const double imass);

  /**
   * Sets the link length (m).
   *
   * @param ilinkLen new link length
   */
  void setLinkLength(const double ilinkLen);

  /**
   * Sets the static friction (N*m).
   *
   * @param imuStatic new static friction
   */
  void setStaticFriction(const double imuStatic);

  /**
   * Sets the dynamic friction (N*m).
   *
   * @param imuDynamic new dynamic friction
   */
  void setDynamicFriction(const double imuDynamic);

  /**
   * Sets the timestep (sec).
   *
   * @param itimestep new timestep
   */
  void setTimestep(const double itimestep);

  /**
   * Returns the current angle (angle in rad).
   *
   * @return the current angle
   */
  double getAngle() const;

  /**
   * Returns the current omgea (angular velocity in rad / sec).
   *
   * @return the current omega
   */
  double getOmega() const;

  /**
   * Returns the current acceleration (angular acceleration in rad / sec^2).
   *
   * @return the current acceleration
   */
  double getAcceleration() const;

  protected:
  double inputTorque = 0; // N*m
  double maxTorque = 20;  // N*m
  double angle = 0;       // rad
  double omega = 0;       // rad / sec
  double accel = 0;       // rad / sec^2
  double mass;            // kg
  double linkLen;         // m
  double muStatic;        // N*m
  double muDynamic;       // N*m
  double timestep;        // sec
  double I = 0;           // moment of inertia

  const double minTimestep = 0.000001; // 1 us
};
}

#endif