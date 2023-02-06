#pragma once
#include "Effects.OpacityEffect.g.h"

namespace winrt::Robmikh::Interop::Composition::Effects::implementation
{
    struct OpacityEffect : OpacityEffectT<OpacityEffect, EffectBase>
    {
        OpacityEffect() : OpacityEffectT<OpacityEffect, EffectBase>(1, 1)
        {
            Opacity(1.0f);
        }

        winrt::guid EffectId() noexcept override { return CLSID_D2D1Opacity; }
        EffectBase::PropertyMappings const& GetPropertyMappings() noexcept override
        {
            static EffectBase::PropertyMappings mappings =
            {
                { L"Opacity", { D2D1_OPACITY_PROP_OPACITY, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT  } },
            };
            return mappings;
        }

        float Opacity() { return GetBoxedProperty<float, float>(D2D1_OPACITY_PROP_OPACITY); }
        void Opacity(float value)
        {
            if (value < 0.0f || value > 1.0f)
            {
                throw winrt::hresult_invalid_argument();
            }

            SetBoxedProperty<float, float>(D2D1_OPACITY_PROP_OPACITY, value);
        }
        winrt::Windows::Graphics::Effects::IGraphicsEffectSource Source() { return m_sources.GetAt(0); }
        void Source(winrt::Windows::Graphics::Effects::IGraphicsEffectSource const& value) { m_sources.SetAt(0, value); }
    };
}
namespace winrt::Robmikh::Interop::Composition::Effects::factory_implementation
{
    struct OpacityEffect : OpacityEffectT<OpacityEffect, implementation::OpacityEffect>
    {
    };
}
