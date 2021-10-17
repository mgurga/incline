#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
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

class World
{
public:
    int offset = 0;

    Node* getSegment(Scene *scene_, ResourceCache *cache, int id)
    {
        return createFlatSegment(scene_, cache, id);
    }

private:
    Node* createFlatSegment(Scene *scene_, ResourceCache *cache, int id)
    {
        // floorNode just holds
        Node* floorNode = scene_->CreateChild(Urho3D::String(id));
        floorNode->SetDirection(Vector3::FORWARD);
        floorNode->SetPosition(Vector3(0, 0, 0));
        floorNode->SetEnabled(false);

        // creates a lot of cubes to give it a nice look but they have no collision
        for(int i = 0; i < 10; i++)
        {
            for(int j = 0; j < 5; j++)
            {
                Node* fluffNode = floorNode->CreateChild();
                fluffNode->SetPosition(Vector3((j * 4), (-i * 1.75), (i * 3.7)));
                fluffNode->SetScale(Vector3(4, 0.1, 4));
                fluffNode->Pitch(25);
                StaticModel* fluffObject = fluffNode->CreateComponent<StaticModel>();
                fluffObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
                fluffObject->SetMaterial(cache->GetResource<Material>("Materials/GreenTransparent.xml"));
                fluffObject->GetMaterial()->SetTechnique(0, cache->GetResource<Technique>("Techniques/NoTextureUnlitAlpha.xml"));
                fluffObject->GetMaterial()->SetFillMode(FILL_WIREFRAME);
                auto* body = fluffNode->CreateComponent<RigidBody>();
                body->SetCollisionLayer(2);
                auto* shape = fluffNode->CreateComponent<CollisionShape>();
                // shape->SetBox(Vector3::ONE);
                shape->SetBox(Vector3(1, 1, 1));
                // shape->SetConvexHull(fluffObject->GetModel(), 0,, Vector3((j * 0.2) - 0.4, (-i * 0.12) + 1.2, (i * 0.15) - 1));
            }
        }
        return floorNode;
    }
};
