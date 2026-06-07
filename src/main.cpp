#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool g_autoEnabled = false;
static bool g_wasInAir    = false;

static bool obstacleAhead(PlayerObject* player, PlayLayer* layer) {
    if (!player || !layer) return false;
    float px = player->getPositionX();
    float py = player->getPositionY();
    CCRect zone(px + 5.f, py - 15.f, 60.f, 30.f);
    auto objs = layer->m_objects;
    for (int i = 0; i < (int)objs->count(); i++) {
        auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
        if (!obj || !obj->isVisible()) continue;
        int t = (int)obj->m_objectType;
        if (t == 0 || t >= 6) continue;
        if (zone.intersectsRect(obj->boundingBox())) return true;
    }
    return false;
}

class $modify(MyPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);
        if (!g_autoEnabled) return;
        auto* p = m_player1;
        if (!p || m_isDead || m_hasCompletedLevel) return;
        bool danger = obstacleAhead(p, this);
        if (danger && !g_wasInAir) {
            this->pushButton(0, true);
            g_wasInAir = true;
        } else if (!danger && g_wasInAir) {
            this->releaseButton(0, true);
            g_wasInAir = false;
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        g_wasInAir = false;
    }

    void levelComplete() {
        PlayLayer::levelComplete();
        if (g_autoEnabled) {
            Notification::create(
                "Рівень пройдено!",
                CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"),
                3.f
            )->show();
            g_autoEnabled = false;
        }
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto btn = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyPlayLayer::onToggle), 1.f
        );
        btn->toggle(g_autoEnabled);

        auto lbl = CCLabelBMFont::create("AUTO", "goldFont.fnt");
        lbl->setScale(0.5f);
        lbl->setPosition({0.f, -18.f});
        btn->addChild(lbl);

        auto menu = CCMenu::create();
        menu->addChild(btn);
        auto sz = CCDirector::sharedDirector()->getWinSize();
        menu->setPosition({sz.width - 30.f, sz.height - 30.f});
        menu->setZOrder(100);
        this->addChild(menu);
        return true;
    }

    void onToggle(CCObject*) {
        g_autoEnabled = !g_autoEnabled;
        g_wasInAir    = false;
        Notification::create(
            g_autoEnabled ? "AUTO: УВІМКНЕНО" : "AUTO: ВИМКНЕНО",
            CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"),
            2.f
        )->show();
    }
};
