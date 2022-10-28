# How to create a custom DTO

## DTO definition
You will need to create your custom DTO to transfer data over network.  
A DTO is a simple structure that contains all the fields you want to transfer.

> __Note__ : The structure need to be packed to be sent over the network. No padding must be left over so any client programmed in a different language can read the data correctly.

```cpp
//packing start for MSVC compiler
#ifdef _WIN32
    #pragma pack(push, 1)
#endif

// DTO definition
struct SampleDto {
    std::string value;
}

#ifdef _WIN32
    // packing end for MSVC compiler
    ;
    #pragma pack(pop)
#else
    // packing for linux compilers
    __attribute__((packed));
#endif
```

## DTO serialization
The library provides a `SerializerTrait` trait to serialize your DTO. There is already a specialization for all standard layout types.  
If your DTO has only standard layout types, you can use the `SerializerTrait` trait as is. Otherwise, you will need to specialize it for your DTO.

```cpp
// Specialization of the SerializerTrait for the SampleDto
namespace polymorph::network
{
    template<>
    struct SerializerTrait<SampleDto>
    {
        static std::vector<std::byte> serialize(const SampleDto &data)
        {
            std::vector<std::byte> buffer(data.value.size());

            std::memcpy(buffer.data(), data.value.data(), data.value.size());
            return buffer;
        }

        static SampleDto deserialize(const std::vector<std::byte> &buffer)
        {
            SampleDto dto;

            dto.value = std::string(reinterpret_cast<const char *>(buffer.data()), buffer.size());
            return dto;
        }
    };
}
```
It is recommended to define the specialization of the `SerializerTrait` in the same file as the DTO definition.