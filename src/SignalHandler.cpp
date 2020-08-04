#include "SignalHandler.h"

#include <iostream>
#include <signal.h>

namespace ddgen {
bool SignalHandler::_shallStop = false;
bool SignalHandler::_shallConfigurationUpdate = false;

SignalHandler::SignalHandler()
{
    _registerSignals();
}

bool SignalHandler::shallStop()
{
    return _shallStop;
}

void SignalHandler::_registerSignals()
{
    struct sigaction new_action;

    new_action.sa_handler = SignalHandler::_handleSignals;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGHUP, &new_action, NULL);
    sigaction(SIGTERM, &new_action, NULL);
}

void SignalHandler::_handleSignals(int sigNum)
{
    switch (sigNum) {
    case SIGTERM:
        std::cout << "SIGTERM signal received, quitting ..." << std::endl;
        _shallStop = true;
        break;
    case SIGHUP:
        std::cout << "SIGHUP signal received, requesting a configuration reread ..." << std::endl;
        _shallConfigurationUpdate = true;
        break;
    default:
        std::cout << "Ungandled signal received " << sigNum << std::endl;
    }
}

} // namespace ddgen
