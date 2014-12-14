local testobj = SceneManager.create("TestOBJ")
CompManager.addModel(testobj, "models/falcon/falcon.scene.xml")
local transform = CompManager.getTransform(testobj)
Transform.setPosition(transform, Vec3(0, 10, 0), true)
::attachScript(testobj, "Test")
local model = Model.create("models/falcon/falcon.scene.xml")
CompManager.addRigidBody(testobj,
						 CollisionShapes.createMesh(model, false),
						 1.0,
						 0.5)
