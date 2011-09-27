################################  agent.sh  ####################################
#
#  Declare functions for ssh-agent:
#     agent         Start the ssh-agent.
#     kill.agent    Kill an ssh-agent.
#...............................................................................


#  When connecting from Windows, declare SSH_AUTH_SOCK with the value:
#     /tmp/.ssh-socket
#
#  Now a matching version is established for Cygwin.
#
SSH_AUTH_SOCK=/tmp/.ssh-socket
export SSH_AUTH_SOCK


################################################################################
#
function agent              # Activate the SSH agent.
{
#  The pid for the agent is saved in /tmp/.ssh-agent-pid so it can be
#  killed from any session.
#...............................................................................


# Attempt to list the fingerprints represented by the agent.
# The redirection stuff does not work in Cygwin.
# ssh-add -l 2>&1 >/dev/null
#
# 0 - Already connected.
# 1 - The agent has no identities.
# 2 - Couldn't connect to ssh-agent (one needs to be started).
#
ssh-add -l

if [ $? = 2 ]; then
    echo  "agent:  We'll have to try and start an authentication agent ..."
    ssh-agent -a ${SSH_AUTH_SOCK} > /tmp/.ssh-script

    if [ $? != 0 ]; then
        # The socket may still be busy.  Wait and try again.
        #
        echo  "agent:  ... Please wait a few seconds ..."
        sleep  3

        ssh-agent -a ${SSH_AUTH_SOCK} > /tmp/.ssh-script
    fi

    if [ $? = 0 ]; then
        . /tmp/.ssh-script

       echo  ${SSH_AGENT_PID} > /tmp/.ssh-agent-pid
       echo  "agent:  ... Started ssh-agent; add your pass phrase ..."
       ssh-add

    else
       echo  "agent:  ERROR:  Could not start the ssh-agent."
    fi
fi
}


################################################################################
#
function kill.agent      #  Use this function to manually kill the agent.
{
#...............................................................................


if [ -f /tmp/.ssh-agent-pid ]; then
    SSH_AGENT_PID=`cat  /tmp/.ssh-agent-pid`
    echo  "killing ssh agent [pid = ${SSH_AGENT_PID}]"

    # Delete all identities from the agent.
    ssh-add  -D

    # Kill the agent.
    kill  ${SSH_AGENT_PID}
fi

}

#
###############################  end  agent.sh  ################################
