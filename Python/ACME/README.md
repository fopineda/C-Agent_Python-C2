# ACME
## Advanced CNO management environment

### Overview

We need to develop a C2 server and cli to control and manage agents.  We will use this
application during the capstone to remotely manage agents and pivot through the network.
The C2 will need to have the following basic functionallity:

1. Accept agent connections on a tcp port
2. Deserialize Response structures
3. Serialize Command structures
4. Send an recieve byte streams over a tcp socket
5. Track unique agents when they connect
6. Parse CLI for commands and send command appropriately

A framework has been provided for you along with unit tests to help get started with
development. See protocol.md for details on the network protocol the agent will use.
The unit tests cover a large part of the requirements but not all of them, you will have
to add for functionality and test to complete this project. You may decide to change the
protocol, or add / remove features not specified in the guidance, but the class will
assume that you are following these guides and it will be more difficult for the
instructors to assist you if you do.
