#pragma once

namespace ddgen
{
class SignalHandler
{
public:
    SignalHandler();
    bool shallStop() const;
    virtual ~SignalHandler() = default;
private:
    static void _handleSignals(int sigNum);
    void _registerSignals() const;
public:
    static bool _shallStop;
    static bool _shallConfigurationUpdate;
};
}
