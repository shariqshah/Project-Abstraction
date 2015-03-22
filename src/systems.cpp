#include "systems.h"
#include "scenemanager.h"
// #include "componentmanager.h"
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
#include "material.h"
#include "texture.h"
#include "cpu.h"
#include "gui.h"
#include "editor.h"

namespace System
{
	namespace
	{
		bool    sPhysicsEnabled      = true;
		Sphere* tmpShape             = NULL;
		CModel* suzanneModel         = NULL;
		CollisionShape* statCollMesh = NULL;
		CollisionShape* hullCollMesh = NULL;
	}
	
	void initialize()
	{
		// sPhysicsEnabled = true;
		// Physics::initialize(Vec3(0.f, -9.8f, 0.f));
		// CompManager::initialize();
		// Cpu::initialize();
		// statCollMesh = new CollisionMesh(suzanneModel, true);
		// hullCollMesh = new CollisionMesh(suzanneModel, false);
		// tmpShape = new Sphere(1.f);

		ScriptEngine::initialize();

		Log::generateBindings();
		MathTypes::generateBindings();
		Input::generateBindings();
		Transform::generateBindings();
		Material::generateBindings();
		Texture::generateBindings();
		Renderer::Model::generateBindings();
		Renderer::Light::generateBindings();
		Renderer::Camera::generateBindings();
		GO::generateBindings();
		Physics::generateBindings();
		Physics::RigidBody::generateBindings();
		SceneManager::generateBindings();
		Gui::generateBindings();

		ScriptEngine::runScript("../content/scripts/scriptManager.nut");
		ScriptEngine::runScript("../content/scripts/init.nut");

		ScriptEngine::addScript("../content/scripts/helloAngel.as");
		Editor::initialize();
	}
	
	Vec3 generateRandom()
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		return Vec3 (r, g, b);
	}

	void syncPhysicsTransform(GameObject* gameObject)
	{
		// check if the gameobject's transform has been modified by
		// someone other than bullet. if so, then update rigidbody's
		// transform and force it to activate.
		// if(GO::hasComponent(gameObject, Component::RIGIDBODY))
		// {
		// 	// auto transform  = CompManager::getTransform(gameObject);
		// 	auto rBody      = CompManager::getRigidBody(gameObject);

		// 	if(Renderer::isTransformed(gameObject->node))
		// 	{
		// 		Mat4 transformMat = transform->transMat;
		// 		Physics::RigidBody::setTransform(rBody, transformMat);
		// 		Physics::RigidBody::setActivation(rBody, true);
		// 	}
		// }
	}
	
	void update(float deltaTime, GameObject* gameObject)
	{
		syncPhysicsTransform(gameObject); // Should be the last thing so as to
		                                  // do to let other systems modify the
		                                  // transform then sync Physics.
	}

	void update(float deltaTime, bool* quit)
	{
		// if(Input::isReleased(Input::Key::F1))
		// 	Renderer::setDebugLevel(DebugLevel::NONE);
		// if(Input::isReleased(Input::Key::F2))
		// 	Renderer::setDebugLevel(DebugLevel::MEDIUM);
		// if(Input::isReleased(Input::Key::F3))
		// 	Renderer::setDebugLevel(DebugLevel::HIGH);
		// if(Input::isReleased(Input::Key::F4))
		// 	Renderer::toggleWireframe();
		// if(Input::isReleased(Input::Key::F5))
		// 	Renderer::toggleDebugView();
		// if(Input::isReleased(Input::Key::F6))
		// 	Physics::enableDebugDraw(true);
		// if(Input::isReleased(Input::Key::F7))
		// 	Physics::enableDebugDraw(false);
		// if(Input::isReleased(Input::Key::F8))
		// 	Physics::nextDebugMode();
		if(Input::isReleased(Input::Key::F9))
			Log::setEnabled(true);
		if(Input::isReleased(Input::Key::F10))
			Log::setEnabled(false);
		if(Input::isReleased(Input::Key::Q) && Input::isPressed(Input::Key::LSUPER))
			*quit = true;
		
		System::CameraSystem::updateFreeCamera(deltaTime);
		ScriptEngine::executeFunction("updateObjects", deltaTime);
		Editor::update(deltaTime, quit);
		
		// GOMap* sceneObjects = SceneManager::getSceneObjects();
		// for(GOMap::iterator it = sceneObjects->begin();
		// 	it != sceneObjects->end();
		// 	it++)
		// {
		// 	update(deltaTime, it->second);
		// }

		// if(Input::isReleased(Input::Key::C))
		// 	sPhysicsEnabled ? sPhysicsEnabled = false : sPhysicsEnabled = true;

		// if(sPhysicsEnabled)
		// 	Physics::update(deltaTime);
		Renderer::Camera::updateAllCameraViews();
		Transform::resetAllTransformFlags();
		SceneManager::update();
		// if(Input::isReleased(Input::Key::K9))
		// {
		// 	// auto test = SceneManager::find("TestOBJ");
		// 	// GO::reloadScript(test, "Test");
		// 	GO::reloadScript("TestOBJ", "Test");
		// }
		ScriptEngine::executeFunction("void main()");
	}

	void cleanup()
	{
		SceneManager::cleanup();
		Transform::cleanup();
		Editor::cleanup();
		//CompManager::cleanup();
		// Physics::cleanup();
		// Renderer::Model::cleanup();
		// ScriptEngine::cleanup();
	}
}
