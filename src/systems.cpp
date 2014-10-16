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
			
			float increment = 10.f * deltaTime;
			glm::vec3 translation(0.f);
			
			if(Input::isReleased(Input::Key::P) && gameObject->compareTag("child"))
				transform->setUpVector(glm::vec3(0, 1, 0));

			if(Input::isReleased(Input::Key::F) && gameObject->compareTag("child"))
				transform->setLookAt(glm::vec3(0));
				// transform->rotate(glm::vec3(0, 1, 0), increment * 5);

			if(Input::isReleased(Input::Key::V) && gameObject->compareTag("child"))
				SceneManager::setParentAsRoot(gameObject);
			
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
				transform->rotate(glm::vec3(1, 0, 0),  increment * 5);
			if(Input::isPressed(Input::Key::N))
				transform->rotate(glm::vec3(1, 0, 0), -increment * 5);

			if(Input::isReleased(Input::Key::K1))
				light->setFov(90);
			if(Input::isReleased(Input::Key::K2))
				light->setFov(1802);
			if(Input::isReleased(Input::Key::K3))
				light->setFov(270);
			if(Input::isReleased(Input::Key::K4))
				light->setFov(360);

			if(light->isShadowCaster())
			{
				if(Input::isReleased(Input::Key::NP_PLUS))
					light->setShadowMapCount(light->getShadowMapCount() + 1);
				if(Input::isReleased(Input::Key::NP_MINUS))
					light->setShadowMapCount(light->getShadowMapCount() - 1);
				Renderer::addText(std::to_string(light->getShadowMapCount()));
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
			if(Input::isReleased(Input::Key::Z))
				camera->setOcclusionCulling(true);
			if(Input::isReleased(Input::Key::X))
				camera->setOcclusionCulling(false);

			if(Input::isReleased(Input::Key::ENTER))
			{
				GOPtr newLight = SceneManager::createGameObject("newLight");
				newLight->setTag("child");
				auto light = newLight->addComponent<Light>(newLight->getNode(),
														   "newLight");
				newLight->addComponent<Model>(newLight->getNode(),
											  "models/test/test.scene.xml");
				light->setColor(generateRandom());
				light->setShadowCaster(false);
				auto lightTransform = newLight->getComponent<Transform>();
				//lightTransform->setPosition(transform->getPosition());
				//lightTransform->setForward(transform->getForward());

				// auto falcon = SceneManager::find("Falcon");
				// bool success = SceneManager::setParent(newLight.get(), falcon.get());
				// lightTransform->setPosition(glm::vec3(0.f));
				lightTransform->translate(glm::vec3(0, 20, 0));
				// success ? Log::message("success") : Log::message("fail");

				newLight->addComponent<RigidBody>(1.f, 1.f,
												  lightTransform->getPosition(),
												  lightTransform->getRotation());
			}
		}
	}
	
	void update(float deltaTime, GameObject *gameObject)
	{
		System::CameraSystem::updateFreeCamera(deltaTime, gameObject);
		
		debug(deltaTime, gameObject);

		if(gameObject->hasComponents((long)ComponentType::RIGIDBODY))
		{
			auto transform  = gameObject->getComponent<Transform>();
			auto rBody      = gameObject->getComponent<RigidBody>();

			glm::vec3 position;
			glm::quat rotation;

			rBody->getTransformation(&position, &rotation);

			transform->setPosition(position);
			transform->setRotation(rotation);
		}
		
	}

	void update(float deltaTime)
	{
		if(Input::isReleased(Input::Key::F1))
			Renderer::setDebugLevel(DebugLevel::NONE);
		if(Input::isReleased(Input::Key::F2))
			Renderer::setDebugLevel(DebugLevel::MEDIUM);
		if(Input::isReleased(Input::Key::F3))
			Renderer::setDebugLevel(DebugLevel::HIGH);
		if(Input::isReleased(Input::Key::F4))
			Renderer::toggleWireframe();
		if(Input::isReleased(Input::Key::F5))
			Renderer::toggleDebugView();
		
		GOMap* sceneObjects = SceneManager::getSceneObjects();
		for(GOMap::iterator it = sceneObjects->begin();
			it != sceneObjects->end();
			it++)
		{
			update(deltaTime, it->second.get());
		}

		if(Input::isReleased(Input::Key::C))
		{
			SceneManager::remove("Falcon");
			// auto parent = SceneManager::getParent(newLight.get());

			// auto pT = parent->getComponent<Transform>();
			// pT->rotate(glm::vec3(0, 1, 0), 10 * deltaTime);
		}
			

		SceneManager::update();
	}
}
