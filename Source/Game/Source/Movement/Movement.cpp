// ◦ Xyz ◦

#include "Movement.h"
#include <Core.h>
#include <Common/Help.h>
#include <Draw/Camera/CameraControlOutside.h>
#include <Draw2/Draw2.h>
#include <Draw2/Shader/ShaderDefault.h>
#include <Draw2/Shader/ShaderLine.h>
#include "Object/Map.h"
#include <Object/Object.h>
#include <Object/Model.h>
#include "Glider/Glider.h"
#include "ImGuiManager/UI.h"
#include "ImGuiManager/Editor/Console.h"

ModelPtr _skyboxModel;

void Movement::init()
{
	ShaderDefault::Instance().Init("Default.vert", "Default.frag");
	ShaderLinePM::Instance().Init("LinePM.vert", "Line.frag");

	InitСameras();
	Load();
	InitPhysic();

	UI::ShowWindow<Editor::Console>();
}

void Movement::close()
{
	Save();
}

void Movement::update()
{
	if (_mapGame) {
		volatile static double deltaTime = 1.f;
		if (Engine::Physics::updateScene(deltaTime)) {
			_mapGame->updatePhysixs();
		}

		_mapGame->action();

		// Player
		if (CameraControlOutside* cameraPtr = dynamic_cast<CameraControlOutside*>(_camearCurrent.get())) {
			if (Object::Ptr objectPtr = _mapGame->getObjectPtrByName("Player")) {
				const glm::vec3 pos = objectPtr->getPos();
				cameraPtr->SetPosOutside(pos);
			}
		}
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

	for (Object::Ptr& objectPtr : _mapGame->GetObjects()) {
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
	if (_mapGame = make_shared<Map>("PhysX/MapPhysX")) {
		Draw2::SetClearColor(_mapGame->getRed(), _mapGame->getGreen(), _mapGame->getBlue(), _mapGame->getAlpha());

		for (int iX = -300; iX < 300; iX += 100) {
			for (int iY = -300; iY < 300; iY += 100) {
				if (iX == 0 && iY == 0) {
					continue;
				}
				Object& object = _mapGame->addObjectToPos("Plane100", { iX, iY, 0.f });
				object.setTypeActorPhysics(Engine::Physics::Type::TRIANGLE);
			}
		}

		// Player
		Object::Ptr gliderPtr(new Glider("Player", "Car", { 0.f, 0.f, 100.f }));
		static_cast<Glider*>(gliderPtr.get())->EnableControl(true);
		_mapGame->addObject(gliderPtr);
	}

	return true;
}

void Movement::Save()
{
}

void Movement::InitСameras()
{
	_camearCurrent = std::make_shared<CameraControlOutside>();
	if (CameraControlOutside* cameraPtr = dynamic_cast<CameraControlOutside*>(_camearCurrent.get())) {
		cameraPtr->SetPerspective(10000000.f, 1.f, 45.f);
		cameraPtr->SetPos({ 50.f, 50.f, 50.f });
		cameraPtr->SetDirect({ -0.524f, -0.514f, -0.679f });
		cameraPtr->SetDistanceOutside(50.f);
		cameraPtr->SetSpeed(1.0);
		cameraPtr->Enable(true);
	}
}

void Movement::InitPhysic() {
	if (!_mapGame) {
		return;
	}

	if (!Engine::Physics::init()) {
		return;
	}

	if (!Engine::Physics::createScene()) {
		return;
	}

	_mapGame->initPhysixs();
}
