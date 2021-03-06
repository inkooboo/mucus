//
//  arrow.hpp
//  mucus
//
//  Created by inkooboo on 9/5/12.
//
//

#ifndef __mucus__effects__arrow__
#define __mucus__effects__arrow__

# include "defs.hpp"
# include "primitives.hpp"

# include "effect_base.hpp"

# include <memory>

namespace effects
{
    
    struct Arrow : public EffectBase
    {
        static std::weak_ptr<Arrow> create(const pr::Vec2 &position, cc::CCNode *parent, float direction, float duration);
        
        virtual void update(float dt) override;
        virtual cc::CCNode * node() override;
        
        void update_position(const pr::Vec2 &position);
        void update_direction(float direction);
    private:
        pr::Vec2 m_position;
        cc::CCSprite *m_sprite;
        float m_direction;
    };
}




#endif /* defined(__mucus__effects__arrow__) */
