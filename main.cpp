#include <iostream>

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/DebugNew.h>

#include "Player.cpp"
#include "World.cpp"

using namespace Urho3D;
using namespace std;

class Incline : public Application
{
public:
    SharedPtr<Scene> scene_;
    Node* camNode;
    Camera* cam;
    Player plr;
    World wld;
    Context* ctx;
    ResourceCache* cache;
    HashSet<int> keysDown;
    bool sideview = false;
    int segmentsgenerated = 0;
    bool generating = false;
    double frame = 0;

    Incline(Context* context) : Application(context)
    {
        ctx = context;
    }

    virtual void Setup()
    {
        engineParameters_["FullScreen"] = false;
        engineParameters_["WindowWidth"] = 1280;
        engineParameters_["WindowHeight"] = 720;
        engineParameters_["WindowResizable"] = true;
        engineParameters_["WindowTitle"] = "Incline";

        int rand = Time::GetSystemTime();
        SetRandomSeed(rand);
        cout << "using random seed: " << rand << endl;
    }

    virtual void Start()
    {
        GetSubsystem<Input>()->SetMouseVisible(true);
        GetSubsystem<Input>()->SetMouseGrabbed(false);

        cache = GetSubsystem<ResourceCache>();
        GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

        scene_ = new Scene(context_);
        scene_->CreateComponent<Octree>();
        scene_->CreateComponent<DebugRenderer>();

        plr.Init(scene_, cache);

        // no skybox needed, black background works fine for now.
//        Node* skyNode = scene_->CreateChild("Sky");
//        skyNode->SetScale(500.0f);
//        Skybox* skybox = skyNode->CreateComponent<Skybox>();
//        skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
//        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

        camNode=scene_->CreateChild("Camera");
        cam = camNode->CreateComponent<Camera>();
        cam->SetFarClip(250);

        auto* loadingText = GetSubsystem<UI>()->GetRoot()->CreateChild<Text>("loadingText");
        loadingText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 21);
        loadingText->SetTextAlignment(HA_CENTER);
        loadingText->SetHorizontalAlignment(HA_CENTER);
        loadingText->SetVerticalAlignment(VA_CENTER);
        loadingText->SetPosition(0, 0);
        loadingText->SetSize(1280, 720);

        auto* scoreText = GetSubsystem<UI>()->GetRoot()->CreateChild<Text>("scoreText");
        scoreText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30);
        scoreText->SetTextAlignment(HA_LEFT);
        scoreText->SetPosition(0, 0);

        auto* speedText = GetSubsystem<UI>()->GetRoot()->CreateChild<Text>("speedText");
        speedText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
        speedText->SetTextAlignment(HA_LEFT);
        speedText->SetPosition(0, 35);

        generateWorld(50);

        Renderer* renderer=GetSubsystem<Renderer>();
        SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camNode->GetComponent<Camera>()));
        renderer->SetViewport(0, viewport);

        SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Incline, HandleKeyDown));
        SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Incline, HandleKeyUp));
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Incline, HandleUpdate));
        SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Incline, HandlePostUpdate));

        // FIX: for some reason the 1st segment starts at a very weird position
        plr.node->SetPosition(Vector3(9.25, 20, 7));
    }

    void generateWorld(int segments)
    {
        if(generating) return;

        generating = true;
        Text* loadingText = GetSubsystem<UI>()->GetRoot()->GetChildDynamicCast<Text>("loadingText", false);
        loadingText->SetVisible(true);
        cout << "generating " << segments << " segments" << endl;

        for (int i = 0; i < segments; i++) {
            wld.getSegment(scene_, cache, segmentsgenerated + i)->SetPosition(Vector3(wld.offset, -(segmentsgenerated + i) * 20, (segmentsgenerated + i) * 37));
            wld.offset += Random(-10, 10);
            loadingText->SetText("Loading...\n" + String(i + 1) + "/" + String(segments));
            engine_->RunFrame();
        }

        segmentsgenerated += segments;
        loadingText->SetVisible(false);
        cout << "done generating segments" << endl;
        generating = false;
    }

    void HandleUpdate(StringHash eventType,VariantMap& eventData)
    {
        using namespace Update;

        // update player camera
        Vector3 campos = plr.node->GetPosition();
        if (sideview) {
            campos.x_ -= 20;
        } else {
            campos.z_ -= 20;
            campos.y_ += 15;
        }
        camNode->SetPosition(campos);
        camNode->LookAt(plr.node->GetPosition());

        // update score
        plr.score = floor(plr.node->GetPosition().z_ / 37);
        GetSubsystem<UI>()->GetRoot()->GetChildDynamicCast<Text>("scoreText", false)->
                SetText(Urho3D::String(plr.score));

        // update speed every 10th frame
        if(std::fmod(frame, 10) == 0) {
            GetSubsystem<UI>()->GetRoot()->GetChildDynamicCast<Text>("speedText", false)->
                    SetText(plr.lastPos.Lerp(plr.node->GetPosition(), 1).ToString());
            plr.lastPos = plr.node->GetPosition();
        }

        // generate more world if player is close to end
        if (segmentsgenerated - plr.score < 2 && !generating)
            generateWorld(10);

        // movement keys
        if (keysDown.Contains(KEY_RIGHT))
            plr.body->ApplyImpulse(Vector3::RIGHT * 0.15);
        if (keysDown.Contains(KEY_LEFT))
            plr.body->ApplyImpulse(Vector3::LEFT * 0.15);
        if (keysDown.Contains(KEY_DOWN))
            plr.body->ApplyImpulse(Vector3::BACK * 0.2);
        if (keysDown.Contains(KEY_UP))
            plr.body->ApplyImpulse(Vector3::FORWARD * 0.1);

        frame++;
    }

    void HandleKeyDown(StringHash eventType,VariantMap& eventData)
    {
        using namespace KeyDown;

        int key = eventData[P_KEY].GetInt();
        if (key == KEY_ESCAPE)
            engine_->Exit();
        if (key == KEY_SPACE)
            sideview = !sideview;
        if (key == KEY_R)
            plr.node->SetPosition(Vector3(0, 20, 0));
        keysDown.Insert(key);
    }

    void HandleKeyUp(StringHash eventType,VariantMap& eventData)
    {
        using namespace KeyUp;
        int key = eventData[P_KEY].GetInt();
        keysDown.Erase(key);
    }

    void HandlePostUpdate(StringHash eventType,VariantMap& eventData)
    {
    }


private:
};

URHO3D_DEFINE_APPLICATION_MAIN(Incline)
