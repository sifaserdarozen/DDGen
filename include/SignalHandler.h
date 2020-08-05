#pragma once

namespace ddgen
{
class SignalHandler
{
public:
    SignalHandler();
    static bool shallStop();
    virtual ~SignalHandler() = default;
private:
    static void _handleSignals(int sigNum);
    static void _registerSignals();
public:
    static bool _shallStop;
    static bool _shallConfigurationUpdate;
};
}
