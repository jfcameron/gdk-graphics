// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_UNIFORMCOLLECTION_H
#define GDK_GFX_UNIFORMCOLLECTION_H

#include <gdk/opengl.h>

#include <iosfwd>
#include <map>
#include <string>

namespace gdk
{
    /// \brief is used to supply data to shaderprograms.
    ///
    /// \detailed It is a baseclass that manages, binds and unbinds a collection of T to a
    /// shaderprogram
    ///
    /// \todo Cache uniform locations. currently uniform locations are resolved at every bind
    template<typename T> class UniformCollection
    {
    protected:
        std::map<std::string, T> m_Map = {}; //!< internal map
            
    public:
        //! adds a T to the map at aName, overwriting if a T is already there.
        void put(const std::string &aName, const T &aItem)
        {
            if (m_Map.find(aName) != m_Map.end()) m_Map.erase(aName);
            
            m_Map.insert({aName, aItem});
        }

        //! returns a T at aName
        /// \Warning I dont think the failure case is safe for all types T. Need to unit test different categories of T
        T get(const std::string &aName) const
        {
            return m_Map.find(aName);
        }

        //! Uploads uniform data to currently used program
        virtual void bind(const GLuint aProgramHandle) const = 0;

        //! zeroes uniform data in currently used program
        virtual void unbind(const GLuint aProgramHandle) const = 0;
            
        UniformCollection &operator=(const UniformCollection &) = delete;
        UniformCollection &operator=(UniformCollection &&) = delete;
      
        UniformCollection() = default;
        UniformCollection(const UniformCollection &) = default;
        UniformCollection(UniformCollection &&) = default;
        virtual ~UniformCollection() = default;
    };
}

#endif
