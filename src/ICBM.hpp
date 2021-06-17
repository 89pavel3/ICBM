#pragma once

#include <raylib.h> 

#include <cmath>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

//----------------------------------------------------------------------------------
// Some Consts
//----------------------------------------------------------------------------------
inline const int MAX_MISSILES = 100;
inline const int MAX_INTERCEPTORS = 30;
inline const int MAX_EXPLOSIONS = 100;
inline const int TURRETS_AMOUNT = 2;   // Should not be changed
inline const int BUILDINGS_AMOUNT = 7; // Should not be changed

inline const int TURRET_WIDTH = 51;
inline const int TURRET_HEIGHT = 80;

inline const int BUILDING_WIDTH = 63;
inline const int BUILDING_HEIGHT = 90;

inline const int MISSILE_SPEED = 1;
inline const int MISSILE_LAUNCH_FRAMES = 80;

inline const int INTERCEPTOR_SPEED = 10;

inline const int EXPLOSION_RADIUS = 40;
inline const int EXPLOSION_INCREASE_TIME = 90; // In frames
inline const int EXPLOSION_TOTAL_TIME = 210;   // In frames
inline const Color EXPLOSION_COLOR =                                                        \
    (Color) { 200, 50, 50, 125 };

inline const int INTERCEPTOR_COOLDOWN = 60;
inline const int SWARM_COOLDOWN = 300;
inline const int LASERGUN_COOLDOWN = 120;
inline const int AIRBURST_COOLDOWN = 120;
inline const int SWARM_DURATION = 60;

//------------------------------------------------------------------------------------------
// Types and Structures Definition
//------------------------------------------------------------------------------------------

/// Particle
/**
 * Родительский класс для всех снарядов
 */
class Particle {
  public:
    Vector2 origin;    ///< Начальная позиция
    Vector2 position;  ///< Текущая позиция
    Vector2 objective; ///< Конечная позиция
    Vector2 speed;     ///< Скорость

    bool explosive; ///< Взорвется ли снаряд при колизии
    bool active;    ///< Активность снаряда

    /**
     * @brief Construct a new Particle object
     *
     * @param originP Начальная позиция
     * @param positionP Текущая позиция
     * @param objectiveP Конечная позиция
     * @param speedP Скорость
     * @param explosiveP Взорвется ли снаряд при колизии
     * @param activeP Активность снаряда
     */
    Particle(Vector2 originP = Vector2{0, 0}, Vector2 positionP = Vector2{0, 0},
             Vector2 objectiveP = Vector2{0, 0}, Vector2 speedP = Vector2{0, 0},
             bool explosiveP = 0, bool activeP = 0) {
        this->origin = originP;
        this->position = positionP;
        this->objective = objectiveP;
        this->speed = speedP;

        this->explosive = explosiveP;
        this->active = activeP;
    };

    ~Particle() {}
};

/// Missile
/*!
 * Дочерний класс Missile (вражеские ракеты) от Particle
 */
///
class Missile : public Particle {
  public:
    Missile() : Particle() { explosive = 1; };
};

/// Interceptor
/*!
 * Дочерний класс Interceptor (дружественные ракеты) от Particle
 */
class Interceptor : public Particle {
  public:
    Interceptor() : Particle() { explosive = 1; };
};

/// Laser
/*!
 * Нереализованный режим огня - лазер
 */
typedef struct Laser {
    Vector2 origin;    ///< Начальная позиция
    Vector2 objective; ///< Точка задающая луч

    int frame; ///< Количество кадров, прощедших с активации лазера

    bool active; ///< Работает ли лазер
} Laser;

/// Airburst
/*!
 * Нереализованный режим огня - разрывной снаряд, разрывается на множество
 * осколков
 */
typedef struct Airburst {
    Vector2 origin;    ///< Начальная позиция
    Vector2 position;  ///< Текущая позиция
    Vector2 objective; ///< Конечная позиция
    Vector2 speed;     ///< Скорость

    int numberOfShrapnel = 10; ///< Количество осколков

    bool explosive = 0; ///< Взорвется ли снаряд при колизии
    bool active;        ///< Активность снаряда
} Airburst;

/// Shrapnel
/*!
 * Нереализованный класс нереализовванного режима ведения огня
 */
class Shrapnel : public Particle {
  public:
    Shrapnel() : Particle(){};
};

/// Explosion
/*!
 * Струтктура описывающая взрыв
 */
typedef struct Explosion {
    Vector2 position;       ///< Позиция вызыва
    float radiusMultiplier; ///< Параметр задающий радиус
    int frame; ///< Количество кадров, прощедших с активации взрыва
    bool active; ///< Активность взрыва
} Explosion;

/// Turret
/*!
 * Струтктура описывающая турели
 */
typedef struct Turret {
    Vector2 position;   ///< Позиция турели
    Vector2 objective;  ///< Позиция, по которой турель ведёт огонь
    bool active;        ///< Жива ли турель
} Turret;

/// Building
/*!
 * Струтктура описывающая мирные здания
 */
typedef struct Building {
    Vector2 position; ///< Позиция здания
    bool active;      ///< Живо ли здание
} Building;

/// Textures
/*!
 * Струтктура описывающая текстуры (просто для удобства)
 */
typedef struct Textures {
    Vector2 center; ///< Центр текстуры
    Vector2 origin; ///< Позиция текстуры на экране
    int angle;      ///< Угол поворота текстуры
} Textures;

inline enum FireModes {
    INTERCEPTOR,       // single missile
    SWARMING_MISSILES, // cascade of missiles
    LASERGUN,          // lasergun
    AIRBURST           // missile explode with shrapnel
} FireModes;

//------------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------------

inline int screenWidth = 1080; ///< Screen width in pixels
inline int screenHeight = 720; ///< Screen height in pixels

inline float inGameTime; ///< Time since the start of the game
inline float groundPositionScale =
    static_cast<float>(0.91); ///< Height of the ground above the bottom of the screen in ratio

inline int framesCounter = 0; ///< Number of frames since start of the game
inline bool gameOver = false; ///< Game over value
inline bool pause = false;    ///< Game pause value
inline int score = 0;         ///< Game score value
inline int fireMode;          ///< Current fire mode

inline std::vector<Missile> missile(MAX_MISSILES); ///< Array with all missiles
inline std::vector<Interceptor>
    interceptor(MAX_INTERCEPTORS);          ///< Array with all interceptors
inline std::vector<Explosion> explosion(MAX_EXPLOSIONS); ///< Array with all explosions
inline std::vector<Turret> turret(TURRETS_AMOUNT);       ///< Array with all turrets
inline std::vector<Building> building(BUILDINGS_AMOUNT); ///< Array with all buildings
inline int explosionIndex = 0;

/// Array with names of fire modes
inline std::vector<std::string> fireModes = {
    "INTERCEPTOR", // single missile
    "SWARM",       // cascade of missiles
    "LASERGUN",    // lasergun
    "AIRBURST"     // anti-aircraft explosive projectile
};

/// Array of all cooldowns
inline std::vector<float> cooldowns(8);
/// Array with cooldowns of fire modes
inline const std::vector<float> cooldownsOriginal = {60, 300, 120, 120};

//------------------------------------------------------------------------------------------
// Resources Declaration
//------------------------------------------------------------------------------------------

inline Textures bg;
inline Textures grass;
inline Textures turretTop[2];
inline Textures turretBottom[2];
 
inline Texture2D T_bg;
inline Texture2D T_grass;
inline Texture2D T_building;
inline Texture2D T_turretTop;
inline Texture2D T_turretBottom;

inline  Font font;

inline Music music;

//------------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------------
/**
 * @brief Initialize new game
 */
void InitGame(void);

/**
 * @brief Main function, which update all game logic (one frame)
 */
void UpdateGame(void);

/**
 * @brief Draw all elements (one frame)
 */
void DrawGame(void);

/**
 * @brief Upload all content: textures, music
 */
void UploadGame(void);

/**
 * @brief Unload all content
 */
void UnloadGame(void);

// Additional module functions

/**
 * @brief Check collision particle with bounds, and optionaly with objective,
 * explosions, turrets and buildings
 *
 * @param particle Particle
 * @param withBounds check collision with bounds
 * @param withObjective check collision with objective
 * @param withExplosions check collision with explosions
 * @param withTurrets check collision with explosions
 * @param withBuildings check collision with buildings
 *
 * @return particle.active which says that particle still alive or already not
 */
bool CheckCollisionParticle(Particle particle, bool withBounds,
                            bool withObjective, bool withExplosions,
                            bool withTurrets, bool withBuildings);

/**
 * @brief Update all explosions
 */
void UpdateExplosions();

/**
 * @brief Update all interceptors position and check collision with it
 */
void UpdateInterceptors();

/**
 * @brief Update all missiles position and check collision with it
 */
void UpdateMissiles();

/**
 * @brief Initiate one of the fire modes
 */
void CreateOutgoingFire();

/**
 * @brief Creates some missiles
 */
void CreateIncomingFire();

/**
 * @brief Reduces all cooldowns to zero
 */
void UpdateCooldown();

void CreateOutgoingInterceptor();
void CreateOutgoingSwarmingMissiles();
void CreateOutgoingLaserBeam();
void CreateOutgoingAirburst();
void CreateShrapnel();
void DrawCooldownBox();

/**
 * @brief Scales the rectangle along the x and y axis
 *
 * @param rec Original rectangle
 * @param xscale x-axis scale
 * @param yscale y-axis scale
 *
 * @return Scaled Rectangle
 */
Rectangle RectangleScale(Rectangle rec, int xscale, int yscale);

/**
 * @brief Draw texture
 *
 * @param sprite Original texture
 * @param textures Textures type which describe sprite
 * @param angle Angle at which the texture will be rotated
 * @param flipx flip along x-asis
 * @param flipy flip along y-asis
 *
 * @return Draw sprite
 */
void DrawSprite(Texture2D sprite, Textures textures, int angle,
                       bool flipx, bool flipy);
