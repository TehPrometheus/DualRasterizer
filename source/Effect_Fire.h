#pragma once
#include "Effect.h"
class Effect_Fire final : public Effect
{
public: 
    // -----------------------------------------------
    // Constructor and destructor
    // -----------------------------------------------
    Effect_Fire(ID3D11Device* pDeviceInput, const std::wstring& pathInput);
    ~Effect_Fire();
    
    // -----------------------------------------------
// Copy/move constructors and assignment operators
// -----------------------------------------------
    Effect_Fire(const Effect_Fire& other)                 = default;
    Effect_Fire(Effect_Fire&& other) noexcept             = default;
    Effect_Fire& operator=(const Effect_Fire& other)      = default;
    Effect_Fire& operator=(Effect_Fire&& other) noexcept  = default;

}; 

