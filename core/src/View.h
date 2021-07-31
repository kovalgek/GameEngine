#pragma once

class ViewModel;

class View {
public:
    virtual void present() = 0;
    virtual ViewModel &getViewModel() = 0;
};