#include "data/vector.hpp"

#ifndef TOPAZ_PHYSICS_OBJECT_HPP
#define TOPAZ_PHYSICS_OBJECT_HPP

class PhysicsObject
{
public:
    PhysicsObject(float mass, Vector3F velocity = {}, float moment_of_inertia = 1.0f, Vector3F angular_velocity = {}, std::initializer_list<Vector3F> forces = {}, std::initializer_list<Vector3F> torques = {});
    /**
     * Get the net-force acting on this PhysicsObject.
     * @return - The net-force, in kilograms world-units per second squared (kg w s^(-2)).
     */
    Vector3F net_force() const;
    /**
     * Get the net-torque acting on this PhysicsObject.
     * @return - The net-torque, in newton-metres.
     */
    Vector3F net_torque() const;
    /**
     * Get the acceleration of this PhysicsObject.
     * @return - The acceleration, in  world-units per second squared (w s^(-2)).
     */
    Vector3F get_acceleration() const;
    /**
     * Get the rotational acceleration of this PhysicsObject.
     * @return - The rotational acceleration, in radians per second squared.
     */
    Vector3F get_angular_acceleration() const;
    /**
     * Updates the velocity with the delta
     */
    virtual void update(float delta_time);

    void add_force(Vector3F force);
    void add_torque(Vector3F torque);

    /// Mass, in kilograms.
    float mass;
    /// Velocity, in world-units per second.
    Vector3F velocity;
    /// Moment of inertia, in kilogram metre squared.
    float moment_of_inertia;
    /// Angular-velocity, in radians per second.
    Vector3F angular_velocity;
protected:
    /// Container of all the forces acting upon the PhysicsObject.
    std::vector<Vector3F> forces;
    /// Container of all the torques (rotational forces) acting upon the PhysicsObject.
    std::vector<Vector3F> torques;
};


#endif //TOPAZ_PHYSICS_OBJECT_HPP
