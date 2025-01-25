// ◦ Xyz ◦

#include "Movement.h"
#include <stdio.h>
#include <stdarg.h>
#include <Core.h>
#include <Log.h>
#include <Common/Help.h>
#include <Draw/Camera/CameraControlOutside.h>
#include <Draw2/Draw2.h>
#include <Draw2/Shader/ShaderDefault.h>
#include <Draw2/Shader/ShaderLine.h>
#include "Object/Map.h"
#include <Object/Object.h>
#include <Object/Trigger/Trigger.h>
#include <Object/Model.h>
#include "Glider/Glider.h"
#include "ImGuiManager/UI.h"
#include "ImGuiManager/Editor/Console.h"
#include "Cameras/CameraGlider.h"
#include <Callback/Callback.h>

ModelPtr _skyboxModel;
std::map<std::string, std::string> _portalDirect;

void Movement::init()
{
	ShaderDefault::Instance().Init("Default.vert", "Default.frag");
	ShaderLinePM::Instance().Init("LinePM.vert", "Line.frag");

	InitСameras();
	Load();
	InitPhysic();
	InitCallback();

	UI::ShowWindow<Editor::Console>();
}

void Movement::close()
{
	Save();
}

void Movement::update()
{
	if (Map::Ptr currentMap = Map::GetFirstCurrentMapPtr()) {
		volatile static double deltaTime = 1.f;
		if (Engine::Physics::updateScene(deltaTime)) {
			currentMap->updatePhysixs();
		}

		currentMap->action();

		if (CameraGlider* cameraPtr = dynamic_cast<CameraGlider*>(_camearCurrent.get())) {
			if (!_cameraType) {
				if (Glider* objectPtr = GetPlayerGlider()) {
					const glm::vec3 pos = objectPtr->getPos();
					cameraPtr->SetPosOutside(pos);
				}
				cameraPtr->Rotate(Engine::Callback::deltaMousePos());
			}
		}

		currentMap->RemoveDeferredObjects();
	}
}

void Movement::draw()
{
	Draw2::Viewport();
	Draw2::ClearColor();
	Camera::Set<Camera>(_camearCurrent);

	// SkyBox
	if (_skyboxModel) {
		auto camPos = Camera::_currentCameraPtr->Pos();
		glm::mat4x4 mat(1.f);
		mat[3][0] = camPos.x;
		mat[3][1] = camPos.y;
		mat[3][2] = camPos.z;

		static float rotateZ = 0.f;
		mat = glm::rotate(mat, rotateZ, {0.f, 0.f, 1.f});
		rotateZ += 0.0001f;
		ShaderDefault::Instance().Use();
		Draw2::SetModelMatrixClass<ShaderDefault>(mat);

		float color4[] = { 1.f, 1.f, 1.f, 1.f };
		Draw2::SetColorClass<ShaderDefault>(color4);

		Draw2::Draw(*_skyboxModel);
		Draw2::ClearDepth();
	}
	else {
		_skyboxModel = Model::getByName("SkyBox");
	}

	// Draw
	ShaderDefault::Instance().Use();
	Draw2::DepthTest(true);

	for (Object::Ptr& objectPtr : Map::GetFirstCurrentMap().GetObjects()) {
		Draw2::SetModelMatrixClass<ShaderDefault>(objectPtr->getMatrix());
		Draw2::Draw(objectPtr->getModel());
	}

	Draw2::DrawFunctions();
}

void Movement::resize()
{
	if (_camearCurrent)
		_camearCurrent->Resize();
}

bool Movement::Load()
{
	//GenerateMap();
	_portalDirect.emplace("PhysX/MapPhysX", "PhysX/MapMountainousRelief");
	_portalDirect.emplace("PhysX/MapMountainousRelief", "PhysX/MapPhysX");

	SetCurrentMap("PhysX/MapPhysX");

	return true;
}

void Movement::Save()
{
}

void Movement::InitСameras()
{
	_camearCurrent = std::make_shared<CameraGlider>();
	if (CameraGlider* cameraPtr = dynamic_cast<CameraGlider*>(_camearCurrent.get())) {
		cameraPtr->SetPerspective(10000000.f, 1.f, 45.f);
		cameraPtr->SetPos({ 150.f, 150.f, 150.f });
		cameraPtr->SetDirect({ -0.524f, -0.514f, -0.679f });
		cameraPtr->SetDistanceOutside(75.f);
		cameraPtr->SetSpeed(1.0);
		cameraPtr->Enable(true);

		cameraPtr->GetCallbackPtr()->add(Engine::CallbackType::SCROLL, [cameraPtr](const Engine::CallbackEventPtr& callbackEventPtr) {
			if (static_cast<Engine::TapCallbackEvent*>(callbackEventPtr.get())->getId() == Engine::VirtualTap::SCROLL_BOTTOM) {
				float dist = cameraPtr->GetDistanceOutside();
				dist += 10.f;
				if (dist < 1000.f) {
					cameraPtr->SetDistanceOutside(dist);
				}
			}
			if (static_cast<Engine::TapCallbackEvent*>(callbackEventPtr.get())->getId() == Engine::VirtualTap::SCROLL_UP) {
				float dist = cameraPtr->GetDistanceOutside();
				dist -= 10.f;
				if (dist > 25.f) {
					cameraPtr->SetDistanceOutside(dist);
				}
			}
		});
	}
}

void Movement::InitPhysic() {
	Map::Ptr currentMap = Map::GetFirstCurrentMapPtr();
	if (!currentMap) {
		return;
	}

	if (!Engine::Physics::init()) {
		return;
	}

	if (!Engine::Physics::createScene()) {
		return;
	}

	currentMap->initPhysixs();
}

void Movement::InitCallback()
{
	_callbackPtr = std::make_shared<Engine::Callback>(Engine::CallbackType::RELEASE_KEY, [this](const Engine::CallbackEventPtr& callbackEventPtr) {
		if (dynamic_cast<Engine::KeyCallbackEvent*>(callbackEventPtr.get())->_id == Engine::VirtualKey::ESCAPE) {
			Engine::Core::close();
		}

#if _DEBUG
		if (dynamic_cast<Engine::KeyCallbackEvent*>(callbackEventPtr.get())->_id == Engine::VirtualKey::F1) {
			if (Engine::Callback::pressKey(Engine::VirtualKey::CONTROL)) {
				_cameraType = !_cameraType;
				Engine::Callback::SetResetMouseToCenter(!_cameraType);

				if (Glider* glider = GetPlayerGlider()) {
					glider->EnableControl(!_cameraType);
				}
			}
		}
#endif
	});

#if _DEBUG
	_callbackPtr->debugName = "Movement";
#endif
}

void Movement::GenerateMap()
{
	/*if (_mapGame = Map::getByName("PhysX/MapPhysX")) {
		Draw2::SetClearColor(_mapGame->getRed(), _mapGame->getGreen(), _mapGame->getBlue(), _mapGame->getAlpha());

		
		int size = 900;
		
		for (int iX = -size; iX <= size; iX += 300) {
			for (int iY = -size; iY <= size; iY += 300) {
				std::string nameModel;
				const int numEnding = iX == 0 && iY == 0 ? 0 : help::random_i(0, 6);

				switch (numEnding) {
				case 0: nameModel = "Kolonna_300x300_0"; break;
				case 1: nameModel = "Kolonna_300x300_10"; break;
				case 2: nameModel = "Kolonna_300x300_20"; break;
				case 3: nameModel = "Kolonna_300x300_30"; break;
				case 4: nameModel = "Kolonna_300x300_40"; break;
				case 5: nameModel = "Kolonna_300x300_50"; break;
				case 6: nameModel = "Kolonna_300x300_100"; break;
				default: nameModel = "Kolonna_300x300_0";
				};

				Object& object = _mapGame->addObjectToPos(nameModel, { iX, iY, 0.f });
				object.setTypeActorPhysics(Engine::Physics::Type::TRIANGLE);
			}
		}

		for (int iX = -size; iX <= size; iX += 300) {
			int iY = -(size + 300);
			Object& object = _mapGame->addObjectToPos("Kolonna_300x300_100_280", { iX, iY, 0.f });
			object.setTypeActorPhysics(Engine::Physics::Type::TRIANGLE);
		}
		for (int iX = -size; iX <= size; iX += 300) {
			int iY = size + 300;
			Object& object = _mapGame->addObjectToPos("Kolonna_300x300_100_280", { iX, iY, 0.f });
			object.setTypeActorPhysics(Engine::Physics::Type::TRIANGLE);
		}
		for (int iY = -size; iY <= size; iY += 300) {
			int iX = -(size + 300);
			Object& object = _mapGame->addObjectToPos("Kolonna_300x300_100_280", { iX, iY, 0.f });
			object.setTypeActorPhysics(Engine::Physics::Type::TRIANGLE);
		}
		for (int iY = -size; iY <= size; iY += 300) {
			int iX = size + 300;
			Object& object = _mapGame->addObjectToPos("Kolonna_300x300_100_280", { iX, iY, 0.f });
			object.setTypeActorPhysics(Engine::Physics::Type::TRIANGLE);
		}

		// Portals
		{
			Object& object = _mapGame->addObjectToPos("Portal", { 50.f, 50.f, 50.f });
			object.setTypeActorPhysics(Engine::Physics::Type::TRIANGLE);
			
		}

		_mapGame->Save();
	}*/
}

Glider* Movement::GetPlayerGlider()
{
	return  dynamic_cast<Glider*>(Map::GetFirstCurrentMap().getObjectPtrByName("Player").get());
}

void Movement::SetCurrentMap(const std::string& name)
{
	if (name.empty()) {
		return;
	}

	if (Map::Ptr currentMap = Map::GetFirstCurrentMapPtr()) {
		currentMap->RemoveObject("Player");
		currentMap->releasePhysixs();
	}

	Map::Ptr currentMap = Map::getByName(name);
	if (!currentMap) {
		return;
	}

	Map::SetCurrentMap(currentMap);
	InitPhysic();

	Object::Ptr playerPtr = currentMap->getObjectPtrByName("Player");
	if (!playerPtr) {
		Object::Ptr playerPtr(new Glider("Player", "NLO", glm::vec3(0.f, 0.f, 1000.f)));
		static_cast<Glider*>(playerPtr.get())->EnableControl(true);
		currentMap->addObject(playerPtr);
	}

	playerPtr = currentMap->getObjectPtrByName("Player");
	if (playerPtr) {
		Object::Ptr portalPtr = currentMap->getObjectPtrByName("Portal00");
		if (portalPtr) {
			glm::vec3 pos = portalPtr->getPos();
			glm::vec3 posTop(pos.x + 75.f, pos.y, pos.z);
			playerPtr->setPos(posTop);

			Log("SET: {} -> {} pos: [{}]", _portalDirect[currentMap->getName()], currentMap->getName(), posTop);
		}
	}

	// Triggers
	{
		const std::string triggerName = "Trigger00";
		Trigger::CenterDistance* trigger = dynamic_cast<Trigger::CenterDistance*>(currentMap->GetAdditObjectsByName(triggerName).get());
		if (!trigger) {
			Object::Ptr portalPtr = currentMap->getObjectPtrByName("Portal00");

			if (portalPtr) {
				glm::vec3 pos = portalPtr->getPos();

				Object::Ptr triggerObjectPtr(new Object(triggerName, "Sphere25", pos));
				currentMap->addObject(triggerObjectPtr);

				trigger = new Trigger::CenterDistance(pos, 25.f, triggerName);
				currentMap->additObjects.emplace_back(trigger);
			}
		}
		if (trigger) {
			if (auto glider = currentMap->getObjectPtrByName("Player")) {
				trigger->AddObject(glider, [this]() {
					SetCurrentMap(_portalDirect[Map::GetFirstCurrentMap().getName()]);
				});
			}
		}
	}
}
