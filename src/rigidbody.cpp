#include "rigidbody.h"
#include "gameobject.h"
#include "scriptengine.h"
#include "utilities.h"
#include "log.h"
#include "passert.h"
#include "physics.h"
#include "transform.h"
#include "geometry.h"
#include "collisionshapes.h"

#include "../include/bullet/btBulletDynamicsCommon.h"

namespace RigidBody
{
	namespace
	{
		std::vector<CRigidBody>      freeList;
		std::vector<btRigidBody*>    rigidBodies;
		CollisionShape*              defaultShape;

		void removeCollisionObject(btRigidBody *body)
		{
			btDiscreteDynamicsWorld* world = Physics::getWorld();
			world->removeRigidBody(body);
			if(body && body->getMotionState()) delete body->getMotionState();
			body == NULL ? Log::message("RB is NULL") : delete body;
		}
	}

	void initialize()
	{
		defaultShape = new Sphere();
	}
	
	CRigidBody create(GameObject*     gameObject,
					  CollisionShape* shape,
					  btMotionState*  motionState,
					  float           mass,
					  float           restitution)
	{
		CRigidBody rbHandle = -1;
		if(!shape) shape = defaultShape;
		if(shape->isValid())
		{
			btDiscreteDynamicsWorld* world = Physics::getWorld();
			btVector3 inertia(0, 0, 0);
			if(mass != 0) shape->getCollisionShape()->calculateLocalInertia(mass, inertia);
			btRigidBody::btRigidBodyConstructionInfo consInfo(mass,
															  motionState,
															  shape->getCollisionShape(),
															  inertia);
			btRigidBody* body = new btRigidBody(consInfo);
			body->setRestitution(restitution);
			intptr_t temp = (intptr_t)gameObject->node;
			body->setUserPointer((void*)temp);
			world->addRigidBody(body);
			
			if(!freeList.empty())
			{
				rbHandle = freeList.back();
				freeList.pop_back();

				if(rigidBodies[rbHandle] != NULL)
					Log::warning("Overwriting Rigidbody!");
			
				rigidBodies[rbHandle] = body;
			}
			else
			{
				rigidBodies.push_back(body);
				rbHandle = (int32_t) (rigidBodies.size() - 1);
			}
			CTransform* transform = GO::getTransform(gameObject);
			setTransform(rbHandle, transform->transMat);
		}
		else
		{
			Log::error("RigidBody::create", "Invalid collision shape, rigidbody not created");
		}
		return rbHandle;
	}
	
	void setTransform(CRigidBody body, Mat4 transformMat)
	{
		btTransform transform;
		transform.setFromOpenGLMatrix(glm::value_ptr(transformMat));
		rigidBodies[body]->setWorldTransform(transform);
	}

	void setActivation(CRigidBody body, bool activation)
	{
		rigidBodies[body]->activate(activation);
	}

	void setTransform(CRigidBody body, Vec3 position, Quat rotation)
	{
		btTransform transform;
		transform.setOrigin(Utils::toBullet(position));
		transform.setRotation(Utils::toBullet(rotation));
		rigidBodies[body]->setWorldTransform(transform);
	}

	void getTransform(CRigidBody body, glm::vec3* position, glm::quat* rotation)
	{
		btTransform transform = rigidBodies[body]->getWorldTransform();
		if(position)
			*position = Utils::toGlm(transform.getOrigin());
		if(rotation)
			*rotation = Utils::toGlm(transform.getRotation());
	}

	void remove(CRigidBody body)
	{
		if(rigidBodies[body])
		{
			removeCollisionObject(rigidBodies[body]);
			rigidBodies[body] = NULL;
			freeList.push_back(body);
		}
		else
		{
			Log::warning("RigidBody " + std::to_string(body) + "does not exist so not removed");
		}
	}

	void setKinematic(CRigidBody body, bool kinematic)
	{
		btRigidBody* temp = rigidBodies[body];
		btDiscreteDynamicsWorld* world = Physics::getWorld();
		world->removeRigidBody(temp);

		if(kinematic)
		{
			temp->setFlags(rigidBodies[body]->getFlags() |
						   btCollisionObject::CF_KINEMATIC_OBJECT |
						   btCollisionObject::CF_NO_CONTACT_RESPONSE);
			temp->setActivationState(DISABLE_DEACTIVATION);
		}
		else
		{
			//rigidBodies[body]->setFlags(~btCollisionObject::CF_KINEMATIC_OBJECT);
			temp->setFlags(rigidBodies[body]->getFlags() |
						   btCollisionObject::CF_STATIC_OBJECT);
			temp->setActivationState(WANTS_DEACTIVATION);
			//rigidBodies[body]->activate(true);
		}

		world->addRigidBody(temp);
		// if(kinematic)
		// {
		// 	rigidBodies[body]->setFlags(rigidBodies[body]->getFlags() |
		// 								 btCollisionObject::CF_KINEMATIC_OBJECT |
		// 								 btCollisionObject::CF_NO_CONTACT_RESPONSE);
		// 	rigidBodies[body]->setActivationState(DISABLE_DEACTIVATION);
		// }
		// else
		// {
		// 	//rigidBodies[body]->setFlags(~btCollisionObject::CF_KINEMATIC_OBJECT);
		// 	rigidBodies[body]->setFlags(rigidBodies[body]->getFlags() |
		// 								 ~(btCollisionObject::CF_KINEMATIC_OBJECT) |
		// 								 btCollisionObject::CF_STATIC_OBJECT);
		// 	rigidBodies[body]->setActivationState(WANTS_DEACTIVATION);
		// 	//rigidBodies[body]->activate(true);
		// }
		
	}

	void setMass(CRigidBody body, float mass)
	{
		btCollisionShape* shape = rigidBodies[body]->getCollisionShape();
		btVector3 inertia(0, 0, 0);
		if(mass <  0.f) mass = 0.f;
		if(mass != 0.f) shape->calculateLocalInertia(mass, inertia);
		rigidBodies[body]->setMassProps(mass, inertia);
	}

	float getMass(CRigidBody body)
	{
		float mass = rigidBodies[body]->getInvMass();
		if(mass != 0.f) mass = 1.f / mass;
		return mass;
	}

	float getRestitution(CRigidBody body)
	{
		return rigidBodies[body]->getRestitution();
	}
		
	void setRestitution(CRigidBody body, float restitution)
	{
		if(restitution < 0.f) restitution *= -1.f;
		rigidBodies[body]->setRestitution(restitution);
	}
		
	float getFriction(CRigidBody body)
	{
		return rigidBodies[body]->getFriction();
	}
		
	void setFriction(CRigidBody body, float friction)
	{
		if(friction < 0.f) friction *= -1.f;
		rigidBodies[body]->setFriction(friction);
	}

	void applyForce(CRigidBody body, Vec3 force, Vec3 relPos)
	{
		rigidBodies[body]->applyForce(Utils::toBullet(force),
									  Utils::toBullet(relPos));
	}

	void generateBindings()
	{
		asIScriptEngine* engine = ScriptEngine::getEngine();
		engine->SetDefaultNamespace("Rigidbody");
		int rc = engine->RegisterGlobalFunction("void setKinematic(int, bool)",
												asFUNCTION(setKinematic),
												asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("float getMass(int)",
											asFUNCTION(getMass),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setMass(int, float)",
											asFUNCTION(setMass),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("float getRestitution(int)",
											asFUNCTION(getRestitution),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setRestitution(int, float)",
											asFUNCTION(setRestitution),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("float getFriction(int)",
											asFUNCTION(getFriction),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setFriction(int, float)",
											asFUNCTION(setFriction),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void applyForce(int, const Vec3, const Vec3 = Vec3(0.f))",
											asFUNCTION(applyForce),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		rc = engine->RegisterGlobalFunction("void setColllisionShape(int, CollisionShape@)",
											asFUNCTION(setCollisionShape),
											asCALL_CDECL);
		PA_ASSERT(rc >= 0);
		engine->SetDefaultNamespace("");
	}

	void setCollisionShape(CRigidBody body, CollisionShape* shape)
	{
		if(!shape) shape = defaultShape;
		if(shape->isValid())
			rigidBodies[body]->setCollisionShape(shape->getCollisionShape());
		else
			Log::error("RigidBody::setCollisionShape", "Invalid collision shape");
	}

	const char* getCollisionShapeName(CRigidBody body)
	{
		const char* name = "INVALID";
		int type = getCollisionShapeType(body);
		switch(type)
		{
		case CS_SPHERE:       name = "Sphere";         break;
		case CS_BOX:          name = "Box";            break;
		case CS_CAPSULE:      name = "Capsule";        break;
		case CS_PLANE:        name = "Plane";          break;
		case CS_CONE:         name = "CONE";           break;
		case CS_CYLINDER:     name = "Cylinder";       break;
		case CS_CONCAVE_MESH: name = "Concave Mesh";   break;
		case CS_CONVEX_MESH:  name = "Convex Mesh";    break;
		}
		return name;
	}

	void cleanup()
	{
		btDiscreteDynamicsWorld* world = Physics::getWorld();
		//Remove collision objects
		for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject *obj = world->getCollisionObjectArray()[i];
			removeCollisionObject(btRigidBody::upcast(obj));
		}
		rigidBodies.clear();
	}

	bool createFromJSON(CRigidBody body, const rapidjson::Value& value)
	{
		using namespace rapidjson;
		bool success = true;
		const char* error = "Invalid value in a field";
		if(value.IsObject())
		{
			if(value.HasMember("Mass") && value["Mass"].IsNumber())
			{
				const Value& massNode = value["Mass"];
				float mass = (float)massNode.GetDouble();
				if(mass >= 0)
					setMass(body, mass);
				else
					success = false;
			}
			else
			{
				success = false;
				Log::error("RigidBody::createFromJSON", "Error loading Mass");
			}

			if(value.HasMember("Friction") && value["Friction"].IsNumber())
			{
				const Value& frictionNode = value["Friction"];
				float friction = (float)frictionNode.GetDouble();
				if(friction >= 0)
					setFriction(body, friction);
				else
					success = false;
			}
			else
			{
				success = false;
				Log::error("RigidBody::createFromJSON", "Error loading Friction");
			}

			if(value.HasMember("Restitution") && value["Restitution"].IsNumber())
			{
				const Value& restitutionNode = value["Restitution"];
				float restitution = (float)restitutionNode.GetDouble();
				if(restitution >= 0)
					setRestitution(body, restitution);
				else
					success = false;
			}
			else
			{
				success = false;
				Log::error("RigidBody::createFromJSON", "Error loading Restitution");
			}

			if(value.HasMember("IsKinematic") && value["IsKinematic"].IsBool())
			{
				const Value& isKinematicNode = value["IsKinematic"];
				setKinematic(body, isKinematicNode.GetBool());
			}
			else
			{
				success = false;
				Log::error("RigidBody::createFromJSON", "Error loading IsKinematic");
			}

			if(value.HasMember("Shape") && value["Shape"].IsObject())
			{
				const Value& shapeNode = value["Shape"];
				if(shapeNode.HasMember("Type") && shapeNode["Type"].IsInt())
				{
					int type = shapeNode["Type"].GetInt();
					CollisionShape* shape  = NULL;
					switch(type)
					{
					case CS_SPHERE:
					{
						if(shapeNode.HasMember("Radius") && shapeNode["Radius"].IsNumber())
							shape = new Sphere((float)shapeNode["Radius"].GetDouble());
						else
							shape = new Sphere();
					}
					break;
					case CS_BOX:
					{
						Vec3 halfExt(0.5f);
						if(shapeNode.HasMember("HalfExt") && shapeNode["HalfExt"].IsArray())
						{
							const Value& halfExtNode = shapeNode["HalfExt"];
							int count = halfExtNode.Size() < 3 ? halfExtNode.Size() : 3;
							for(int i = 0; i < count; i++)
							{
								if(halfExtNode[i].IsNumber())
									halfExt[i] = halfExtNode[i].GetDouble();
								else
									success = false;
							}
							shape = new Box(halfExt);
						}
						else
						{
							shape = new Box();
						}
					}
					break;
					case CS_CAPSULE:
					{
						float radius = 1.f;
						float height = 2.f;
						if(shapeNode.HasMember("Radius") && shapeNode["Radius"].IsNumber())
						{
							radius = (float)shapeNode["Radius"].GetDouble();
							if(shapeNode.HasMember("Height") && shapeNode["Height"].IsNumber())
								height = (float)shapeNode["Height"].GetDouble();
							shape = new Capsule(radius, height);
						}
						else
						{
							shape = new Capsule();
						}
					}
					break;
					case CS_PLANE:
					{
						Vec3  normal(0, 1, 0);
						float margin = 0.001f;
						if(shapeNode.HasMember("Normal") && shapeNode["Normal"].IsArray())
						{
							const Value& normalNode = shapeNode["Normal"];
							int count = normalNode.Size() < 3 ? normalNode.Size() : 3;
							for(int i = 0; i < count; i++)
							{
								if(normalNode[i].IsNumber())
									normal[i] = normalNode[i].GetDouble();
								else
									success = false;
							}
							if(shapeNode.HasMember("Margin") && shapeNode["Margin"].IsNumber())
								margin = (float)shapeNode["Margin"].GetDouble();
							shape = new Plane(normal, margin);
						}
						else
						{
							shape = new Plane();
						}
					}
					break;
					case CS_CONE:
					{
						float radius = 1.f;
						float height = 2.f;
						if(shapeNode.HasMember("Radius") && shapeNode["Radius"].IsNumber())
						{
							radius = (float)shapeNode["Radius"].GetDouble();
							if(shapeNode.HasMember("Height") && shapeNode["Height"].IsNumber())
								height = (float)shapeNode["Height"].GetDouble();
							shape = new Cone(radius, height);
						}
						else
						{
							shape = new Cone();
						}
					}
					break;
					case CS_CYLINDER:
					{
						Vec3 halfExt(0.5f);
						Vec3 axis(0, 1, 0);
						if(shapeNode.HasMember("HalfExt") && shapeNode["HalfExt"].IsArray())
						{
							const Value& halfExtNode = shapeNode["HalfExt"];
							int count = halfExtNode.Size() < 3 ? halfExtNode.Size() : 3;
							for(int i = 0; i < count; i++)
							{
								if(halfExtNode[i].IsNumber())
									halfExt[i] = halfExtNode[i].GetDouble();
								else
									success = false;
							}
							
							if(shapeNode.HasMember("Axis") && shapeNode["Axis"].IsArray())
							{
								const Value& axisNode = shapeNode["Axis"];
								int count = axisNode.Size() < 3 ? axisNode.Size() : 3;
								for(int i = 0; i < count; i++)
								{
									if(axisNode[i].IsNumber())
										axis[i] = axisNode[i].GetDouble();
									else
										success = false;
								}
							}
							shape = new Cylinder(halfExt, axis);
						}
						else
						{
							shape = new Cylinder();
						}
					}
					break;
					case CS_CONCAVE_MESH:
					{
						const char* filename = "default.pamesh";
						bool isTriMesh = false;
						if(shapeNode.HasMember("GeometryName") && shapeNode["GeometryName"].IsString())
						{
							const Value& geometryNameNode = shapeNode["GeometryName"];
							filename = shapeNode["GeometryName"].GetString();
							shape = new CollisionMesh(filename, isTriMesh);
						}
						else
						{
							shape = new CollisionMesh();
						}
					}
					break;
					case CS_CONVEX_MESH:
					{
						const char* filename = "default.pamesh";
						bool isTriMesh = true;
						if(shapeNode.HasMember("GeometryName") && shapeNode["GeometryName"].IsString())
						{
							const Value& geometryNameNode = shapeNode["GeometryName"];
							filename = shapeNode["GeometryName"].GetString();
							shape = new CollisionMesh(filename, isTriMesh);
						}
						else
						{
							shape = new CollisionMesh();
						}
					}
					break;
					default:
						error = "Invalid collision shape type";
						success = false;
						break;
					}
					setCollisionShape(body, shape);
				}
				else
				{
					error   = "Shape type not specified";
					success = false;
				}
			}
			else
			{
				success = false;
			}
			
			setActivation(body, true);
		}
		else
		{
			error   = "'RigidBody' must be an object";
			success = false;
		}
		if(!success) Log::error("RigidBody::createFromJSON", error);
		return success;
	}

	bool writeToJSON(CRigidBody body, rapidjson::Writer<rapidjson::StringBuffer>& writer)
	{
		using namespace rapidjson;
		bool success = true;
		writer.Key("RigidBody");
		writer.StartObject();
		writer.Key("Mass");        writer.Double(getMass(body));
		writer.Key("Friction");    writer.Double(getFriction(body));
		writer.Key("Restitution"); writer.Double(getRestitution(body));
		writer.Key("IsKinematic"); writer.Bool(isKinematic(body));
		writer.Key("Shape");
		writer.StartObject();
		
		int type = getCollisionShapeType(body);
		btCollisionShape* bulletShape = rigidBodies[body]->getCollisionShape();
		intptr_t index = (intptr_t)bulletShape->getUserPointer();
		CollisionShape* shape = Physics::getCollisionShapeAtIndex((int)index);
		writer.Key("Type"); writer.Int(type);
		switch(type)
		{
		case CS_SPHERE:
		{
			Sphere* sphere = (Sphere*)shape;
			writer.Key("Radius"); writer.Double(sphere->radius);
		}
		break;
		case CS_BOX:
		{
			Box* box = (Box*)shape;
			writer.Key("HalfExt");
			writer.StartArray();
			for(int i = 0; i < 3; i++) writer.Double(box->halfExtent[i]);
			writer.EndArray();
		}
		break;
		case CS_CAPSULE:
		{
			Capsule* capsule = (Capsule*)shape;
			writer.Key("Radius"); writer.Double(capsule->radius);
			writer.Key("Height"); writer.Double(capsule->height);
		}
		break;
		case CS_PLANE:
		{
			Plane* plane = (Plane*)shape;
			writer.Key("Margin"); writer.Double(plane->margin);
			writer.Key("Normal");
			writer.StartArray();
			for(int i = 0; i < 3; i++) writer.Double(plane->normal[i]);
			writer.EndArray();
		}
		break;
		case CS_CONE:
		{
			Cone* cone = (Cone*)shape;
			writer.Key("Radius"); writer.Double(cone->radius);
			writer.Key("Height"); writer.Double(cone->height);
		}
		break;
		case CS_CYLINDER:
		{
			Cylinder* cylinder = (Cylinder*)shape;
			writer.Key("Axis");
			writer.StartArray();
			for(int i = 0; i < 3; i++) writer.Double(cylinder->axis[i]);
			writer.EndArray();
			writer.Key("HalfExt");
			writer.StartArray();
			for(int i = 0; i < 3; i++) writer.Double(cylinder->halfExtent[i]);
			writer.EndArray();
		}
		break;
		case CS_CONVEX_MESH:
		{
			CollisionMesh* collisionMesh = (CollisionMesh*)shape;
			const std::string geomtryName = Geometry::getName(collisionMesh->geometryIndex);
			writer.Key("GeometryName"); writer.String(geomtryName.c_str(), geomtryName.size());
		}
		break;
		case CS_CONCAVE_MESH:
		{
			CollisionMesh* collisionMesh = (CollisionMesh*)shape;
			const std::string geomtryName = Geometry::getName(collisionMesh->geometryIndex);
			writer.Key("GeometryName"); writer.String(geomtryName.c_str(), geomtryName.size());
		}
		break;
		}
		writer.EndObject();
		
		writer.EndObject();
		return success;
	}

	bool isKinematic(CRigidBody body)
	{
		return rigidBodies[body]->isKinematicObject();
	}

	int getCollisionShapeType(CRigidBody body)
	{
		btCollisionShape* bulletShape = rigidBodies[body]->getCollisionShape();
		intptr_t index = (intptr_t)bulletShape->getUserPointer();
		CollisionShape* shape = Physics::getCollisionShapeAtIndex((int)index);
		return shape->getType();
	}
}
