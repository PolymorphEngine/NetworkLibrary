# Polymorph Network Library (PNL)


A simple library to use in your networked projects.

* Support for TCP and UDP
* Support for IPv4
* Safe and easy to use
* Cross-platform
* Tested with GCC and Visual Studio
* Tested with Valgrind


## ⚙ How does it work?

The library has a simple API, which is divided into two parts: the `Client` and the `Server`.
They both have pretty much the same API, but some methods are exclusive.  
The client and server are designed to exchange Packets.

### What is a packet ? 
A Packet just a simple structure containing base information and a payload. The payload can be any type of data.  
Since the data transferred over the network needs to be converted to a byte array, the library provides a `SerializerTrait` trait to do that.
Where the library deliver all its power is in the `SerializerTrait` trait. Indeed, this trait can be specialized to serialize user defined types.  

As long as you provide a serializer/deserializer for your type, you can send it over the network.

### What is the `SessionStore` ?
The `SessionStore` is a simple class that stores the sessions of the clients connected to the server.
This `SessionStore` can be shared across a UDP and a TCP server. This allows the clients to keep their session between the two servers.  
To do so, it generates an authentication token for each client. This token is then sent to the client and is used to identify the client on the other server and safely attribute the same session.

### What is an `OpId` ?
The `OpId` is a simple identifier for a packet. It is used to identify the packet type and call the appropriate callback after casting the received byte stream into a header + payload  form.
A DTO type can have multiple `OpId` associated with it. This allows you to have multiple callbacks for the same type.   
Example:
```cpp
struct SampleDto {
    static constexpr OpId opIdXPos = 0;
    static constexpr OpId opIdYPos = 1;
    int value;
};

// Registering a callback for the X position
client.registerReceiveHandler<SampleDto>(SampleDto::opIdXPos, [](const PacketHeader &header, const SampleDto &payload) {
    std::cout << "Client received X position: " << payload.value << std::endl;
    return true;
});
client.registerReceiveHandler<SampleDto>(SampleDto::opIdYPos, [](const PacketHeader &header, const SampleDto &payload) {
    std::cout << "Client received Y position: " << payload.value << std::endl;
    return true;
});
```


## 📁 Add the library to your project
There is two ways to add the library to your project:
* Add the library as a submodule
* Use the CMake FetchContent module

### Add the library as a submodule
```bash
git submodule add https://github.com/PolymorphEngine/NetworkLibrary.git
```
You will then need to compile it or, if you are using CMake, add it to your project.
```cmake
add_subdirectory(NetworkLibrary)
```

### Use the CMake FetchContent module
You will need to download the [FinderPolymorphNetworkLibrary.cmake](FindPolymorphNetworkLibrary.cmake) (also available in releases)
```cmake
include(path/to/FindPolymorphNetworkLibrary.cmake)
```
Then, you will have to reload the CMake cache and the library will be downloaded and the headers will be available.

## 🔨 How to use the lib ?

### How to create a server/client ?
See the [TCP](docs/TCP.md) and [UDP](docs/UDP.md)  to see how to use the library in your project.

### How to create a custom DTO ?
See the [DTO](docs/DTO.md) page to see how to define your DTOs for your protocol.
