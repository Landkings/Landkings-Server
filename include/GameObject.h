#pragma once
#include <string>
#include <memory>
#include <functional>
#include <map>

#include "Scene.h"
#include "Vec2i.h"
#include "Hitbox.h"
#include "lua.hpp"
#include "LuaHelper.h"
#include "Inventory.h"

namespace Engine {

#define ACTION \
  E(Move, Move, Action) \
  E(Attack, Attack, Action) \
  E(Block, Block, Action) \
  E(Empty, Empty, Action)

#define DIRECTION \
  E(Up, Up, Direction) \
  E(Right, Right, Direction) \
  E(Down, Down, Direction) \
  E(Left, Left, Direction) \
  E(Unknown, Unknown, Direction)

#define ATTACK_TYPE \
  E(Fast, Fast, AttackType) \
  E(Strong, Strong, AttackType)

#define ATTACK_DIRECTION \
  E(Head, Head, AttackDirection) \
  E(Torso, Torso, AttackDirection) \
  E(Legs, Legs, AttackDirection)

#define MOVEMENT_TYPE \
  E(Default, Default, MovementType) \
  E(Sprint, Sprint, MovementType)

#define PARAMETERS \
    E(MovementSpeed, MovementSpeed, Parameters) \
    E(AttackSpeed, AttackSpeed, Parameters) \
    E(AttackRange, AttackRange, Parameters) \
    E(HitPoints, HitPoints, Parameters) \
    E(StaminaPoints, StaminaPoints, Parameters) \
    E(AttackDamage, AttackDamage, Parameters) \
    E(VisionRange, VisionRange, Parameters) \
    E(StaminaCostReduction, StaminaCostReduction, Parameters) \
    E(StaminaRegenFrequency, StaminaRegenFrequency, Parameters)

#define OBJECT_TYPE \
    E(Player, Player, ObjectType) \
    E(HealingItem, HealingItem, ObjectType) \
    E(ExpItem, ExpItem, ObjectType) \
    E(NPC, NPC, ObjectType)

#define E C_ENUM_HELPER
enum class Action {
    ACTION
    Size
};

enum class Direction {
    DIRECTION
    Size
};

enum class AttackType {
    ATTACK_TYPE
    Size
};

enum class MovementType {
    MOVEMENT_TYPE
    Size
};

enum class AttackDirection {
    ATTACK_DIRECTION
    Size
};

enum class Parameters {
    PARAMETERS
    Size
};

enum class ObjectType {
    OBJECT_TYPE
    Size
};
#undef E

//#define E C_ENUM_TO_STRING_HELPER
//static std::map<Action, std::string> actionStrings = {
//    ACTION
//};
//
//static std::map<Direction, std::string> directionStrings = {
//    DIRECTION
//};
//#undef E

enum class SpriteDirection {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3
};

const Vec2i directions[4] = {
        Vec2i(0, -1),
        Vec2i(1, 0),
        Vec2i(0, 1),
        Vec2i(-1, 0)
};

    struct ParametersHasher {
        std::size_t operator()(const Parameters &p) const {
            return std::hash<int>()(1);
        }
    };

//namespace std {
//    template<>
//    struct hash<Parameters> {
//        size_t operator()(const Parameters &p) const {
//            return std::hash<Parameters>()(p);
//        }
//    };
//}

class GameObject {
public:
    GameObject(Scene* scene, ObjectType type, Vec2i pos = Vec2i(), std::string name = "", HitBox hbox = HitBox());
    Vec2i getPosition() const { return position; }
    virtual void setPosition(Vec2i pos) { position = pos; }
    virtual void setName(std::string newName) { name = newName; }
    virtual HitBox getHitbox() const { return hbox; }
    virtual void setHitbox(HitBox& hitbox) { hbox = hitbox; }
    virtual std::string getName() { return name; }
    virtual int getX() const { return position.getX(); }
    virtual int getY() const { return position.getY(); }
    virtual int getWidth() const { return hbox.getWidth(); }
    virtual int getHeight() const { return hbox.getHeight(); }
    virtual std::string getID() const { return name; }
    virtual bool isPassable() { return false; }
    virtual GameObject* clone() = 0;
    virtual void luaPush(lua_State *state) = 0;
    virtual ObjectType getType() { return type; }
protected:
    Vec2i position;
    HitBox hbox;
    Scene *scene;
    ObjectType type;
    std::string name;
};

class Item : public GameObject {
public:
    Item(Scene *scene, Vec2i pos, std::string name, HitBox hbox, int size, int maxCharges,
         int useCooldown, int staminaCost, int actionCooldown, int spriteId, ObjectType type) :
        GameObject(scene, type, pos, name, hbox),
        maxCharges(maxCharges),
        consumedCharges(0),
        size(size),
        isInInventory(false),
        useCooldown(useCooldown),
        staminaCost(staminaCost),
        actionCooldown(actionCooldown),
        spriteId(spriteId) {
    }
    virtual void use(Character* target) = 0;
    virtual bool used() const { return consumedCharges == maxCharges; }
    virtual int chargesLeft() const { return maxCharges - consumedCharges ; }
    virtual int getSize() const { return size; }
    virtual int getUseCooldown() const { return useCooldown; }
    virtual int getStaminaCost() const { return staminaCost; }
    virtual int getActionCooldown() const { return actionCooldown; }
    //virtual void collect(Character* target);
    virtual bool inInventory() const { return isInInventory; }
    void luaPush(lua_State *state) override;
    virtual int getSpriteId() { return spriteId; }
protected:

    int luaGetObjectType(lua_State *state);
    int luaGetPosition(lua_State *state);

    int maxCharges;
    int consumedCharges;
    int size;
    bool isInInventory;
    int useCooldown;
    int staminaCost;
    int actionCooldown;
    int spriteId;
};

class HealingItem : public Item {
public:
    HealingItem(Scene *scene, Vec2i pos, HitBox hbox, int amount, int size, int maxCharges,
                int useCooldown, int staminaCost, int actionCooldown, int spriteId) :
        Item(scene, pos, "HealingItem", hbox, size, maxCharges, useCooldown, staminaCost, actionCooldown, spriteId, ObjectType::HealingItem),
        healAmount(amount) {
    }
    void use(Character *target) override;
    GameObject *clone();
    bool isPassable() { return true; }
private:
    int healAmount;
};

class ExpItem : public Item {
public:
    ExpItem(Scene *scene, Vec2i pos, HitBox hbox, int amount, int size,
            int maxCharges, int useCooldwn, int staminaCost, int actionCooldown, int spriteId) :
        Item(scene, pos, "ExpItem", hbox, size, maxCharges, useCooldwn, staminaCost, actionCooldown, spriteId, ObjectType::ExpItem),
        expAmount(amount) {}
    void use(Character *target) override;
    GameObject *clone();
    bool isPassable() { return true; }
private:
    int expAmount;
};

class Character : public GameObject {
public:
    Character(Scene *scene, Vec2i pos = Vec2i(), std::string tmpLuaName = "", HitBox hbox = HitBox(20, 20), ObjectType type = ObjectType::NPC);
    Character(Scene *scene, std::string luaCode, std::string name, Vec2i pos = Vec2i(), ObjectType type = ObjectType::NPC);
    //int write(lua_State *state);
    void update();
    void luaPush(lua_State *state) override;
    void attack(Character *target);
    void move(Vec2i newPos);
    void takeDamage(int amount);
    void loadLuaCode(std::string luaCode);
    bool blocking() const { return action == Action::Block; }
    void loseStamina(int amount);
    void gainStamina(int amount);
    void gainDefaultStamina();
    void gainHp(int amount);
    void block(int amount);
    void takeItem(Item *item);
    void useItem(Item *item);
    virtual void gainExp(int amount);
    void payAttackCost();
    virtual int getExpValue();
    GameObject* clone();
    ~Character();

    //getters and setters
    virtual int getSpeed() const { return speed; }
    virtual int getHp() const { return hitPoints; }
    virtual void setHp(const int hp) { hitPoints = hp; }
    virtual const int getMaxHp() { return maxHitPoints + 10 * parameters[Parameters::HitPoints]; } //TODO: replace with constant
    virtual const int getStamina() { return stamina; }
    virtual const int getMaxStamina() { return maxStamina + 10 * parameters[Parameters::StaminaPoints]; } //TODO: replace with constant
    virtual const int getDamage() { return damage + 1 * parameters[Parameters::AttackDamage]; } //TODO: replace with constant
    virtual void setDirection(const Direction dir) { direction = dir; }
    virtual void setTarget (GameObject *targ) { target = targ; }
    virtual const int getMoveCooldown() { return std::max(0, moveCooldown - 1 * parameters[Parameters::MovementSpeed]); } //TODO: replace with constant
    virtual const int getAttackCooldown() { return std::max(0, attackCooldown - 10 * parameters[Parameters::AttackSpeed]); } //TODO: replace with constant
    virtual long long getNextMoveTime() const { return nextMoveTime; }
    virtual long long getNextAttackTime() const {return nextAttackTime; }
    virtual const int getVisionRange() { return visionRange + 50 * parameters[Parameters::VisionRange]; }
    virtual SpriteDirection getSpriteDirection() const { return spriteDirection; }
    virtual bool isOnCooldown() const { return nextAttackTime > scene->getTime() || nextMoveTime > scene->getTime(); }
    virtual const int getAttackRange() { return attackRange + 5 * parameters[Parameters::AttackRange]; } //TODO: replace with constants
    virtual AttackType getAttackType() const { return attackType; }
    virtual AttackDirection getBlockDirection() const { return blockDirection; }
    virtual AttackDirection getAttackDirection() const { return attackDirection; }
    virtual long long getNextStaminaRegenTime() const { return nextStaminaRegenTime; }
    virtual const int getAttackStaminaCost() const { return attackStaminaCost; }
    virtual const int getMoveStaminaCost() const { return moveStaminaCost; }
    virtual const int getBlockStaminaCost() const { return blockStaminaCost; }
    virtual const int getSprintStaminaCost() const { return sprintStaminaCost; }
    virtual const int getLevel() { return level; }
    virtual void disableStaminaRegen() { isStaminaRegenAvailable = false; }
    virtual void enableStaminaRegen() { isStaminaRegenAvailable = true; }
    virtual bool isUsingAction() { return usingAction; }
protected:
    virtual void init();
    virtual void initLuaState();
    virtual void closeLuaState();
    virtual void move();
    virtual void attack();
    virtual void block();
    virtual void takeItem();

    //lua functions
    virtual int luaSetAction(lua_State *state);
    virtual int luaGetAction(lua_State *state);
    virtual int luaSetDirection(lua_State *state);
    virtual int luaGetDirection(lua_State *state);
    virtual int luaSetTarget(lua_State *state);
    virtual int luaGetTarget(lua_State *state);
    virtual int luaGetPosition(lua_State *state);
    virtual int luaGetObjectPosition(lua_State *state);
    virtual int luaGetStamina(lua_State *state);
    virtual int luaGetHp(lua_State *state);
    virtual int luaSetAttackType(lua_State *state);
    virtual int luaSetAttackDirection(lua_State *state);
    virtual int luaSetBlockDirection(lua_State *state);
    virtual int luaSetMovementType(lua_State *state);
    virtual int luaGetMovementType(lua_State *state);
    virtual int luaGetMe(lua_State *state); //maybe delete
    virtual int luaGetAttackStaminaCost(lua_State *state);
    virtual int luaGetMoveStaminaCost(lua_State *state);
    virtual int luaGetBlockStaminaCost(lua_State *state);
    virtual int luaGetSprintStaminaCost(lua_State *state);
    virtual int luaLevelUp(lua_State *state);
    virtual int luaGetAvailableSkillPoints(lua_State *state);
    virtual int luaGetCurrentExp(lua_State *state);
    virtual int luaGetNextLevelExp(lua_State *state);
    virtual int luaGetParameterLevel(lua_State *state);
    virtual int luaGetParameterLevelUpCost(lua_State *state);
    virtual int luaCanMove(lua_State *state);
    virtual int luaUseItem(lua_State *state);
    virtual int luaCanAttack(lua_State *state);
    virtual int luaGetObjectType(lua_State *state);

    void luaCountHook(lua_State *state, lua_Debug *ar);

    int luaContinuationTest(lua_State *state, int status, lua_KContext ctx);

    //attributes
    Action action;
    Direction direction;
    AttackType attackType;
    MovementType movementType;
    AttackDirection attackDirection;
    AttackDirection blockDirection;
    GameObject *target;
    int hitPoints;
    int maxHitPoints;
    int speed;
    int damage;
    int stamina;
    int maxStamina;
    int attackRange;
    long long nextMoveTime;   //probably should refactor. Either unite cooldown variables or move up them.
    long long nextAttackTime; //
    long long nextStaminaRegenTime;
    int moveCooldown;
    int attackCooldown;
    int visionRange;
    bool isStaminaHpRegenAvailable;
    bool isStaminaRegenAvailable;
    int maxStaminaTicksRequirement;
    int maxStaminaTicks;
    int attackStaminaCost;
    int moveStaminaCost;
    int blockStaminaCost; //maybe delete
    int sprintStaminaCost;
    int level;
    int nextLevelExp;
    int currentExp;
    int skillPoints;
    bool usingAction;
    std::unordered_map<Parameters, int, ParametersHasher> parameters;
    SpriteDirection spriteDirection;
    Inventory inventory;
    lua_State *L;
};
typedef std::shared_ptr<Character> PCharacter;

class Player : public Character {
public:
    Player(Scene *scene, Vec2i pos = Vec2i(), std::string tmpLuaName = "", HitBox hbox = HitBox(20, 20));
    Player(Scene *scene, std::string luaCode, std::string name, Vec2i pos = Vec2i());
    int getExpValue();
    void gainExp(int amount);
protected:
    void init();
};
typedef std::shared_ptr<Player> PPlayer;

}
