#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/DebugNew.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

using namespace Urho3D;

class Player
{
public:
    Node* node;
    StaticModel* sphereObject;
    RigidBody* body;
    int score = 0;
    Vector3 lastPos = Vector3::ZERO;

    Player()
    {
    }

    void Init(Scene *scene_, ResourceCache *cache) {
        node = scene_->CreateChild("PlayerSphere");
        node->SetScale(Vector3(3,3,3));
        node->SetPosition(Vector3(0, 0, 0));

        body = node->CreateComponent<RigidBody>();
        body->SetCollisionLayer(1);
        body->SetCollisionEventMode(COLLISION_ALWAYS);
        body->SetMass(2);
        body->SetLinearDamping(0.20);

        sphereObject=node->CreateComponent<StaticModel>();
        sphereObject->SetModel(cache->GetResource<Model>("Models/Sphere.mdl"));
        sphereObject->SetMaterial(cache->GetResource<Material>("Materials/GreenTransparent.xml"));
        sphereObject->GetMaterial()->SetFillMode(FILL_WIREFRAME);
        sphereObject->GetMaterial()->SetTechnique(0, cache->GetResource<Technique>("Techniques/NoTextureUnlitAlpha.xml"));
        sphereObject->SetCastShadows(false);

        auto* shape = node->CreateComponent<CollisionShape>();
        shape->SetSphere(1.25f);
    }
};
