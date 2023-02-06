#pragma once
#include "Effects.BlendEffect.g.h"

namespace winrt::Robmikh::Interop::Composition::Effects::implementation
{
    struct BlendEffect : BlendEffectT<BlendEffect, EffectBase>
    {
        BlendEffect() : BlendEffectT<BlendEffect, EffectBase>(1, 2)
        {
            Mode(BlendEffectMode::Multiply);
        }

        winrt::guid EffectId() noexcept override { return CLSID_D2D1Blend; }
        EffectBase::PropertyMappings const& GetPropertyMappings() noexcept override
        {
            static EffectBase::PropertyMappings mappings =
            {
                { L"Mode", { D2D1_BLEND_PROP_MODE, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT } },
            };
            return mappings;
        }

        winrt::Robmikh::Interop::Composition::Effects::BlendEffectMode Mode() { return GetBoxedProperty<uint32_t, BlendEffectMode>(D2D1_BLEND_PROP_MODE); }
        void Mode(winrt::Robmikh::Interop::Composition::Effects::BlendEffectMode const& value) { SetBoxedProperty<uint32_t, BlendEffectMode>(D2D1_BLEND_PROP_MODE, value); }
        winrt::Windows::Graphics::Effects::IGraphicsEffectSource Background() { return m_sources[0]; }
        void Background(winrt::Windows::Graphics::Effects::IGraphicsEffectSource const& value) { m_sources[0] = value; }
        winrt::Windows::Graphics::Effects::IGraphicsEffectSource Foreground() { return m_sources[1]; }
        void Foreground(winrt::Windows::Graphics::Effects::IGraphicsEffectSource const& value) { m_sources[1] = value; }
    };
}
namespace winrt::Robmikh::Interop::Composition::Effects::factory_implementation
{
    struct BlendEffect : BlendEffectT<BlendEffect, implementation::BlendEffect>
    {
    };
}
