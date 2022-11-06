# Polymorph Network Library (TCP part)
> Here is the documentation for the TCP part. If you want to implement an UDP connection check [this](UDP.md) out.

As mentioned in the [README](/readme.md), the library has 2 major class that you will have to instantiate. For TCP those class are `polymorph::network::tcp::Server` and `polymorph::network::tcp::Client`.

For readability’s sake, namespaces `polymorph::network::` and `polymorph::network::tcp::` will be omitted but keep in mind you need to mention them or use the following directive : 
```c++
using namespace polymorph::network;
using namespace polymorph::network::tcp;
``` 


## Server

### 1) Creating a server
The `Server` has a construction method create that takes a port as parameter. It retiurns a `std::unique_ptr<Server>` that you will have to store as long as you want the server to be running. 
```c++ 
// Create a server on port 4242
std::unique_ptr<Server> server = Server::create(4242);
```

### 2) Register packet handlers
You can now register callbacks to handle received packet. Here is an example of registering a callback for a packet with a payload of type `SampleDto` :
```c++
server->registerReceiveHandler<SampleDto>(SampleDto::opId, [](const PacketHeader &header, const APyaloadType &payload) {
    std::cout << "Server received \"" <<  payload.value << "\" from client with session id " << header.sId << std::endl;
    return true; // or false if you want to disconnect the client
});
```
It is recommended to put a public static variable in all your DTOs to always have their associated opId. This is why ```SampleDto::opId``` is used in the snippet.

🎉 Congratulations, you have created your server and registered your first callback !  

### 3) Start the server
Now you will have to start it in order to accept incoming connections. To do so, call the `Server::start()` method.
```cpp
server->start();
```

### 4) Send packets
You can now send packets to your clients. To do so, you will have to get the `SessionId` of the client you want to send a packet to. You can find it in received packet callbacks in the header. 
```cpp
server->sendTo<SampleDto>(SampleDto::opId, payload, clientSessionId, [](const PacketHEader &header, const SampleDto &payload) {
    std::cout << "Server sent packet to client" << std::endl;
});
// or, to send to all clients
server->send(SampleDto::opId, payload);
```

## Client

### 1) Creating a client
The `Client` has a construction method `create` that takes a host string and a port as parameters . You have to pass the address and the port of a running (or soon) server.  
It retiurns a `std::unique_ptr<Client>` that you will have to store as long as you want the client to be running.
```cpp
std::unique_ptr<Client> client = Client::create("127.0.0.1", 4242);
```

### 2) Registering callbacks
You can now register callbacks to handle received packet. Here is an example of registering a callback for a packet with a payload of type `SampleDto` :
```c++
client->registerReceiveHandler<SampleDto>(SampleDto::opId, [](const PacketHeader &header, const APyaloadType &payload) {
    std::cout << "Client received : " <<  payload.value << std::endl;
    return true; // or false if you want to disconnect the client
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
You can see a complete example of echo TCP server and client in the [example](/examples/tcpEcho) folder.