// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <unordered_map>
#include <variant>

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    class model
    {
    public:
        /// \brief raw vertex data used to generate the model
        /// \warn views do not own their data; it is the user's responsibility to make sure the data is valid until the model is generated and to clean up afterwards.
        //TODO how to handle multiple vbos? hmm
        //struct vertex_data_view
        //{   
            //TODO: implement
            //Goals: allow attribute types to vary, tightly packed into 1 vbo.
            // each buffer may be dedicated to a single attribute set or could be interlaced.
            // each buffer has a type that maps to the GL attribute primitive types. -> look this up.

            struct attribute_data_view
            {
                using byte_type = char;
                using unsigned_byte_type = unsigned char;
                using short_type = short;
                using unsigned_short_type = unsigned short;
                using float_type = float;

                //! possible attrib component types
                enum class component_type
                {
                    byte_type,
                    unsigned_byte_type,
                    short_type,
                    unsigned_short_type,
                    float_type
                }
                component_type;
                
                //! number of components per attribute
                size_t components_per_attribute;

                //! total number of attributes in the attribute data
                size_t dataSize;
                
                //! ptr to the beginning of the attribute list
                std::variant<byte_type *, unsigned_byte_type *, short_type, unsigned_short_type *, float_type *> data; 

                //attribute_data_view(const std::vector<
            };

            /*using byte_attribute_data_view = attribute_data_view<char>;
            using unsigned_byte_attribute_data_view = attribute_data_view<unsigned char>;
            using short_attribute_data_view = attribute_data_view<short>;
            using unsigned_short_attribute_data_view = attribute_data_view<unsigned short>;
            using float_attribute_data_view = attribute_data_view<float>;*/

            //using attributes = std::unordered_map</*attribname*/std::string, attribute>;
        //};

        virtual ~model() = default;

    protected:
        model() = default;
    };
}

#endif
