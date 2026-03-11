// Copyright @Dave Scholze 2026

#pragma once

class LWindow
{
public:

    virtual ~LWindow() = default;

    virtual void OnBeginPlay(){}

    virtual void Tick(){}

    virtual void OnEndPlay(){}

};
