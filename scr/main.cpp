#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

static bool g_autoEnabled = false;
static bool g_wasInAir = false;

static bool obstacleAhead(PlayerObject* player, GJBaseGameLayer* layer) {
    if (!player || !layer) return false;
    float px = player->getPositionX();
    float py = player->getPositionY();
    CCRect checkZone(px + 5.f, py - 15.f, 60.f, 30.f);
    auto& objs = layer->m_objects;
    for (int i = 0; i < objs->count(); i++) {
        auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
        if (!obj || !obj->isVisible()) continue;
        int type = obj->m_objectType;
        if (type == 0 || type >= 6) continue;
        if (checkZone.intersectsRect(obj->boundingBox())) return true;
    }
    return false;
}

class $modify(GJBaseGameLayer) {
    void update(float dt) {
        GJBaseGameLayer::update(dt);
        if (!g_autoEnabled) return;
        auto* player = m_player1;
        if (!player || m_isDead || m_hasCompletedLevel) return;
        bool danger = obstacleAhead(player, this);
        if (danger && !g_wasInAir) {
            this->pushButton(0, true);
            g_wasInAir = true;
        } else if (!danger && g_wasInAir) {
            this->releaseButton(0, true);
            g_wasInAir = false;
        }
    }
};

class $modify(PlayLayer) {
    void resetLevel() {
        PlayLayer::resetLevel();
        g_wasInAir = false;
    }

    void levelComplete() {
        PlayLayer::levelComplete();
        if (g_autoEnabled) {
            Notification::create("Рівень пройдено!", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"), 3.f)->show();
            g_autoEnabled = false;
        }
    }

    bool init(GJGameLevel
