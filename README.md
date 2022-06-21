# Wireless Mesh Network (WMN) for IoT

This is an implements of a Wireless Mesh Network stack for using with different radio technologies.
The stack is intended to be as agnostic as possible from the underlying radio technology in use,
allowing it to be used for a wide variety of applications with different constraints and
requirements.

This stack is intended for use to create Wireless Sensor Networks and Internet of Things applications that 
have low-power constraints, and some degree of real-time transmission, depending on the target application.

A WMN consist of five layers, as shown in the following figure.
| NetworkArchitecture |
| :---: |
| Application Layer |
| Transport Layer |
| Network Layer |
| MAC Layer |
| Physical Layer |

We will be implementing only the MAC, Network, and Transport Layers. The physical layer is already 
implemented by the radio devices and is not necessary to create one ourselves, although a controller might be necessary.
The application layer will be left to use in specific applications, we will  provide the necessary 
interface for the application to use the network.

Currently, the network is intended to run on [RIOT OS] (https://www.riot-os.org) and Linux. RIOT already provides
driver implementations for different radios, and provide an common interface to accessing and configuring the
devices.

The network is intended to be hybrid: a backbone of static nodes with little resouce limitation, with a 
set of resource-constrained nodes. For the static nodes, we will have a concentrator, which is capable of
using up to 9 different channels simultanously. Eight of those channels are for communicating the 
mobile nodes with the static ones, and the remaining one is for communicating the static nodes among each other.
The static nodes relay the information to the gateway or set of gateways, that have Internet connection.

The mobile nodes will have a single radio multiple channels approach, and will gather the data from
the environment.

In the following, we describe each component of the network.

## The MAC Layer
As mentioned, the mobile nodes have the following restrictions/requisites:
- Are resource-constrained in nature.
- Use a single-radio multiple-channel approach.

So, we need a MAC protocol that saves energy, handles multiple channels on a single radio, and is as lightweight as possible. 
The currently proposed MAC is known as Multi-Channel Lighweight Medium Access Control (MCLMAC) [^1]. It
consists of two state machines: one for handling the connection of the devices to the network, and another
for handling the power-mode of the radio and the transmission of packets.

The first state machine, known as the MAC state machine, has the following states:
   x x x x x x x            x x x x x x x            x x x x x x x            x x x x x x x
 x               x        x               x        x    Timeslot   x        x               x
x  Initialization x ---> x Synchronization x ---> x   and channel   x ---> x      Medium     x
x                 x      x                 x      x    selection    x      x      Accesss    x
 x               x        x               x        x               x        x               x
   x x x x x x x            x x x x x x x            x x x x x x x            x x x x x x x
        ^                       ^                          |                        |
        |                       |--------No slot/channel----                        |
        ----------------------------Collision---------------------------------------|

Each state has the following functions:
- The _Initialization_ state will hear for incomming packets on the network. When a packet is detected, the 
machine passes to the next state: Synchronization. If no packets are found, it is assume that the node
is the first on the network, and a flag is set to indicate this case. This state will also generate a 
random number called *wakeup_frame* which will indicate when the node should join the network.
- The _Synchronization_ state will hear for incoming control packets to extract the follwoing information:
the network's time, the time when the network was created, the number of hops to the sink or gateway,
the network's current frame, and the network's current slot. This data will be used for synchronizing 
the node to the network.
- The MAC will have different frequencies and slots for allowing the nodes to transmit packets. The state
_Timeslot and channel selection_ will allow the node to select the node's channel and slot for the transmission
of the packets. If no channel or frequency is found, it will return an error and will go back to the 
Synchronization state.
- Once a channel and slot is selected, the machine passes to the _Medium Access_ state, which will
execute the Power Mode state machine.

The PowerMode State Machine has the following states:
                                                  x x x x x x x
                                                x               x
                        ----------------------x     Transmit      x
                       /                      x                   x
                      /                         x               x
                     /                            x x x x x x x
                    /                           ^
                   /                           /
                  /                           /
   x x x x x x x v          x x x x x x x    /
 x               x        x               x /
x     Passive     x ---> x     Active      x\
x                 x      x                 x \
 x               x        x               x   \
   x x x x x x x ^          x x x x x x x      \
                  \                             v
                   \                              x x x x x x x
                    \                           x               x
                     \                        x     Receive       x
                      \ ----------------------x                   x
                                                x               x
                                                  x x x x x x x
Each state has the following function:
- The state _Passive_ puts the radio to sleep, to allow minimal energy consumption.
It also read/write packets received from/send to upper layers from the queue. It will also increase
the current slot and, if necessary, the current frame.
- The _Active_ state wake ups the radio, and initiates the common frequency phase. In this phase, all 
radios are on and are set to a common frequency, known as the CF channel. In this phase, the nodes
hear for incoming cf packets, which will indicate which node wants to communicate with which other
node. If the node should send or receive packets to/from other nodes, it executes the split phase,
otherwise it returns to the Passive state.
- The _Transmit_ state if for node which want to communicate with other nodes. This state executes the
split corresponding to the transmission of packets. The node should first send a control packet, used
for synchronization among the nodes. After this, all the data packets are sent. Once all the data 
packets are sent, or the timer expires, the machine transits to the Passive state.
- The _Receive_ state executes the split phase corresponding to the reception of packets. It will hear 
first for incoming control packets and check for synchronization. Once such packet is received, the 
node will hear for any incoming data packet. Once all the packets are received, or the timer expires,
the node will transit to the Passive state.

### References
[^1]: Incel Ozlem D., van Hoesel Lodewijk, Jansen Pierre, and Havinga Paul *MC-LMAC: A multi-channel MAC protocol for wireless sensor networks.* In Ad Hoc Networks 9 (2011), Elsevier.
