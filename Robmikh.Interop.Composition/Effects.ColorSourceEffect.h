#pragma once
#include "Effects.ColorSourceEffect.g.h"

namespace winrt::Robmikh::Interop::Composition::Effects::implementation
{
    struct ColorSourceEffect : ColorSourceEffectT<ColorSourceEffect, EffectBase>
    {
        ColorSourceEffect() : ColorSourceEffectT<ColorSourceEffect, EffectBase>(1, 0)
        {
            Color({ 255, 0, 0, 0 });
        }

        winrt::guid EffectId() noexcept override { return CLSID_D2D1Flood; }
        EffectBase::PropertyMappings const& GetPropertyMappings() noexcept override
        {
            static EffectBase::PropertyMappings mappings =
            {
                { L"Color", { D2D1_FLOOD_PROP_COLOR, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4 } },
                { L"ColorHdr", { D2D1_FLOOD_PROP_COLOR, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN } },
            };
            return mappings;
        }

        winrt::Windows::UI::Color Color() { return GetBoxedProperty<float[4], winrt::Windows::UI::Color>(D2D1_FLOOD_PROP_COLOR); }
        void Color(winrt::Windows::UI::Color const& value) { SetBoxedProperty<float[4], winrt::Windows::UI::Color>(D2D1_FLOOD_PROP_COLOR, value); }

        winrt::Windows::Foundation::Numerics::float4 ColorHdr() { return GetBoxedProperty<float[4], winrt::Windows::Foundation::Numerics::float4>(D2D1_FLOOD_PROP_COLOR); }
        void ColorHdr(winrt::Windows::Foundation::Numerics::float4 const& value) { SetBoxedProperty<float[4], winrt::Windows::Foundation::Numerics::float4>(D2D1_FLOOD_PROP_COLOR, value); }
    };
}
namespace winrt::Robmikh::Interop::Composition::Effects::factory_implementation
{
    struct ColorSourceEffect : ColorSourceEffectT<ColorSourceEffect, implementation::ColorSourceEffect>
    {
    };
}
