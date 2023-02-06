#pragma once
#include "Effects.CompositeEffect.g.h"

namespace winrt::Robmikh::Interop::Composition::Effects::implementation
{
    struct CompositeEffect : CompositeEffectT<CompositeEffect, EffectBase>
    {
        CompositeEffect() : CompositeEffectT<CompositeEffect, EffectBase>(1, 0)
        {
            Mode(CompositeMode::SourceOver);
        }

        winrt::guid EffectId() noexcept override { return CLSID_D2D1Composite; }
        EffectBase::PropertyMappings const& GetPropertyMappings() noexcept override
        {
            static EffectBase::PropertyMappings mappings =
            {
                { L"Mode", { D2D1_COMPOSITE_PROP_MODE, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT  } },
            };
            return mappings;
        }

        winrt::Robmikh::Interop::Composition::Effects::CompositeMode Mode() { return GetBoxedProperty<uint32_t, CompositeMode>(D2D1_COMPOSITE_PROP_MODE); }
        void Mode(winrt::Robmikh::Interop::Composition::Effects::CompositeMode const& value) { SetBoxedProperty<uint32_t, CompositeMode>(D2D1_COMPOSITE_PROP_MODE, value); }
        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Graphics::Effects::IGraphicsEffectSource> Sources() { return m_sources; }
    };
}
namespace winrt::Robmikh::Interop::Composition::Effects::factory_implementation
{
    struct CompositeEffect : CompositeEffectT<CompositeEffect, implementation::CompositeEffect>
    {
    };
}
