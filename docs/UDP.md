# Polymorph Network Library (UDP part)
> Here is the documentation for the UDP part. If you want to implement an TCP connection check [this](TCP.md) out.

As mentioned in the [README](/readme.md), the library has 2 major class that you will have to instantiate. For UDP those class are `polymorph::network::udp::Server` and `polymorph::network::udp::Client`.

For readability’s sake, namespaces `polymorph::network::` and `polymorph::network::udp::` will be omitted but keep in mind you need to mention them or use the following directive : 
```c++
using namespace polymorph::network;
using namespace polymorph::network::udp;
``` 

## UDP specifics
The  implemented protocol is based on top of UDP so there is no builtin confirmation that a packet has been received. Fortunately, the library provides a builtin resend timeout and acknowledgement of packets.  
This is why you have to create a "safeties mapping", which is a map of `OpId` to `bool`. This map will be used to know if a packet is "important" or not and needs to be resent until acknowledgement.


## Server

### 1) Creating a server
The `Server` has a construction method `create` that takes a port as parameter. 
```cpp
//create the safeties map
std::map<OpId, bool> safetiesMapping = {
    { SampleDto::opId, true }, // SampleDto is an important packet, it will be resent if we do not receive a confirmation of its reception. It will also send an ACK packet if the server receives a packet with this OpId
    { AnotherDto::opId, false } // This packet is not important, it will be sent once
};

// Then create the server that will listen on port 4242
std::unique_ptr<Server> server = Server::create(4242, safetiesMapping);
```
It is recommended to put a public static variable in all your DTOs to always have their associated opId. This is why ```SampleDto::opId``` is used in the snippet.


### 2) Register packet handlers
You can now register callbacks to handle received packet. Here is an example of registering a callback for a packet with a payload of type `SampleDto` :
```c++
server->registerReceiveHandler<SampleDto>(SampleDto::opId, [](const PacketHeader &header, const APyaloadType &payload) {
    std::cout << "Server received \"" <<  payload.value << "\" from client with session id " << header.sId << std::endl;
    // Note that before calling the callback, the server will check if the packet is important or not. If it is, it will send an ACK packet
});
```

🎉 Congratulations, you have created your server and registered your first callback !  

### 3) Start the server
Now you will have to start it in order to accept incoming connections.
```c++
server->start();
```

### 4) Send packets
You can now send packets to your clients. To do so, you will have to get the `SessionId` of the client you want to send a packet to. You can find it in received packet callbacks in the header. 
```cpp
server->sendTo<SampleDto>(SampleDto::opId, payload, clientSessionId, [](const PacketHEader &header, const SampleDto &payload) {
    std::cout << "Server sent packet to client" << std::endl;
});
// or, to send to all clients
server.send(SampleDto::opId, payload);
```

## Client

### 1) Creating a client
The `Client` has a construction method that takes a host string, a port and a `std::map<OpId, bool>` as parameters . You have to pass the address and the port of a running (or soon) server.
The safety mapping should be the same as the one used by the server. You will also have to create a connector with a reference of the created client.
```cpp
std::map<OpId, bool> safetiesMapping = {
    { SampleDto::opId, true }, // SampleDto is an important packet, it will be resent if we do not receive a confirmation of its reception. It will also send an ACK packet if the client receives a packet with this OpId
    { AnotherDto::opId, false } // This packet is not important, it will be sent once
};

std::unique_ptr<Client> client = Client::create("127.0.0.1", 4242, safetiesMapping);
```

### 2) Registering callbacks
You can now register callbacks to handle received packet. Here is an example of registering a callback for a packet with a payload of type `SampleDto` :
```c++
client->registerReceiveHandler<SampleDto>(SampleDto::opId, [](const PacketHeader &header, const APyaloadType &payload) {
    std::cout << "Client received : " <<  payload.value << std::endl;
    // Note that before calling the callback, the client will check if the packet is important or not. If it is, it will send an ACK packet
});
```

### 3) Connecting to the server
You can now call the `Client::connect()` to initiate the connection with the server. You pass a callback which will be called when the server has accepted/refused the connection. 
```c++
client->connect([](bool authorized, SessionId id) {
    if (authorized) {
        std::cout << "Connected with session id " << id << std::endl;
    } else {
        std::cout << "Connection failed" << std::endl;
    }
});
```

### 4) Sending packets
You can now send packets to the server. Here is an example of sending a packet with a payload of type `SampleDto` :
```c++
SampleDto payload;
payload.value = 42;
client->send(SampleDto::opId, payload, [](const PacketHeader &header, const SampleDto &payload) {
    std::cout << "Client sent packet to server";
});
```


## Server and Client
You can see a complete example of echo UDP server and client in the [example](/examples/udpEcho) folder.