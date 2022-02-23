# Message Queues for IPC

This folder contains an API for the message queues necessary for IPC for the different layers
on the Network Stack to pass the different packets among them. The API will work on both Linux and RIOT, and will hide away the details of the handling the queues on each OS, so a single interface can be used on both parts.

The API will handle the access to the queue, the message passing, and necessary functionality for the queues on each OS to work. It is possible to create several queues with different requierements in space and message size, and the API handles all internally.

The queues necessary are the following:
 - A set of queues for MAC <-> Routing layers.
 - A set of queues for Routing <-> Transport layers.
 - A set of queues for Routing <-> Application layers.
 - A set of queues for Transport <-> Application layers.

The MAC layer will send / receive data packets and control packets to / from the routing layers, which will then send on the medium. The MAC will also send the CBT metric, or the time the channel is used.

The routing layer will send / receive control and data packets to / from the MAC and Transport layers, necessary for the proper function of the network stack.

The Transport layer will send / receive control and data packets to / from the Transport layer. It will also receive data messages from the Application layer, which will fragment and process and then send to the rest of the stack. It will also receive message fragments from the lower layers, which will
gather together and then send to the Application layer as a complete message.

The Application layer will send data on the node's velocity and energy so the routing protocol
can function properly. This queue is necessary only for the client nodes.

This API will work both on RIOT and Linux.
