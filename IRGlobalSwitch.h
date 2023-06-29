//
// Created by gao on 6/29/23.
//

#ifndef SYSYLEX_IRGLOBALSWITCH_H
#define SYSYLEX_IRGLOBALSWITCH_H

#include <stack>

namespace IRCtrl
{

    template<typename T>
    class IRSwitch
    {
    public:
        virtual void push() = 0;

        virtual void set(T) = 0;

        virtual T get() = 0;

        virtual void pop() = 0;
    };

    class IRBoolSwitch : public IRSwitch<bool>
    {
    public:
        IRBoolSwitch() = default;

        explicit IRBoolSwitch(bool init) : _cur(init) {}

        void push() override
        {
            _stack.push(_cur);
        }

        void set(bool t) override
        {
            _cur = t;
        }

        void pop() override
        {
            _stack.pop();
        }

        bool get() override
        {
            return _cur;
        }

    private:
        std::stack<bool> _stack;
        bool _cur = false;
    };

    class IRGlobalSwitch
    {
    public:
        IRBoolSwitch isConst;
    };

} // IRCtrl

#endif //SYSYLEX_IRGLOBALSWITCH_H