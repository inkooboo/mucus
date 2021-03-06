#include "view.hpp"

#include "master.hpp"
#include "resource_utils.hpp"
#include "physics.hpp"
#include "app_delegate.h"
#include "effects/flying_text.hpp"

#include <algorithm>

void View::start()
{
    m_game_layer = cc::Layer::create();

    m_scene->addChild(m_game_layer);

    cc::MenuItemImage *pClose = cc::MenuItemImage::create(
                                                      res::picture("CloseNormal").c_str(),
                                                      res::picture("CloseSelected").c_str(),
                                                      [](cc::Object *) {
                                                            master_t::subsystem<AppDelegate>().end_application();
                                                      });

    pClose->setPosition( cc::Point(m_size.width - 27, m_size.height - 28) );

    cc::MenuItemImage *pReload = cc::MenuItemImage::create(
                                                       res::picture("shesterenka").c_str(),
                                                       res::picture("shesterenka_p").c_str(),
                                                       [this](cc::Object *) {
                                                            /// test effects. Remove it.
                                                            pr::Vec2 world_size = master_t::subsystem<Physics>().worldSize();
                                                            world_size *= .5f;

                                                            effects::FlyingText::create(world_size, m_game_layer, "Test Text", 14, cc::ccc3(190, 255, 190), 5.f);
                                                            ///
                                                       });

    pReload->setPosition( cc::Point(m_size.width - 27, 28) );

    cc::Vector<cc::MenuItem *> items;
    items.pushBack(pClose);
    items.pushBack(pReload);
    createGameLayerMenu(items);
}

void View::manageCameraPositionAndScale(float t)
{
//    Player &player = master_t::subsystem<Player>();
//
//        if (player.isAvatarCreated())
//        {
//            pr::Vec2 body_postion = player.getBody()->getPosition();
//
//            m_cur_positon = body_postion;
//        }

    validateScale();
    validatePosition();
}

void View::stop()
{
    m_scene->removeChild(m_game_layer, true);
}

View::View()
{
    m_scene = cc::Scene::create();
    m_size = cc::Director::sharedDirector()->getWinSize();
}

View::~View()
{
}

cc::Scene * View::scene()
{
    return m_scene;
}

void View::reload(cc::Size bg_size, pr::Vec2 world_size)
{
    stop();
    start();

    float x_world_scale = bg_size.width / world_size.x;
    float y_world_scale = bg_size.height / world_size.y;

    assert(x_world_scale == y_world_scale && "background and world should be with corresponding proportions");

    m_world_scale = x_world_scale;

    float x_view_scale = m_size.width / bg_size.width;
    float y_view_scale = m_size.height / bg_size.height;

    m_min_view_scale = std::max(x_view_scale, y_view_scale);
    m_max_view_scale = m_min_view_scale * 2;

    m_default_view_scale = (m_min_view_scale + m_max_view_scale) / 2;

    m_view_scale = m_default_view_scale;

    m_half_screen_in_world_size.x = pixelToWorld(screenToPixel(m_size.width)) / 2;;
    m_half_screen_in_world_size.y = pixelToWorld(screenToPixel(m_size.height)) / 2;

    m_cur_positon = pr::Vec2(world_size.x / 2, world_size.y / 2);

}

void View::createGameLayerMenu(const cc::Vector<cc::MenuItem *> &items)
{
    cc::Menu* menu = cc::Menu::createWithArray(items);
    menu->setPosition(cc::CCPointZero);
    menu->setVisible(true);
    m_game_layer->addChild(menu, 100);
}


float View::pixelScale() const
{
    return m_view_scale;
}

cc::Point View::toScreenCoordinates(pr::Vec2 world_coord) const
{
    world_coord += m_half_screen_in_world_size;
    world_coord -= m_cur_positon;
    return cc::Point(pixelToScreen(worldToPixel(world_coord.x)), /*m_size.height - */pixelToScreen(worldToPixel(world_coord.y)));
}

pr::Vec2 View::toWorldCoordinates(cc::Point screen_coord) const
{
    pr::Vec2 world_coord = pr::Vec2(pixelToWorld(screenToPixel(screen_coord.x)), pixelToWorld(screenToPixel(/*m_size.height - */screen_coord.y)));
    world_coord += m_cur_positon;
    world_coord -= m_half_screen_in_world_size;
    return world_coord;
}


float View::worldToPixel(float world_size) const
{
    return world_size * m_world_scale;
}

float View::pixelToScreen(float pixel_size) const
{
    return pixel_size * m_view_scale;
}

float View::screenToPixel(float screen_size) const
{
    return screen_size / m_view_scale;
}

float View::pixelToWorld(float pixel_size) const
{
    return pixel_size / m_world_scale;
}

cc::Layer * View::gameLayer()
{
    return m_game_layer;
}


pr::Vec2 View::currentCameraPosition() const
{
    return m_cur_positon;
}

void View::validateScale()
{
    if (m_view_scale < m_min_view_scale)
    {
        m_view_scale = m_min_view_scale;
    }
    if (m_view_scale > m_max_view_scale)
    {
        m_view_scale = m_max_view_scale;
    }
}

void View::validatePosition()
{
    m_half_screen_in_world_size.x = pixelToWorld(screenToPixel(m_size.width)) / 2;;
    m_half_screen_in_world_size.y = pixelToWorld(screenToPixel(m_size.height)) / 2;

    float x_margin = m_half_screen_in_world_size.x;
    float y_margin = m_half_screen_in_world_size.y;

    pr::Vec2 world_size = master_t::subsystem<Physics>().worldSize();

    if (m_cur_positon.x < x_margin)
    {
        m_cur_positon.x = x_margin;
    }
    if (m_cur_positon.x > world_size.x - x_margin)
    {
        m_cur_positon.x = world_size.x - x_margin;
    }

    if (m_cur_positon.y < y_margin)
    {
        m_cur_positon.y = y_margin;
    }
    if (m_cur_positon.y > world_size.y - y_margin)
    {
        m_cur_positon.y = world_size.y - y_margin;
    }

}

void View::moveViewBy(float dx, float dy)
{
    dx = -pixelToWorld(screenToPixel(dx));
    dy = -pixelToWorld(screenToPixel(dy));

    m_cur_positon.x += dx;
    m_cur_positon.y += dy;
    validatePosition();
}

void View::moveViewToPosition(const pr::Vec2 &position)
{
    m_cur_positon = position;

    validatePosition();
}

void View::addSprite(cc::Node *sprite, int z_order)
{
    m_game_layer->addChild( sprite, z_order );
}

void View::removeSprite(cc::Node *sprite)
{
    sprite->removeFromParentAndCleanup(true);
}

void View::drawSpriteHelper(cc::Node *sprite, pr::Vec2 position, float angle)
{
    const cc::Point &prev_position = sprite->getPosition();
    cc::Point cur_position = toScreenCoordinates(position);
    if (prev_position.x != cur_position.x || prev_position.y != cur_position.y)
    {
        sprite->setPosition(cur_position);
    }

    float prev_scale = sprite->getScale();
    float cur_scale = pixelScale();
    if (prev_scale != cur_scale)
    {
        sprite->setScale(cur_scale);
    }

    //rotation in Box2d - in radiance
    //rotation in cocos2d-x - in degrees
    float prev_angle = sprite->getRotation();
    float cur_angle = -angle * 180/b2_pi; //FIXME minus ??? are you sure?
    if (prev_angle != angle)
    {
        sprite->setRotation( cur_angle );
    }
}


