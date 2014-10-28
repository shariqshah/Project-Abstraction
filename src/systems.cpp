#include "systems.h"

namespace System
{
	namespace
	{
		static bool sPhysicsEnabled;
		static Sphere *tmpShape = new Sphere(1.f);
		static std::shared_ptr<Model> suzanneModel;
		static CollisionShape* statCollMesh;
		static CollisionShape* hullCollMesh;
	}
	
	void initialize()
	{
		sPhysicsEnabled = true;
		Physics::initialize(glm::vec3(0.f, -9.8f, 0.f));
		suzanneModel = std::make_shared<Model>("models/test/test.scene.xml");
		statCollMesh = new CollisionMesh(suzanneModel, true);
		hullCollMesh = new CollisionMesh(suzanneModel, false);
	}
	
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
			
			if(gameObject->compareTag("suzanne"))
			{
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
			}
			
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
				// auto light = newLight->addComponent<Light>(newLight->getNode(),
				// 										   "newLight");
				auto model = newLight->addComponent<Model>(newLight->getNode(),
														   "models/test/test.scene.xml");
				// light->setColor(generateRandom());
				// light->setShadowCaster(false);
				auto lightTransform = newLight->getComponent<Transform>();
				
				// auto falcon = SceneManager::find("Falcon");
				// bool success = SceneManager::setParent(newLight.get(), falcon.get());
				lightTransform->setPosition(transform->getPosition());
				lightTransform->setForward(transform->getForward());
				//lightTransform->translate(glm::vec3(0, 20, 0));
				// success ? Log::message("success") : Log::message("fail");
				if(Input::isPressed(Input::Key::LCTRL))
				{
					auto rigidBody = newLight->addComponent<RigidBody>(lightTransform,
																	   statCollMesh,
																	   0.f);
					rigidBody->applyForce(transform->getForward() * 2000.f);
				}
				else if(Input::isPressed(Input::Key::LSHIFT))
				{
					auto rigidBody = newLight->addComponent<RigidBody>(lightTransform,
																	   hullCollMesh);
					rigidBody->applyForce(transform->getForward() * 2000.f);
				}
				else
				{
					auto rigidBody = newLight->addComponent<RigidBody>(lightTransform,
																	   tmpShape);
					rigidBody->applyForce(transform->getForward() * 2000.f);
				}
					
			}
		}

		if(gameObject->compareTag("suzanne"))
		{
			glm::vec3 translation(0.f);
			float increment = 10.f * deltaTime;
				
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

			auto rBody = gameObject->getComponent<RigidBody>();
			
			if(Input::isReleased(Input::Key::G))
				rBody->setKinematic(true);
			if(Input::isReleased(Input::Key::H))
				rBody->setKinematic(false);

		    auto model = gameObject->getComponent<Model>();

			float* verts = model->getVertices();

			if(verts == NULL)
				Log::message("There's a bug to hunt");
		}
	}

	void syncPhysicsTransform(GameObject* gameObject)
	{
		// check if the gameobject's transform has been modified by
		// someone other than bullet. if so, then update rigidbody's
		// transform and force it to activate.
		if(gameObject->hasComponents((long)ComponentType::RIGIDBODY))
		{
			auto transform  = gameObject->getComponent<Transform>();
			auto rBody      = gameObject->getComponent<RigidBody>();

			if(Renderer::isTransformed(gameObject->getNode()))
			{
				glm::mat4 transformMat = transform->getTransformMat();
				rBody->setTransform(transformMat);
				rBody->setActivation(true);
			}
		}
	}
	
	void update(float deltaTime, GameObject* gameObject)
	{
		System::CameraSystem::updateFreeCamera(deltaTime, gameObject);
		
		debug(deltaTime, gameObject);
		syncPhysicsTransform(gameObject);	
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
		if(Input::isReleased(Input::Key::F6))
			Log::setEnabled(true);
		if(Input::isReleased(Input::Key::F7))
			Log::setEnabled(false);
		
		
		GOMap* sceneObjects = SceneManager::getSceneObjects();
		for(GOMap::iterator it = sceneObjects->begin();
			it != sceneObjects->end();
			it++)
		{
			update(deltaTime, it->second.get());
		}

		if(Input::isReleased(Input::Key::C))
			sPhysicsEnabled ? sPhysicsEnabled = false : sPhysicsEnabled = true;

		if(sPhysicsEnabled)
			Physics::update(deltaTime);
		
		SceneManager::update();
	}

	void cleanup()
	{
		Physics::cleanup();
		suzanneModel.reset();
	}
}
