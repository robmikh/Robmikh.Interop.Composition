#pragma once
#include "Effects.GaussianBlurEffect.g.h"

namespace winrt::Robmikh::Interop::Composition::Effects::implementation
{
    struct GaussianBlurEffect : GaussianBlurEffectT<GaussianBlurEffect, EffectBase>
    {
        GaussianBlurEffect() : GaussianBlurEffectT<GaussianBlurEffect, EffectBase>(3, 1)
        {
            BlurAmount(3.0f);
            Optimization(EffectOptimization::Balanced);
            BorderMode(EffectBorderMode::Soft);
        }

        winrt::guid EffectId() noexcept override { return CLSID_D2D1GaussianBlur; }
        EffectBase::PropertyMappings const& GetPropertyMappings() noexcept override
        {
            static EffectBase::PropertyMappings mappings =
            {
                { L"BlurAmount", { D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT } },
                { L"Optimization", { D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT } },
                { L"BorderMode", { D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, ABI::Windows::Graphics::Effects::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT } },
            };
            return mappings;
        }

        float BlurAmount() { return GetBoxedProperty<float, float>(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION); }
        void BlurAmount(float value) 
        { 
            if (value < 0.0f || value > 250.0f)
            {
                throw winrt::hresult_invalid_argument();
            }

            SetBoxedProperty<float, float>(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, value); 
        }
        winrt::Robmikh::Interop::Composition::Effects::EffectOptimization Optimization() { return GetBoxedProperty<uint32_t, EffectOptimization>(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION); }
        void Optimization(winrt::Robmikh::Interop::Composition::Effects::EffectOptimization const& value) { SetBoxedProperty<uint32_t, EffectOptimization>(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, value); }
        winrt::Robmikh::Interop::Composition::Effects::EffectBorderMode BorderMode() { return GetBoxedProperty<uint32_t, EffectBorderMode>(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE); }
        void BorderMode(winrt::Robmikh::Interop::Composition::Effects::EffectBorderMode const& value) { SetBoxedProperty<uint32_t, EffectBorderMode>(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, value); }
        winrt::Windows::Graphics::Effects::IGraphicsEffectSource Source() { return m_sources.GetAt(0); }
        void Source(winrt::Windows::Graphics::Effects::IGraphicsEffectSource const& value) { m_sources.SetAt(0, value); }
    };
}
namespace winrt::Robmikh::Interop::Composition::Effects::factory_implementation
{
    struct GaussianBlurEffect : GaussianBlurEffectT<GaussianBlurEffect, implementation::GaussianBlurEffect>
    {
    };
}
