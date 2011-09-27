#ifndef LMController_H
#define LMController_H

#include "LMCommon.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The LMController class implements the controller that
 * dedices what actions to take for distribution strategies.
 */
class LMController
{

  // Method(s)
public:
    /**
     * Returns the singleton of the LMController
     */
    static LMController* get_singleton()
    {
        if (_singleton == 0)
          _singleton = new LMController();
        return _singleton;
    }

    /**
     * Set the current time
     */
    void set_current_time(double current_time) { _current_time = current_time; }

    /**
     * Redistribute load
     */
    void redistribute_load();


private:
    /**
     * Constructs a LMController.
     */
    LMController();

    /**
     * Warmup peoriod load distribution
     */
    void warm_up_load_distribution();

    /**
     * Initial Global operator distribution
     */
    void initial_load_distribution();

    /**
     * Dynamic operator distribution
     */
    void dynamic_load_distribution();

    /**
     * The handle of the LMController
     */
    static LMController* _singleton;

    /**
     * The time for decision makeing
     */
    double _current_time;

    /**
     * Is true before initial load distribution
     */
    bool _initial_distribution_needed;

    /**
     * The time the LMController stats
     */
    double _starting_time;
};

BOREALIS_NAMESPACE_END
#endif

