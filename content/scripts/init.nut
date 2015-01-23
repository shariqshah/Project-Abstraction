local testobj = SceneManager.create("TestOBJ")
local model = CModel();
Model.loadFromFile("teapot.pamesh", model)
Log.message(model.filename)
model.material = Mat_Type.PHONG
Log.message(model.material)
GO.addModel(testobj, model)
local transform = GO.getTransform(testobj)
Transform.setPosition(transform, Vec3(0, 10, 5), true)
::attachScript(testobj, "Test")

local reloader = SceneManager.create("Reloader")
::attachScript(reloader, "Reloader")
