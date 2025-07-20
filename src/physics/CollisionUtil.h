#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>

struct OBB {
    glm::vec3 center;
    glm::vec3 halfSize;
    glm::mat3 orientation;
};

struct CollisionManifold {
    bool collided = false;
    glm::vec3 normal = glm::vec3(0.0f);
    float depth = 0.0f;
};

inline OBB CreateOBB(const TransformComponent& transform, const BoxColliderComponent& collider) {
    glm::vec3 center = transform.translation + collider.offset;
    glm::vec3 halfSize = collider.size;
    glm::quat quaternion = glm::quat(glm::radians(transform.rotation));
    glm::mat3 orientation = glm::mat3_cast(quaternion);
    return { center, halfSize, orientation };
}

inline CollisionManifold SAT3DTest(const OBB& a, const OBB& b) {
    const float EPSILON = 0.0001f;
    CollisionManifold result;
    glm::mat3 R, AbsR;

    glm::vec3 smallestAxis;
    float minOverlap = FLT_MAX;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R[i][j] = glm::dot(a.orientation[i], b.orientation[j]);

    glm::vec3 t = b.center - a.center;
    t = glm::vec3(glm::dot(t, a.orientation[0]),
                  glm::dot(t, a.orientation[1]),
                  glm::dot(t, a.orientation[2]));

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            AbsR[i][j] = std::abs(R[i][j]) + EPSILON;

    auto testAxis = [&](glm::vec3 axis, float ra, float rb, float dist) {
        float overlap = ra + rb - std::abs(dist);
        if (overlap < 0.0f) {
            result.collided = false;
            return false;
        }
        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis * ((dist < 0) ? -1.0f : 1.0f);
        }
        return true;
    };

    for (int i = 0; i < 3; i++) {
        float ra = a.halfSize[i];
        float rb = b.halfSize[0] * AbsR[i][0] + b.halfSize[1] * AbsR[i][1] + b.halfSize[2] * AbsR[i][2];
        if (!testAxis(a.orientation[i], ra, rb, t[i])) return result;
    }

    for (int i = 0; i < 3; i++) {
        float ra = a.halfSize[0] * AbsR[0][i] + a.halfSize[1] * AbsR[1][i] + a.halfSize[2] * AbsR[2][i];
        float rb = b.halfSize[i];
        float dist = glm::dot(t, glm::vec3(R[0][i], R[1][i], R[2][i]));
        if (!testAxis(b.orientation[i], ra, rb, dist)) return result;
    }

    result.collided = true;
    result.normal = glm::normalize(smallestAxis);
    result.depth = minOverlap;
    return result;
}

inline bool RayIntersectsOBB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const OBB& box, float& tHit) {
    const float EPSILON = 0.0001f;

    glm::vec3 p = box.center - rayOrigin;
    tHit = 0.0f;
    float tMax = FLT_MAX;

    for (int i = 0; i < 3; ++i) {
        glm::vec3 axis = box.orientation[i];
        float e = glm::dot(axis, p);
        float f = glm::dot(axis, rayDir);

        if (std::abs(f) > EPSILON) {
            float t1 = (e + box.halfSize[i]) / f;
            float t2 = (e - box.halfSize[i]) / f;

            if (t1 > t2) std::swap(t1, t2);
            tHit = std::max(tHit, t1);
            tMax = std::min(tMax, t2);

            if (tHit > tMax) return false;
        } else {
            // Ray is parallel to this axis; check for overlap
            if (-e - box.halfSize[i] > 0.0f || -e + box.halfSize[i] < 0.0f)
                return false;
        }
    }

    return true;
}

struct HitResult {
    entt::entity entity;
    std::string tag;
    float tHit;
};