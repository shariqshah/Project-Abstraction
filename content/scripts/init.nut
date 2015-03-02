local testobj = SceneManager.create("TestOBJ")
local model = CModel()
Model.loadFromFile("quad.pamesh", model)
Log.message(model.filename)
model.material = Mat_Type.PHONG_TEXTURED
model.materialUniforms.texture = Texture.create("font.png")
Log.message(model.material)
GO.addModel(testobj, model)
local transform = GO.getTransform(testobj)
Transform.setPosition(transform, Vec3(0, 4, 3), true)
::attachScript(testobj, "Test")

local reloader = SceneManager.create("Reloader")
::attachScript(reloader, "Reloader")
