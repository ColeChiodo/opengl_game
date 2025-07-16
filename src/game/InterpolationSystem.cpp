#include "InterpolationSystem.h"
#include <glm/glm.hpp>
#include <algorithm>

void InterpolationSystem::Process(Scene& scene, float deltaTime, int localPeerID) {
    auto view = scene.registry.view<TransformComponent, InterpolationComponent, NetworkedComponent>();

    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& interpolation = view.get<InterpolationComponent>(entity);
        auto& networked = view.get<NetworkedComponent>(entity);

        if (networked.peerID == localPeerID)
            continue;

        if (interpolation.duration <= 0.0f) {
            transform.translation = interpolation.targetPos;
            transform.rotation = interpolation.targetRot;
            continue;
        }

        interpolation.elapsedTime = std::min(interpolation.elapsedTime + deltaTime, interpolation.duration);

        float t = interpolation.elapsedTime / interpolation.duration;

        if (t >= 1.0f) {
            transform.translation = interpolation.targetPos;
            transform.rotation = interpolation.targetRot;
        } else {
            transform.translation = glm::mix(interpolation.previousPos, interpolation.targetPos, t);
            transform.rotation   = glm::mix(interpolation.previousRot, interpolation.targetRot, t);
        }
    }
}