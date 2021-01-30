#include "geo/boundary/aabb.hpp"
#include "geo/boundary/sphere.hpp"

namespace tz::geo
{
    BoundaryAABB::BoundaryAABB(tz::Vec3 min, tz::Vec3 max): min(min), max(max)
    {
        this->ensure();
    }

    tz::Vec3 BoundaryAABB::get_minimum() const
    {
        return this->min;
    }

    tz::Vec3 BoundaryAABB::get_maximum() const
    {
        return this->max;
    }

    void BoundaryAABB::set_minimum(tz::Vec3 min)
    {
        this->min = min;
        this->ensure();
    }

    void BoundaryAABB::set_maximum(tz::Vec3 max)
    {
        this->max = max;
        this->ensure();
    }

    bool BoundaryAABB::collides(const tz::Vec3& point) const
    {
        auto leq_all = [](const tz::Vec3& a, const tz::Vec3& b)->bool
        {
            return a[0] <= b[0]
                && a[1] <= b[1]
                && a[2] <= b[2];
        };
        return leq_all(this->min, point) && leq_all(point, this->max);
    }

    bool BoundaryAABB::collides(const BoundaryAABB& box) const
    {
        auto box_collides = [](const BoundaryAABB& a, const BoundaryAABB& b)->bool
        {
            return (a.min[0] <= b.max[0] && a.max[0] >= b.min[0])
                && (a.min[1] <= b.max[1] && a.max[1] >= b.min[1])
                && (a.min[2] <= b.max[2] && a.max[2] >= b.min[2]);
        };
        return box_collides(*this, box);
    }

    bool BoundaryAABB::collides(const BoundarySphere& sphere) const
    {
        return sphere.collides(*this);
    }

    void BoundaryAABB::ensure()
    {
        if(this->min.length() > this->max.length())
        {
            std::swap(this->min, this->max);
        }
    }
}