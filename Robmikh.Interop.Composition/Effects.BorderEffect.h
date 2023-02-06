#pragma once
#include "Effects.BorderEffect.g.h"

namespace winrt::Robmikh::Interop::Composition::Effects::implementation
{
    struct BorderEffect : BorderEffectT<BorderEffect, EffectBase>
    {
        BorderEffect() : BorderEffectT<BorderEffect, EffectBase>(2, 1)
        {
            ExtendX(EdgeBehavior::Clamp);
            ExtendY(EdgeBehavior::Clamp);
        }

        winrt::guid EffectId() noexcept override { return CLSID_D2D1Border; }
        EffectBase::PropertyMappings const& GetPropertyMappings() noexcept override
        {
            static EffectBase::PropertyMappings mappings =
            {
                { L"ExtendX", { D2D1_BORDER_PROP_EDGE_MODE_X, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT } },
                { L"ExtendY", { D2D1_BORDER_PROP_EDGE_MODE_Y, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT } },
            };
            return mappings;
        }

        winrt::Robmikh::Interop::Composition::Effects::EdgeBehavior ExtendX() { return GetBoxedProperty<uint32_t, EdgeBehavior>(D2D1_BORDER_PROP_EDGE_MODE_X); }
        void ExtendX(winrt::Robmikh::Interop::Composition::Effects::EdgeBehavior const& value) { SetBoxedProperty<uint32_t, EdgeBehavior>(D2D1_BORDER_PROP_EDGE_MODE_X, value); }
        winrt::Robmikh::Interop::Composition::Effects::EdgeBehavior ExtendY() { return GetBoxedProperty<uint32_t, EdgeBehavior>(D2D1_BORDER_PROP_EDGE_MODE_Y); }
        void ExtendY(winrt::Robmikh::Interop::Composition::Effects::EdgeBehavior const& value) { SetBoxedProperty<uint32_t, EdgeBehavior>(D2D1_BORDER_PROP_EDGE_MODE_Y, value); }
        winrt::Windows::Graphics::Effects::IGraphicsEffectSource Source() { return m_sources.GetAt(0); }
        void Source(winrt::Windows::Graphics::Effects::IGraphicsEffectSource const& value) { m_sources.SetAt(0, value); }
    };
}
namespace winrt::Robmikh::Interop::Composition::Effects::factory_implementation
{
    struct BorderEffect : BorderEffectT<BorderEffect, implementation::BorderEffect>
    {
    };
}
