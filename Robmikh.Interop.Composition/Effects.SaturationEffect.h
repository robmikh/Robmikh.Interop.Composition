#pragma once
#include "Effects.SaturationEffect.g.h"

namespace winrt::Robmikh::Interop::Composition::Effects::implementation
{
    struct SaturationEffect : SaturationEffectT<SaturationEffect, EffectBase>
    {
        SaturationEffect() : SaturationEffectT<SaturationEffect, EffectBase>(1, 1)
        {
            Saturation(0.5f);
        }

        winrt::guid EffectId() noexcept override { return CLSID_D2D1Saturation; }
        EffectBase::PropertyMappings const& GetPropertyMappings() noexcept override
        {
            static EffectBase::PropertyMappings mappings =
            {
                { L"Saturation", { D2D1_SATURATION_PROP_SATURATION, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT  } },
            };
            return mappings;
        }

        float Saturation() { return GetBoxedProperty<float, float>(D2D1_SATURATION_PROP_SATURATION); }
        void Saturation(float value) 
        { 
            if (value < 0.0f || value > 2.0f)
            {
                throw winrt::hresult_invalid_argument();
            }

            SetBoxedProperty<float, float>(D2D1_SATURATION_PROP_SATURATION, value); 
        }
        winrt::Windows::Graphics::Effects::IGraphicsEffectSource Source() { return m_sources.GetAt(0); }
        void Source(winrt::Windows::Graphics::Effects::IGraphicsEffectSource const& value) { m_sources.SetAt(0, value); }
    };
}
namespace winrt::Robmikh::Interop::Composition::Effects::factory_implementation
{
    struct SaturationEffect : SaturationEffectT<SaturationEffect, implementation::SaturationEffect>
    {
    };
}
