local testobj = SceneManager.create("TestOBJ")
local model = GO.addModel(testobj, "quad.pamesh")
Model.setMaterialType(model, Mat_Type.PHONG_TEXTURED);
model.materialUniforms.texture = Texture.create("font.png")
local transform = GO.getTransform(testobj)
Transform.setPosition(transform, Vec3(0, 4, 3), true)
::attachScript(testobj, "Test")

local reloader = SceneManager.create("Reloader")
::attachScript(reloader, "Reloader")
