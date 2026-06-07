#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static bool g_autoEnabled = false;

class $modify(MyPlayLayer, PlayLayer) {

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

    void update(float dt) {
        PlayLayer::update(dt);
        if (!g_autoEnabled || !m_player1) return;

        float px = m_player1->getPositionX();
        float py = m_player1->getPositionY();
        CCRect zone(px + 5.f, py - 15.f, 60.f, 30.f);

        for (int i = 0; i < (int)m_objects->count(); i++) {
            auto obj = static_cast<GameObject*>(m_objects->objectAtIndex(i));
            if (!obj || !obj->isVisible()) continue;
            if (zone.intersectsRect(obj->boundingBox())) {
                this->handleButton(true, (int)PlayerButton::Jump, true);
                this->handleButton(false, (int)PlayerButton::Jump, true);
                break;
            }
        }
    }

    void onToggle(CCObject*) {
        g_autoEnabled = !g_autoEnabled;
        Notification::create(
            g_autoEnabled ? "AUTO: УВІМКНЕНО" : "AUTO: ВИМКНЕНО",
            CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"),
            2.f
        )->show();
    }
};
