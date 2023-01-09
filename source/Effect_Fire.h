#pragma once
#include "Effect.h"
class Effect_Fire :
    public Effect
{
public: 
    Effect_Fire(ID3D11Device* pDeviceInput, const std::wstring& pathInput);
    ~Effect_Fire();
}; 

