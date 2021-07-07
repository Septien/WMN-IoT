# Wireless Mesh Network (WMN) using LoRa as the PHY

This module implements a Wireless Mesh Network using the LoRa as the communication technology.
It is inteded for use for Wireless Sensor Networks and Internet of Things applications that 
do not require a high bandwidth, but still can achieve real-time transmission.

A WMN consist of five layers, as shown in the following figure.
| NetworkArchitecture |
| :---: |
| Application Layer |
| Transport Layer |
| Network Layer |
| MAC Layer |
| Physical Layer |

We will be implementing only the MAC, Network, and Transport Layers. The physical layer is already 
implemented by the LoRa devices and is not necessary to create one ourselves (in any case, a controller
might be required). The application layer will be left to use in specific applications, we will 
provide the necessary interface for the application to use the network.

Currently, the network is intended to run on [RIOT OS] (https://www.riot-os.org) which already has a controller for the LoRa devices (specifically, the SX1276, the one considered for building up the network).

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

So, we need a MAC protocol that saves energy, handles multiple channels on a single radio, and is as lightweight as possible. The currently proposed MAC is known as Multi-Channel Lighweight Medium Access Control. It
consists of two state machines: one for handling the connection of the devices to the network, and another
for handling the power-mode of the device and the transmission of packets.