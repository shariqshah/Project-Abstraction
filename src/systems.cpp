#include "systems.h"
#include "scenemanager.h"
#include "componentmanager.h"
#include "scriptengine.h"
#include "input.h"
#include "camerasystems.h"
#include "physics.h"
#include "gameobject.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "renderer.h"
#include "camera.h"

namespace System
{
	namespace
	{
		static bool    sPhysicsEnabled      = true;
		static Sphere* tmpShape             = NULL;
		static CModel* suzanneModel         = NULL;
		static CollisionShape* statCollMesh = NULL;
		static CollisionShape* hullCollMesh = NULL;
	}
	
	void initialize()
	{
		sPhysicsEnabled = true;
		Physics::initialize(Vec3(0.f, -9.8f, 0.f));
		CompManager::initialize();
		suzanneModel = Renderer::Model::create("models/test/test.scene.xml");
		statCollMesh = new CollisionMesh(*suzanneModel, true);
		hullCollMesh = new CollisionMesh(*suzanneModel, false);
		tmpShape = new Sphere(1.f);

		ScriptEngine::initialize();

		Log::generateBindings();
		Input::generateBindings();
		Transform::generateBindings();
		Renderer::Model::generateBindings();
		Renderer::Light::generateBindings();
		Renderer::Camera::generateBindings();
		GO::generateBindings();
		CompManager::generateBindings();
		Physics::generateBindings();
		Physics::RigidBody::generateBindings();
		SceneManager::generateBindings();

		//ScriptEngine::createScript("../content/scripts/scriptManager.nut");
		ScriptEngine::runScript("../content/scripts/scriptManager.nut");
		ScriptEngine::runScript("../content/scripts/init.nut");
	}
	
	Vec3 generateRandom()
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		return Vec3 (r, g, b);
	}

	void debug(float deltaTime, GameObject *gameObject)
	{
		auto transform = CompManager::getTransform(gameObject);
		
	// 	if(gameObject->hasComponents((long)Component::LIGHT))
	// 	{
	// 		auto light = gameObject->getComponent<Light>();
			
	// 		float increment = 10.f * deltaTime;
	// 		Vec3 translation(0.f);
			
	// 		if(Input::isReleased(Input::Key::P) && gameObject->compareTag("child"))
	// 			transform->setUpVector(Vec3(0, 1, 0));

	// 		if(Input::isReleased(Input::Key::F) && gameObject->compareTag("child"))
	// 			transform->setLookAt(Vec3(0));
	// 			// transform->rotate(Vec3(0, 1, 0), increment * 5);

	// 		if(Input::isReleased(Input::Key::V) && gameObject->compareTag("child"))
	// 			SceneManager::setParentAsRoot(gameObject);
			
	// 		if(Input::isPressed(Input::Key::M))
	// 			transform->rotate(Vec3(1, 0, 0),  increment * 5);
	// 		if(Input::isPressed(Input::Key::N))
	// 			transform->rotate(Vec3(1, 0, 0), -increment * 5);

	// 		if(Input::isReleased(Input::Key::K1))
	// 			light->setFov(90);
	// 		if(Input::isReleased(Input::Key::K2))
	// 			light->setFov(1802);
	// 		if(Input::isReleased(Input::Key::K3))
	// 			light->setFov(270);
	// 		if(Input::isReleased(Input::Key::K4))
	// 			light->setFov(360);

	// 		if(light->isShadowCaster())
	// 		{
	// 			if(Input::isReleased(Input::Key::NP_PLUS))
	// 				light->setShadowMapCount(light->getShadowMapCount() + 1);
	// 			if(Input::isReleased(Input::Key::NP_MINUS))
	// 				light->setShadowMapCount(light->getShadowMapCount() - 1);
	// 			Renderer::addText(std::to_string(light->getShadowMapCount()));
	// 		}
	// 	}

		if(GO::hasComponent(gameObject, Component::CAMERA))
		{
			auto camera = CompManager::getCamera(gameObject);
			
			if(Input::isReleased(Input::Key::K5))
				Renderer::Camera::setPipeline(camera, Pipeline::FORWARD);
			if(Input::isReleased(Input::Key::K6))
				Renderer::Camera::setPipeline(camera, Pipeline::DEFERRED);
			if(Input::isReleased(Input::Key::K7))
				Renderer::Camera::setPipeline(camera, Pipeline::HDR);

			if(Input::isReleased(Input::Key::ENTER))
			{
				GOPtr newLight = SceneManager::create("newLight");
				newLight->tag = "child";
				// auto light = newLight->addComponent<Light>(newLight->getNode(),
				// 										   "newLight");
				CompManager::addModel(newLight, "models/test/test.scene.xml");
				// light->setColor(generateRandom());
				// light->setShadowCaster(false);
				auto lightTransform = CompManager::getTransform(newLight);
				
				// auto falcon = SceneManager::find("Falcon");
				// bool success = SceneManager::setParent(newLight.get(), falcon.get());
				Transform::setPosition(lightTransform, transform->position);
				Transform::setForward(lightTransform, transform->forward);
				//lightTransform->translate(Vec3(0, 20, 0));
				// success ? Log::message("success") : Log::message("fail");

				CRigidBody rigidBody = -1;
				if(Input::isPressed(Input::Key::LCTRL))
				{
					rigidBody = CompManager::addRigidBody(newLight,
														  statCollMesh,
														  0.f);
				}
				else if(Input::isPressed(Input::Key::LSHIFT))
				{
					rigidBody = CompManager::addRigidBody(newLight,
														  hullCollMesh);
				}
				else
				{
					rigidBody = CompManager::addRigidBody(newLight,
														  tmpShape);
				}

				Physics::RigidBody::applyForce(rigidBody,
											   transform->forward * 2000.f);
					
			}
		}

	// 	if(gameObject->compareTag("Falcon"))
	// 	{
	// 		Vec3 translation(0.f);
	// 		float increment = 10.f * deltaTime;
				
	// 		if(Input::isPressed(Input::Key::I))
	// 			translation.z -= increment;
	// 		if(Input::isPressed(Input::Key::K))
	// 			translation.z += increment;
	// 		if(Input::isPressed(Input::Key::J))
	// 			translation.x -= increment;
	// 		if(Input::isPressed(Input::Key::L))
	// 			translation.x += increment;
	// 		if(Input::isPressed(Input::Key::U))
	// 			translation.y += increment;
	// 		if(Input::isPressed(Input::Key::O))
	// 			translation.y -= increment;
			

	// 		if(translation.x != 0 || translation.y != 0 || translation.z != 0)
	// 			transform->translate(translation, Transform::Space::LOCAL);

	// 		auto rBody = gameObject->getComponent<RigidBody>();
			
	// 		if(Input::isReleased(Input::Key::G))
	// 			rBody->setKinematic(true);
	// 		if(Input::isReleased(Input::Key::H))
	// 			rBody->setKinematic(false);
	// 	}
	}

	void syncPhysicsTransform(GameObject* gameObject)
	{
		// check if the gameobject's transform has been modified by
		// someone other than bullet. if so, then update rigidbody's
		// transform and force it to activate.
		if(GO::hasComponent(gameObject, Component::RIGIDBODY))
		{
			auto transform  = CompManager::getTransform(gameObject);
			auto rBody      = CompManager::getRigidBody(gameObject);

			if(Renderer::isTransformed(gameObject->node))
			{
				Mat4 transformMat = transform->transMat;
				Physics::RigidBody::setTransform(rBody, transformMat);
				Physics::RigidBody::setActivation(rBody, true);
			}
		}
	}
	
	void update(float deltaTime, GameObject* gameObject)
	{	
		debug(deltaTime, gameObject);
		syncPhysicsTransform(gameObject); // Should be the last thing so as to
		                                  // do to let other systems modify the
		                                  // transform then sync Physics.
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
			Physics::enableDebugDraw(true);
		if(Input::isReleased(Input::Key::F7))
			Physics::enableDebugDraw(false);
		if(Input::isReleased(Input::Key::F8))
			Physics::nextDebugMode();
		if(Input::isReleased(Input::Key::F9))
			Log::setEnabled(true);
		if(Input::isReleased(Input::Key::F10))
			Log::setEnabled(false);
		
		System::CameraSystem::updateFreeCamera(deltaTime);
		ScriptEngine::executeFunction("updateObjects", deltaTime);
		
		GOMap* sceneObjects = SceneManager::getSceneObjects();
		for(GOMap::iterator it = sceneObjects->begin();
			it != sceneObjects->end();
			it++)
		{
			update(deltaTime, it->second);
		}

		if(Input::isReleased(Input::Key::C))
			sPhysicsEnabled ? sPhysicsEnabled = false : sPhysicsEnabled = true;

		if(sPhysicsEnabled)
			Physics::update(deltaTime);
		
		SceneManager::update();

		if(Input::isReleased(Input::Key::K9))
		{
			// auto test = SceneManager::find("TestOBJ");
			// GO::reloadScript(test, "Test");
			GO::reloadScript("TestOBJ", "Test");
		}
	}

	void cleanup()
	{
		SceneManager::cleanup();
		CompManager::cleanup();
		Physics::cleanup();
		Renderer::Model::remove(*suzanneModel);
		delete suzanneModel;
		ScriptEngine::cleanup();
	}
}
