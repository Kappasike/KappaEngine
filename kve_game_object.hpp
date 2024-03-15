#pragma once

#include "kve_model.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

namespace kve {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{1.f, 1.f, 1.f};
		glm::vec3 rotation{};

		// Matrix translate * Ry * Rx * Rz * scale transformation
		// Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class KveGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, KveGameObject>;

		static KveGameObject createGameObject() {
			static id_t currentId = 0;
			return KveGameObject{ currentId++ };
		}

		KveGameObject(const KveGameObject&) = delete;
		KveGameObject& operator=(const KveGameObject&) = delete;
		KveGameObject(KveGameObject&&) = default;
		KveGameObject& operator=(KveGameObject&&) = default;

		id_t const getId() { return id; }

		std::shared_ptr<KveModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		KveGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}