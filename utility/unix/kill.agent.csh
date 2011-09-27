#!/bin/csh -f

##############################  kill.agent.csh  ################################
#
#  <<< begin .logout >>>
#  kill.agent.csh
#  <<< end .logout >>>
#...............................................................................


if ( ${?SSH_AGENT_PID} ) then
    echo  "killing ssh agent [pid = ${SSH_AGENT_PID}]"

    # Delete all identities from the agent.
    ssh-add  -D

    # Kill the agent.
    kill  ${SSH_AGENT_PID}

    # Delete these to signal the agent is not running.
    unsetenv SSH_AGENT_PID
    unsetenv SSH_AUTH_SOCK
endif

#
#############################  end  kill.agent.csh  ############################
