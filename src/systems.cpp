#include "systems.h"

namespace System
{
	glm::vec3 generateRandom()
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		return glm::vec3 (r, g, b);
	}

	void debug(float deltaTime, GameObject *gameObject)
	{
		auto transform = gameObject->getComponent<Transform>();
		
		if(gameObject->hasComponents((long)ComponentType::LIGHT))
		{
			auto light = gameObject->getComponent<Light>();
			
			float increment = 5.f * deltaTime;
			glm::vec3 translation(0.f);
				
			if(Input::isPressed(Input::Key::I))
				translation.z -= increment;
			if(Input::isPressed(Input::Key::K))
				translation.z += increment;
			if(Input::isPressed(Input::Key::J))
				translation.x -= increment;
			if(Input::isPressed(Input::Key::L))
				translation.x += increment;
			if(Input::isPressed(Input::Key::U))
				translation.y += increment;
			if(Input::isPressed(Input::Key::O))
				translation.y -= increment;

			if(translation.x != 0 || translation.y != 0 || translation.z != 0)
				transform->translate(translation, Transform::Space::LOCAL);

			if(Input::isPressed(Input::Key::M))
				transform->rotate(glm::vec3(1, 0, 0), 5 * deltaTime);
			if(Input::isPressed(Input::Key::N))
				transform->rotate(glm::vec3(1, 0, 0), -5 * deltaTime);

			if(Input::isReleased(Input::Key::G))
				light->setShadowMapCount(1);
			if(Input::isReleased(Input::Key::H))
				light->setShadowMapCount(0);

			if(Input::isReleased(Input::Key::K1))
				light->setFov(90);
			if(Input::isReleased(Input::Key::K2))
				light->setFov(180);
			if(Input::isReleased(Input::Key::K3))
				light->setFov(270);
			if(Input::isReleased(Input::Key::K4))
				light->setFov(360);

			if(Input::isReleased(Input::Key::V))
			{
				if(gameObject->hasComponents((long)ComponentType::MODEL))
					gameObject->removeComponent("Model");
			}

			if(Input::isReleased(Input::Key::C))
		    {
				auto suzanne = SceneManager::find(12);

				if(suzanne)
				{
					auto light =suzanne->addComponent<Light>(suzanne->getNode(),
						                                     "suzanneLight");
					if(light)
					{
						light->setColor(glm::vec3(1, 0, 0));
						light->setShadowMapCount(0);
					}
				}
			}

			if(Input::isReleased(Input::Key::SPACE))
				transform->setLookAt(glm::vec3(0));

			if(Input::isReleased(Input::Key::Z))
			{
				Resource mat = Renderer::Resources::get(ResourceType::MATERIAL,
														"pipelines/globalSettings.material.xml");
				glm::vec4 ambientLight(0.12f, 0.12f, 0.16f, 1.0f);
				bool ret = Renderer::Resources::setUniform(mat,
														   "ambientLight",
														   ambientLight);

				if(ret)
					Log::message("changed!");
				else
					Log::message("fail");
			}
		}

		if(gameObject->hasComponents((long)ComponentType::CAMERA))
		{
			auto camera = gameObject->getComponent<Camera>();
			
			if(Input::isReleased(Input::Key::K5))
				camera->setPipeline(Pipeline::FORWARD);
			if(Input::isReleased(Input::Key::K6))
				camera->setPipeline(Pipeline::DEFERRED);
			if(Input::isReleased(Input::Key::K7))
				camera->setPipeline(Pipeline::HDR);

			if(Input::isReleased(Input::Key::ENTER))
			{
				GOPtr newLight = SceneManager::createGameObject("newLight");
				auto light = newLight->addComponent<Light>(newLight->getNode(),
														   "newLight");
				light->setColor(generateRandom());
				light->setShadowCaster(false);
				auto lightTransform = newLight->getComponent<Transform>();
				lightTransform->setPosition(transform->getPosition());
				lightTransform->setForward(transform->getForward());
			}
		}
	}
	
	void update(float deltaTime, GameObject *gameObject)
	{
		System::CameraSystem::updateFreeCamera(deltaTime, gameObject);

		debug(deltaTime, gameObject);
		
		auto transform = gameObject->getComponent<Transform>();
		
		if(transform->needsSync())
		{
			Renderer::setNodeTransform(gameObject->getNode(),
									   transform->getModelMatrix());
			transform->setSynced();
		}
	}
}
