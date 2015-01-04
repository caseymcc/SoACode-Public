///
/// SpaceSystem.h
/// Seed of Andromeda
///
/// Created by Benjamin Arnold on 8 Dec 2014
/// Copyright 2014 Regrowth Studios
/// All Rights Reserved
///
/// Summary:
/// Implementation of a Star System with ECS
///

#pragma once

#ifndef SpaceSystem_h__
#define SpaceSystem_h__


#include "AxisRotationComponent.h"
#include "NamePositionComponent.h"
#include "OrbitComponent.h"
#include "SphericalGravityComponent.h"
#include "SphericalTerrainComponent.h"

#include <Vorb/IOManager.h>
#include <Vorb/ComponentTable.hpp>
#include <Vorb/ECS.h>

#define SPACE_SYSTEM_CT_NAMEPOSITIION_NAME "NamePosition"
#define SPACE_SYSTEM_CT_AXISROTATION_NAME "AxisRotation"
#define SPACE_SYSTEM_CT_ORBIT_NAME "Orbit"
#define SPACE_SYSTEM_CT_SPHERICALTERRAIN_NAME "SphericalTerrain"

class App;
class Binary;
class Camera;
class GasGiantKegProperties;
class PlanetKegProperties;
class PlanetLoader;
class SpriteBatch;
class SpriteFont;
class StarKegProperties;
class SystemBodyKegProperties;

namespace vorb {
    namespace core {
        namespace graphics {
            class TextureRecycler;
            class GLProgramManager;
        }
    }
}

enum class BodyType {
    NONE,
    PLANET,
    STAR,
    GAS_GIANT
};

class SystemBody {
public:
    ~SystemBody() { delete entity; }

    nString name = "";
    nString parentName = "";
    SystemBody* parent = nullptr;
    vcore::Entity* entity = nullptr;
    BodyType type = BodyType::NONE;
};


class SpaceSystem : public vcore::ECS {
    friend class SpaceSystemRenderStage;
    friend class MainMenuSystemViewer;
public:
    SpaceSystem();
    ~SpaceSystem();

    void init(vg::GLProgramManager* programManager);

    /// Updates the space system
    /// @param time: The time in seconds
    /// @param cameraPos: Position of the camera
    void update(double time, const f64v3& cameraPos);
    
    /// Updates openGL specific stuff, should be called on render thread
    void glUpdate();

    /// Adds a solar system and all its bodies to the system
    /// @param filePath: Path to the solar system directory
    void addSolarSystem(const nString& filePath);

    /// Targets a named body
    /// @param name: Name of the body
    void targetBody(const nString& name) {
        auto& it = m_bodyLookupMap.find(name);
        if (it != m_bodyLookupMap.end()) {
            targetComponent = it->second;
        }
    }

    /// Changes target by offset
    /// @param offset: Integer offset by which to change target
    void offsetTarget(int offset) {
        targetComponent += offset;
        if (targetComponent >= m_namePositionCT.getComponentListSize()) {
            targetComponent = 1;
        } else if (targetComponent <= 0) {
            targetComponent = m_namePositionCT.getComponentListSize() - 1;
        }
    }

    /// Gets the position of the targeted entity
    /// @return position
    f64v3 getTargetPosition() {
        m_mutex.lock();
        f64v3 pos = m_namePositionCT.get(targetComponent).position;
        m_mutex.unlock();
        return pos;
    }

    /// Gets the position of the targeted entity
    /// @return radius
    f64 getTargetRadius() {
        return m_sphericalGravityCT.get(targetComponent).radius;
    }

    /// Gets the name of the targeted component
    /// @return position
    nString getTargetName() {
        return m_namePositionCT.get(targetComponent).name;
    }

protected:
    bool loadBodyProperties(const nString& filePath, const SystemBodyKegProperties* sysProps, SystemBody* body);

    void addPlanet(const SystemBodyKegProperties* sysProps, const PlanetKegProperties* properties, SystemBody* body);

    void addStar(const SystemBodyKegProperties* sysProps, const StarKegProperties* properties, SystemBody* body);

    void addGasGiant(const SystemBodyKegProperties* sysProps, const GasGiantKegProperties* properties, SystemBody* body);

    bool loadSystemProperties(const nString& dirPath);

    void calculateOrbit(vcore::EntityID entity, f64 parentMass, bool isBinary);

    void setOrbitProperties(vcore::ComponentID cmp, 
                            const SystemBodyKegProperties* sysProps);

    vcore::ComponentID targetComponent = 1; ///< Current namePositionComponent we are focusing on

    vcore::ComponentTable<NamePositionComponent> m_namePositionCT;
    vcore::ComponentTable<AxisRotationComponent> m_axisRotationCT;
    vcore::ComponentTable<OrbitComponent> m_orbitCT;
    vcore::ComponentTable<SphericalGravityComponent> m_sphericalGravityCT;
    vcore::ComponentTable<SphericalTerrainComponent> m_sphericalTerrainCT;

    IOManager m_ioManager;

    std::mutex m_mutex;

    vg::TextureRecycler* m_normalMapRecycler = nullptr; ///< For recycling normal maps
    
    PlanetLoader* m_planetLoader = nullptr;

    vg::GLProgramManager* m_programManager = nullptr;

    std::map<nString, Binary*> m_binaries; ///< Contains all binary systems
    std::map<nString, SystemBody*> m_systemBodies; ///< Contains all system bodies

    SpriteBatch* m_spriteBatch = nullptr;
    SpriteFont* m_spriteFont = nullptr;

    std::map<nString, vcore::EntityID> m_bodyLookupMap;

    nString m_dirPath; ///< Path to the main directory
    nString m_systemDescription; ///< textual description of the system
};

#endif // SpaceSystem_h__